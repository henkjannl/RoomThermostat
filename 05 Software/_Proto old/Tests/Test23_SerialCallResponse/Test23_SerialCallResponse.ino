#include "FS.h"
#include "SPIFFS.h"

void setup() {
  // start serial port at 9600 bps:
  Serial.begin(115200);
  SPIFFS.begin();
  delay(200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() {
  static int inByte = 0;         // incoming serial byte
  
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // read first analog input, divide by 4 to make the range 0-255:

    File file = SPIFFS.open("/logfile.log");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("******************************************");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
    Serial.println("******************************************");

    // Clear serial buffer
    if (Serial.available() > 0) {
      // get incoming byte:
      inByte = Serial.read();
    }
  }
}
