#include "chu_init.h"
#include "gpio_cores.h"
#include "ps2_core.h"
#include "spi_core.h"
#include "vga_core.h"

// === CONFIG ===
#define COMBO_MAX 9
#define COMBO_MIN 4

// === GLOBALS ===
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
SpiCore jstk_spi(get_slot_addr(BRIDGE_BASE, S4_JSTK));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpoCore motor(get_slot_addr(BRIDGE_BASE, S14_PMOD_GPO));
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
FrameCore frame(FRAME_BASE);

int combo_len = 4;
char combo[COMBO_MAX] = {'U', 'D', 'L', 'R'};
char input_buffer[COMBO_MAX] = {'U', 'D', 'L', 'R'};
int input_index = 0;
bool locked = false;

// Write and Clear
void write_msg(OsdCore *osd_p, const char *msg, int col_start, int row, int length) {
   for (int i = 0; i < length; i++) osd_p->wr_char(col_start + i, row, msg[i]);
}

void clear_msg(OsdCore *osd_p, int col_start, int row, int length) {
   for (int i = 0; i < length; i++) osd_p->wr_char(col_start + i, row, ' ');
}

// === JOYSTICK HANDLING ===
char get_joystick_direction(SpiCore *spi_p) {
   const uint8_t DUMMY = 0x00;
   uint8_t rx[5] = {0};

   spi_p->set_freq(1000000);
   spi_p->set_mode(0, 0);

   spi_p->assert_ss(0);
   for (int i = 0; i < 5; i++) {
      rx[i] = spi_p->transfer(DUMMY);
      sleep_us(15);
   }
   spi_p->deassert_ss(0);

   uint16_t x = ((rx[1] << 8) | rx[0]) & 0x03FF;
   uint16_t y = ((rx[3] << 8) | rx[2]) & 0x03FF;

   if (x == 0 && y == 0) return 0;
   if (x > 1023 || y > 1023) return 0;

   static char last_dir = 0;
   char current_dir = 0;

   if (x > 750) current_dir = 'R';
   else if (x < 100) current_dir = 'L';
   else if (y > 800) current_dir = 'U';
   else if (y < 250) current_dir = 'D';

   if (current_dir != 0 && current_dir != last_dir) {
      last_dir = current_dir;
      return current_dir;
   }

   if (current_dir == 0) last_dir = 0;
   return 0;
}

void log_direction(char dir) {
   uart.disp("Joystick: ");
   uart.disp(dir);
   uart.disp("\n\r");
   input_buffer[input_index % combo_len] = dir;
   input_index++;
}

// === LOCK MECHANISM ===
void single_step(int dir) {
   const int steps[4] = {0x01, 0x02, 0x04, 0x08};
   static int current_step = 0;
   for (int i = 0; i < 512; i++) {
      motor.write(steps[current_step]);
      current_step = (current_step + dir + 4) % 4;
      sleep_ms(5);
   }
   motor.write(0x00);
}

void lock_motor() {
   uart.disp("LOCKING...\n\r");
   write_msg(&osd,"LOCKING...  ", 40, 19, 12);

   for (int i = 0; i < combo_len; i++) {
      input_buffer[i] = 'X';
   }

   single_step(1);
   locked = true;
}

void unlock_motor() {
   uart.disp("UNLOCKING...\n\r");
   write_msg(&osd,"UNLOCKING...", 40, 19, 12);
   single_step(-1);
   locked = false;
}

// === COMBO LOGIC ===
bool combo_matches() {
   if (input_index < combo_len) return false;
   for (int i = 0; i < combo_len; i++) {
      int idx = (input_index - combo_len + i) % combo_len;
      if (input_buffer[idx] != combo[i]) return false;
   }
   return true;
}

void drain_keyboard() {
   char junk;
   while (ps2.get_kb_ch(&junk)) {}
}

