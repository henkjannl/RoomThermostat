#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

#define RECONNECT_INTERVAL   5*60*1000
#define TEMP_MEAS_INTERVAL     10*1000 
#define BOILER_INTERVAL         5*1000 

/* ======================================
   PID CONTROLLER CLASS DEFINITION
   ====================================== */
class PID {
  public:
    float KP = 10;
    float KI = 0.02;  
    float KD = 0;

    // Upper and lower bounds on requested boiler temperature
    float outputHigh = 60;
    float outputLow  =  5;
      
    float setpoint=4.0;
    
    unsigned long currentTime;
    unsigned long previousTime;
    
    float currentTemperature;
    float previousTemperature;

    float boilerTemperature;
    float boilerPercentage;

    float timeStep;

    float ierr = 0;
    float previousError = 0;
    float derivative = 0;

    float P;
    float I;
    float D;
    
    float newValue(float newTemperature);
};

float PID::newValue(float newTemperature) {

  // Update temperature
  previousTemperature = currentTemperature;
  currentTemperature  = newTemperature;

  // Store time in native format to ensure right outcome on millis() overflow 
  previousTime=currentTime;
  currentTime=millis();
  timeStep=(currentTime-previousTime)/1000.0f; // converted to seconds here
    
  // Calculate the control error
  float error = setpoint - currentTemperature;
  
  // Calculate the integral error
  ierr += error * timeStep;  
  
  // !! D-action probably not implemented well yet !!
  // perhaps we need to filter the signal to prevent noise in the D-action
  // Calculate the measurement derivative
  derivative = (timeStep!=0) ? (error - previousError) / timeStep : 0;
  
  // Calculate the PID output
  P = KP * error;        // proportional contribution
  I = KI * ierr;         // integral contribution  
  D = KD * derivative;   // differential contribution
  boilerTemperature = P+I+D;
  
  // Implement anti-windup
  // !! Should the (error<0) clause be there? !! What does it do?
  if ((boilerTemperature < outputLow)  and (error<0)) ierr -= error * timeStep;    
  if ((boilerTemperature > outputHigh) and (error>0)) ierr -= error * timeStep;    
  
  // Clip output
  boilerTemperature = max(outputLow, min(outputHigh, boilerTemperature));
  boilerPercentage=(boilerTemperature-outputLow)/(outputHigh-outputLow);
  
  //Serial.printf("setpoint= %.3f°C measured=%.3f°C P=%.3f I=%.3f D=%.3f boiler=%.3f°C\n",setpoint, currentTemperature, P, I, D, boilerTemperature);

  previousError = error;

  return boilerTemperature;
};

/* ======================================
   GLOBAL VARIABLES
   ====================================== */
OneWire oneWire(PIN_ONE_WIRE_BUS);                         // Bus for the temperature sensors 
DallasTemperature dallasSensor(&oneWire);                  // Driver for temperature sensors
OpenTherm opentherm(PIN_OPENTHERM_IN, PIN_OPENTHERM_OUT);  // Driver for boiler
PID pidController;    

//Interrupthandler for OpenTherm
void ICACHE_RAM_ATTR handleInterrupt() {
  opentherm.handleInterrupt();
}


// Sensor readings expire after three missed measurement periods.
const unsigned long SENSOR_MAX_AGE_MS = 30000;
bool conversionPending = false;
unsigned long conversionStarted = 0;
unsigned long lastValidMeasurement = 0;

bool roomTemperatureValid() {
  return controllerData.temperatureMeasurementOK &&
         millis() - lastValidMeasurement < SENSOR_MAX_AGE_MS;
}

void beginTemperatureConversion() {
  dallasSensor.requestTemperatures();
  conversionStarted = millis();
  conversionPending = true;
}

void TemperatureMeasure() {
  float sum = 0;
  int count = 0;
  for (int i = 0; i < controllerData.numSensors; ++i) {
    float value = dallasSensor.getTempC(controllerData.sensorAddress[i]);
    if (isfinite(value) && value >= -40 && value <= 80) { sum += value; ++count; }
  }
  controllerData.temperatureMeasurementOK = count > 0;
  if (count) {
    controllerData.rawTemperature = sum / count;
    controllerData.lastTemperatureMeasurement = timeNow();
    lastValidMeasurement = millis();
  }
}

void TemperatureSensorConnect() {
  dallasSensor.begin();
  controllerData.numSensors = 0;
  int found = dallasSensor.getDeviceCount();
  for (int i = 0; i < found && controllerData.numSensors < MAX_NUM_SENSORS; ++i) {
    auto &address = controllerData.sensorAddress[controllerData.numSensors];
    if (dallasSensor.getAddress(address, i)) {
      dallasSensor.setResolution(address, TEMPERATURE_PRECISION);
      ++controllerData.numSensors;
    }
  }
  controllerData.temperatureMeasurementOK = false;
  dallasSensor.setWaitForConversion(false);
  beginTemperatureConversion();
}

