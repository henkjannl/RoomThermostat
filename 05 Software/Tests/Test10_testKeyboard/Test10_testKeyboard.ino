#include "a_Constants.h"
#include "b_Buttons.h"
 
void setup() {
  Serial.begin(115200);
  startKeyboard();
}

void loop() {
  
  if(btnUpTouched) {
    Serial.println("UP");
    btnUpTouched=false;
  };

  if(btnSelectTouched) {
    Serial.println("SELECT");
    btnSelectTouched=false;
  }

  if(btnDownTouched) {
    Serial.println("DOWN");
    btnDownTouched=false;
  }

  volatile unsigned long tstart;
  tstart=micros();
  
  int b1 = touchRead(PIN_BUT_UP    );
  int b2 = touchRead(PIN_BUT_SELECT); 
  int b3 = touchRead(PIN_BUT_DOWN  ); 

  volatile unsigned long tstop;
  tstop=micros();

  Serial.printf("%04d %04d %04d %04d\n", b1, b2, b3, tstop-tstart);
  delay(500);
}
