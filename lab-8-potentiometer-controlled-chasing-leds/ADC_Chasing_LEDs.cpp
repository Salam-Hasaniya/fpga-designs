/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief ADC-controlled chasing LED on Nexys4 DDR board
 *********************************************************************/

#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"

// Map function for doubles
double map_double(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Chasing LED function controlled by ADC input
void ADC_chasing_LEDs(XadcCore *adc_p, GpoCore *led_p, GpiCore *sw_p) {
    int sw_val, direction = 0;
    uint32_t led_state = 0x0001;
    int delay_ms, temp_delay = 1;

    while (1) {
        sw_val = sw_p->read();
        double adc_val = adc_p->read_adc_in(0);

        if (sw_val & 0x01) {
            led_state = 0x0001;
            led_p->write(led_state);
            continue;
        } else {
            if (led_state == 0x8000) direction = 1;
            else if (led_state == 0x0001) direction = 0;

            led_state = (direction == 0) ? (led_state << 1) : (led_state >> 1);

            led_p->write(led_state);

            delay_ms = (int)map_double(adc_val, 0.0, 1.0, 50.0, 1000.0);
            if (delay_ms != temp_delay) {
                temp_delay = delay_ms;
                uart.disp("Current delay: ");
                uart.disp(delay_ms);
                uart.disp("\n\r");
            }

            sleep_ms(delay_ms);
        }
    }
}

// Core instances
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));

// Main function
int main() {
    while (1) {
        ADC_chasing_LEDs(&adc, &led, &sw);
    }
}
