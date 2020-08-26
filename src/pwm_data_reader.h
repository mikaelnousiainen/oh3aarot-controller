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

#ifndef OH3AAROT_CONTROLLER_PWM_DATA_READER_H
#define OH3AAROT_CONTROLLER_PWM_DATA_READER_H

#include <Arduino.h>
#include "tc_lib.h"

template<arduino_due::tc_lib::timer_ids TIMER>
class PwmDataReader {
private:
    arduino_due::tc_lib::capture <TIMER> &pwm_capture_pin;
    double duty_usecs = 0;
    double period_usecs = 0;
    double angle_degrees = 0;
    bool overrun = false;
    bool stopped = false;

public:
    PwmDataReader(arduino_due::tc_lib::capture <TIMER> &capture_pin, uint32_t capture_window) : pwm_capture_pin(
            capture_pin)
    {
        pwm_capture_pin.config((capture_window / 100) << 1);
    }

    void read()
    {
        uint32_t status, duty = 0, period = 0;
        status = this->pwm_capture_pin.get_duty_and_period(duty, period);

        auto ticks_per_usec = static_cast<double>(pwm_capture_pin.ticks_per_usec());

        if (ticks_per_usec > 0) {
            this->duty_usecs = static_cast<double>(duty) / ticks_per_usec;
            this->period_usecs = static_cast<double>(period) / ticks_per_usec;
        } else {
            this->duty_usecs = 0;
            this->period_usecs = 0;
        }

        if (period_usecs > 0) {
            this->angle_degrees = 360 * (duty_usecs / period_usecs);
        } else {
            this->angle_degrees = 0;
        }

        this->overrun = pwm_capture_pin.is_overrun(status);
        this->stopped = pwm_capture_pin.is_stopped(status);
    };

    String to_angle_string()
    {
        return String(angle_degrees, 1)
               + String(is_overrun() ? " OVERRUN" : "")
               + String(is_stopped() ? " STOPPED" : "");
    }

    String to_string()
    {
        return String(duty_usecs, 1) + String("us ")
               + String(period_usecs, 1) + String("us ")
               + String(angle_degrees, 1) + String("deg")
               + String(is_overrun() ? " OVERRUN" : "")
               + String(is_stopped() ? " STOPPED" : "");
    }

    double duty()
    { return duty_usecs; };

    double period()
    { return period_usecs; };

    double angle()
    { return angle_degrees; };

    bool is_overrun()
    { return overrun; };

    bool is_stopped()
    { return stopped; };
};

extern PwmDataReader<arduino_due::tc_lib::timer_ids::TIMER_TC0> pwm_data_reader;

#endif
