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

#ifndef OH3AAROT_CONTROLLER_CONTROLLER_CLIENT_H
#define OH3AAROT_CONTROLLER_CONTROLLER_CLIENT_H

#include <Ethernet.h>
#include "print.h"
#include "config.h"

#define CLIENT_INPUT_NEW_COMMAND 1
#define CLIENT_INPUT_WAITING 0
#define CLIENT_INPUT_TOO_LONG -1

String IpAddressToString(const IPAddress &ipAddress)
{
    return String(ipAddress[0]) + String(".") +
           String(ipAddress[1]) + String(".") +
           String(ipAddress[2]) + String(".") +
           String(ipAddress[3]);
}

class ControllerClient {
private:
    char client_command_buffer[ETHERNET_CLIENT_COMMAND_LENGTH];
    int client_command_length;
    char client_command[ETHERNET_CLIENT_COMMAND_LENGTH];
    bool monitor;

public:
    EthernetClient client;

    ControllerClient(EthernetClient ethernet_client)
    {
        this->client = ethernet_client;
        this->monitor = false;
        client_command[0] = '\0';
        client_command_length = 0;
    }

    bool is_monitor_enabled()
    {
        return monitor;
    }

    void set_monitor_enabled(bool enable_monitor)
    {
        this->monitor = enable_monitor;
    }

    int process_input()
    {
        while (client.available() > 0) {
            char c = client.read();

            if (client_command_length >= (ETHERNET_CLIENT_COMMAND_LENGTH - 1)) {
                client_command_buffer[0] = '\0';
                client_command_length = 0;
                return CLIENT_INPUT_TOO_LONG;
            }

            if (c == '\r' || c == '\t') {
                continue;
            }
            if (c == '\n') {
                strcpy(client_command, client_command_buffer);
                client_command_buffer[0] = '\0';
                client_command_length = 0;
                return CLIENT_INPUT_NEW_COMMAND;
            }

            client_command_buffer[client_command_length] = c;
            client_command_buffer[client_command_length + 1] = '\0';
            client_command_length++;
        }

        return CLIENT_INPUT_WAITING;
    }

    char *get_command()
    {
        return client_command;
    }

    bool connected()
    {
        return client && client.connected();
    }

    void stop()
    {
        return client.stop();
    }

    bool cleanup()
    {
        if (!client.connected()) {
            p("Closed TCP connection to %s:%d\n", IpAddressToString(client.remoteIP()).c_str(), client.remotePort());
            delay(2);
            client.stop();
            return true;
        }

        return false;
    }
};

#endif
