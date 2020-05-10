#include "ControlUnit.h"
#include "Utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "printf_tools.h"

//  Auxiliar flags
uint8_t FLAG_INITIAL_SETUP_FINISHED = 0x00;
uint8_t FLAG_SAFE_RESET_FINISHED    = 0x00;
uint8_t FLAG_TX_POSITION            = 0x00;
uint8_t FLAG_DISPLACEMENT_FINISHED  = 0x00;
uint8_t FLAG_PWM_ENABLE             = 0x00;

//  Counters for hall sensors
uint16_t count_pulse_s1             = 0x0000;
uint16_t count_pulse_s2             = 0x0000;

//  Calculation of hall sensors
uint8_t pulse_rate                 = 0x00;
uint8_t pulse_rate_s1              = 0x00;
uint8_t pulse_rate_s2              = 0x00;

//  SPI buffers
uint8_t spiCodeRx                  = 0x00;
uint8_t spiCodeTx                  = 0x00;

//  Functions for operating modes
void Wait             (void);
void InitialSetup     (void);    
void SafeReset        (void);           
void BasicExtended    (void);
void BasicRefracted   (void);
void AdvancedExtended (void);
void AdvancedRefracted(void);


ISR(INT0_vect) {
  count_pulse_s1++;
}

ISR(INT1_vect) {
  count_pulse_s2++;
}

ISR(SPI_STC_vect){
  spiCodeRx = SPDR;
  
  if (spiCodeRx == 0xFF) //  Mode transmit
    SPDR = spiCodeTx;
  else 
    SPDR = spiCodeRx;   //   Mode receive
}

int main () {
  cli();
  
  init_USART(); 
  init_printf_tools();
  
  InitPorts();
  //InitAdc();
  //SetupExternalInterrupts(ENABLE);
  //SetupPwm(50);
  SetupSpi();
  
  sei();

  uint8_t current_state = WAIT;
  uint8_t next_state    = WAIT;

  while (1) {
    
    if (current_state != next_state) {
      current_state = next_state;
      sprintf(msg, "%x\n", current_state);
      UARTPrintf(msg);
    }
        
    switch (current_state) {    
      case WAIT:
        Wait();
      break;
      case INITIAL_SETUP:
        InitilSetup();
      break;
      case BASIC_EXTENDED:
        BasicExtended(); 
      break;
      case BASIC_REFRACTED:
        BasicRefracted(); 
      break;
      case ADVANCED_EXTENDED:
        AdvancedExtended()
      break;
      case ADVANCED_REFRACTED:
        AdvancedRefracted();
      break;
      default:
        Wait();
      break;    
    }

    switch (current_state) {
      case WAIT:
        if (spiCodeRx == INITIALSETUP())
          next_state = INITIAL_SETUP;
        else if (spiCodeRx == SAFERESET()) 
          next_state = SAFE_RESET;
        else if (spiCodeRx == BASICEXTENDED())
          next_state = BASIC_EXTENDED;
        else if (spiCodeRx == BASICREFRACTED())
          next_state = BASIC_REFRACTED;
        else if (spiCodeRx == ADVANCEDEXTENDED())
          next_state = ADVANCED_EXTENDED;
        else if (spiCodeRx == ADVANCEDREFRACTED())
          next_state = ADVANCED_REFRACTED;
      break;
      case INITIAL_SETUP:
        if (FLAG_INITIAL_SETUP_FINISHED == SET)
          next_state = WAIT;
      break;
      case SAFE_RESET:
        if (FLAG_SAFE_RESET_FINISHED == SET)
          next_state = WAIT;
      break;
      case BASIC_EXTENDED:
        if (spiCodeRx == STOP())
          next_state = WAIT;
        else if (HALL_S1 == LOW && HALL_S2 == LOW)
          next_state = WAIT; 
      break;
      case BASIC_REFRACTED:
        if (spiCodeRx == STOP())
          next_state = WAIT;
        else if (HALL_S1 == LOW && HALL_S2 == LOW)
          next_state = WAIT;       
      break;
      case ADVANCED_EXTENDED:
        if (FLAG_DISPLACEMENT_FINISHED == SET)
          next_state = WAIT;
      break;
      case ADVANCED_REFRACTED:
        if (FLAG_DISPLACEMENT_FINISHED == SET)
          next_state = WAIT;
      break;   
      default:
        next_state = WAIT;
      break;
    }
  }
}

void Wait(void) {
  INA  = 0;
  INB  = 0;
  SEL0 = 0;
  PWM  = 0;
}

void InitialSetup(void) {
  uint8_t initial_setup_current_state = IS_INIT;
  uint8_t initial_setup_next_state    = IS_INIT;
  while (1) {
    switch (initial_setup_current_state){
      case IS_INIT:
        if ( ( ( PIND & (1<<HALL_S1) ) == 0 ) && ( ( ( PIND & (1<<HALL_S2) ) == 0 ))
          initial_setup_next_state = IS_EXTENDED;
      break;
      
      default:
      break;
    }
    switch (initial_setup_current_state) {
      case IS_INIT:
        INA  = LOW;
        INB  = LOW;
        SEL0 = LOW;
        PWM  = LOW;
        SetupExternalInterrupts(DISABLE);
      break;
      case IS_EXTENDED:
        INA  = HIGH ;
        SEL0 = HIGH ;
        PWM  = HIGH ;
        INB  = LOW  ;
      break;
      case IS_STOP:
        INA             = LOW   ;
        SEL0            = LOW   ;
        PWM             = LOW   ;
        INB             = LOW   ;
        count_pulse_s1  = 0x0000;
        count_pulse_s2  = 0x0000;
        SetupExternalInterrupts(ENABLE);
      break;
      case IS_REFRACTED:
        INA  = LOW  ;
        SEL0 = LOW  ;
        INB  = HIGH ;
        PWM  = HIGH ;        
      break;
      case IS_SEND_PULSE_RATE:
        INA  = LOW;
        INB  = LOW;
        SEL0 = LOW;
        PWM  = LOW;
        SetupExternalInterrupts(DISABLE);
        pulse_rate_s1 = (uint8_t)((count_pulse_s1)/stroke_lenght);
        pulse_rate_s2 = (uint8_t)((count_pulse_s2)/stroke_lenght);;             
        //  Pulse avarage of two hall sensors.
        pulse_rate = (pulse_rate_s1 + pulse_rate_s2) / 2;
        spiCodeTx = pulse_rate;
        //char msg[10];
        //sprintf(msg, "%x\n", pulse_rate_s2);
        //UARTPrintf(msg);
      break;
      case IS_FINISHED:
        INA  = LOW;
        INB  = LOW;
        SEL0 = LOW;
        PWM  = LOW;
        SetupExternalInterrupts(DISABLE);
        while(spiCodeRx != SUCESS());
        FLAG_INITIAL_SETUP_FINISHED = SET;
      break;
      default:
        INA  = LOW;
        INB  = LOW;
        SEL0 = LOW;
        PWM  = LOW;
        SetupExternalInterrupts(DISABLE);
      break;
    }  
  }
  

}    
void SafeReset(void) {

}           
void BasicExtended(void) {

}
void BasicRefracted(void) {

}
void AdvancedExtended(void) {

}
void AdvancedRefracted(void) {

}

