#pragma once
#include <type_traits>

// Core 1 owns controllerData. Core 0 owns this separate presentation copy.
ControllerData_t networkData;
String networkLogReport;
String networkTimingReport;
uint32_t networkAppliedRevision = 0; // Worker-only.
struct NetworkState {
  decltype(ControllerData_t::boilerTemperatureValid) boilerTemperatureValid;
  decltype(ControllerData_t::hotWaterTemperatureValid) hotWaterTemperatureValid;
  decltype(ControllerData_t::boilerTemperatureAt) boilerTemperatureAt;
  decltype(ControllerData_t::hotWaterTemperatureAt) hotWaterTemperatureAt;

  decltype(ControllerData_t::actualBoilerTemperature) actualBoilerTemperature;
  decltype(ControllerData_t::actualDomesticHotWaterTemperature) actualDomesticHotWaterTemperature;
  decltype(ControllerData_t::boilerPercentage) boilerPercentage;
  decltype(ControllerData_t::boilerResponse) boilerResponse;
  decltype(ControllerData_t::centralHeatingActive) centralHeatingActive;
  decltype(ControllerData_t::dayTypes) dayTypes;
  decltype(ControllerData_t::enableCentralHeating) enableCentralHeating;
  decltype(ControllerData_t::enableHotWater) enableHotWater;
  decltype(ControllerData_t::highTemp) highTemp;
  decltype(ControllerData_t::hotWaterActive) hotWaterActive;
  decltype(ControllerData_t::lastBoilerUpdate) lastBoilerUpdate;
  decltype(ControllerData_t::lowTemp) lowTemp;
  decltype(ControllerData_t::multipleForever) multipleForever;
  decltype(ControllerData_t::overrideMultiple) overrideMultiple;
  decltype(ControllerData_t::overrideMultipleCount) overrideMultipleCount;
  decltype(ControllerData_t::overrideSetpoint) overrideSetpoint;
  decltype(ControllerData_t::overrideToday) overrideToday;
  decltype(ControllerData_t::overrideTomorrow) overrideTomorrow;
  decltype(ControllerData_t::reasonForSetpoint) reasonForSetpoint;
  decltype(ControllerData_t::regularWeek) regularWeek;
  decltype(ControllerData_t::requestedBoilerTemperature) requestedBoilerTemperature;
  decltype(ControllerData_t::requestedDomesticHotWaterSetpoint) requestedDomesticHotWaterSetpoint;
  decltype(ControllerData_t::sensorOffset) sensorOffset;
  decltype(ControllerData_t::temperatureMeasurementOK) temperatureMeasurementOK;
  decltype(ControllerData_t::temperatureSetpoint) temperatureSetpoint;
  decltype(ControllerData_t::weekendSleep) weekendSleep;
  decltype(ControllerData_t::weekendWakeUp) weekendWakeUp;
  decltype(ControllerData_t::workAtOfficeComeIn) workAtOfficeComeIn;
  decltype(ControllerData_t::workAtOfficeGoOut) workAtOfficeGoOut;
  decltype(ControllerData_t::workAtOfficeSleep) workAtOfficeSleep;
  decltype(ControllerData_t::workAtOfficeWakeUp) workAtOfficeWakeUp;
  decltype(ControllerData_t::workFromHomeSleep) workFromHomeSleep;
  decltype(ControllerData_t::workFromHomeWakeUp) workFromHomeWakeUp;
  decltype(ControllerData_t::rawTemperature) rawTemperature;
  uint32_t revision;
  char logReport[4096];
  char timingReport[2048];
};
struct NetworkPacket {
  NetworkState state;
};
struct NetworkReply {
  userEventMessage_t message;
  uint32_t revision;
};
struct NetworkStatus {
  bool connected;
  bool timeSynched;
  int timeZoneOffset;
  int timeDSToffset;
  unsigned long heartbeatMs;
  unsigned long maxOperationMs;
};
static_assert(std::is_trivially_copyable<NetworkPacket>::value, "Queue packets must own no dynamic memory");
static_assert(std::is_trivially_copyable<userEventMessage_t>::value, "Commands must be value copies");
QueueHandle_t networkSnapshots = nullptr;
QueueHandle_t networkReplies = nullptr;
QueueHandle_t networkStatusQueue = nullptr;
unsigned long droppedNetworkReplies = 0; // Core 1 only.
unsigned long networkMaxOperationMs = 0; // Core 1's received diagnostic.
unsigned long networkLastHeartbeatMs = 0;
unsigned long displayMaxStepUs = 0; // Core 1 only; exported as snapshot text.
unsigned long displayAllocationFailures = 0;
unsigned long displayMaxPixelTransferUs = 0;
unsigned long displayMaxFontLoadUs = 0;
unsigned long displayMaxTextDrawUs = 0;
unsigned long displayMaxSpritePushUs = 0;

