#pragma once

#include <map>

using namespace std;

void logMessage(String message) {
  Serial.println(message);
}

// Busy time bins
enum busyTime_t { btTemperature, btMenu, btDisplay, btTelegram, btController, btBacklight, btWifi, btTimeSynch, btOpentherm, btBoiler};

typedef std::map<busyTime_t, String > busyTimeLabel_t;

busyTimeLabel_t BUSY_TIME_LABEL = {
  { btTemperature, "Temperature" }, 
  { btMenu       , "Menu"        },
  { btDisplay    , "Display"     },
  { btTelegram   , "Telegram"    },
  { btController , "Controller"  },
  { btBacklight  , "Backlight"   },
  { btWifi       , "WiFi"        },
  { btTimeSynch  , "Time"        },
//  { btOpentherm  , "Opentherm"   },
//  { btBoiler     , "Boiler"      }
};

class BusyTime {
  public:
    void start() { lastStarted=millis(); };
    void finish();
    float average();
    float stdev();
    float peakDuration() { return peak; };
    bool hasMeasurements() { return (N>0); };

  protected:
    unsigned long lastStarted;
  
    // stdev=sqrt( (sum{X²-2*X*µ+µ²}=sqrt( (sum(X²)-sum(X)²/N)) / (N-1) )
    float N=0; 
    float sumX=0;
    float sumX2=0;
    float peak = 0;
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
  char buffer[40];

  for( auto & process : processes) {
    if( process.second.hasMeasurements() ) {
      snprintf(buffer, 40, "%s: %.1f ± %.1f ms max=%.1f ms\n", BUSY_TIME_LABEL[process.first].c_str(), process.second.average(), process.second.stdev(), process.second.peakDuration() );
      result+= String(buffer);    
    }
  }

  return result;
};
