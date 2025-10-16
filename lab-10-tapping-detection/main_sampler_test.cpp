/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Basic test of nexys4 ddr mmio cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

// #define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"
#include <math.h>

/**
 * blink once per second for 5 times.
 * provide a sanity check for timer (based on SYS_CLK_FREQ)
 * @param led_p pointer to led instance
 */
void timer_check(GpoCore *led_p) {
   int i;

   for (i = 0; i < 5; i++) {
      led_p->write(0xffff);
      sleep_ms(500);
      led_p->write(0x0000);
      sleep_ms(500);
      debug("timer check - (loop #)/now: ", i, now_ms());
   }
}

/**
 * check individual led
 * @param led_p pointer to led instance
 * @param n number of led
 */
void led_check(GpoCore *led_p, int n) {
   int i;

   for (i = 0; i < n; i++) {
      led_p->write(1, i);
      sleep_ms(100);
      led_p->write(0, i);
      sleep_ms(100);
   }
}

/**
 * leds flash according to switch positions.
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */
void sw_check(GpoCore *led_p, GpiCore *sw_p) {
   int i, s;

   s = sw_p->read();
   for (i = 0; i < 30; i++) {
      led_p->write(s);
      sleep_ms(50);
      led_p->write(0);
      sleep_ms(50);
   }
}

/**
 * uart transmits test line.
 * @note uart instance is declared as global variable in chu_io_basic.h
 */
void uart_check() {
   static int loop = 0;

   uart.disp("uart test #");
   uart.disp(loop);
   uart.disp("\n\r");
   loop++;
}

/**
 * read FPGA internal voltage temperature
 * @param adc_p pointer to xadc instance
 */

void adc_check(XadcCore *adc_p, GpoCore *led_p) {
   double reading;
   int n, i;
   uint16_t raw;

   for (i = 0; i < 5; i++) {
      // display 12-bit channel 0 reading in LED
      raw = adc_p->read_raw(0);
      raw = raw >> 4;
      led_p->write(raw);
      // display on-chip sensor and 4 channels in console
      uart.disp("FPGA vcc/temp: ");
      reading = adc_p->read_fpga_vcc();
      uart.disp(reading, 3);
      uart.disp(" / ");
      reading = adc_p->read_fpga_temp();
      uart.disp(reading, 3);
      uart.disp("\n\r");
      for (n = 0; n < 4; n++) {
         uart.disp("analog channel/voltage: ");
         uart.disp(n);
         uart.disp(" / ");
         reading = adc_p->read_adc_in(n);
         uart.disp(reading, 3);
         uart.disp("\n\r");
      } // end for
      sleep_ms(2000);
   }
}

/**
 * tri-color led dims gradually
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */

void pwm_3color_led_check(PwmCore *pwm_p) {
   int i, n;
   double bright, duty;
   const double P20 = 1.2589;  // P20=100^(1/20); i.e., P20^20=100

   pwm_p->set_freq(50);
   for (n = 0; n < 3; n++) {
      bright = 1.0;
      for (i = 0; i < 20; i++) {
         bright = bright * P20;
         duty = bright / 100.0;
         pwm_p->set_duty(duty, n);
         pwm_p->set_duty(duty, n + 3);
         sleep_ms(100);
      }
      sleep_ms(300);
      pwm_p->set_duty(0.0, n);
      pwm_p->set_duty(0.0, n + 3);
   }
}

/**
 * Test debounced buttons
 *   - count transitions of normal and debounced button
 * @param db_p pointer to debouceCore instance
 */

void debounce_check(DebounceCore *db_p, GpoCore *led_p) {
   long start_time;
   int btn_old, db_old, btn_new, db_new;
   int b = 0;
   int d = 0;
   uint32_t ptn;

   start_time = now_ms();
   btn_old = db_p->read();
   db_old = db_p->read_db();
   do {
      btn_new = db_p->read();
      db_new = db_p->read_db();
      if (btn_old != btn_new) {
         b = b + 1;
         btn_old = btn_new;
      }
      if (db_old != db_new) {
         d = d + 1;
         db_old = db_new;
      }
      ptn = d & 0x0000000f;
      ptn = ptn | (b & 0x0000000f) << 4;
      led_p->write(ptn);
   } while ((now_ms() - start_time) < 5000);
}

