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

#ifndef OH3AAROT_CONTROLLER_CONTROLLER_COMMAND_HANDLER_H
#define OH3AAROT_CONTROLLER_CONTROLLER_COMMAND_HANDLER_H

#include "iointerface.h"
#include "controller_client.h"
#include "pwm_data_reader.h"

class ControllerCommandHandler {
private:
    IOInterface *io;
    double azimuth_offset;
    double target_az = 0;
    bool target_az_set = false;

public:
    explicit ControllerCommandHandler(IOInterface *io, double azimuth_offset)
    {
        this->io = io;
        this->azimuth_offset = azimuth_offset;
    }

    double get_az()
    {
        double angle = pwm_data_reader.angle();

        if (io->getThreshold1State()) {
            if (angle >= 270 && angle < 360) {
                angle = -(360 - angle);
            }
        } else if (io->getThreshold2State()) {
            if (angle >= 0 && angle < 110) {
                angle += 360;
            }
        }

        return angle + azimuth_offset;
    }

    void set_az(double az)
    {
        double current_az = get_az();

        target_az = az;
        target_az_set = true;

        if (current_az < (az - ANGLE_THRESHOLD)) {
            io->setCounterClockwise(false);
            io->setClockwise(true);
        } else if (current_az > (az + ANGLE_THRESHOLD)) {
            io->setClockwise(false);
            io->setCounterClockwise(true);
        } else {
            io->setClockwise(false);
            io->setCounterClockwise(false);
        }
    }

    void stop_if_direction_target_reached()
    {
        double current_angle = get_az();

        if (target_az_set) {
            if (io->getClockwise()) {
                if (current_angle >= (target_az - ANGLE_THRESHOLD)) {
                    io->setClockwise(false);
                    target_az_set = false;
                }
            }

            if (io->getCounterClockwise()) {
                if (current_angle <= (target_az + ANGLE_THRESHOLD)) {
                    io->setCounterClockwise(false);
                    target_az_set = false;
                }
            }
        }

        if (io->getLimit2State() && io->getClockwise()) {
            io->setClockwise(false);
            target_az_set = false;
        }
        if (io->getLimit1State() && io->getCounterClockwise()) {
            io->setCounterClockwise(false);
            target_az_set = false;
        }
    }

    String get_flags()
    {
        String flags = String();

        if (io->getClockwise()) {
            flags.concat("CW,");
        }
        if (io->getCounterClockwise()) {
            flags.concat("CCW,");
        }
        if (io->getThreshold1State()) {
            flags.concat("T1,");
        }
        if (io->getThreshold2State()) {
            flags.concat("T2,");
        }
        if (io->getLimit1State()) {
            flags.concat("L1,");
        }
        if (io->getLimit2State()) {
            flags.concat("L2");
        }
        if (flags.endsWith(",")) {
            flags.remove(flags.length() - 1, 1);
        }

        return flags;
    }

    int get_speed()
    {
        return io->getSpeed();
    }

    void set_speed(int speed)
    {
        io->setSpeed(speed);
    }

    void stop()
    {
        io->setClockwise(false);
        io->setCounterClockwise(false);
        target_az_set = false;
    }

    void park()
    {
        set_az(0);
    }

    void reset()
    {
        stop();
        io->setSpeed(DEFAULT_SPEED);
    }

    void move_cw()
    {
        io->setCounterClockwise(false);
        io->setClockwise(true);
    }

    void move_ccw()
    {
        io->setClockwise(false);
        io->setCounterClockwise(true);
    }

    bool handle_command(String command, ControllerClient *client, Print *response)
    {
        command.trim();

        if (command.length() == 0) {
            return true;
        }

        int first_space = command.indexOf(' ');
        String name;
        if (first_space >= 0) {
            name = command.substring(0, first_space);
        } else {
            name = command;
        }

        if (name == "AZ" && first_space > 0) {
            String az_string = command.substring(first_space + 1);
            az_string.trim();
            double az_angle = az_string.toDouble();
            // TODO: detect angle parse errors!

            if (az_angle < AZIMUTH_MINIMUM || az_angle > AZIMUTH_MAXIMUM) {
                response->println("ERROR INVALID AZIMUTH");
                return false;
            }

            set_az(az_angle);
            response->print("OK AZ ");
            response->println(az_angle);
        } else if (name == "AZ?") {
            String az_string = String(get_az(), 1);
            response->print("OK AZ ");
            response->println(az_string.c_str());
        } else if (name == "MOVE" && first_space > 0) {
            String direction = command.substring(first_space + 1);
            direction.trim();

            if (direction == "CW") {
                move_cw();
            } else if (direction == "CCW") {
                move_ccw();
            } else {
                response->println("ERROR INVALID DIRECTION");
                return false;
            }

            response->print("OK MOVE ");
            response->println(direction);
        } else if (name == "STATE") {
            String az_string = String(get_az(), 1);
            String speed_string = String(get_speed());

            response->print("OK STATE AZ=");
            response->print(az_string.c_str());
            response->print(" SPEED=");
            response->print(speed_string.c_str());
            response->print(" FLAGS=");
            response->println(get_flags());
        } else if (name == "SPEED" && first_space > 0) {
            String speed_string = command.substring(first_space + 1);
            speed_string.trim();
            double speed = speed_string.toInt();
            // TODO: detect speed parse errors!

            if (speed < 0 || speed > 100) {
                response->println("ERROR INVALID SPEED");
                return false;
            }

            set_speed(speed);
            response->print("OK SPEED ");
            response->println(speed);
        } else if (name == "SPEED?") {
            String speed_string = String(get_speed());
            response->print("OK SPEED ");
            response->println(speed_string.c_str());
        } else if (name == "STOP") {
            stop();
            response->println("OK STOP");
        } else if (name == "PARK") {
            park();
            response->println("OK PARK");
        } else if (name == "RESET") {
            reset();
            response->println("OK RESET");
        } else if (name == "MONITOR" && first_space > 0) {
            String monitor_string = command.substring(first_space + 1);
            monitor_string.trim();
            bool monitor = monitor_string.toInt() != 0;
            client->set_monitor_enabled(monitor);
            response->print("OK MONITOR ");
            response->println(monitor ? "1" : "0");
        } else if (name == "INFO") {
            response->println("OK INFO " APP_VERSION_STRING);
        } else if (name == "AZLIMITS") {
            response->print("OK AZLIMITS MIN=");
            response->print(AZIMUTH_MINIMUM);
            response->print(" MAX=");
            response->println(AZIMUTH_MAXIMUM);
        } else if (name == "AZOFFSET" && first_space > 0) {
            String az_string = command.substring(first_space + 1);
            az_string.trim();
            double az_offset = az_string.toDouble();
            // TODO: detect offset parse errors!

            if (az_offset < -360 || az_offset > 360) {
                response->println("ERROR INVALID AZIMUTH OFFSET");
                return false;
            }

            azimuth_offset = az_offset;
            response->print("OK AZOFFSET ");
            response->println(az_offset);
        } else if (name == "AZOFFSET?") {
            response->print("OK AZOFFSET ");
            response->println(azimuth_offset);
        } else {
            response->println("ERROR INVALID COMMAND");
            return false;
        }

        return true;
    }
};

#endif
