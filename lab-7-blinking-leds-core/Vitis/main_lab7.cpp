/*****************************************************************//**
 * @file main_blinker.cpp
 *
 * @brief Simple test for BlinkerCore
 *
 * @author p chu
 * @version v1.0
 *********************************************************************/

#include "chu_init.h"
#include "gpio_cores.h"

// Instantiate BlinkerCore
BlinkerCore blinker(get_slot_addr(BRIDGE_BASE, S4_BLINKER));

void blinker_test(BlinkerCore *blinker_p) {
   blinker_p->set_blink_rate(0, 500);   
   blinker_p->set_blink_rate(1, 1000); 
   blinker_p->set_blink_rate(2, 2000);  
   blinker_p->set_blink_rate(3, 4000); 
}


int main() {
   blinker_test(&blinker);

   while (1) {
      sleep_ms(1000); 
   }
}
