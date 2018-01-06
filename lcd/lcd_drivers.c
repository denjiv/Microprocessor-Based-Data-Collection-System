#include "lcd_drivers.h"

void clockToggle() {
    LCD_CLK_SetLow();
    __delay_us(100);
    LCD_CLK_SetHigh();
    __delay_us(100);
    LCD_CLK_SetLow();
}

void enableToggle() {
    LCD_CLK_SetLow();
    __delay_us(100);
    LCD_EN_SetHigh();
    __delay_us(100);
    LCD_EN_SetLow();
    __delay_us(100);
}

void clear_screen() {
    sendCommand(0x00);
    __delay_us(150); 
    sendCommand(0x01); //clear display
    __delay_us(17);
}

void temp_screen() {
    temperature();
    sendString(temp_buf);
}

void flow_screen() {
    getFrequency();
    sendString(flow_buf);
}

void sal_screen() {
    salinity();
    sendString(sal_buf);
}

void car_screen() {
    carbon();
    sendString(car_buf);
}

void sendCommand(unsigned char command) {
    unsigned char mask = 16;
    unsigned char pinout;
    
    for (int i = 0; i < 5; i++) {
        pinout = command & mask;
        if (pinout == 0) {
            LCD_SER_SetLow();
        } else {
            LCD_SER_SetHigh();
        }
        clockToggle();
        mask = mask >> 1;
    }
    
    clockToggle();
    enableToggle();
}

void lcd_init() {
    __delay_ms(16); //sleep 15 ms after power on
    sendCommand(0x03); //function set
    __delay_ms(5); //sleep more than 4.1 ms
    sendCommand(0x03); //function set
    __delay_us(150); 
    sendCommand(0x03); //function set
    __delay_us(150); 
    sendCommand(0x02); //function set
    __delay_us(150); 
    
    sendCommand(0x02);
    __delay_us(150); 
    sendCommand(0x08); //function set command:two lines
    __delay_us(150); 
   
    sendCommand(0x00);
    __delay_us(150); 
    sendCommand(0x08);
    __delay_us(150); 
    
    sendCommand(0x00);
    __delay_us(150); 
    sendCommand(0x01); //clear display
    __delay_us(17);
    
    sendCommand(0x00);
    __delay_us(150); 
    sendCommand(0x06); //entry mode: increment mode off, shift operation on
    __delay_us(150); 
    
    sendCommand(0x00);
    __delay_us(150); 
    sendCommand(0x0F); // display on: yes cursor yes blinking
    
}

void temp_convert(unsigned char sign) {
    if (sign == 'C' & temp_buf[13] == 'F') {
        temp = temp * (9/5) + 32;
    } else if (sign == 'F' & temp_buf[13] == 'C') {
        temp = (temp - 32) * (5/9);
    }
    sprintf(temp_buf+16, "%6f", temp);
    sendString(temp_buf);
}

void sendChar(unsigned char letter) {
    LCD_SER_SetHigh();
    clockToggle();
    
    unsigned char mask = 128;
    unsigned char pinout;
    
    for (int i = 0; i < 4; i++) {
        pinout = mask & letter;
        if (pinout == 0) {
            LCD_SER_SetLow();
        } else {
            LCD_SER_SetHigh();
        }
        clockToggle();
        mask = mask >> 1;
    }
    clockToggle();
    enableToggle();
    
    LCD_SER_SetHigh();
    clockToggle();

    for (int i = 0; i < 4; i++) {
        pinout = mask & letter;
        if (pinout == 0) {
            LCD_SER_SetLow();
        } else {
            LCD_SER_SetHigh();
        }
        clockToggle();
        mask = mask >> 1;
    }
    clockToggle();
    enableToggle();
}

void sendString(char* string) {
    int i = 0;
    while ((string[i] != '\n')) {
        
        if (i == 16) {
            //shift cursor to next line
            sendCommand(0xc);
            __delay_us(150);
            sendCommand(0x0);
            __delay_us(150);
        }
        
        sendChar(string[i]);
        i++;
    }
}

