#pragma once

static_assert(CONFIG_ARDUINO_RUNNING_CORE == 1, "Control/UI must remain on core 1");

TaskHandle_t networkTaskHandle = nullptr;

void networkWorker(void *) {
  Serial.printf("Network worker core=%d priority=%u\n", xPortGetCoreID(), (unsigned)uxTaskPriorityGet(nullptr));
  setupWifi(networkData);
  // This receiver must hear the battery remote's short discovery bursts even
  // while associated with the access point. Leave deep sleep on the remote.
  if (!WiFi.setSleep(false)) Serial.println("ESP-NOW warning: could not disable Wi-Fi modem sleep");
  else Serial.println("ESP-NOW receiver: Wi-Fi modem sleep disabled");
  if (!RemoteTransport::begin()) Serial.println("ESP-NOW unavailable; thermostat remains autonomous");
  else Serial.println("ESP-NOW v2 control service ready; follows the Wi-Fi channel");
  bool telegramStarted = false;
  unsigned long lastTelegramAttempt = millis() - 30000UL;
  static NetworkPacket packet; // Avoid large snapshot buffers on task stack.
  unsigned long maxOperation = 0;
  for (;;) {
    unsigned long started = millis();
    if (xQueueReceive(networkSnapshots, &packet, 0) == pdPASS) applyNetworkPacket(packet);
    // Keep acknowledgement ordering; never answer using state older than its command.
    NetworkReply reply;
    if (telegramStarted && wifiConnected() && xQueuePeek(networkReplies, &reply, 0) == pdPASS) {
      if (xQueueReceive(networkSnapshots, &packet, 0) == pdPASS) applyNetworkPacket(packet);
      if (static_cast<int32_t>(networkAppliedRevision - reply.revision) >= 0) {
        xQueueReceive(networkReplies, &reply, 0);
        String chatID(reply.message.chatID);
        auto it = userConversation.find(chatID);
        if (it != userConversation.end()) it->second.respondToUser(bot, reply.message);
      }
    }
    reconnectWiFiIfNeeded(networkData);
    syncTimeIfNeeded(networkData); // Also refresh local DST status while offline.
    if (wifiConnected()) {
      requestTimeZoneIfNeeded(networkData);
      if (networkData.timeSynched && networkData.botChatID.length() != 0) {
        if (!telegramStarted && millis() - lastTelegramAttempt >= 30000UL) {
          lastTelegramAttempt = millis();
          telegramStarted = startTelegram();
        }
        if (telegramStarted) checkTelegramIfNeeded();
      }
    }
    maxOperation = max(maxOperation, millis() - started);
    NetworkStatus status{wifiConnected(), networkData.timeSynched, networkData.timeZoneOffset,
                         networkData.timeDSToffset, millis(), maxOperation};
    xQueueOverwrite(networkStatusQueue, &status);
    vTaskDelay(pdMS_TO_TICKS(20)); // Never busy-spin or starve core 0 system tasks.
  }
}

bool startNetworkWorker() {
  // Called before worker creation: this is the only transfer of owning String/container objects.
  networkData = controllerData;
  networkSnapshots = xQueueCreate(1, sizeof(NetworkPacket));
  networkReplies = xQueueCreate(20, sizeof(NetworkReply));
  networkStatusQueue = xQueueCreate(1, sizeof(NetworkStatus));
  if (!networkSnapshots || !networkReplies || !networkStatusQueue) {
    Serial.println("Network queues unavailable; local control remains active");
    return false;
  }
  // 2.1.1 measured 5,416 bytes peak usage; retain 6,872 bytes observed margin.
  BaseType_t result = xTaskCreatePinnedToCore(networkWorker, "network", 12288, nullptr, 1, &networkTaskHandle, 0);
  if (result != pdPASS) Serial.println("Network task unavailable; local control remains active");
  return result == pdPASS;
}

void serviceNetworkMessages() {
  NetworkStatus status;
  if (networkStatusQueue && xQueueReceive(networkStatusQueue, &status, 0) == pdPASS) {
    icon_t icon = status.connected ? iconWifiConnected : iconWifiLost;
    bool changed = controllerData.dspWifiIcon != icon || controllerData.timeSynched != status.timeSynched ||
                   controllerData.timeZoneOffset != status.timeZoneOffset || controllerData.timeDSToffset != status.timeDSToffset;
    controllerData.dspWifiIcon = icon;
    controllerData.timeSynched = status.timeSynched;
    controllerData.timeZoneOffset = status.timeZoneOffset;
    controllerData.timeDSToffset = status.timeDSToffset;
    networkMaxOperationMs = status.maxOperationMs;
    networkLastHeartbeatMs = status.heartbeatMs;
    if (changed) sendMessage(sndWifi, cmdSilentUpdate, controllerQueue);
  }
  publishNetworkSnapshot();
}