/**
 * Test pattern in 7-segment LEDs
 * @param sseg_p pointer to 7-seg LED instance
 */

void sseg_check(SsegCore *sseg_p) {
   int i, n;
   uint8_t dp;

   //turn off led
   for (i = 0; i < 8; i++) {
      sseg_p->write_1ptn(0xff, i);
   }
   //turn off all decimal points
   sseg_p->set_dp(0x00);

   // display 0x0 to 0xf in 4 epochs
   // upper 4  digits mirror the lower 4
   for (n = 0; n < 4; n++) {
      for (i = 0; i < 4; i++) {
         sseg_p->write_1ptn(sseg_p->h2s(i + n * 4), 3 - i);
         sseg_p->write_1ptn(sseg_p->h2s(i + n * 4), 7 - i);
         sleep_ms(300);
      } // for i
   }  // for n
      // shift a decimal point 4 times
   for (i = 0; i < 4; i++) {
      bit_set(dp, 3 - i);
      sseg_p->set_dp(1 << (3 - i));
      sleep_ms(300);
   }
   //turn off led
   for (i = 0; i < 8; i++) {
      sseg_p->write_1ptn(0xff, i);
   }
   //turn off all decimal points
   sseg_p->set_dp(0x00);

}

/**
 * Test adxl362 accelerometer using SPI
 */

void gsensor_check(SpiCore *spi_p, GpoCore *led_p) {
   const uint8_t RD_CMD = 0x0b;
   const uint8_t PART_ID_REG = 0x02;
   const uint8_t DATA_REG = 0x08;
   const float raw_max = 127.0 / 2.0;  //128 max 8-bit reading for +/-2g

   int8_t xraw, yraw, zraw;
   float x, y, z;
   int id;

   spi_p->set_freq(400000);
   spi_p->set_mode(0, 0);
   // check part id
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(PART_ID_REG);  // part id address
   id = (int) spi_p->transfer(0x00);
   spi_p->deassert_ss(0);
   uart.disp("read ADXL362 id (should be 0xf2): ");
   uart.disp(id, 16);
   uart.disp("\n\r");
   // read 8-bit x/y/z g values once
   spi_p->assert_ss(0);    // activate
   spi_p->transfer(RD_CMD);  // for read operation
   spi_p->transfer(DATA_REG);  //
   xraw = spi_p->transfer(0x00);
   yraw = spi_p->transfer(0x00);
   zraw = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);
   x = (float) xraw / raw_max;
   y = (float) yraw / raw_max;
   z = (float) zraw / raw_max;
   uart.disp("x/y/z axis g values: ");
   uart.disp(x, 3);
   uart.disp(" / ");
   uart.disp(y, 3);
   uart.disp(" / ");
   uart.disp(z, 3);
   uart.disp("\n\r");
   //sleep_ms(500);

}

/*
 * read temperature from adt7420
 * @param adt7420_p pointer to adt7420 instance
 */
void adt7420_check(I2cCore *adt7420_p, GpoCore *led_p) {
   const uint8_t DEV_ADDR = 0x4b;
   uint8_t wbytes[2], bytes[2];
   //int ack;
   uint16_t tmp;
   float tmpC;

   // read adt7420 id register to verify device existence
   // ack = adt7420_p->read_dev_reg_byte(DEV_ADDR, 0x0b, &id);

   wbytes[0] = 0x0b;
   adt7420_p->write_transaction(DEV_ADDR, wbytes, 1, 1);
   adt7420_p->read_transaction(DEV_ADDR, bytes, 1, 0);
   uart.disp("read ADT7420 id (should be 0xcb): ");
   uart.disp(bytes[0], 16);
   uart.disp("\n\r");
   //debug("ADT check ack/id: ", ack, bytes[0]);
   // read 2 bytes
   //ack = adt7420_p->read_dev_reg_bytes(DEV_ADDR, 0x0, bytes, 2);
   wbytes[0] = 0x00;
   adt7420_p->write_transaction(DEV_ADDR, wbytes, 1, 1);
   adt7420_p->read_transaction(DEV_ADDR, bytes, 2, 0);

   // conversion
   tmp = (uint16_t) bytes[0];
   tmp = (tmp << 8) + (uint16_t) bytes[1];
   if (tmp & 0x8000) {
      tmp = tmp >> 3;
      tmpC = (float) ((int) tmp - 8192) / 16;
   } else {
      tmp = tmp >> 3;
      tmpC = (float) tmp / 16;
   }
   uart.disp("temperature (C): ");
   uart.disp(tmpC);
   uart.disp("\n\r");
   led_p->write(tmp);
   sleep_ms(1000);
   led_p->write(0);
}

