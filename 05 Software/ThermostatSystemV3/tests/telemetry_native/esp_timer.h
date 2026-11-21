#pragma once
extern int64_t fakeUs;
inline int64_t esp_timer_get_time() {return fakeUs;}
