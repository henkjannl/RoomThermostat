#pragma once

#include "FS.h"
#include "SPIFFS.h"

#include "b_Buttons.h"


String currentDate() {
  time_t now;
  struct tm * local;
  char buffer[20];

  time(&now);
  local = localtime(&now);
  snprintf(buffer, 20, "%04d-%02d-%02d", local->tm_year+1900, local->tm_mon+1, local->tm_mday);
  return String(buffer);
};

String currentTime() {
  time_t now;
  struct tm * local;
  char buffer[20];

  time(&now);
  local = localtime(&now);
  snprintf(buffer, 20, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
  return String(buffer);
};


void newEventLog() {
  File file;
  
  disableKeyboard();

  file = SPIFFS.open(EVENT_LOG, "w");
  file.print("Date\t");
  file.print("Time\t");
  file.print("Message\t");
  file.print("\n");
  file.close();

  enableKeyboard();
};

void deleteEventLog(){
  disableKeyboard();

  Serial.printf("Deleting file: %s\r\n", EVENT_LOG);
  Serial.println(SPIFFS.remove(EVENT_LOG) ? "- file deleted" : "- delete failed");

  newEventLog();

  enableKeyboard();
};

void logMessage(String message) {
  File file;

  disableKeyboard();

  file = SPIFFS.open(EVENT_LOG, "a");
  file.print(currentDate()); file.print("\t");
  file.print(currentTime()); file.print("\t");
  file.println(message);
  file.close();

  enableKeyboard();
};
