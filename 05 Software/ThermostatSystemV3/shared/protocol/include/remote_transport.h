#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <atomic>
#include <mbedtls/md.h>
#include "remote_protocol.h"
#include "pairing_secret.h"

namespace RemoteTransport {
using namespace RemoteProtocol;
struct Incoming { uint8_t mac[6]; Packet packet; };
static QueueHandle_t inbox = nullptr;
static std::atomic<bool> ready{false}, sending{false};
static uint8_t ownMac[6];
static uint8_t peerMac[6] = {};
static bool hasPeer = false;

inline bool same(const uint8_t *a, const uint8_t *b) { return memcmp(a,b,6)==0; }
inline bool authenticate(Packet &packet, bool verify) {
  uint8_t digest[32];
  if (mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), PAIR_AUTH_KEY,
      sizeof(PAIR_AUTH_KEY), reinterpret_cast<const uint8_t*>(&packet), offsetof(Packet,tag), digest)) return false;
  if (!verify) { memcpy(packet.tag,digest,16); return true; }
  uint8_t difference=0;
  for (int i=0;i<16;++i) difference |= digest[i]^packet.tag[i];
  return difference==0;
}
inline void received(const uint8_t *mac, const uint8_t *data, int length) {
  if (!inbox || length != sizeof(Packet)) return;
  Incoming item;
  memcpy(item.mac,mac,6); memcpy(&item.packet,data,sizeof(Packet));
  xQueueSend(inbox,&item,0); // Never block the Wi-Fi task.
}
inline void sent(const uint8_t *, esp_now_send_status_t) { sending.store(false); }
inline bool begin() {
  inbox=xQueueCreate(6,sizeof(Incoming));
  if (!inbox) return false;
  if (esp_wifi_get_mac(WIFI_IF_STA,ownMac)!=ESP_OK || esp_now_init()!=ESP_OK) return false;
  if (esp_now_set_pmk(PAIR_PMK)!=ESP_OK) return false;
  esp_now_peer_info_t broadcast{};
  memcpy(broadcast.peer_addr,BROADCAST,6); broadcast.ifidx=WIFI_IF_STA;
  if (esp_now_add_peer(&broadcast)!=ESP_OK) return false;
  esp_now_register_recv_cb(received); esp_now_register_send_cb(sent);
  ready.store(true);
  return true;
}
inline bool setPeer(const uint8_t *mac) {
  if (hasPeer && same(mac,peerMac)) return true;
  if (sending.load()) return false;
  if (hasPeer) esp_now_del_peer(peerMac);
  hasPeer=false;
  esp_now_peer_info_t peer{};
  memcpy(peer.peer_addr,mac,6); memcpy(peer.lmk,PAIR_LMK,16);
  peer.ifidx=WIFI_IF_STA; peer.channel=0; peer.encrypt=true;
  if (esp_now_add_peer(&peer)!=ESP_OK) return false;
  memcpy(peerMac,mac,6); hasPeer=true; return true;
}
inline uint8_t channel() {
  uint8_t primary=0; wifi_second_chan_t secondary;
  esp_wifi_get_channel(&primary,&secondary); return primary;
}
inline bool send(Packet packet, bool discovery=false) {
  if (!ready.load() || sending.exchange(true)) return false;
  memcpy(packet.source,ownMac,6); packet.channel=channel();
  if (!authenticate(packet,false)) { sending.store(false); return false; }
  const uint8_t *destination = discovery ? BROADCAST : packet.target;
  if (esp_now_send(destination,reinterpret_cast<uint8_t*>(&packet),sizeof(packet))!=ESP_OK) {
    sending.store(false); return false;
  }
  return true;
}
inline bool poll(Incoming &item) {
  if (!ready.load() || xQueueReceive(inbox,&item,0)!=pdPASS) return false;
  Packet &p=item.packet;
  if (p.magic!=MAGIC || p.version!=WIRE_VERSION || !same(p.source,item.mac) ||
      (!same(p.target,ownMac) && !same(p.target,BROADCAST)) || !authenticate(p,true)) {
    p.kind=0;
  }
  return true;
}
inline void end() {
  ready.store(false);
  esp_now_unregister_recv_cb(); esp_now_unregister_send_cb(); esp_now_deinit();
  // Queue retained until deep sleep: avoid deleting it while a callback finishes.
}
}
