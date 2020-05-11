#include "ControlUnit.h"
#include "Utils.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "printf_tools.h"

//  Auxiliar flags
uint8_t FLAG_INITIAL_SETUP_FINISHED       = 0x00;
uint8_t FLAG_SAFE_RESET_FINISHED          = 0x00;
uint8_t FLAG_RX_STROKE_LENGHT             = 0x00;
uint8_t FLAG_EXTERNAL_INTERRUPTS          = 0x00;
uint8_t FLAG_DISPLACEMENT_FINISHED        = 0x00;
uint8_t FLAG_SETUP_COMMUNICATION_FINISHED = 0x00;


//  Counters for hall sensors
uint16_t count_pulse_s1             = 0x0000;
uint16_t count_pulse_s2             = 0x0000;

//  Calculation of hall sensors
uint8_t pulse_rate                 = 0x00;
uint8_t pulse_rate_s1              = 0x00;
uint8_t pulse_rate_s2              = 0x00;

//  Soft start and stop
uint8_t enable_start_stop_soft     = 0x00;
uint8_t accelaration_rate          = 0x00;
uint8_t accelaration_time          = 0x00;

uint8_t stroke_lenght              = 0x00;

//  SPI buffers
uint8_t spiCodeRx                  = 0x00;
uint8_t spiCodeTx                  = 0x00;

//  Functions for operating modes
void Wait             (void);
void SetupComunication(void);
void InitialSetup     (void);    
void SafeReset        (void);           
void BasicExtended    (void);
void BasicRefracted   (void);
void AdvancedExtended (void);
void AdvancedRefracted(void);

uint8_t ReadHallSensors  (void);
void Stopped  (void);     
void Extended (void);
void Refracted(void);

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
  InitAdc();
  SetupExternalInterrupts(ENABLE);
  SetupSpi();
  
  sei();
  //Refracted();
  uint8_t current_state = WAIT;
  uint8_t next_state    = WAIT;

  char msg[10];
  printf(msg, "STROKE:%d\n", stroke_lenght);
  //UARTPrintf(msg);
  printf(msg, "PULSE_RATE:\n", pulse_rate);
  //UARTPrintf(msg);
  printf(msg, "%ACCELARATION RATE:d\n", accelaration_rate);
  //UARTPrintf(msg);
  printf(msg, "%ACCELARATION_TIME:d\n", accelaration_time);
  //UARTPrintf(msg);

  while (1) {
    switch (current_state) {    
      case WAIT:
        Wait();
      break;
      case SETUP_COMMUNICATION:
        SetupComunication();
      case INITIAL_SETUP:
        InitialSetup();
      break;
      case BASIC_EXTENDED:
        BasicExtended(); 
      break;
      case BASIC_REFRACTED:
        BasicRefracted(); 
      break;
      case ADVANCED_EXTENDED:
        AdvancedExtended();
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
        else if (spiCodeRx == SETUPCOMMUNICATION())
          next_state = SETUP_COMMUNICATION;
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
      case SETUP_COMMUNICATION:
        if ( spiCodeRx == DISCONNECTED())
          next_state = WAIT; 
      break; 
      case INITIAL_SETUP:
        if (FLAG_INITIAL_SETUP_FINISHED == HIGH)
          next_state = WAIT;
      break;
      case SAFE_RESET:
        if (FLAG_SAFE_RESET_FINISHED == HIGH)
          next_state = WAIT;
      break;
      case BASIC_EXTENDED:
        if (spiCodeRx == STOP())
          next_state = WAIT;
        else if ( ReadHallSensors() == LOW )
          next_state = WAIT; 
      break;
      case BASIC_REFRACTED:
        if (spiCodeRx == STOP())
          next_state = WAIT;
        else if ( ReadHallSensors() == LOW )
          next_state = WAIT;       
      break;
      case ADVANCED_EXTENDED:
        if (FLAG_DISPLACEMENT_FINISHED == HIGH)
          next_state = WAIT;
      break;
      case ADVANCED_REFRACTED:
        if (FLAG_DISPLACEMENT_FINISHED == HIGH)
          next_state = WAIT;
      break;   
      default:
        next_state = WAIT;
      break;
    }

        
    if (current_state != next_state) {
      current_state = next_state;
      char msg[10];
      printf(msg, "CURRENT_STATE:%x\n", current_state);
      //UARTPrintf(msg);
    }
      
  }
}

