class MilliSecTimer {
public:
    // Constructor
    MilliSecTimer(unsigned long intervalMs, bool autoReset = true)
        : interval(intervalMs), autoReset(autoReset), previous(millis()) {}

    // Start over
    void reset() {
        previous = millis();
    }

    // Check if time has lapsed; optionally auto-reset
    bool lapsed() {
        unsigned long now = millis();
        if (now - previous >= interval) {
            if (autoReset) {
                // Align next tick to exact multiple
                previous += interval;
                // Handles millis() rollover correctly
                if (now - previous >= interval) {
                    // If drifted multiple intervals, skip ahead
                    previous = now;
                }
            }
            return true;
        }
        return false;
    }

    // Check how much time has passed since last reset
    unsigned long elapsed() const {
        return millis() - previous;
    }

    // Remaining time until lapse (0 if already lapsed)
    unsigned long remaining() const {
        long remaining = interval - (millis() - previous);
        return remaining > 0 ? remaining : 0;
    }

private:
    unsigned long previous;
    unsigned long interval;
    bool autoReset;
};
