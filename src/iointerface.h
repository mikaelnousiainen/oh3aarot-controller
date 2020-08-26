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

#ifndef OH3AAROT_CONTROLLER_IOINTERFACE_H
#define OH3AAROT_CONTROLLER_IOINTERFACE_H

class IOInterface {
private:
    static volatile bool threshold1;
    static volatile bool threshold2;
    static volatile bool limit1;
    static volatile bool limit2;
    static volatile int speed_raw;

    static bool readPin(int pin)
    {
        return digitalRead(pin) != 0;
    }

    static void writePin(int pin, bool active)
    {
        digitalWrite(pin, active ? HIGH : LOW);
    }

    static void threshold1Change()
    {
        threshold1 = readPin(PIN_THRESHOLD_1);
    }

    static void threshold2Change()
    {
        threshold2 = readPin(PIN_THRESHOLD_2);
    }

    static void limit1Change()
    {
        limit1 = readPin(PIN_LIMIT_1);
    }

    static void limit2Change()
    {
        limit2 = readPin(PIN_LIMIT_2);
    }

public:
    IOInterface();

    void setClockwise(bool active);
    bool getClockwise();
    void setCounterClockwise(bool active);
    bool getCounterClockwise();

    bool getThreshold1State();
    bool getThreshold2State();
    bool getLimit1State();
    bool getLimit2State();

    int getSpeed();
    void setSpeed(int speed);
};

#endif
