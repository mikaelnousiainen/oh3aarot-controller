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

#include <Arduino.h>
#include <Ethernet.h>
#include "utility/w5100.h"

#include "config.h"
#include "print.h"
#include "iointerface.h"
#include "pwm_data_reader.h"
#include "controller_command_handler.h"
#include "controller_client_manager.h"

// Network settings

IPAddress ip_address;
const uint16_t tcp_port = SERVER_TCP_PORT;

byte mac_address[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Application code

capture_tc0_declaration(); // TC0 and channel 0
PwmDataReader<arduino_due::tc_lib::timer_ids::TIMER_TC0> pwm_data_reader(capture_tc0, PWM_CAPTURE_WINDOW_DURATION);

IOInterface *io;
EthernetServer *server;
ControllerCommandHandler *command_handler;
ControllerClientManager *client_manager;

void setup_server()
{
    delay(100);
    // Manually reset the Ethernet shield, as it does not happen automatically at power-on
    pinMode(PIN_ETHERNET_RESET, OUTPUT);
    digitalWrite(PIN_ETHERNET_RESET, LOW);
    delay(200);
    digitalWrite(PIN_ETHERNET_RESET, HIGH);
    pinMode(PIN_ETHERNET_RESET, INPUT);
    delay(100);

    bool valid_ip_address = ip_address.fromString(SERVER_IP_ADDRESS);
    if (!valid_ip_address) {
        p("Invalid IP address: %s\n", SERVER_IP_ADDRESS);
    }

    EthernetClass::init(PIN_ETHERNET_CS);

    EthernetClass::begin(mac_address, ip_address);

    while (EthernetClass::hardwareStatus() == EthernetNoHardware) {
        p("Ethernet shield not found\n");
        delay(1000);
        EthernetClass::begin(mac_address, ip_address);
    }

    p("Ethernet shield initialized\n");

    if (EthernetClass::linkStatus() == LinkOFF) {
        p("Ethernet cable is not connected\n");
    }

    server = new EthernetServer(tcp_port);

    server->begin();

    p("TCP server is listening at %s:%d\n", IpAddressToString(EthernetClass::localIP()).c_str(), tcp_port);
}

void setup()
{
    SERIAL_PORT.begin(SERIAL_PORT_SPEED);

    p(APP_VERSION_STRING "\n");

    io = new IOInterface();
    command_handler = new ControllerCommandHandler(io, ROTATOR_AZIMUTH_OFFSET_DEGREES);
    client_manager = new ControllerClientManager(command_handler);

    setup_server();
}

void loop()
{
    command_handler->stop_if_direction_target_reached();
    client_manager->cleanup();

    EthernetClient new_client = server->accept();

    if (new_client) {
        client_manager->add_client(new_client);
    }

    bool push_to_clients = client_manager->is_time_to_push_to_clients();

    String pwm_data_string;
    if (push_to_clients) {
        pwm_data_reader.read();

        client_manager->push_to_monitoring_clients("STATE");
    }

    client_manager->process_input();
}
