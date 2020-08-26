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

#include "config.h"
#include "iointerface.h"

IOInterface::IOInterface()
{
    pinMode(PIN_CW, OUTPUT);
    pinMode(PIN_CCW, OUTPUT);
    pinMode(PIN_THRESHOLD_1, INPUT_PULLUP);
    pinMode(PIN_THRESHOLD_2, INPUT_PULLUP);
    pinMode(PIN_LIMIT_1, INPUT_PULLUP);
    pinMode(PIN_LIMIT_2, INPUT_PULLUP);

    pinMode(PIN_SPEED, OUTPUT);

    analogWriteResolution(12);
    analogReadResolution(12);

    attachInterrupt(digitalPinToInterrupt(PIN_THRESHOLD_1), threshold1Change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_THRESHOLD_2), threshold2Change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_LIMIT_1), limit1Change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_LIMIT_2), limit2Change, CHANGE);

    setClockwise(false);
    setCounterClockwise(false);
    setSpeed(DEFAULT_SPEED);
}

void IOInterface::setClockwise(bool active)
{
    writePin(PIN_CW, active);
}

bool IOInterface::getClockwise()
{
    return readPin(PIN_CW);
}

void IOInterface::setCounterClockwise(bool active)
{
    writePin(PIN_CCW, active);
}

bool IOInterface::getCounterClockwise()
{
    return readPin(PIN_CCW);
}

bool IOInterface::getThreshold1State()
{
    return threshold1;
}

bool IOInterface::getThreshold2State()
{
    return threshold2;
}

bool IOInterface::getLimit1State()
{
    return limit1;
}

bool IOInterface::getLimit2State()
{
    return limit2;
}

int IOInterface::getSpeed()
{
    return (100 * speed_raw / 4095);
}

void IOInterface::setSpeed(int speed)
{
    speed_raw = (4095 * speed) / 100;
    if (speed_raw < 4095) {
        speed_raw++;
    }
    analogWrite(PIN_SPEED, speed_raw);
}

volatile bool IOInterface::threshold1 = false;
volatile bool IOInterface::threshold2 = false;
volatile bool IOInterface::limit1 = false;
volatile bool IOInterface::limit2 = false;
volatile int IOInterface::speed_raw = DEFAULT_SPEED;