enum class BoilerStep : uint8_t { Idle, Status, Heating, HotWater, ReadHeating, ReadHotWater };
BoilerStep boilerStep = BoilerStep::Idle;
bool boilerWaiting = false;
bool boilerCycleOK = true;
unsigned long boilerRequestStarted = 0;
unsigned long lastBoilerCycle = 0;
bool firstBoilerCycle = true;
unsigned long boilerMaxTransmitGapMs = 0;
unsigned long boilerLateCycles = 0;
unsigned long lastBoilerTransmit = 0;
bool boilerHasValidStatus = false;
unsigned long lastBoilerValidStatus = 0;
unsigned long boilerMaxValidStatusGapMs = 0;

void updateHeatDemand() {
  pidController.setpoint = controllerData.temperatureSetpoint;
  if (!roomTemperatureValid()) {
    pidController.ierr = 0;
    pidController.currentTime = millis();
    controllerData.requestedBoilerTemperature = pidController.outputLow;
    controllerData.boilerPercentage = 0;
    controllerData.enableCentralHeating = false;
    return;
  }
  controllerData.requestedBoilerTemperature = pidController.newValue(controllerData.measuredTemperature());
  controllerData.boilerPercentage = pidController.boilerPercentage;
  controllerData.P = pidController.P;
  controllerData.I = pidController.I;
  controllerData.D = pidController.D;
  controllerData.ierr = pidController.ierr;
  controllerData.derivative = pidController.derivative;
  controllerData.enableCentralHeating =
    controllerData.measuredTemperature() < controllerData.temperatureSetpoint + 2.0 &&
    controllerData.boilerPercentage >= 0.05;
}

void finishBoilerCycle() {
  controllerData.boilerOnline = boilerCycleOK;
  if (!boilerCycleOK) {
    controllerData.centralHeatingActive = false;
    controllerData.hotWaterActive = false;
    controllerData.flameOn = false;
  } else controllerData.lastBoilerUpdate = timeNow();
  controllerData.dspBoilerIcon = boilerCycleOK ? iconBoilerConnected : iconBoilerLost;
  controllerData.dspShowerIcon = controllerData.hotWaterActive ? iconShowerOn : iconShowerOff;
  const icon_t flames[] = {iconFlame0,iconFlame1,iconFlame2,iconFlame3,iconFlame4,iconFlame5,iconFlame6,iconFlame7,iconFlame8};
  float value = controllerData.boilerPercentage;
  int index = value < .05 ? 0 : value < .10 ? 1 : value < .15 ? 2 : value < .20 ? 3 :
              value < .35 ? 4 : value < .50 ? 5 : value < .65 ? 6 : value < .80 ? 7 : 8;
  controllerData.dspFlameIcon = boilerCycleOK && controllerData.enableCentralHeating ? flames[index] : iconFlame0;
  boilerStep = BoilerStep::Idle;
  sendMessage(sndTemperature, cmdSilentUpdate, controllerQueue);
}

