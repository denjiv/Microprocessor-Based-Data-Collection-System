#include "frequency.h"

void getFrequency() {
    FREQ_RST_SetHigh();
    __delay_us(10);
    //NCLR_SetLow();
    FREQ_RST_SetLow();
    //NCLR_SetHigh();

    GATE_SetHigh();
    __delay_ms(1000);
    GATE_SetLow();

    //SHIFT_SetHigh();

    SHIFT_SetLow();
    __delay_ms(1);
    SHIFT_SetHigh();
    FREQ_INH_SetLow();

    unsigned int freq1[8];
    unsigned int freq2[8];
    //SHIFT_SetHigh();
    for (int i = 0; i < 8; i++) {

        FREQ_CLK_SetHigh();
        __delay_ms(1);
        FREQ_CLK_SetLow();
        freq1[i] = FREQ1_GetValue();
        freq2[i] = FREQ2_GetValue();

    }
    FREQ_INH_SetHigh();
    unsigned int frequency = 0;

    for (int i = 4; i < 8; i++) {
        frequency += freq1[i] * pow(2,(i - 4));
    }

    for (int i = 0; i < 8; i++) {
        frequency += freq2[i] * pow(2,(i + 4));
    }

    double converted = frequency * 0.1;

    sprintf(flow_buf+16, "%f", converted);
    strcat(flow_buf,"\n");
}