void captureNetworkPacket(NetworkPacket &packet, bool reports) {
  static uint32_t revision = 0;
  memcpy(&packet.state.boilerTemperatureValid, &controllerData.boilerTemperatureValid, sizeof(packet.state.boilerTemperatureValid));
  memcpy(&packet.state.hotWaterTemperatureValid, &controllerData.hotWaterTemperatureValid, sizeof(packet.state.hotWaterTemperatureValid));
  memcpy(&packet.state.boilerTemperatureAt, &controllerData.boilerTemperatureAt, sizeof(packet.state.boilerTemperatureAt));
  memcpy(&packet.state.hotWaterTemperatureAt, &controllerData.hotWaterTemperatureAt, sizeof(packet.state.hotWaterTemperatureAt));
  memcpy(&packet.state.actualBoilerTemperature, &controllerData.actualBoilerTemperature, sizeof(packet.state.actualBoilerTemperature));
  memcpy(&packet.state.actualDomesticHotWaterTemperature, &controllerData.actualDomesticHotWaterTemperature, sizeof(packet.state.actualDomesticHotWaterTemperature));
  memcpy(&packet.state.boilerPercentage, &controllerData.boilerPercentage, sizeof(packet.state.boilerPercentage));
  memcpy(&packet.state.boilerResponse, &controllerData.boilerResponse, sizeof(packet.state.boilerResponse));
  memcpy(&packet.state.centralHeatingActive, &controllerData.centralHeatingActive, sizeof(packet.state.centralHeatingActive));
  memcpy(&packet.state.dayTypes, &controllerData.dayTypes, sizeof(packet.state.dayTypes));
  memcpy(&packet.state.enableCentralHeating, &controllerData.enableCentralHeating, sizeof(packet.state.enableCentralHeating));
  memcpy(&packet.state.enableHotWater, &controllerData.enableHotWater, sizeof(packet.state.enableHotWater));
  memcpy(&packet.state.highTemp, &controllerData.highTemp, sizeof(packet.state.highTemp));
  memcpy(&packet.state.hotWaterActive, &controllerData.hotWaterActive, sizeof(packet.state.hotWaterActive));
  memcpy(&packet.state.lastBoilerUpdate, &controllerData.lastBoilerUpdate, sizeof(packet.state.lastBoilerUpdate));
  memcpy(&packet.state.lowTemp, &controllerData.lowTemp, sizeof(packet.state.lowTemp));
  memcpy(&packet.state.multipleForever, &controllerData.multipleForever, sizeof(packet.state.multipleForever));
  memcpy(&packet.state.overrideMultiple, &controllerData.overrideMultiple, sizeof(packet.state.overrideMultiple));
  memcpy(&packet.state.overrideMultipleCount, &controllerData.overrideMultipleCount, sizeof(packet.state.overrideMultipleCount));
  memcpy(&packet.state.overrideSetpoint, &controllerData.overrideSetpoint, sizeof(packet.state.overrideSetpoint));
  memcpy(&packet.state.overrideToday, &controllerData.overrideToday, sizeof(packet.state.overrideToday));
  memcpy(&packet.state.overrideTomorrow, &controllerData.overrideTomorrow, sizeof(packet.state.overrideTomorrow));
  memcpy(&packet.state.reasonForSetpoint, &controllerData.reasonForSetpoint, sizeof(packet.state.reasonForSetpoint));
  memcpy(&packet.state.regularWeek, &controllerData.regularWeek, sizeof(packet.state.regularWeek));
  memcpy(&packet.state.requestedBoilerTemperature, &controllerData.requestedBoilerTemperature, sizeof(packet.state.requestedBoilerTemperature));
  memcpy(&packet.state.requestedDomesticHotWaterSetpoint, &controllerData.requestedDomesticHotWaterSetpoint, sizeof(packet.state.requestedDomesticHotWaterSetpoint));
  memcpy(&packet.state.sensorOffset, &controllerData.sensorOffset, sizeof(packet.state.sensorOffset));
  memcpy(&packet.state.temperatureMeasurementOK, &controllerData.temperatureMeasurementOK, sizeof(packet.state.temperatureMeasurementOK));
  memcpy(&packet.state.temperatureSetpoint, &controllerData.temperatureSetpoint, sizeof(packet.state.temperatureSetpoint));
  memcpy(&packet.state.weekendSleep, &controllerData.weekendSleep, sizeof(packet.state.weekendSleep));
  memcpy(&packet.state.weekendWakeUp, &controllerData.weekendWakeUp, sizeof(packet.state.weekendWakeUp));
  memcpy(&packet.state.workAtOfficeComeIn, &controllerData.workAtOfficeComeIn, sizeof(packet.state.workAtOfficeComeIn));
  memcpy(&packet.state.workAtOfficeGoOut, &controllerData.workAtOfficeGoOut, sizeof(packet.state.workAtOfficeGoOut));
  memcpy(&packet.state.workAtOfficeSleep, &controllerData.workAtOfficeSleep, sizeof(packet.state.workAtOfficeSleep));
  memcpy(&packet.state.workAtOfficeWakeUp, &controllerData.workAtOfficeWakeUp, sizeof(packet.state.workAtOfficeWakeUp));
  memcpy(&packet.state.workFromHomeSleep, &controllerData.workFromHomeSleep, sizeof(packet.state.workFromHomeSleep));
  memcpy(&packet.state.workFromHomeWakeUp, &controllerData.workFromHomeWakeUp, sizeof(packet.state.workFromHomeWakeUp));
  memcpy(&packet.state.rawTemperature, &controllerData.rawTemperature, sizeof(packet.state.rawTemperature));
  packet.state.revision = ++revision;
  if (reports) {
    controllerData.datalogger.str().toCharArray(packet.state.logReport, sizeof(packet.state.logReport));
    controllerData.logBusyTime.report().toCharArray(packet.state.timingReport, sizeof(packet.state.timingReport));
    size_t used = strlen(packet.state.timingReport);
    snprintf(packet.state.timingReport + used, sizeof(packet.state.timingReport) - used,
      "\nDisplay max step: %lu us; max pixel chunk: %lu us; allocation failures: %lu\nButton max poll gap: %lu us\nFont load max: %lu us; text draw max: %lu us; sprite transfer max: %lu us\n",
      displayMaxStepUs, displayMaxPixelTransferUs, displayAllocationFailures, keyboardMaxPollGapUs,
      displayMaxFontLoadUs, displayMaxTextDrawUs, displayMaxSpritePushUs);
  }
}