void Wait(void) {
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << INB  );
  PORTD &= ~( 1 << SEL0 );
  PORTB &= ~( 1 << PWM  );
  
  SetupExternalInterrupts(DISABLE);

  FLAG_INITIAL_SETUP_FINISHED       = CLEAR;
  FLAG_RX_STROKE_LENGHT             = CLEAR;
  FLAG_EXTERNAL_INTERRUPTS          = CLEAR;
  FLAG_DISPLACEMENT_FINISHED        = CLEAR;
  FLAG_SAFE_RESET_FINISHED          = CLEAR;
  FLAG_SETUP_COMMUNICATION_FINISHED = CLEAR;

}

void SetupComunication (void) {
  while (spiCodeRx != SENDINGSTROKELENGHT());
  while (spiCodeRx == SENDINGSTROKELENGHT());
  stroke_lenght = spiCodeRx;
  
  while (spiCodeRx != SENDINGPULSERATE());
  while (spiCodeRx == SENDINGPULSERATE());
  pulse_rate = spiCodeRx;
    
  while (spiCodeRx != SENDINGACCELRATIONRATE());
  while (spiCodeRx == SENDINGACCELRATIONRATE());
  accelaration_rate = spiCodeRx;

    
  while (spiCodeRx != SENDINGACCELRATIONTIME());
  while (spiCodeRx == SENDINGACCELRATIONTIME());
  accelaration_time = spiCodeRx;

  while (spiCodeRx != SENDINGSOFTSTARTSTOP());
  while (spiCodeRx == SENDINGSOFTSTARTSTOP());
  enable_start_stop_soft = spiCodeRx;
}

void InitialSetup(void) {
  uint8_t initial_setup_current_state = IS_INIT;
  uint8_t initial_setup_next_state    = IS_INIT;
  
  while (1) {
    switch (initial_setup_current_state) {
      case IS_INIT:
        Stopped();
      break;
      case IS_EXTENDED:
        Extended();
      break;
      case IS_STOP:
        Stopped();
        SetupExternalInterrupts(ENABLE);
        count_pulse_s1  = 0x0000;
        count_pulse_s2  = 0x0000;
        FLAG_EXTERNAL_INTERRUPTS = SET;
      break;
      case IS_REFRACTED:
        Refracted();
      break;
      case IS_SEND_PULSE_RATE:
        Stopped();
        SetupExternalInterrupts(DISABLE);
        pulse_rate_s1 = (uint8_t)((count_pulse_s1)/stroke_lenght);
        pulse_rate_s2 = (uint8_t)((count_pulse_s2)/stroke_lenght);;             
        pulse_rate = (pulse_rate_s1 + pulse_rate_s2) / 2;
        spiCodeTx = pulse_rate;
        //char msg[10];
        //sprintf(msg, "%x\n", pulse_rate_s2);
        //UARTPrintf(msg);
      break;
      case IS_FINISHED:
        Stopped();
        SetupExternalInterrupts(DISABLE);
        while(spiCodeRx != SUCESS());
        FLAG_INITIAL_SETUP_FINISHED = SET;
      break;
      default:
        Stopped();
        SetupExternalInterrupts(DISABLE);
      break;
    }
    switch (initial_setup_current_state) {
      case IS_INIT:
        if ( FLAG_RX_STROKE_LENGHT == HIGH )
          initial_setup_next_state = IS_EXTENDED;
      break;
      case IS_EXTENDED:
        if (  ReadHallSensors() == LOW )
          initial_setup_next_state = IS_STOP;        
      break;
      case IS_STOP:
        if ( FLAG_EXTERNAL_INTERRUPTS == HIGH )
          initial_setup_next_state = IS_REFRACTED;    
      break;
      case IS_REFRACTED:
        if ( ReadHallSensors() == LOW )
          initial_setup_next_state = IS_SEND_PULSE_RATE; 
      break;
      case IS_SEND_PULSE_RATE:
        if (FLAG_INITIAL_SETUP_FINISHED == HIGH)
          return;
      break;          
      default:
        initial_setup_next_state == IS_STOP;
      break;
    }
    if (initial_setup_current_state != initial_setup_next_state) {
      initial_setup_current_state = initial_setup_next_state;
      //char msg[10];
      //sprintf(msg, "%x\n", current_state);
      //UARTPrintf(msg);
    }
  
  }
}    
void SafeReset(void) {
  uint8_t safe_reset_current_state = SR_EXTENDED;
  uint8_t safe_reset_next_state    = SR_EXTENDED;
  
  while (1) {   
    switch (safe_reset_current_state) {
      case SR_EXTENDED:
        Extended();
      break;
      case SR_REFRACTED:
        Refracted();
      case SR_FINISHED:
        Stopped();
        return;
      break;
      default:
        Stopped();
      break;
    }
    
    switch (safe_reset_current_state) {
      case SR_EXTENDED:
        if ( ReadHallSensors() == LOW )
          safe_reset_next_state = SR_REFRACTED;        
      break;
      case SR_REFRACTED:
        if ( ReadHallSensors() == LOW )
          safe_reset_next_state = SR_FINISHED; 
      break;       
      default:
        safe_reset_next_state == SR_FINISHED;
      break;
    }

    if (safe_reset_current_state != safe_reset_next_state) {
      safe_reset_current_state = safe_reset_next_state;
      //char msg[10];
      //sprintf(msg, "%x\n", current_state);
      //UARTPrintf(msg);
    }
  }
}  