void ps2_check(Ps2Core *ps2_p) {
   int id;
   int lbtn, rbtn, xmov, ymov;
   char ch;
   unsigned long last;

   uart.disp("\n\rPS2 device (1-keyboard / 2-mouse): ");
   id = ps2_p->init();
   uart.disp(id);
   uart.disp("\n\r");
   last = now_ms();
   do {
      if (id == 2) {  // mouse
         if (ps2_p->get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov)) {
            uart.disp("[");
            uart.disp(lbtn);
            uart.disp(", ");
            uart.disp(rbtn);
            uart.disp(", ");
            uart.disp(xmov);
            uart.disp(", ");
            uart.disp(ymov);
            uart.disp("] \r\n");
            last = now_ms();

         }   // end get_mouse_activitiy()
      } else {
         if (ps2_p->get_kb_ch(&ch)) {
            uart.disp(ch);
            uart.disp(" ");
            last = now_ms();
         } // end get_kb_ch()
      }  // end id==2
   } while (now_ms() - last < 5000);
   uart.disp("\n\rExit PS2 test \n\r");

}

/**
 * play primary notes with ddfs
 * @param ddfs_p pointer to ddfs core
 * @note: music tempo is defined as beats of quarter-note per minute.
 *        60 bpm is 1 sec per quarter note
 * @note "click" sound due to abrupt stop of a note
 *
 */
void ddfs_check(DdfsCore *ddfs_p, GpoCore *led_p) {
   int i, j;
   float env;

   //vol = (float)sw.read_pin()/(float)(1<<16),
   ddfs_p->set_env_source(0);  // select envelop source
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(500);
   ddfs_p->set_env(1.0);   // set volume
   ddfs_p->set_carrier_freq(262);
   sleep_ms(2000);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(2000);
   // volume control (attenuation)
   ddfs_p->set_env(0.0);   // set volume
   env = 1.0;
   for (i = 0; i < 1000; i++) {
      ddfs_p->set_env(env);
      sleep_ms(10);
      env = env / 1.0109; //1.0109**1024=2**16
   }
   // frequency modulation 635-912 800 - 2000 siren sound
   ddfs_p->set_env(1.0);   // set volume
   ddfs_p->set_carrier_freq(635);
   for (i = 0; i < 5; i++) {               // 10 cycles
      for (j = 0; j < 30; j++) {           // sweep 30 steps
         ddfs_p->set_offset_freq(j * 10);  // 10 Hz increment
         sleep_ms(25);
      } // end j loop
   } // end i loop
   ddfs_p->set_offset_freq(0);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(1000);
}

/**
 * play primary notes with ddfs
 * @param adsr_p pointer to adsr core
 * @param ddfs_p pointer to ddfs core
 * @note: music tempo is defined as beats of quarter-note per minute.
 *        60 bpm is 1 sec per quarter note
 *
 */
void adsr_check(AdsrCore *adsr_p, GpoCore *led_p, GpiCore *sw_p) {
   const int melody[] = { 0, 2, 4, 5, 7, 9, 11 };
   int i, oct;

   adsr_p->init();
   // no adsr envelop and  play one octave
   adsr_p->bypass();
   for (i = 0; i < 7; i++) {
      led_p->write(bit(i));
      adsr_p->play_note(melody[i], 3, 500);
      sleep_ms(500);
   }
   adsr_p->abort();
   sleep_ms(1000);
   // set and enable adsr envelop
   // play 4 octaves
   adsr_p->select_env(sw_p->read());
   for (oct = 3; oct < 6; oct++) {
      for (i = 0; i < 7; i++) {
         led_p->write(bit(i));
         adsr_p->play_note(melody[i], oct, 500);
         sleep_ms(500);
      }
   }
   led_p->write(0);
   // test duration
   sleep_ms(1000);
   for (i = 0; i < 4; i++) {
      adsr_p->play_note(0, 4, 500 * i);
      sleep_ms(500 * i + 1000);
   }
}

