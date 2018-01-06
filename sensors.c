#include "sensors.h"


void salinity() {
    double converted = 0;

    ADC_Initialize();
    uint16_t val = ADC_GetConversion(SALINITY);
    write_sram_sal(val);
    //resolution of adc = 0.4/(2^10 - 1) = 0.00039
    for (int i = 0; i < 10; ++i) {
  		unsigned int bit0 = (val >> i) & 1;
  		converted += pow(2,i) * bit0;
  	}
    converted = converted * 0.001;

    double salinity = 0;
    salinity = 225 * converted - 17.5;

    sprintf(sal_buf+16, "%f", salinity);
}

void carbon () {
    double converted = 0;

    ADC_Initialize();
    uint16_t val = ADC_GetConversion(CARBON);
    write_sram_car(val);
    for (int i = 0; i < 10; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    converted = converted * 0.001;

    double carbon = 0;
    carbon = 1387.76 * converted + 3.1;

    sprintf(car_buf+16, "%6f", carbon);
}


void temperature () {
    double converted = 0;

    ADC_Initialize();
    uint16_t val = ADC_GetConversion(TEMPERATURE);
    write_sram_temp(val);
    for (int i = 0; i < 10; ++i) {
        unsigned int bit0 = (val >> i) & 1;
        converted += pow(2,i) * bit0;
    }
    converted = converted * 0.001;

    double temp = 275 * converted - 10.75;
    if (temp_buf[13] == 'F') {
        temp = temp * (9.0/5) + 32.0;
    }
    sprintf((void*)(&temp_buf+16), "%6f", temp);
}