void BasicExtended(void) {
  Extended();
}

void BasicRefracted(void) {
  Refracted();
}

void AdvancedExtended(void) {
  FLAG_DISPLACEMENT_FINISHED = CLEAR;
  spiCodeTx = SEND_SHIFT();
  while ( spiCodeRx != SENDING_SHIFT() );
  while ( spiCodeRx == SENDING_SHIFT() );
  uint8_t shift = spiCodeRx;
  uint8_t pulses = shift*pulse_rate;
  
  SetupExternalInterrupts(ENABLE);
  count_pulse_s1 = 0;
  count_pulse_s2 = 0;
  
  Extended();
  while ( ( count_pulse_s1 < pulses ) || ( count_pulse_s2 < pulses ) );
  Stopped();

  SetupExternalInterrupts(DISABLE);
  FLAG_DISPLACEMENT_FINISHED = SET;
}

void AdvancedRefracted(void) {
  FLAG_DISPLACEMENT_FINISHED = CLEAR;
  spiCodeTx = SEND_SHIFT();
  while ( spiCodeRx != SENDING_SHIFT() );
  while ( spiCodeRx == SENDING_SHIFT() );
  uint8_t shift = spiCodeRx;
  uint8_t pulses = shift*pulse_rate;
  
  SetupExternalInterrupts(ENABLE);
  count_pulse_s1 = 0;
  count_pulse_s2 = 0;
  
  Refracted();
  while ( ( count_pulse_s1 < pulses ) || ( count_pulse_s2 < pulses ) );
  Stopped();

  SetupExternalInterrupts(DISABLE);
  FLAG_DISPLACEMENT_FINISHED = SET;
}

uint8_t ReadHallSensors  (void) {
  if ( ( ( PIND & (1<<HALL_S1) ) == LOW ) && ( ( ( PIND & (1<<HALL_S2) ) == LOW ) ) )
    return LOW;
  else 
    return HIGH;
}

void Stopped  (void) {
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << INB  );
  PORTD &= ~( 1 << SEL0 );
  PORTB &= ~( 1 << PWM  );  
}   

void Extended (void) {
  PORTD &= ~( 1 << INB  );
  PORTD |=  ( 1 << INA  );
  PORTD |=  ( 1 << SEL0 );
  PORTB |=  ( 1 << PWM  );  
}

void Refracted(void) {
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << SEL0 );
  PORTD |=  ( 1 << INB  );
  PORTB |=  ( 1 << PWM  );
}