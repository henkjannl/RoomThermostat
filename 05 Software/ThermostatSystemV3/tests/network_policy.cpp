#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "network_policy.h"
// Same units/conversion as the pinned Arduino-ESP32 2.0.17 implementation.
struct Client {
  uint32_t socketMs=0,handshakeMs=0;
  void setTimeout(uint32_t seconds){socketMs=seconds*1000;}
  void setHandshakeTimeout(uint32_t seconds){handshakeMs=seconds*1000;}
};
int main(){
  Client client;NetworkPolicy::configureTelegramClient(client);
  assert(client.socketMs==3000);assert(client.handshakeMs==5000);
  assert(NetworkPolicy::TelegramResponseWaitMs==1500);
  assert(client.socketMs!=3000000);
  puts("PASS: actual Telegram client configuration gives a 3-second socket timeout, 5-second handshake timeout and 1500-ms response wait.");
}
