/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC18F25K22
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#define SLAVE_I2C_GENERIC_RETRY_MAX     100

// UART send function
void sendUART(char * val) {
    int i = 0;
    while (val[i] != '\0') {
        EUSART1_Write(val[i]);
        i++;
    }
    EUSART1_Write('\n');
}

// Bit to double salinity calculation
double send_sal(uint16_t val) {
    double converted = 0;
    for (int i = 0; i < 10; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    converted = converted * 0.001;

    double salinity = 0;
    salinity = 225 * converted - 17.5;
    return salinity;
}

// Bit to double temperature calculation
double send_temp(uint16_t val) {
    double converted = 0;
    for (int i = 0; i < 10; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    converted = converted * 0.001;

    double temperature = 300 * converted - 40;
    return temperature;
}

// Bit to double carbon calculation
double send_car(uint16_t val) {
    double converted = 0;
    for (int i = 0; i < 10; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    converted = converted * 0.001;

    double carbon = 0;
    carbon = 1387.76 * converted + 3.1;
    return carbon;
}

// Bit to double flow rate calculation
double send_flow(uint16_t val) {
    int converted = 0;
    for (int i = 0; i < 12; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    double flowr = 0;
    flowr = converted * 0.1;
    return flowr;
}

void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // define onstants
    uint16_t dataAddress = 0x10;
    uint8_t         writeBuffer[3];
    uint16_t        timeOut;
    I2C1_MESSAGE_STATUS status;
    uint8_t slave_out[33];
    int size_buf = 0;
    bool read_i2c  = false;
    bool tempC = true;
    uint8_t last_sensor = 0;
    uint8_t station = 0x18;
    while (1)
    {
        // check for station
        status = I2C1_MESSAGE_PENDING;
        if (uart_in >= 0x31 && uart_in <= 0x3F) {
            station = 0x18 + (uart_in - 0x31);
            uart_in = 0;
        }
        // check for input of uart
        if (uart_in != 0) {
            writeBuffer[0] = (dataAddress >> 8);                // high address
            writeBuffer[1] = (uint8_t)(dataAddress);            // low address

            // data to be written
            writeBuffer[2] = uart_in;
            if (uart_in == 'o') {
                size_buf = 3;
                read_i2c  = true;
            } else if (uart_in == 'm') {
                size_buf = 33;
                read_i2c = true;
            } else {
                read_i2c = false;
                if (uart_in == 'C') {
                    tempC = true;
                } else if (uart_in == 'F') {
                    tempC = false;
                } else if (uart_in == 's' || uart_in == 't' || uart_in == 'c' || uart_in == 'f') {
                    last_sensor = uart_in;
                }
            }


            // Now it is possible that the slave device will be slow.
            // As a work around on these slaves, the application can
            // retry sending the transaction
            timeOut = 0;
            while(status != I2C1_MESSAGE_FAIL) {
                // write one byte to EEPROM (3 is the number of bytes to write)
                I2C1_MasterWrite(writeBuffer, 3, station, &status);

                // wait for the message to be sent or status has changed.
                while(status == I2C1_MESSAGE_PENDING);

                if (status == I2C1_MESSAGE_COMPLETE)
                    break;

                // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                //               or I2C1_DATA_NO_ACK,
                // The device may be busy and needs more time for the last
                // write so we can retry writing the data, this is why we
                // use a while loop here

                // check for max retry and skip this byte
                if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                    break;
                else
                    timeOut++;
            }

            if (status == I2C1_MESSAGE_COMPLETE && read_i2c) {
                    // this portion will read the byte from the memory location.
                    timeOut = 0;
                    while(status != I2C1_MESSAGE_FAIL) {
                        // write one byte to EEPROM (2 is the count of bytes to write)
                        I2C1_MasterRead(slave_out, size_buf, station, &status);

                        // wait for the message to be sent or status has changed.
                        while(status == I2C1_MESSAGE_PENDING);

                        if (status == I2C1_MESSAGE_COMPLETE)
                            break;

                        // if status is  I2C1_MESSAGE_ADDRESS_NO_ACK,
                        //               or I2C1_DATA_NO_ACK,
                        // The device may be busy and needs more time for the last
                        // write so we can retry writing the data, this is why we
                        // use a while loop here

                        // check for max retry and skip this byte
                        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
                            break;
                        else
                            timeOut++;
                    }
                    uint16_t data = 0;
                    double val;
                    char output[50];
                    // parse the data from the slave
                    for (int i = 1; i<size_buf; i++) {
                        if (!(i%2)) {
                            data = data + ((uint16_t)slave_out[i] << 8);
                            if (last_sensor == 'c') {
                                val = send_car(data);
                            } else if (last_sensor == 's') {
                                val = send_sal(data);
                            } else if (last_sensor == 'f') {
                                val = send_flow(data);
                            } else {
                                val = send_temp(data);
                                if (tempC == false) {
                                    val = val * (9.0/5) + 32.0;
                                }
                            } // send to UART
                            sprintf(output, "%f", val);
                            sendUART(output);
                            data = 0;data = (uint16_t) slave_out[i];
                        } else {
                            data = (uint16_t) slave_out[i];
                        }

                    }
                    EUSART1_Write('\n');
                }
            uart_in = 0;
        }
    }
}
