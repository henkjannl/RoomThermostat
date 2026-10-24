#pragma once
#include <stdint.h>

namespace NetworkPolicy {
// Arduino-ESP32 2.0.17 WiFiClientSecure::setTimeout takes SECONDS.
// HTTPClient::setTimeout and UniversalTelegramBot::waitForResponse take ms.
constexpr uint32_t TlsSocketTimeoutSeconds=3;
constexpr uint32_t TlsHandshakeTimeoutSeconds=5;
constexpr uint32_t TelegramResponseWaitMs=1500;
template<class Client> void configureTelegramClient(Client&client){
  client.setTimeout(TlsSocketTimeoutSeconds);
  client.setHandshakeTimeout(TlsHandshakeTimeoutSeconds);
}
}
