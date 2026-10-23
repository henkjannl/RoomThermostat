// OpenTherm signal, through a 330 ohm and 2k2 voltage divider
// 2k2 between OpenTherm and sensor pin
// 330 ohm between sensor pin and ground
// other OpenTherm signal to ground
// This way, the max 18V signal is converted to 2.4V
int SENSOR_PIN = 35;    

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

const int NUM_SAMPLES=3*32;
const long THRESHOLD = 0;
volatile int samples[NUM_SAMPLES];
volatile int lastGoodSamples[NUM_SAMPLES];
volatile byte sampleID = 0;
volatile long sampleSum = 0;
volatile long lastGoodSum = 0;

void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  volatile bool setSema=false;
  portENTER_CRITICAL_ISR(&timerMux);
  sampleID++;
  if(sampleID==NUM_SAMPLES) {
    if(sampleSum>THRESHOLD) {
      for(sampleID=0; sampleID<NUM_SAMPLES; sampleID++) lastGoodSamples[sampleID]=samples[sampleID];
      lastGoodSum=sampleSum;
      setSema=true;
    }
    sampleID=0;
    sampleSum=0;
    
  }
  samples[sampleID] = analogRead(SENSOR_PIN);
  sampleSum+=samples[sampleID];

  portEXIT_CRITICAL_ISR(&timerMux);
  if(setSema) xSemaphoreGiveFromISR(timerSemaphore, NULL);

}

void setup() {
  Serial.begin(115200);

  delay(500);
  while(true) {
  //for(int i=0; i<100; i++) {
    Serial.printf("%d %d %d %d %d\n",analogRead(35), analogRead(32), analogRead(34), analogRead(36), analogRead(39) );
    delay(50);
  }

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 300, true);

  timerAlarmEnable(timer);
}

void loop() {
  
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    int samples[NUM_SAMPLES];
    portENTER_CRITICAL(&timerMux);
    for(int i=0; i<NUM_SAMPLES; i++) samples[i]=lastGoodSamples[i];
    portEXIT_CRITICAL(&timerMux);
    for(int i=0; i<NUM_SAMPLES; i++) Serial.printf("%d\n",samples[i]);
    Serial.println(0);
    Serial.println(lastGoodSum);
    Serial.println(0);
  }
  delay(10);
}
