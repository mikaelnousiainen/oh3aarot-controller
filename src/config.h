/**
 * OH3AA antenna rotator controller firmware
 * Copyright (C) 2020 Mikael Nousiainen OH3BHX <mikael.nousiainen@iki.fi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OH3AAROT_CONTROLLER_CONFIG_H
#define OH3AAROT_CONTROLLER_CONFIG_H

// Application info

#define APP_NAME "OH3AA antenna rotator controller server"
#define APP_VERSION "0.2.0"

#define APP_VERSION_STRING APP_NAME " v" APP_VERSION

// Network configuration

#define SERVER_IP_ADDRESS "192.168.0.33"
#define SERVER_TCP_PORT 1234

// Pin definitions

#define PIN_CW 24 // OUT: Turn clockwise
#define PIN_CCW 25 // OUT: Turn counter-clockwise
#define PIN_THRESHOLD_1 26 // IN: Indicator for azimuth < 0
#define PIN_THRESHOLD_2 27 // IN: Indicator for azimuth > 360
#define PIN_LIMIT_1 28 // IN: Indicator for lowest possible azimuth
#define PIN_LIMIT_2 29 // IN: Indicator for highest possible azimuth
#define PIN_SPEED DAC1 // OUT DAC1 = PIN 67: Analog voltage from 0.55V to 2.75 V for rotator speed

#define PIN_ETHERNET_CS 10 // CS (chip select) pin for the W5100 Ethernet controller chip
#define PIN_ETHERNET_RESET 30 // Pin connected to W5100 Ethernet shield reset to allow automatic reset at power-on

// Default settings

#define SERIAL_PORT_SPEED 115200

#define AZIMUTH_MINIMUM -90
#define AZIMUTH_MAXIMUM 450
#define DEFAULT_SPEED 50 // Range: 0-100
#define ANGLE_THRESHOLD 0.3

#define CLIENT_PUSH_INTERVAL 100 // milliseconds
#define PWM_CAPTURE_WINDOW_DURATION 10 * 1200 * 100 // hundredths of microseconds

// Rotator settings

#define ROTATOR_AZIMUTH_OFFSET_DEGREES 0

// Network connection handling

#define ETHERNET_CLIENT_COUNT 8
#define ETHERNET_CLIENT_COMMAND_LENGTH 32

#endif
