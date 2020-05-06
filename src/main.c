#include "ControlUnit.h"
#include "Utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "printf_tools.h"

// Global count of hall sensors pulses
uint16_t count_pulse_s1         = 0x0000;
uint16_t count_pulse_s2         = 0x0000;

uint8_t spiCodeRx = 0;
uint8_t spiCodeTx = 100;

ISR(INT0_vect) {
  count_pulse_s1++;
}

ISR(INT1_vect) {
  count_pulse_s2++;
}

ISR(SPI_STC_vect){
  spiCodeRx = SPDR;
  SPDR = spiCodeRx; 
}

int main () {
  cli();
  init_USART(); 
  init_printf_tools();
  
  InitPorts();
  //InitAdc();
 // SetupExternalInterrupts(ENABLE);
  //SetupPwm(50);
  SetupSpi();
  sei();
  while (1) {
    
  }
}