void read_combo_with_joystick(char *dest, int length, OsdCore *osd_p, int col, int row) {
   int count = 0;
   while (count < length) {
      char d = 0;
      while ((d = get_joystick_direction(&jstk_spi)) == 0) sleep_ms(10);
      dest[count++] = d;

      clear_msg(osd_p, col, row, length * 2);
      for (int i = 0; i < count; i++) osd_p->wr_char(col + i * 2, row, dest[i]);
      while (get_joystick_direction(&jstk_spi) != 0) sleep_ms(10);
   }
   drain_keyboard();
}

// === DISPLAY / OSD ===

void update_combo_osd(OsdCore *osd_p, int col_start, int row_start) {
   const char *len_msg = "Current Combination Length: ";
   for (int i = 0; i < 28; i++) osd_p->wr_char(col_start + i, row_start, len_msg[i]);
   osd_p->wr_char(col_start + 28, row_start, combo_len + '0');

   for (int i = 0; i < COMBO_MAX; i++) osd_p->wr_char(col_start + i, row_start + 1, ' ');

   int start_idx = (input_index >= combo_len) ? (input_index - combo_len) : 0;
   int count = (input_index >= combo_len) ? combo_len : input_index;
   for (int i = 0; i < count; i++) {
      int buffer_idx = (start_idx + i) % combo_len;
      osd_p->wr_char(col_start + (combo_len - count + i * 2), row_start + 1, input_buffer[buffer_idx]);
   }
}

void show_current_combo() {
   uart.disp("Current combo (");
   uart.disp(combo_len);
   uart.disp("): ");
   for (int i = 0; i < combo_len; i++) {
      uart.disp(combo[i]);
      uart.disp(" ");
   }
   uart.disp("\n\r");
}

void prompt_menu() {
   show_current_combo();
   uart.disp("--- Lock System Menu ---\n\r");
   uart.disp("1: Set combo length (4-9) and define combo\n\r");
   uart.disp("2: Set new combination\n\r");
   uart.disp("3: Lock the system\n\r");
}

// Displays only the current combo
void display_current_combo_osd(OsdCore *osd_p, int col, int row) {
   clear_msg(osd_p, col, row, 30);
   write_msg(osd_p, "Current combo:", col, row, 14);
   for (int i = 0; i < combo_len; i++) {
      osd_p->wr_char(col + 15 + i * 2, row, combo[i]);
   }
}

// Displays the regular menu (without touching the combo line)
void display_menu_osd(OsdCore *osd_p, int col, int row) {
   write_msg(osd_p, "--- Lock System Menu ---", col, row , 24);
   write_msg(osd_p, "1: Set combo length (4-9)", col, row + 1, 27);
   write_msg(osd_p, "2: Set new combination", col, row + 2, 24);
   write_msg(osd_p, "3: Lock the system", col, row + 3, 20);
}

void display_unlock_prompt_osd(OsdCore *osd_p, int col, int row) {
   write_msg(osd_p, "--- Lock System Menu ---", col, row, 24);
   write_msg(osd_p, "Enter correct combination", col, row + 1, 26);
   write_msg(osd_p, "3: Unlock the system", col, row + 2, 21);
}


void update_lock_status(bool locked, OsdCore *osd_p) {
   if (locked) write_msg(osd_p, "Lock Status: Locked    ", 35, 20, 21);
   else        write_msg(osd_p, "Lock Status: Unlocked  ", 35, 20, 21);
}

void clear_menu_osd(OsdCore *osd_p, int col, int row) {
   // Clears 4 rows starting from row + 1
   for (int i = 0; i <= 4; i++) {
      clear_msg(osd_p, col, row + i, 30);
   }
}