/**
 * core test
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */
void show_test_id(int n, GpoCore *led_p) {
   int i, ptn;

   ptn = n; //1 << n;
   for (i = 0; i < 20; i++) {
      led_p->write(ptn);
      sleep_ms(30);
      led_p->write(0);
      sleep_ms(30);
   }
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
double map_double(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void ADC_chasing_LEDs(XadcCore *adc_p, GpoCore *led_p, GpiCore *sw_p) {
    int sw_val, direction;
    uint32_t led_state = 0x0001;
    direction = 0;

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


void Spectrum_Display(PwmCore *pwm_p, XadcCore *adc_p, SsegCore *sseg_p ) {
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;

	double adc_val = adc_p->read_adc_in(0);  // Normalized between 0.0 and 1.0

	if (adc_val < 0.167) {
		red = 1.0;
		green = map_double(adc_val, 0.0, 0.166, 0.0, 1.0);
		blue = 0.0;
	} else if (adc_val < 0.334) {
		red = map_double(adc_val, 0.167, 0.333, 1.0, 0.0);
		green = 1.0;
		blue = 0.0;
	} else if (adc_val < 0.5) {
		red = 0.0;
		green = 1.0;
		blue = map_double(adc_val, 0.334, 0.499, 0.0, 1.0);
	} else if (adc_val < 0.667) {
		red = 0.0;
		green = map_double(adc_val, 0.5, 0.666, 1.0, 0.0);
		blue = 1.0;
	} else if (adc_val < 0.834) {
		red = map_double(adc_val, 0.667, 0.833, 0.0, 1.0);
		green = 0.0;
		blue = 1.0;
	} else {
		red = 1.0;
		green = 0.0;
		blue = map_double(adc_val, 0.834, 1.0, 1.0, 0.0);
	}

	pwm_p->set_duty(red, 2);
	pwm_p->set_duty(red, 5);
	pwm_p->set_duty(green, 1);
	pwm_p->set_duty(green, 4);
	pwm_p->set_duty(blue, 0);
	pwm_p->set_duty(blue, 3);

	uart.disp("ADC Input: ");
	uart.disp(adc_val);
	uart.disp("\n\r");

	int adc_scaled = (int)(adc_val * 1000);
	int hundreds = (adc_scaled / 100) % 10;
	int tens = (adc_scaled / 10) % 10;
	int ones = adc_scaled % 10;

	sseg_p->write_1ptn(sseg_p->h2s(hundreds), 2);  // tens place
	sseg_p->write_1ptn(sseg_p->h2s(tens), 1);  // tens place
	sseg_p->write_1ptn(sseg_p->h2s(ones), 0);  // ones place
	sseg_p->write_1ptn(sseg_p->h2s(0), 3);  // ones place
	sseg_p->set_dp(1 << 3);

}


void tapping_detection(SpiCore *spi_p, GpoCore *led_p) {
   const uint8_t RD_CMD = 0x0b;			//Command to tell accelerometer we want to read
   const uint8_t DATA_REG = 0x08;		//where X Y Z data starts
   const float raw_max = 127.0 / 2.0;	//scales 8 bit values to G units

   int8_t xraw, yraw, zraw;
   float x, y, z;
   float mag, tap_strength;
   int level = 0;

   //read accelerometer data
   spi_p->assert_ss(0);
   spi_p->transfer(RD_CMD);
   spi_p->transfer(DATA_REG);
   //Simply reads raw acceleration for each axis
   xraw = spi_p->transfer(0x00);
   yraw = spi_p->transfer(0x00);
   zraw = spi_p->transfer(0x00);
   spi_p->deassert_ss(0);

   //convert to g-units abd take absolute value
   x = fabs((float)xraw / raw_max);
   y = fabs((float)yraw / raw_max);
   z = fabs((float)zraw / raw_max);

   //compute magnitude
   mag = sqrt(x * x + y * y + z * z);
   //subtract 1 g (normal gravity) to show extra motion
   tap_strength = fabs(mag - 1.0f);

   //we use 0.0f numbers bc we are working with floats instead of doubles
   if (tap_strength > 0.95f) level = 16;
   else if (tap_strength > 0.90f) level = 15;
   else if (tap_strength > 0.85f) level = 14;
   else if (tap_strength > 0.80f) level = 13;
   else if (tap_strength > 0.75f) level = 12;
   else if (tap_strength > 0.70f) level = 11;
   else if (tap_strength > 0.65f) level = 10;
   else if (tap_strength > 0.60f) level = 9;
   else if (tap_strength > 0.55f) level = 8;
   else if (tap_strength > 0.50f) level = 7;
   else if (tap_strength > 0.45f) level = 6;
   else if (tap_strength > 0.40f) level = 5;
   else if (tap_strength > 0.35f) level = 4;
   else if (tap_strength > 0.30f) level = 3;
   else if (tap_strength > 0.25f) level = 2;
   else if (tap_strength > 0.20f) level = 1;

   if (level >= 1) {
      uint16_t led_mask = (1 << level) - 1; //level 4 example: 00001 << 4 = 10000 - 1 = 01111
      led_p->write(led_mask);

      uart.disp("Tap detected! Magnitude level: ");
      uart.disp(level);
      uart.disp("\n\r");
   } else {
      led_p->write(0x0000);
   }

   sleep_ms(100);
}

void init_tapping_detection (SpiCore *spi_p) {
	const uint8_t RD_CMD = 0x0b;
	const uint8_t PART_ID_REG = 0x02;
	int id;

	//set SPI communication parameters for ADXL362
	spi_p->set_freq(400000);
	spi_p->set_mode(0, 0);

	//verify part ID
	spi_p->assert_ss(0);				//activate device
	spi_p->transfer(RD_CMD);			//read command
	spi_p->transfer(PART_ID_REG);		//Register: part ID
	id = (int)spi_p->transfer(0x00); 	//read a byte
	spi_p->deassert_ss(0);				//deactivate device

	uart.disp("ADXL362 Init - Device ID (should be 0xF2): ");
	uart.disp(id, 16);
	uart.disp("\n\r");
}





GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));
I2cCore adt7420(get_slot_addr(BRIDGE_BASE, S10_I2C));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
DdfsCore ddfs(get_slot_addr(BRIDGE_BASE, S12_DDFS));
AdsrCore adsr(get_slot_addr(BRIDGE_BASE, S13_ADSR), &ddfs);


int main() {
   //uint8_t id, ;

   //timer_check(&led);

	//CUSTOM
		//init_tapping_detection(&spi);
   while (1) {
      //show_test_id(1, &led);
      //led_check(&led, 16);
      //sw_check(&led, &sw);
      //show_test_id(3, &led);
      //uart_check();
      //debug("main - switch value / up time : ", sw.read(), now_ms());
      //show_test_id(5, &led);
      //adc_check(&adc, &led);
      //show_test_id(6, &led);
      //pwm_3color_led_check(&pwm);
      //show_test_id(7, &led);
      //debounce_check(&btn, &led);
      //show_test_id(8, &led);
      //sseg_check(&sseg);
      //show_test_id(9, &led);
      //gsensor_check(&spi, &led);
      //show_test_id(10, &led);
      //adt7420_check(&adt7420, &led);
      //show_test_id(11, &led);
      //ps2_check(&ps2);
      //show_test_id(12, &led);
      //ddfs_check(&ddfs, &led);
      //show_test_id(13, &led);
      //adsr_check(&adsr, &led, &sw);

	   //CUSTOM
	   	   //ADC_chasing_LEDs (&adc, &led, &sw);
	   	   //Spectrum_Display(&pwm, &adc, &sseg);
	   	   tapping_detection(&spi, &led);

   } //while
} //main

