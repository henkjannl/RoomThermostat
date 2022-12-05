#pragma once

void logMessage(String message) {
  Serial.println(message);
}

// Busy time bins
enum busyTime_t { btTemperature, btWifi, btTimeSynch, btDisplay, btBacklight, btOpentherm, btBoiler, btController, btTelegram };

typedef std::map<busyTime_t, String > busyTimeLabel_t;

busyTimeLabel_t BUSY_TIME_LABEL = {
  { btTemperature, "Temperature" }, 
  { btDisplay    , "Display"     },
  { btWifi       , "WiFi"        },
  { btTimeSynch  , "Time"        },
  { btBacklight  , "Backlight"   },
  { btOpentherm  , "Opentherm"   },
  { btBoiler     , "Boiler"      },
  { btController , "Controller"  },
  { btTelegram   , "Telegram"    }
};

class BusyTime {
  public:
    void start() { lastStarted=millis(); };
    void finish();
    float average();
    float stdev();

  protected:
    unsigned long lastStarted;
  
    // stdev=sqrt( (sum{X²-2*X*µ+µ²}=sqrt( (sum(X²)-sum(X)²/N)) / (N-1) )
    float N=0; 
    float sumX=0;
    float sumX2=0;
};

void BusyTime::finish() {
  float duration = millis()-lastStarted;
  N+=1;
  sumX+= duration;
  sumX2+= sq(duration);
};

float BusyTime::average() {
  return (N>0) ? (float) sumX/N : 0;
}

float BusyTime::stdev() {
  float numerator = sumX2 - (sq(sumX)/N);
  return ( N>0 ) ? sqrt( numerator/(N-1) ) : 0;
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
    snprintf(buffer, 40, "%s: %.1f ± %.1f ms\n", BUSY_TIME_LABEL[process.first].c_str(), process.second.average(), process.second.stdev());
    result+= String(buffer);    
  }

  return result;
};
