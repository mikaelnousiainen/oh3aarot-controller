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

#ifndef OH3AAROT_CONTROLLER_CONTROLLER_CLIENT_MANAGER_H
#define OH3AAROT_CONTROLLER_CONTROLLER_CLIENT_MANAGER_H

#include "controller_client.h"
#include "controller_command_handler.h"
#include "print.h"

class ControllerClientManager {
private:
    ControllerClient *clients[ETHERNET_CLIENT_COUNT]{};
    ControllerCommandHandler *handler;
    unsigned long last_client_push_time = 0;

public:
    explicit ControllerClientManager(ControllerCommandHandler *handler)
    {
        this->handler = handler;

        for (auto &client : this->clients) {
            client = nullptr;
        }
    }

    bool add_client(EthernetClient ethernet_client)
    {
        bool free_client_slot_found = false;
        p("New TCP connection from %s:%d\n", IpAddressToString(ethernet_client.remoteIP()).c_str(),
                ethernet_client.remotePort());

        for (auto &client : clients) {
            if (client == nullptr) {
                client = new ControllerClient(ethernet_client);
                free_client_slot_found = true;
                break;
            }
        }

        if (!free_client_slot_found) {
            ethernet_client.println("ERROR: TOO MANY CONNECTIONS");
            p("Cannot handle TCP connection from %s:%d: too many connections\n",
                    IpAddressToString(ethernet_client.remoteIP()).c_str(), ethernet_client.remotePort());
            ethernet_client.stop();
            return false;
        }

        return true;
    }

    void process_input()
    {
        for (auto client : clients) {
            if (client == nullptr) {
                continue;
            }

            int result = client->process_input();

            switch (result) {
                case CLIENT_INPUT_NEW_COMMAND:
                    handler->handle_command(String(client->get_command()), client, &client->client);
                    break;
                case CLIENT_INPUT_TOO_LONG:
                    client->client.println("ERROR COMMAND TOO LONG");
                    break;
                default:
                    break;
            }
        }
    }

    bool push_to_clients(const char *command, bool (ControllerClientManager::*condition)(ControllerClient *client))
    {
        bool handled = false;

        for (auto client : clients) {
            if (client == nullptr) {
                continue;
            }
            if (!(this->*condition)(client)) {
                continue;
            }

            handled |= handler->handle_command(String(command), client, &client->client);
        }

        return handled;
    }

    bool is_client_monitor_enabled(ControllerClient *client)
    {
        return client->is_monitor_enabled();
    }

    bool push_to_monitoring_clients(const char *command)
    {
        return push_to_clients(command, &ControllerClientManager::is_client_monitor_enabled);
    }

    bool is_time_to_push_to_clients()
    {
        unsigned long current_time = millis();
        bool push_to_clients = (current_time - last_client_push_time) >= CLIENT_PUSH_INTERVAL;

        if (push_to_clients) {
            last_client_push_time = current_time;
        }

        return push_to_clients;
    }

    void cleanup()
    {
        for (byte i = 0; i < ETHERNET_CLIENT_COUNT; i++) {
            if (clients[i] == nullptr) {
                continue;
            }

            p("Client: %d\n", i);
            if (clients[i]->cleanup()) {
                clients[i] = nullptr;
            }
        }
    }
};

#endif
