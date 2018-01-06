// Toggle the enable pin
void write_en(void) {
  EN = 0;
	__delay_us(1);
	EN = 1;
	__delay_us(1);
	EN = 0;
	__delay_us(1);
}

// Shift register clock toggle
void sr_clk_pulse(void) {
	SR_CLK = 0;
	__delay_us(1);
	SR_CLK = 1;
	__delay_us(1);
	SR_CLK = 0;
	__delay_us(1);
}

// Storage register clock toggle
void st_clk_pulse(void) {
	ST_CLK = 0;
	__delay_us(1);
	ST_CLK = 1;
	__delay_us(1);
	ST_CLK = 0;
	__delay_us(1);
}

// Write to the lcd display based on pin values in val (shift reg)
void write_lcd(unsigned char val) {
  unsigned char i;
  unsigned char pin_out;
  unsigned char mask = 128;
	__delay_ms(1);
	for(i = 0; i < 8; i++) {
        pin_out = val & mask;
        if (pin_out == 0) {
        	SER = 0;
        } else {
          SER = 1;
        }
  	mask = mask >> 1;
  	sr_clk_pulse();
	}
	st_clk_pulse(); // push finalized output out of storage register
	write_en(); // allow LCD to pick up data from stortge register
}

// Initialize the display
void init_display(void) {
  RS = 0;
  __delay_ms(21);
	write_lcd(0x30);
	__delay_ms(5);
	write_lcd(0x30);
	__delay_ms(1);
	write_lcd(0x30);
	__delay_ms(1);

	write_lcd(0x38); // function set
	write_lcd(0x08); // Display OFF
	write_lcd(0x01); // Clear Display
	__delay_ms(1);
	write_lcd(0x06); // Entry Mode
	write_lcd(0x0F); // Display ON
	// Initialization Complete

  __delay_ms(1);
	RS = 1;
}

// Called when user wants to send info to device
void device_write(const char* bufSource) {
    unsigned char i = 0;
    __delay_ms(1);

    while (bufSource[i] != '\n') {
        RS = 0;
        // special characters
        // '^' is the character for LCD clear
        if (bufSource[i] == '@') {
            write_lcd(0x01);
            cur_pos = 0;
        // '^' is the character for cursor to shift to opposite row
        } else if (bufSource[i] == '^') {
        	// if cursor on first line, shift to bottom line
        	if (cur_pos <= 15) {
                write_lcd((unsigned char) cur_pos + 64 + 128);
                cur_pos = cur_pos + 16;
        	} else if (cur_pos <= 31) {
                write_lcd((unsigned char) cur_pos + 48 - 64 + 128);
                cur_pos = cur_pos - 16;
            }
        // '<' is the character for cursor shift left
        } else if (bufSource[i] == '<') {
        	// Edge case, set DD RAM to last address of second line of screen
        	if (cur_pos == 0) {
        		write_lcd(0xCF);
        		cur_pos = 31;
        	// Edge case, set DD RAM to last address of first line of screen
        	} else if (cur_pos == 16) {
        		write_lcd(0x8F);
        		cur_pos = 15;
        	} else {
        		write_lcd(0x10);
            cur_pos--;
        	}
        // '>' is the character for cursor shift right
        } else if (bufSource[i] == '>') {
        	// Edge case, set DD RAM to first address of second line of screen
        	if (cur_pos == 15) {
        		write_lcd(0xc0);
        		cur_pos = 16;
        	// Edge case, set return home
        	} else if (cur_pos == 31) {
        		write_lcd(0x02);
        		cur_pos = 0;
        	} else {
        		write_lcd(0x14);
            cur_pos++;
        	}
        // normal character entry
        } else {
	        RS = 1;
	        write_lcd((unsigned char) bufSource[i]);
	        cur_pos++;
        }
        i++;

        // if first line is full, set curser to line 2
        RS = 0;
        if (cur_pos == 16) {
            write_lcd(0xc0);
        // if LCD is full, set cursor home
        } else if (cur_pos == 32) {
            write_lcd(0x02);
            cur_pos = 0;
        }
    }
}
