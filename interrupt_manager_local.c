// code sample from the MCC generated interupt handler file
// Interrupt manager is set to set flags, which are handled in the main application.
void interrupt INTERRUPT_InterruptManager (void) {
    // interrupt handler
    if(INTCONbits.PEIE == 1 && PIE2bits.BCL1IE == 1 && PIR2bits.BCL1IF == 1)
    { // bus collision interrupt
        I2C1_BusCollisionISR();
    }
    else if(INTCONbits.PEIE == 1 && PIE1bits.SSP1IE == 1 && PIR1bits.SSP1IF == 1)
    { // i2c interrupt
        I2C1_ISR();
    }
    else if(INTCONbits.PEIE == 1 && PIE1bits.TX1IE == 1 && PIR1bits.TX1IF == 1)
    { // uart interrupt
        EUSART1_Transmit_ISR();
    }
    else if(INTCONbits.PEIE == 1 && PIE1bits.RC1IE == 1 && PIR1bits.RC1IF == 1)
    { //uart interrupt
        uart_in = RCREG1;
        if (uart_in == 'c') {
            TEST_LED_Toggle();
        }
    }
