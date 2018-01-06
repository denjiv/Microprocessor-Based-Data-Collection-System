/*
 * File:   sram.h
 * Author: Administrator
 *
 * Created on November 1, 2017, 11:00 AM
 */

#ifndef SRAM_H
#define	SRAM_H


#include "mcc_generated_files/mcc.h"

// memory starting postions for each sensors
#define TEMP_STR_PTR 0x00
#define CAR_STR_PTR 0x20
#define SAL_STR_PTR 0x40
#define FREQ_STR_PTR 0x50

// mask for the header of the memory
#define HEAD_MASK 0xE0

// Pointers that keep track of the octal buffer
uint8_t temp_ptr = TEMP_STR_PTR;
uint8_t car_ptr = CAR_STR_PTR;
uint8_t sal_ptr = SAL_STR_PTR;
uint8_t freq_ptr = FREQ_STR_PTR;

// Pulse definitions for signals
void WE_SRAM_pulse();
void CLK_OUT_pulse();
void SH_LD_pulse();

// Low level writing and reading to sram
void write_sram(uint8_t addr, uint8_t data);
uint8_t read_sram(uint8_t addr);

// Top level writing and reading form sram
uint16_t read_sram_16(uint8_t addr);
void write_sram_16(uint8_t addr, uint16_t data);

// Writing to each sensor
void write_sram_freq(uint16_t data);
void write_sram_sal(uint16_t data);
void write_sram_car(uint16_t data);
void write_sram_temp(uint16_t data);

// reading from sensor data (16 measurements)
void read_sram_sensor(uint8_t strAddr, uint16_t * val);

void read_sram_temp(uint16_t * val);
void read_sram_sal(uint16_t * val);
void read_sram_car(uint16_t * val);
void read_sram_freq(uint16_t * val);

void sram_init();

#endif	/* SRAM_H */