#pragma once

#include <map>
#include <list>

using namespace std;

/* Rewrite the function like:
#include <iostream>
#include <cstdio>
#include <cstdarg>

void myPrintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int main() {
    myPrintf("Hello, %s!\n", "world");
    return 0;
}
*/
void logMessage(String message) {
  Serial.println(message);
}

// Busy time bins
enum busyTime_t { btTotal, btTemperature, btMenu, btDisplay, btTelegram, btController, btBacklight, btWifi, btTimeSynch, btOpentherm, btBoiler};

typedef std::map< busyTime_t, String > busyTimeLabel_t;

busyTimeLabel_t BUSY_TIME_LABEL = {
  { btTotal,       "Total" }, 
  { btTemperature, "Temperature" }, 
  { btMenu       , "Menu"        },
  { btDisplay    , "Display"     },
  { btTelegram   , "Telegram"    },
  { btController , "Controller"  },
  { btBacklight  , "Backlight"   },
  { btWifi       , "WiFi"        },
  { btTimeSynch  , "Time"        },
//  { btOpentherm  , "Opentherm"   },
  { btBoiler     , "Boiler"      }
};

class BusyTime {
  public:
    void start();
    void finish();
    float average();
    float stdev();
    float peakDuration() { return peak; };
    bool hasMeasurements() { return (N>0); };
    String frequency();

  protected:
    bool hasStarted = false;
    unsigned long firstStart;
    unsigned long lastStarted;
  
    // stdev=sqrt( (sum{X²-2*X*µ+µ²}=sqrt( (sum(X²)-sum(X)²/N)) / (N-1) )
    float N=0; 
    float sumX=0;
    float sumX2=0;
    float peak = 0;
};

void BusyTime::start() { 
  lastStarted=millis(); 
  if( !hasStarted ) {
    firstStart=lastStarted;
    hasStarted=true;
  }
};

void BusyTime::finish() {
  float duration = millis()-lastStarted;
  N     +=    1;
  sumX  +=    duration;
  sumX2 += sq(duration);
  if( peak<duration ) peak = duration;
};

float BusyTime::average() {
  return (N>0) ? (float) sumX/N : 0;
}

float BusyTime::stdev() {
  if(N==0) return 0;
  float numerator = sumX2 - (sq(sumX)/N);
  return sqrt( numerator/(N-1) );
}

String BusyTime::frequency() {
  String result = "-";
  if(N>1) {
    result = String( (N-1) / (lastStarted - firstStart) , 1 ) + " kHz";
  }
  return result;
};

class LogBusyTime {    
  public:
    void start (busyTime_t process) { processes[process].start();  };
    void finish(busyTime_t process) { processes[process].finish(); };
    String report();
  protected:
    std::map<busyTime_t, BusyTime> processes;  
};

String LogBusyTime::report() {
  String result;
  char buffer[70];

  for( auto & process : processes) {
    if( process.second.hasMeasurements() ) {
      snprintf(buffer, 70, "%s: %.1f ± %.1f ms f=%s max=%.1f ms\n", BUSY_TIME_LABEL[process.first].c_str(), process.second.average(), process.second.stdev(), process.second.frequency(), process.second.peakDuration() );
      result+= String(buffer);    
    }
  }

  return result;
};

class DataLogger_t {
  public:
    void header(String & header) { head = header; };
    void log(String & logdata);    
    String str();

    String head;
    std::list<String> records;
};

void DataLogger_t::log(String & logdata) {
  records.push_back(logdata);
  while( records.size() > 24 ) records.pop_front();
};

String DataLogger_t::str() {
  String result = "*" + head + "*\n";
  for (auto & record : records) result+= record + "\n";
  return result;
}
