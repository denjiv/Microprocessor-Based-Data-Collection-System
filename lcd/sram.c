#include "sram.h"

// Read temperature data
void read_sram_temp(uint16_t * val) {
    read_sram_sensor(temp_ptr, val);
}

// Read salinity data
void read_sram_sal(uint16_t * val) {
    read_sram_sensor(sal_ptr, val);
}

// Read carbon data
void read_sram_car(uint16_t * val) {
    read_sram_sensor(car_ptr, val);
}

// Reas frequency data
void read_sram_freq(uint16_t * val) {
    read_sram_sensor(freq_ptr, val);
}

// Read sensor data from the specified address
void read_sram_sensor(uint8_t strAddr, uint16_t * val) {
    for (int i=0; i < 16; i++) {
        *(val+i) = read_sram_16(strAddr);
        strAddr = (strAddr & HEAD_MASK) | ((strAddr + 0x02) & (~HEAD_MASK));
    }
}

// Write temperature data into the octo buffer
void write_sram_temp(uint16_t data) {
    write_sram_16(temp_ptr, data);
    temp_ptr = (temp_ptr & HEAD_MASK) | ((temp_ptr + 0x02) & (~HEAD_MASK));
}

// Write carbon data into the octo buffer
void write_sram_car(uint16_t data) {
    write_sram_16(car_ptr, data);
    car_ptr = (car_ptr & HEAD_MASK) | ((car_ptr + 0x02) & (~HEAD_MASK));
}

// Write the Salinity data into octo buffer
void write_sram_sal(uint16_t data) {
    write_sram_16(sal_ptr, data);
    sal_ptr = (sal_ptr & HEAD_MASK) | ((sal_ptr + 0x02) & (~HEAD_MASK));
}

// Write the frequency into the octo buffer
void write_sram_freq(uint16_t data) {
    write_sram_16(freq_ptr, data);
    freq_ptr = (freq_ptr & HEAD_MASK) | ((freq_ptr + 0x02) & (~HEAD_MASK));
}

// Read 16 bits of data
uint16_t read_sram_16(uint8_t addr) {
    uint16_t val = 0;
    for (int i=1; i >= 0; i--) {
        val = val | ((uint16_t)read_sram(addr + (1 - i)) << (i * 8));
    }
    return val;
}

// Writing 16 bits of data
void write_sram_16(uint8_t addr, uint16_t data) {
    uint16_t mask = 0xFF00;
    for (int i=1; i >= 0; i--) {
        write_sram(addr + (1 - i), (uint8_t)(((uint16_t)data & mask) >> (i * 8)));
        mask = mask >> 8;
    }
}

// Writing instruction of SRAM
void write_sram(uint8_t addr, uint8_t data) {
    // Combine data and address
    uint16_t ser_out = ((uint16_t)data << 8) | addr;
    uint16_t mask = 0x8000;

    OE_SRAM_SetHigh();

    __delay_ms(1);

    // Send address and data through SRAM
    for (int i=0; i < 16; i++) {
        if (ser_out & mask) {
            S_ADR_SetHigh();
        } else {
            S_ADR_SetLow();
        }
        CLK_OUT_pulse();
        mask = mask >> 1;
    }
    CLK_OUT_pulse();
    WE_SRAM_pulse();
}

// Reading instruction of SRAM
uint8_t read_sram(uint8_t addr) {
    uint8_t mask = 0x80;
    OE_SRAM_SetLow();
    CLK_INH_SetHigh();

    // Specify address through shift register
    for (int i=0; i < 8; i++) {
        if (addr & mask) {
            S_ADR_SetHigh();
        } else {
            S_ADR_SetLow();
        }
        CLK_OUT_pulse();
        mask = mask >> 1;
    }
    CLK_OUT_pulse();

    __delay_ms(1);
    SH_LD_pulse();

    uint8_t val = 0x00;

    CLK_IN_SetHigh();
    CLK_INH_SetLow();

    // Recieve values through shift register
    for (int i=7; i >= 0; i--) {
        CLK_IN_SetHigh();
        __delay_ms(1);
        CLK_IN_SetLow();
        val = ((uint8_t)S_DI_GetValue() << i) | val;
        __delay_ms(1);
    }
    CLK_INH_SetHigh();
    return val;
}

// WE_SRAM pin toggle
void WE_SRAM_pulse() {
    WE_SRAM_SetHigh();
	__delay_ms(1);
	WE_SRAM_SetLow();
	__delay_ms(1);
	WE_SRAM_SetHigh();
	__delay_ms(1);
}

// CLK_OUT pin toggle
void CLK_OUT_pulse() {
    CLK_OUT_SetLow();
	__delay_ms(1);
	CLK_OUT_SetHigh();
	__delay_ms(1);
	CLK_OUT_SetLow();
	__delay_ms(1);
}

//  SH_LD pin toggle
void SH_LD_pulse() {
  SH_LD_SetHigh();
	__delay_ms(1);
	SH_LD_SetLow();
	__delay_ms(1);
	SH_LD_SetHigh();
	__delay_ms(1);
}

// SRAM initialization
void sram_init() {
    SH_LD_SetHigh();
    OE_SRAM_SetHigh();
    WE_SRAM_SetHigh();
    CLK_INH_SetLow();
}
