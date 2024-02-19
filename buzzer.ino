#include <ESP8266WiFi.h>       // For the WiFi Sniffer

extern "C" {
#include "user_interface.h"
}

#define BUZZER 5           
#define SERIAL_BAUD 115200 
#define CH_TIME 140        
#define PKT_RATE 5        
#define PKT_TIME 1        
#define ALERT_INTERVAL 500

const short channels[] { 1,2,3,4,5,6,7,8,9,10,11,12,13/*,14*/ };


bool alert_playing { false };     
int packet_rate { 0 };            
int attack_counter { 0 };         
unsigned long update_time { 0 };  /
unsigned long ch_time { 0 };      
unsigned long alert_time { 0 };   

// ===== Sniffer function ===== //
void sniffer(uint8_t *buf, uint16_t len) {
  if (!buf || len < 28) return; // Drop packets without MAC header

  byte pkt_type = buf[12]; // second half of frame control field

  // If captured packet is a deauthentication or dissassociaten frame
  if (pkt_type == 0xA0 || pkt_type == 0xC0) {
    ++packet_rate;
  }
}

// ===== Alert function ===== //
void playAlert() {
  alert_playing = true;
}

void stopAlert() {
  alert_playing = false;
  noTone(BUZZER); // Stop playing the tone
}

// ===== Setup ===== //
void setup() {
  Serial.begin(SERIAL_BAUD); // Start serial communication

  pinMode(BUZZER, OUTPUT); // Init buzzer pin

  WiFi.disconnect();                   // Disconnect from any saved or active WiFi connections
  wifi_set_opmode(STATION_MODE);       // Set device to client/station mode
  wifi_set_promiscuous_rx_cb(sniffer); // Set sniffer function
  wifi_set_channel(channels[0]);        // Set channel
  wifi_promiscuous_enable(true);       // Enable sniffer

  Serial.println("Started \\o/");
}

// ===== Loop ===== //
void loop() {
  unsigned long current_time = millis(); // Get current time (in ms)

  // Update each second (or scan-time-per-channel * channel-range)
  if (current_time - update_time >= (sizeof(channels)*CH_TIME)) {
    update_time = current_time; // Update time variable

    // When detected deauth packets exceed the minimum allowed number
    if (packet_rate >= PKT_RATE) {
      ++attack_counter; // Increment attack counter
    } else {
      if(attack_counter >= PKT_TIME) stopAlert();
      attack_counter = 0; // Reset attack counter
    }

    // When attack exceeds minimum allowed time
    if (attack_counter == PKT_TIME) {
      playAlert();
    }

    Serial.print("Packets/s: ");
    Serial.println(packet_rate);

    packet_rate = 0; // Reset packet rate
  }

  // Channel hopping
  if (sizeof(channels) > 1 && current_time - ch_time >= CH_TIME) {
    ch_time = current_time; // Update time variable

    // Get next channel
    static int ch_index = 0;
    ch_index = (ch_index+1) % (sizeof(channels)/sizeof(channels[0]));
    short ch = channels[ch_index];

    wifi_set_channel(ch);
  }

  // Alert blinking
  if (alert_playing && current_time - alert_time >= ALERT_INTERVAL) {
    alert_time = current_time;
    if (digitalRead(BUZZER) == LOW) {
      tone(BUZZER, 1000); // Turn on the buzzer
    } else {
      noTone(BUZZER); // Turn off the buzzer
    }
  }
}
