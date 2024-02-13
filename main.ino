#include <ESP8266WiFi.h>
extern "C" {
#include "user_interface.h"
}

#define LED 5
#define LED_INVERT true
#define SERIAL_BAUD 115200
#define CH_TIME 140
#define PKT_RATE 5
#define PKT_TIME 1

const short channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

int ch_index = 0;
int packet_rate = 0;
int attack_counter = 0;
bool isAttack = false; // Added variable to track attack state
unsigned long update_time = 0;
unsigned long ch_time = 0;
unsigned long blink_time = 0; // For controlling blink rate

void sniffer(uint8_t *buf, uint16_t len) {
  if (!buf || len < 28) return;
  byte pkt_type = buf[12];
  if (pkt_type == 0xA0 || pkt_type == 0xC0) {
    ++packet_rate;
  }
}

void attack_started() {
  if (!isAttack) {
    Serial.println("ATTACK DETECTED");
    isAttack = true;
  }
}

void attack_stopped() {
  if (isAttack) {
    Serial.println("ATTACK STOPPED");
    digitalWrite(LED, LED_INVERT); // Ensure LED is off when attack stops
    isAttack = false;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LED_INVERT);
  WiFi.disconnect();
  wifi_set_opmode(STATION_MODE);
  wifi_set_promiscuous_rx_cb(sniffer);
  wifi_set_channel(channels[0]);
  wifi_promiscuous_enable(true);
  Serial.println("Started \\o/");
}

void loop() {
  unsigned long current_time = millis();
  if (current_time - update_time >= (sizeof(channels) * CH_TIME)) {
    update_time = current_time;
    if (packet_rate >= PKT_RATE) {
      ++attack_counter;
    } else {
      if (attack_counter >= PKT_TIME) attack_stopped();
      attack_counter = 0;
    }
    if (attack_counter == PKT_TIME) {
      attack_started();
    }
    Serial.print("Packets/s: ");
    Serial.println(packet_rate);
    packet_rate = 0;
  }
  
  // Channel hopping
  if (sizeof(channels) > 1 && current_time - ch_time >= CH_TIME) {
    ch_time = current_time;
    ch_index = (ch_index + 1) % (sizeof(channels) / sizeof(channels[0]));
    wifi_set_channel(channels[ch_index]);
  }

  // Blink LED if under attack
  if (isAttack && current_time - blink_time > 500) { // Blink every 500ms
    digitalWrite(LED, !digitalRead(LED)); // Toggle LED state
    blink_time = current_time;
  }
}
