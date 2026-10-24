"""Source-boundary regression checks; these do not simulate ESP32 hardware."""
from pathlib import Path
import re
import unittest

SRC = Path(__file__).resolve().parents[1] / "thermostat" / "src"


def source(name):
    text = (SRC / name).read_text()
    return re.sub(r"//[^\n]*|/\*.*?\*/", "", text, flags=re.S)


class OwnershipChecks(unittest.TestCase):
    def test_control_task_has_no_network_operations(self):
        main = source("main.cpp")
        self.assertNotRegex(main, r"\b(?:setupWifi|startTelegram|checkTelegramIfNeeded|requestTimeZoneIfNeeded|syncTimeIfNeeded|reconnectWiFiIfNeeded|wifiConnected)\s*\(")
        self.assertNotRegex(main, r"\b(?:WiFi|bot|securedClient)\s*\.")

    def test_worker_never_accesses_control_or_hardware(self):
        worker = source("m_network.h").split("void networkWorker(void *)", 1)[1].split("bool startNetworkWorker", 1)[0]
        for name in ("f_telegram.h", "i_wifi_time.h"):
            worker += source(name)
        self.assertNotRegex(worker, r"\b(?:controllerData|opentherm|dallasSensor|tft|JpegDec|keyboardEnabed)\b")
        self.assertNotRegex(worker, r"\b(?:disableKeyboard|enableKeyboard|touchRead|saveSettingsNVS)\s*\(")

    def test_telegram_fields_are_copied_into_snapshots(self):
        telegram = source("f_telegram.h")
        used = set(re.findall(r"networkData(?:->|\.)(\w+)", telegram))
        used -= {"botChatID", "botToken", "measuredTemperature", "timezone"}  # Startup credentials/method and worker-owned timezone.
        copied = set(re.findall(r"memcpy\(&packet.state\.(\w+)", source("network_messages.h")))
        self.assertFalse(used - copied, f"Missing snapshot fields: {used - copied}")

    def test_core_priority_and_nonblocking_handoff(self):
        worker = source("m_network.h")
        self.assertRegex(worker, r'xTaskCreatePinnedToCore\(networkWorker, "network", 12288, nullptr, 1, &networkTaskHandle, 0\)')
        bridge = source("network_messages.h")
        self.assertIn("xQueueSend(networkReplies, &reply, 0)", bridge)
        self.assertIn("std::is_trivially_copyable<NetworkPacket>", bridge)
        self.assertIn("std::is_trivially_copyable<userEventMessage_t>", bridge)

    def test_flash_save_and_boiler_interval(self):
        self.assertIn("savePending && boilerStep == BoilerStep::Idle && opentherm.isReady()", source("main.cpp"))
        self.assertRegex(source("h_temperature.h"), r"#define BOILER_INTERVAL\s+5\*1000")
        self.assertNotRegex(source("h_temperature.h"), r"opentherm\.(?:sendRequest|setBoilerStatus|setBoilerTemperature|getBoilerTemperature|getDHWTemperature)\(")

    def test_reentrant_time_conversion(self):
        for header in SRC.glob("*.h"):
            self.assertNotRegex(source(header.name), r"\blocaltime\s*\(", header.name)

    def test_wifi_persistence_precedes_initialization(self):
        wifi = source("i_wifi_time.h")
        self.assertLess(wifi.index("WiFi.persistent(false)"), wifi.index("WiFi.mode(WIFI_STA)"))

    def test_telegram_authorization_and_startup(self):
        telegram = source("f_telegram.h").split("void handleNewMessages", 1)[1]
        self.assertLess(telegram.index("chatID != networkData.botChatID"), telegram.index("userConversation.find"))
        worker = source("m_network.h")
        self.assertLess(worker.index("syncTimeIfNeeded(networkData)"), worker.index("telegramStarted = startTelegram()"))
        self.assertIn("if (networkData.timeSynched", worker)
        self.assertIn("if (!bot.setMyCommands(commands)) return false", source("f_telegram.h"))

    def test_clock_initialization_preserves_overrides(self):
        controller = source("g_controller.h")
        self.assertIn("if (!controllerData.timeSynched", controller)
        self.assertIn("bool newDay = continuous && currDay != prevDay", controller)
        self.assertIn("if (!continuous) prevTime = currTime", controller)
        self.assertNotIn("if(currDay != prevDay)", controller)

    def test_valid_boiler_reply_diagnostics(self):
        boiler = source("h_temperature.h")
        self.assertIn("opentherm.getMessageType(response) == expectedType", boiler)
        self.assertIn("boilerMaxValidStatusGapMs", boiler)

    def test_remote_control_remains_on_control_task(self):
        self.assertIn("serviceRemoteLink()", source("main.cpp"))
        self.assertNotIn("serviceRemoteLink", source("m_network.h"))
        link=source("remote_link.h")
        self.assertIn("guard.inspect(request,boot,lease,millis()-request.leaseTime,revision)",link)
        self.assertIn("guard.remember(request,reply)",link)
        self.assertIn("applyRemoteSettings(draft)",link)
        self.assertIn("request.target,RemoteTransport::ownMac",link)
        transport=(SRC.parents[1]/'shared/protocol/include/remote_transport.h').read_text()
        callback=transport.split('inline void received(',1)[1].split('inline void sent(',1)[0]
        self.assertIn('xQueueSend(inbox,&item,0)',callback)
        self.assertNotRegex(callback,r'\b(?:authenticate|apply|paint|saveSettingsNVS)\s*\(')

    def test_period_expires_at_inclusive_end(self):
        controller=source('g_controller.h')
        self.assertIn('controllerData.overrideMultipleCount > 1',controller)
        self.assertIn('controllerData.overrideMultiple = dtAuto',controller)

    def test_telegram_timeout_units_and_diagnostics(self):
        telegram=source('f_telegram.h')
        self.assertNotIn('securedClient.setTimeout(3000)',telegram)
        self.assertIn('NetworkPolicy::configureTelegramClient(securedClient)',telegram)
        self.assertIn('bot.waitForResponse=NetworkPolicy::TelegramResponseWaitMs',telegram)
        self.assertIn('telegramPollFinishedMs.store(millis())',telegram)
        worker=source('m_network.h')
        self.assertIn('markNetworkPhase(NetworkPhase::TelegramPoll)',worker)
        self.assertIn('Network slow/stalled',worker)
        self.assertIn('serviceReceiverWifi()',worker)
        self.assertIn('esp_wifi_get_ps(&powerSave)',source('i_wifi_time.h'))

    def test_remote_flame_uses_thermostat_display_selection(self):
        link=source('remote_link.h')
        self.assertIn('int(controllerData.dspFlameIcon)-int(iconFlame0)',link)
        self.assertIn('reply.flameLevel=',link)


if __name__ == "__main__":
    unittest.main(verbosity=2)
