# ESP8266 WiFi Packet Sniffer and Attack Detector

## Overview
This sketch for the ESP8266 is designed to detect potential denial-of-service (DoS) attacks on WiFi networks by sniffing packets and analyzing the rate at which specific types of packets are received. It uses the ESP8266's promiscuous mode to listen for WiFi packets without being connected to a network.

## Key Components

- **ESP8266WiFi.h**: A library for using the ESP8266's WiFi capabilities.
- **user_interface.h**: An ESP8266 SDK library that provides access to lower-level functionalities, including promiscuous mode.
- **LED Configuration**: An LED is configured to provide visual feedback during an attack.
- **Serial Communication**: Used for outputting the status and packet rate to the serial monitor.

## Configuration

- `LED`: The GPIO pin number connected to an LED for indicating the attack status.
- `LED_INVERT`: A boolean to control the LED's on/off logic depending on how the LED is wired.
- `SERIAL_BAUD`: The baud rate for serial communication.
- `CH_TIME`: The time in milliseconds to spend on each WiFi channel.
- `PKT_RATE`: The minimum packet rate per channel to consider it as an attack.
- `PKT_TIME`: The time threshold to confirm an ongoing attack.

## Variables

- `channels[]`: An array containing the WiFi channels to scan.
- `ch_index`, `packet_rate`, `attack_counter`: Variables for managing channel index, packet rate, and attack detection.
- `isAttack`: A boolean flag indicating whether an attack is detected.
- `update_time`, `ch_time`, `blink_time`: Timing variables for managing updates and LED blinking.

## Functions

- `sniffer(uint8_t *buf, uint16_t len)`: Callback function for the promiscuous mode. It checks for specific packet types and increments the packet rate.
- `attack_started()`: Called when an attack is detected; it updates `isAttack` and outputs a message.
- `attack_stopped()`: Called when an attack stops; it resets `isAttack` and ensures the LED is off.
- `setup()`: Initializes serial communication, configures the LED, sets up the WiFi in station mode, registers the sniffer callback, and enables promiscuous mode.
- `loop()`: Contains the main logic for packet rate analysis, attack detection, channel hopping, and LED blinking. It cycles through WiFi channels, checks the packet rate against the threshold, and toggles the LED during an attack.

## Attack Detection Logic

1. The code listens for WiFi packets across all specified channels.
2. It counts packets of specific types (e.g., association request and deauthentication) as potential indicators of an attack.
3. If the packet rate exceeds `PKT_RATE` consistently for `PKT_TIME`, an attack is considered ongoing.
4. The code signals an attack by printing messages to the serial monitor and blinking an LED rapidly.
5. Once the packet rate drops below the threshold, the attack is considered over, and the LED stops blinking.

## Usage

This script can be used for educational purposes or for monitoring WiFi networks for unusual activities indicating potential DoS attacks. It's a basic example of how to use the ESP8266 for network packet analysis in promiscuous mode.
