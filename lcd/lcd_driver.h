#ifndef LCD_DRIVERS_H
#define LCD_DRIVERS_H

// Define Shift Register Pins
#define SER 2
#define SR_CLK 0
#define ST_CLK 1

// Define LCD Pins
#define RS 3
#define EN 4

unsigned char cur_pos = 0; // current cursor position

void write_en(void);
void sr_clk_pulse(void);
void st_clk_pulse(void);
void write_lcd(uint8_t val);
void init_display(void);
void device_write(const char* bufSource, signed char bufCount);

#endif