void publishNetworkSnapshot() {
  static unsigned long lastPublish = 0;
  if (!networkSnapshots || millis() - lastPublish < 1000) return;
  static NetworkPacket packet;
  captureNetworkPacket(packet, true);
  xQueueOverwrite(networkSnapshots, &packet);
  lastPublish = millis();
}

// Publish post-command state before the acknowledgement. A newer snapshot is also valid.
void enqueueNetworkReply(const userEventMessage_t &message) {
  static NetworkPacket packet;
  captureNetworkPacket(packet, true);
  if (!networkSnapshots || !networkReplies) { ++droppedNetworkReplies; return; }
  xQueueOverwrite(networkSnapshots, &packet);
  NetworkReply reply{message, packet.state.revision};
  if (xQueueSend(networkReplies, &reply, 0) != pdPASS) ++droppedNetworkReplies;
}

void applyNetworkPacket(const NetworkPacket &packet) {
  // Worker-only. Ignore stale periodic snapshots after a newer command reply.
  if (static_cast<int32_t>(packet.state.revision - networkAppliedRevision) <= 0) return;
  memcpy(&networkData.boilerTemperatureValid, &packet.state.boilerTemperatureValid, sizeof(packet.state.boilerTemperatureValid));
  memcpy(&networkData.hotWaterTemperatureValid, &packet.state.hotWaterTemperatureValid, sizeof(packet.state.hotWaterTemperatureValid));
  memcpy(&networkData.boilerTemperatureAt, &packet.state.boilerTemperatureAt, sizeof(packet.state.boilerTemperatureAt));
  memcpy(&networkData.hotWaterTemperatureAt, &packet.state.hotWaterTemperatureAt, sizeof(packet.state.hotWaterTemperatureAt));
  memcpy(&networkData.actualBoilerTemperature, &packet.state.actualBoilerTemperature, sizeof(packet.state.actualBoilerTemperature));
  memcpy(&networkData.actualDomesticHotWaterTemperature, &packet.state.actualDomesticHotWaterTemperature, sizeof(packet.state.actualDomesticHotWaterTemperature));
  memcpy(&networkData.boilerPercentage, &packet.state.boilerPercentage, sizeof(packet.state.boilerPercentage));
  memcpy(&networkData.boilerResponse, &packet.state.boilerResponse, sizeof(packet.state.boilerResponse));
  memcpy(&networkData.centralHeatingActive, &packet.state.centralHeatingActive, sizeof(packet.state.centralHeatingActive));
  memcpy(&networkData.dayTypes, &packet.state.dayTypes, sizeof(packet.state.dayTypes));
  memcpy(&networkData.enableCentralHeating, &packet.state.enableCentralHeating, sizeof(packet.state.enableCentralHeating));
  memcpy(&networkData.enableHotWater, &packet.state.enableHotWater, sizeof(packet.state.enableHotWater));
  memcpy(&networkData.highTemp, &packet.state.highTemp, sizeof(packet.state.highTemp));
  memcpy(&networkData.hotWaterActive, &packet.state.hotWaterActive, sizeof(packet.state.hotWaterActive));
  memcpy(&networkData.lastBoilerUpdate, &packet.state.lastBoilerUpdate, sizeof(packet.state.lastBoilerUpdate));
  memcpy(&networkData.lowTemp, &packet.state.lowTemp, sizeof(packet.state.lowTemp));
  memcpy(&networkData.multipleForever, &packet.state.multipleForever, sizeof(packet.state.multipleForever));
  memcpy(&networkData.overrideMultiple, &packet.state.overrideMultiple, sizeof(packet.state.overrideMultiple));
  memcpy(&networkData.overrideMultipleCount, &packet.state.overrideMultipleCount, sizeof(packet.state.overrideMultipleCount));
  memcpy(&networkData.overrideSetpoint, &packet.state.overrideSetpoint, sizeof(packet.state.overrideSetpoint));
  memcpy(&networkData.overrideToday, &packet.state.overrideToday, sizeof(packet.state.overrideToday));
  memcpy(&networkData.overrideTomorrow, &packet.state.overrideTomorrow, sizeof(packet.state.overrideTomorrow));
  memcpy(&networkData.reasonForSetpoint, &packet.state.reasonForSetpoint, sizeof(packet.state.reasonForSetpoint));
  memcpy(&networkData.regularWeek, &packet.state.regularWeek, sizeof(packet.state.regularWeek));
  memcpy(&networkData.requestedBoilerTemperature, &packet.state.requestedBoilerTemperature, sizeof(packet.state.requestedBoilerTemperature));
  memcpy(&networkData.requestedDomesticHotWaterSetpoint, &packet.state.requestedDomesticHotWaterSetpoint, sizeof(packet.state.requestedDomesticHotWaterSetpoint));
  memcpy(&networkData.sensorOffset, &packet.state.sensorOffset, sizeof(packet.state.sensorOffset));
  memcpy(&networkData.temperatureMeasurementOK, &packet.state.temperatureMeasurementOK, sizeof(packet.state.temperatureMeasurementOK));
  memcpy(&networkData.temperatureSetpoint, &packet.state.temperatureSetpoint, sizeof(packet.state.temperatureSetpoint));
  memcpy(&networkData.weekendSleep, &packet.state.weekendSleep, sizeof(packet.state.weekendSleep));
  memcpy(&networkData.weekendWakeUp, &packet.state.weekendWakeUp, sizeof(packet.state.weekendWakeUp));
  memcpy(&networkData.workAtOfficeComeIn, &packet.state.workAtOfficeComeIn, sizeof(packet.state.workAtOfficeComeIn));
  memcpy(&networkData.workAtOfficeGoOut, &packet.state.workAtOfficeGoOut, sizeof(packet.state.workAtOfficeGoOut));
  memcpy(&networkData.workAtOfficeSleep, &packet.state.workAtOfficeSleep, sizeof(packet.state.workAtOfficeSleep));
  memcpy(&networkData.workAtOfficeWakeUp, &packet.state.workAtOfficeWakeUp, sizeof(packet.state.workAtOfficeWakeUp));
  memcpy(&networkData.workFromHomeSleep, &packet.state.workFromHomeSleep, sizeof(packet.state.workFromHomeSleep));
  memcpy(&networkData.workFromHomeWakeUp, &packet.state.workFromHomeWakeUp, sizeof(packet.state.workFromHomeWakeUp));
  memcpy(&networkData.rawTemperature, &packet.state.rawTemperature, sizeof(packet.state.rawTemperature));
  networkLogReport = packet.state.logReport;
  networkTimingReport = packet.state.timingReport;
  networkAppliedRevision = packet.state.revision;
}