void BoilerCommunicate() {
  opentherm.process();
  if (!opentherm.isReady()) return;
  if (boilerWaiting) {
    boilerWaiting = false;
    auto status = opentherm.getLastResponseStatus();
    unsigned long response = opentherm.getLastResponse();
    Serial.printf("OpenTherm step=%u status=%u elapsed=%lu ms\n", (unsigned)boilerStep,
                  (unsigned)status, millis() - boilerRequestStarted);
    const OpenThermMessageID expected[] = {OpenThermMessageID::Status, OpenThermMessageID::Status,
      OpenThermMessageID::TSet, OpenThermMessageID::TdhwSet, OpenThermMessageID::Tboiler, OpenThermMessageID::Tdhw};
    bool writeRequest = boilerStep == BoilerStep::Heating || boilerStep == BoilerStep::HotWater;
    auto expectedType = writeRequest ? OpenThermMessageType::WRITE_ACK : OpenThermMessageType::READ_ACK;
    bool success = status == OpenThermResponseStatus::SUCCESS &&
                   opentherm.getDataID(response) == expected[static_cast<unsigned>(boilerStep)] &&
                   opentherm.getMessageType(response) == expectedType;
    bool required = boilerStep == BoilerStep::Status || boilerStep == BoilerStep::Heating;
    if (boilerStep == BoilerStep::ReadHeating) {
      controllerData.boilerTemperatureValid = success;
      if (success) controllerData.boilerTemperatureAt = millis();
    }
    if (boilerStep == BoilerStep::ReadHotWater) {
      controllerData.hotWaterTemperatureValid = success;
      if (success) controllerData.hotWaterTemperatureAt = millis();
    }
    if (!success && required) {
      boilerCycleOK = false;
      controllerData.boilerResponse = status == OpenThermResponseStatus::TIMEOUT ? boilerTimeout : boilerInvalid;
      // Abort on failure instead of accumulating multiple response timeouts.
      finishBoilerCycle();
      return;
    }
    if (success && required) controllerData.boilerResponse = boilerSuccess;
    if (success && boilerStep == BoilerStep::Status) {
      unsigned long received = millis();
      if (boilerHasValidStatus) boilerMaxValidStatusGapMs = max(boilerMaxValidStatusGapMs, received - lastBoilerValidStatus);
      lastBoilerValidStatus = received;
      boilerHasValidStatus = true;
      controllerData.centralHeatingActive = opentherm.isCentralHeatingActive(response);
      controllerData.hotWaterActive = opentherm.isHotWaterActive(response);
      controllerData.flameOn = opentherm.isFlameOn(response);
    } else if (success && boilerStep == BoilerStep::ReadHeating) controllerData.actualBoilerTemperature = opentherm.getFloat(response);
    else if (success && boilerStep == BoilerStep::ReadHotWater) controllerData.actualDomesticHotWaterTemperature = opentherm.getFloat(response);
    if (boilerStep == BoilerStep::ReadHotWater) { finishBoilerCycle(); return; }
    boilerStep = static_cast<BoilerStep>(static_cast<uint8_t>(boilerStep) + 1);
    return; // Give the UI a complete loop between requests.
  }
  if (boilerStep == BoilerStep::Idle) {
    if (!firstBoilerCycle && millis() - lastBoilerCycle < BOILER_INTERVAL) return;
    if (!firstBoilerCycle && millis() - lastBoilerCycle > BOILER_INTERVAL + 1000) ++boilerLateCycles;
    firstBoilerCycle = false;
    lastBoilerCycle = millis();
    updateHeatDemand();
    boilerCycleOK = true;
    boilerStep = BoilerStep::Status;
  }
  unsigned long request = 0;
  switch (boilerStep) {
    case BoilerStep::Status:
      // Recheck validity immediately before enabling heat.
      if (!roomTemperatureValid()) controllerData.enableCentralHeating = false;
      request = opentherm.buildSetBoilerStatusRequest(controllerData.enableCentralHeating,
                    controllerData.enableHotWater, controllerData.enableCooling); break;
    case BoilerStep::Heating:
      request = opentherm.buildSetBoilerTemperatureRequest(controllerData.requestedBoilerTemperature); break;
    case BoilerStep::HotWater:
      request = opentherm.buildRequest(OpenThermMessageType::WRITE_DATA, OpenThermMessageID::TdhwSet,
                    opentherm.temperatureToData(controllerData.requestedDomesticHotWaterSetpoint)); break;
    case BoilerStep::ReadHeating: request = opentherm.buildGetBoilerTemperatureRequest(); break;
    case BoilerStep::ReadHotWater:
      request = opentherm.buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Tdhw, 0); break;
    default: return;
  }
  boilerRequestStarted = millis();
  boilerWaiting = opentherm.sendRequestAync(request);
  if (boilerWaiting) {
    unsigned long transmitted = millis();
    if (lastBoilerTransmit) boilerMaxTransmitGapMs = max(boilerMaxTransmitGapMs, transmitted - lastBoilerTransmit);
    lastBoilerTransmit = transmitted;
  }
}

void startTemperature() {
  TemperatureSensorConnect();
  opentherm.begin(handleInterrupt);
}

void checkTemperatureIfNeeded() {
  static unsigned long lastTempMeas = 0;
  static unsigned long lastReconnect = 0;
  unsigned long now = millis();
  // OneWire briefly masks interrupts; keep bus transfers outside OpenTherm reception.
  bool sensorBusAvailable = boilerStep == BoilerStep::Idle && opentherm.isReady();
  if (sensorBusAvailable && conversionPending && now - conversionStarted >= dallasSensor.millisToWaitForConversion(TEMPERATURE_PRECISION)) {
    conversionPending = false;
    TemperatureMeasure();
    lastTempMeas = millis();
    sendMessage(sndTemperature, cmdSilentUpdate, controllerQueue);
    now = millis();
  }
  if (sensorBusAvailable && !conversionPending && now - lastTempMeas >= TEMP_MEAS_INTERVAL) beginTemperatureConversion();
  if (sensorBusAvailable && !conversionPending && now - lastReconnect >= RECONNECT_INTERVAL) {
    lastReconnect = now;
    if (!controllerData.temperatureMeasurementOK) TemperatureSensorConnect();
  }
  BoilerCommunicate();
}
