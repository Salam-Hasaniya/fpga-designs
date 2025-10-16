/*****************************************************************//**
 * @file main_vanilla_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"

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
      sleep_ms(200);
      led_p->write(0, i);
      sleep_ms(200);
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

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void chasing_LEDs(GpoCore *led_p,GpiCore *sw_p) {
	 int s, direction;
	 uint32_t led_state;
	 led_state = 0x0001;
	 direction = 0;

	 int speed_switches, delay_ms;
	 int temp_delay = 1;

	 while(1) {
		 s = sw_p->read();
		 speed_switches = ((s&0x3E)>>1)+1;


		 if (s & 0x01){
		   led_state = 0x0001;
		   led_p->write(led_state);
		   continue;
		 }
		 else {

		   if (led_state==0x8000){
			   direction = 1;
		   } else if(led_state==0x0001) {
			   direction = 0;
		   }

		   if (direction == 0) {
			   led_state<<=1;
		   } else if (direction == 1){
			   led_state>>=1;
		   }
		   	   led_p->write(led_state);
		   	delay_ms = map(speed_switches, 0, 31, 50, 1000);
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


// instantiate switch, led
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));

int main() {

   while (1) {
      //timer_check(&led);
      //led_check(&led, 16);
      //sw_check(&led, &sw);
      //uart_check();
      //debug("main - switch value / up time : ", sw.read(), now_ms());
	   chasing_LEDs(&led, &sw);


   } //while
} //main

