#pragma once
#include <atomic>

enum class NetworkPhase:uint8_t {Startup,Snapshot,Reply,Reconnect,Clock,Timezone,TelegramStart,TelegramPoll,Idle};
std::atomic<uint8_t> networkPhase{uint8_t(NetworkPhase::Startup)};
std::atomic<uint32_t> networkPhaseStartedMs{0},telegramPollStartedMs{0},telegramPollFinishedMs{0},telegramPollCount{0};
std::atomic<bool> telegramPollingPaused{false};
inline void markNetworkPhase(NetworkPhase phase){networkPhaseStartedMs.store(millis());networkPhase.store(uint8_t(phase));}
inline const char*networkPhaseName(uint8_t phase){
  const char*names[]={"startup","snapshot","telegram-reply","wifi-reconnect","clock","timezone","telegram-start","telegram-poll","idle"};
  return phase<sizeof(names)/sizeof(names[0])?names[phase]:"unknown";
}
