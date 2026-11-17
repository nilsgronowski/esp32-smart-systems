#include <Arduino.h>
#include <esp32-hal-log.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_mac.h>

constexpr uint8_t broadcast_address[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
constexpr unsigned long send_interval_ms = 1000;

struct __attribute__((packed)) broadcast_message
{
  uint32_t sequence;
  char text[32];
} message = {};

void on_data_receive(const uint8_t *sender_address, const uint8_t *data, int length)
{
  if (length != static_cast<int>(sizeof(broadcast_message)))
  {
    Serial.printf("ERR: Received data length mismatch. Expected %zu, got %d\n",
                  sizeof(broadcast_message),
                  length);
    return;
  }

  const broadcast_message &received_message = *reinterpret_cast<const broadcast_message *>(data);
  // memcpy(&received_message, data, sizeof(received_message));
  // received_message.text[sizeof(received_message.text) - 1] = '\0';

  Serial.printf("Received from " MACSTR " - sequence: %lu, payload: %.*s\n",
                MAC2STR(sender_address),
                static_cast<unsigned long>(received_message.sequence),
                length - sizeof(uint32_t), received_message.text);
}

bool espnow_setup()
{
  esp_err_t result;

  if ((result = esp_now_init()) != ESP_OK)
  {
    Serial.printf("ERR %d: ESP-NOW initialization failed", result);
    return false;
  }

  esp_now_peer_info_t peer_info = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  /// memcpy(peer_info.peer_addr, broadcast_address, sizeof(broadcast_address));
  peer_info.channel = 0;
  peer_info.encrypt = false;

  // Call esp_now_add_peer() once for each unique peer MAC address.
  // Each call registers one peer;
  // adding the same address again returns an “already exists” error.
  if ((result = esp_now_add_peer(&peer_info)) != ESP_OK)
  {
    Serial.printf("ERR %d: Failed to add broadcast peer", result);
    return false;
  }

  if ((result = esp_now_register_recv_cb(on_data_receive)) != ESP_OK)
  {
    Serial.printf("ERR %d: Failed to register receive callback\n", result);
    return false;
  }

  return true;
}

void setup()
{
  Serial.begin(MONITOR_SPEED);

  WiFi.mode(WIFI_STA);
  WiFi.begin();
  Serial.print("ESP-NOW sender MAC: ");
  Serial.println(WiFi.macAddress());

  if (!espnow_setup())
  {
    Serial.println("Failed to initialize ESP-NOW");
    while (true)
      ;
  }
}

void espnow_send()
{
  ++message.sequence;
  snprintf(message.text, sizeof(message.text), "Hello from ESP-NOW %4d", message.sequence);

  const esp_err_t result = esp_now_send(
      broadcast_address,
      reinterpret_cast<const uint8_t *>(&message),
      sizeof(message));

  if (result == ESP_OK)
  {
    Serial.printf("Broadcast sent: #%lu, %s\n",
                  static_cast<unsigned long>(message.sequence),
                  message.text);
  }
  else
  {
    Serial.printf("Broadcast send failed: %d\n", result);
  }
}

void loop()
{

  static unsigned long last_send_ms = 0;

  if (millis() - last_send_ms < send_interval_ms)
  {
    return;
  }
  last_send_ms = millis();

  espnow_send();
}