// === MAIN ===
int main() {
   uart.disp("Starting Lock System\n\r");
   write_msg(&osd, "Starting Lock System", 5, 10, 20);
   clear_msg(&osd, 5, 10, 20);

   update_lock_status(false, &osd);
   prompt_menu();
   input_index = combo_len;
   //display_current_combo_osd(&osd, 5, 0);
   display_current_combo_osd(&osd, 5, 0);
   display_menu_osd(&osd, 5, 2);
   update_combo_osd(&osd, 30, 25);

   while (1) {
      char dir = get_joystick_direction(&jstk_spi);
      if (dir) {
         log_direction(dir);
         update_combo_osd(&osd, 30, 25);
      }

      char ch;
      if (ps2.get_kb_ch(&ch)) {
    	  if (!locked && ch == '1') {
    		  clear_menu_osd(&osd, 5, 2);
    	     char new_len = 0;

    	     while (1) {
    	        uart.disp("Enter new length (4-9): ");
    	        clear_msg(&osd, 30, 25, 30);
    	        write_msg(&osd, "Enter new length (4-9):   ", 30, 25, 25);
    	        clear_msg(&osd, 30, 26, 30);

    	        while (!ps2.get_kb_ch(&new_len));

    	        if (new_len >= '4' && new_len <= '9') {
    	           combo_len = new_len - '0';

    	           clear_msg(&osd, 30, 25, 30);
    	           write_msg(&osd, "Enter ", 30, 25, 6);
    	           osd.wr_char(36, 25, combo_len + '0');
    	           write_msg(&osd, "directions", 38, 25, 11);

    	           clear_msg(&osd, 30, 24, 30);
    	           read_combo_with_joystick(combo, combo_len, &osd, 30, 26);

    	           for (int i = 0; i < combo_len; i++) input_buffer[i] = combo[i];
    	           input_index = combo_len;
    	           clear_msg(&osd, 5, 13, 30);

    	           uart.disp("New combo set.\n\r");
    	           prompt_menu();
    	           display_current_combo_osd(&osd, 5, 0);
    	           display_menu_osd(&osd, 5, 2);
    	           update_combo_osd(&osd, 30, 25);
    	           break;  // exit loop
    	        } else {
    	           write_msg(&osd, "Invalid length.", 35, 22, 16);
    	           sleep_ms(300);
    	           clear_msg(&osd, 35, 22, 16);
    	        }
    	     }
    	  }


         else if (!locked && ch == '2') {
        	clear_menu_osd(&osd, 5, 2);
            write_msg(&osd, "Set new combo:", 30, 24, 15);
            clear_msg(&osd, 30, 26, 30);
            read_combo_with_joystick(combo, combo_len, &osd, 30, 26);
            for (int i = 0; i < combo_len; i++) input_buffer[i] = combo[i];
            input_index = combo_len;

            uart.disp("New combo set.\n\r");
            clear_msg(&osd, 30, 24, 30);
            prompt_menu();
            display_current_combo_osd(&osd, 5, 0);
            display_menu_osd(&osd, 5, 2);
            update_combo_osd(&osd, 30, 25);
         }

         else if (!locked && ch == '3') {
            lock_motor();
            update_lock_status(true, &osd);
            update_combo_osd(&osd, 30, 25);
            clear_msg(&osd, 40, 19, 12);
            uart.disp("System locked.\n\r");
            clear_menu_osd(&osd, 5, 2);
            display_unlock_prompt_osd(&osd, 5, 2);
         }

         else if (locked && ch == '3') {
            if (combo_matches()) {
               unlock_motor();
               update_lock_status(false, &osd);
               prompt_menu();
               clear_menu_osd(&osd, 5, 2);
               display_current_combo_osd(&osd, 5, 0);
               display_menu_osd(&osd, 5, 2);
               update_combo_osd(&osd, 30, 25);
               clear_msg(&osd, 40, 19, 12);
            } else {
               uart.disp("Wrong combo.\n\r");
               write_msg(&osd, "Wrong combo.", 35, 22, 13);
               sleep_ms(300);
               clear_msg(&osd, 35, 22, 13);
            }
         }
      }

      sleep_ms(30);
   }
}
