#include <avr/io.h>
#include <util/twi.h>
#include "ControlUnit.h"
#include <avr/interrupt.h>



void InitPorts(void){
    //  Digital outputs
    DDRD    |= ( (1<<INA) | (1<<INB) | (1<<SEL0) );  //  Define direction
    DDRB    |= (1<<PWM);
    PORTD   &= ~( (1<<INA) | (1<<INB) | (1<<SEL0) ); //  Sets outputs low
    PORTB   &= ~(1<<PWM);
    
    //  Digital inputs
   DDRD    &= ~( (1<<HALL_S1) | (1<<HALL_S2) );  //  Define direction
   PORTD   |= ( (1<<HALL_S1) | (1<<HALL_S2) );  //  Enable pull-up

}

void InitAdc(void){
    ADMUX   |= (1<<REFS0); // AVCC with external capacitor at AREF pin
    ADMUX   |= (ADMUX & 0xF0) | (ADC_CHANNEL & 0x0F);  //  Select ADC Channel 
    ADMUX   &= ~(1<<ADLAR); //  Right adjust result
    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //  Set prescaller to 128
    ADCSRA  |= (1<<ADEN); //  Enable ADC
}

uint16_t ReadAdc(void){
    //single conversion mode
    ADCSRA |= (1<<ADSC);
    // wait until ADC conversion is complete
    while( ADCSRA & (1<<ADSC) );
    return ADC;
}

void SetupPwm(uint8_t _dutyCycle){
    uint16_t occr1 = (PWM_105HZ+1)/100*_dutyCycle-1;
    //  Non-inverting mode
    TCCR1A  |= (1<<COM1A1) | (0<<COM1A0);
    //  Mode13: Fasr-PWM with TOP=ICR1 
    TCCR1A  |= (1<<WGM11) | (0<<WGM10);
    TCCR1B  |= (1<<WGM13) | (1<<WGM12);
    //  No prescaler
    TCCR1B  |= (0<<CS12) | (1<<CS11) | (1<<CS10);

    ICR1H   = 0xFF & (PWM_105HZ>>8);
    ICR1L   = 0xFF & PWM_105HZ;

    OCR1AH  = 0xFF & (occr1>>8);
    OCR1AL  = 0xFF & occr1;

}

void SetupExternalInterrupts(uint8_t _state) {
    //  Falling edge of INT0/1 generates an interrupt
   // EICRA |= ( (1<<ISC11) | (0<<ISC10) );
    EICRA |= ( (1<<ISC01) | (0<<ISC00) );
    //  Enable/Disable interrupts request
    EIMSK |= ( (_state<<INT0) | (_state<<INB) );
}

void SetupSpi (void){
    //  SPI Pins
        //  Set MISO output, all others input
    DDRB |= (1<<MISO);
    //  Enable global interrupt
    //  Clock Polarity Leading Edge-Rising, Trailing Edge-Falling
    //  Clock Phase Leading Edge-Sample, Trailing Edge-Setup
    //  Enabela SPI
    SPCR |= (1<<SPE);
    SPCR |= (1<<SPIE);

    return;
}

/*void SpiSend (uint8_t data) {
    // load data into register
    SPDR = data;

    // Wait for transmission complete
    while(!(SPSR & (1 << SPIF)));

}*/
