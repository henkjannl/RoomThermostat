#pragma once

void syncTimeIfNeeded(Data & data) {
  // To do: import summertime from internet
  static int resyncTimeInterval = 15*1000; // Until synched, attempt to resync time every 15 seconds
  static unsigned long previousAttempt = 0;

  if (millis() - previousAttempt > resyncTimeInterval){
    previousAttempt=millis();    
    Serial.println("Trying to sync time");
    const char* ntpServer = "pool.ntp.org";
    configTime(data.timeZoneOffset, data.timeDSToffset, ntpServer);

    time_t now;
    time(&now); // Get current time
    struct tm * local;
    local = localtime(&now);
    
    data.timeSynched=(local->tm_year > 70);
    Serial.printf("Year: %d Synched: %s\n", local->tm_year, data.timeSynched ? "Y" : "N");

    if (data.timeSynched) {
      Serial.println("Time synched");
      resyncTimeInterval = 24*60*1000; // If synched, increase resync interval
    }
  }
}
