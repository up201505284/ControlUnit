#include "ControlUnit.h"
#include "Utils.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "printf_tools.h"

//  Auxiliar flags
uint8_t FLAG_BUFFER_SPI_COMPLETE      = CLEAR;
uint8_t FLAG_INITIAL_SETUP_FINISHED   = CLEAR;
uint8_t FLAG_SAFE_RESET_FINISHED      = CLEAR;
uint8_t FLAG_RESET_COMMUNICATION      = CLEAR;
uint8_t FLAG_MOTION_FINSISHED         = CLEAR;
uint8_t FLAG_SOFT_START               = CLEAR;
uint8_t FLAG_SOFT_STOP                = SET;

//  Counters for hall sensors
uint16_t count_pulse_s1     = 0x0000;
uint16_t count_pulse_s2     = 0x0000;

//  Calculation of hall sensors
uint8_t pulse_rate_s1       = 0x00;
uint8_t pulse_rate_s2       = 0x00;

//  Soft start and stop
uint8_t   stroke_lenght     = 0x00;
uint8_t   pulse_rate        = 0x00;
uint16_t  accelaration_time = 0x00;
uint16_t  accelaration_rate = 0x00;
uint8_t   soft_start_stop   = 0x00;
uint8_t   shiftRefracted    = 0x00;
uint8_t   shiftExtended     = 0x00;


//  SPI buffers
uint8_t spiCodeRx           = 0x00;
uint8_t spiBufferRx[256]    = {};
uint8_t posSpiBufferRx      = 0x00;

uint8_t aux = 0;
//  Functions for operating modes
void    Sleep             (void         );
void    InitialSetup      (void         );    
void    SafeReset         (void         ); 
void    Stopped           (void         );
void    BasicExtended     (void         );
void    BasicRefracted    (void         );
void    AdvancedExtended  (uint8_t shift);
void    AdvancedRefracted (uint8_t shift);

//  Auxiliar functions
uint8_t HallSensors       (void                         );
void    Stop              (void                         );     
void    Extend            (void                         );
void    Refract           (void                         );
void    Pwm               (uint8_t mode, uint8_t _state );


uint8_t current_state     = SLEEP;
uint8_t next_state        = SLEEP;

ISR(SPI_STC_vect) {

  spiCodeRx = SPDR;

  if (spiCodeRx == SPICOMMANDINITIAL()) {
    posSpiBufferRx = 0;
    FLAG_BUFFER_SPI_COMPLETE = CLEAR;
    SPDR = SPIREADWITHSUCESS();

  }
  
  else if (spiCodeRx == SPICOMMANDFINISHED()) {
    FLAG_BUFFER_SPI_COMPLETE = SET; 
    SPDR = SPIREADWITHSUCESS();
  }

  else if (spiCodeRx == DISABLESPI()) {
    SPDR = pulse_rate;
  }

  else
    SPDR = SPIREADWITHSUCESS();

  spiBufferRx[posSpiBufferRx] = spiCodeRx;
  posSpiBufferRx++;
}


ISR(INT0_vect) {
  count_pulse_s1++;
}

ISR(INT1_vect) {
  count_pulse_s2++;
}

int main () {
  
  cli();
  
  init_USART(); 
  init_printf_tools();

  InitPorts();
  InitAdc();
  SetupSpi();


  soft_start_stop             = 0x00;
  pulse_rate                  = 0x00;
  accelaration_time           = 0x00;
  accelaration_rate           = 0x00;
  stroke_lenght               = 0x00;
  shiftRefracted              = 0x00;
  shiftExtended               = 0x00;
  posSpiBufferRx              = 0x00;

  FLAG_BUFFER_SPI_COMPLETE    = CLEAR;
  FLAG_INITIAL_SETUP_FINISHED = CLEAR;
  FLAG_SAFE_RESET_FINISHED    = CLEAR;
  FLAG_RESET_COMMUNICATION    = CLEAR;
  FLAG_MOTION_FINSISHED       = CLEAR;
  FLAG_SOFT_START             = CLEAR;
  FLAG_SOFT_STOP              = SET;

 
  current_state               = SLEEP;
  next_state                  = SLEEP;


  sei();
  
  while(1) {

    switch (current_state) {
      case SLEEP:
        Sleep();
        break;
      case INITIAL_SETUP:
        InitialSetup();
        break;
      case SAFE_RESET:
        SafeReset();
        break;
      case STOPPED:
        Stopped();
        break;
      case BASIC_EXTENDED:
        BasicExtended();
        break;
      case BASIC_REFRACTED:
        BasicRefracted();
        break;
      case ADVANCED_EXTENDED:
        AdvancedExtended(shiftExtended);
        break;
      case ADVANCED_REFRACTED:
        AdvancedRefracted(shiftRefracted);
        break;    
      default:
        Sleep();
        break;
    }
    switch (current_state) {
      case SLEEP:
        if (FLAG_RESET_COMMUNICATION == HIGH)
          next_state = STOPPED;
      case INITIAL_SETUP:
        if (FLAG_INITIAL_SETUP_FINISHED == HIGH)
          next_state = STOPPED;
        break;
      case SAFE_RESET:
        if (FLAG_SAFE_RESET_FINISHED == HIGH)
          next_state = STOPPED;
        break;
      case STOPPED:
        if ( (spiBufferRx[MODE] == BASICEXTENDED()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;
          next_state = BASIC_EXTENDED;
        }
        else if ( (spiBufferRx[MODE] == BASICREFRACTED()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;        
          next_state = BASIC_REFRACTED;
        }
        else if ( (spiBufferRx[MODE] == ADVANCEDEXTENDED()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;
          shiftExtended = spiBufferRx[SHIFT];
          next_state = ADVANCED_EXTENDED;
        }
        else if ( (spiBufferRx[MODE] == ADVANCEDREFRACTED()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR; 
          shiftRefracted = spiBufferRx[SHIFT];        
          next_state = ADVANCED_REFRACTED;
        } 
        else if ( (spiBufferRx[MODE] == INITIALSETUP()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;
          next_state = INITIAL_SETUP;
        } 
        else if ( (spiBufferRx[MODE] == SAFERESET()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;
          next_state = SAFE_RESET;
        }
        else if ( (spiBufferRx[MODE] == DISABLESPI()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;
          next_state = SLEEP;
        }
        break;
      case BASIC_EXTENDED:
        if ( (spiBufferRx[MODE] == STOP()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;                   
          next_state = STOPPED;
        }
        else if (ReadAdc() < THRESHOLD*2 )
          next_state = STOPPED;
        break;
      case BASIC_REFRACTED:
        if ( (spiBufferRx[MODE] == STOP()) && (FLAG_BUFFER_SPI_COMPLETE == HIGH) ) {
          FLAG_BUFFER_SPI_COMPLETE = CLEAR;                   
          next_state = STOPPED;  
        }
        else if ( ReadAdc() < THRESHOLD*2 )
          next_state = STOPPED;        
        break;
      case ADVANCED_EXTENDED:
        if (FLAG_MOTION_FINSISHED == HIGH)
          next_state = STOPPED;
        break;
      case ADVANCED_REFRACTED:
        if (FLAG_MOTION_FINSISHED == HIGH) 
          next_state = STOPPED;     
        break;    
      default:
        Sleep();
        break;
      }
    if (next_state != current_state) {
      printf("New State:%d; Previous State:%u CS:%u; Stroke:%u; Pulse rate:%u; Soft start and soft:%u; Accelaration Time:%u; Accelaration Rate: %u\n", next_state, current_state, ReadAdc(), stroke_lenght, pulse_rate, soft_start_stop, accelaration_time, accelaration_rate);
    }
    current_state = next_state;
  }
}

void Sleep (void) {
  
  SetupExternalInterrupts(DISABLE);

  FLAG_INITIAL_SETUP_FINISHED = CLEAR;
  FLAG_SAFE_RESET_FINISHED    = CLEAR;
  FLAG_RESET_COMMUNICATION    = CLEAR;
  FLAG_MOTION_FINSISHED       = CLEAR;
  FLAG_SOFT_START             = CLEAR;
  FLAG_SOFT_STOP              = SET;
  
  if ((FLAG_BUFFER_SPI_COMPLETE == 1) && (spiBufferRx[MODE] == ENABLESPI())) {

    FLAG_BUFFER_SPI_COMPLETE  = CLEAR;
    stroke_lenght             = (spiBufferRx[STROKE_LENGHT_LOW] & 0xFF) | ((spiBufferRx[STROKE_LENGHT_HIGH] << 8) & 0xFF00);    
    pulse_rate                = spiBufferRx[PULSE_RATE_ENABLE];       
    accelaration_time         = (spiBufferRx[ACCELARATION_TIME_LOW] & 0xFF) | ((spiBufferRx[ACCELARATION_TIME_HIGH] << 8) & 0xFF00);
    soft_start_stop           = spiBufferRx[SOFT_START_STOP];  
    accelaration_rate         = accelaration_time/100;
  
    FLAG_RESET_COMMUNICATION  = SET ;
    aux = 0;
  }

  else if ((FLAG_BUFFER_SPI_COMPLETE == 1) && (spiBufferRx[MODE] == UPDATE())) {

    FLAG_BUFFER_SPI_COMPLETE  = CLEAR;
    stroke_lenght             = (spiBufferRx[STROKE_LENGHT_LOW] & 0xFF) | ((spiBufferRx[STROKE_LENGHT_HIGH] << 8) & 0xFF00);    
    pulse_rate                = spiBufferRx[PULSE_RATE_ENABLE];       
    accelaration_time         = (spiBufferRx[ACCELARATION_TIME_LOW] & 0xFF) | ((spiBufferRx[ACCELARATION_TIME_HIGH] << 8) & 0xFF00);
    soft_start_stop           = spiBufferRx[SOFT_START_STOP];   
    accelaration_rate         = accelaration_time/100;
    
    FLAG_RESET_COMMUNICATION  = SET ;
  }
}

void SafeReset (void) {
  Extend();
  for (int i=0; i<30; i++)
    _delay_ms(1);
  while (ReadAdc() > THRESHOLD*2);
  Stop();
  Refract();
  for (int i=0; i<30; i++)
    _delay_ms(1);
  while (ReadAdc() > THRESHOLD*2);
  Stop();
  FLAG_SAFE_RESET_FINISHED = SET;
}

void  InitialSetup (void) {
  Extend();
  for (int i=0; i<30; i++)
    _delay_ms(1);
  while (ReadAdc() > THRESHOLD*2);
  Stop();
  count_pulse_s1 = 0;
  count_pulse_s2 = 0;
  SetupExternalInterrupts(ENABLE);  
  Refract();
  for (int i=0; i<30; i++)
    _delay_ms(1);  
  while (ReadAdc() > THRESHOLD*2);
  Stop();
  SetupExternalInterrupts(DISABLE);
  pulse_rate_s1 = (uint8_t)((count_pulse_s1)/stroke_lenght);
  pulse_rate_s2 = (uint8_t)((count_pulse_s2)/stroke_lenght);;             
  pulse_rate = (pulse_rate_s1 + pulse_rate_s2) / 2;

  FLAG_INITIAL_SETUP_FINISHED = SET;
}

void BasicRefracted (void) {
  Refract();
  for (int i=0; i<30; i++)
    _delay_ms(1);
}

void BasicExtended (void) {
  Extend();
  for (int i=0; i<30; i++)
    _delay_ms(1);
}

void AdvancedExtended (uint8_t shift) {
  FLAG_MOTION_FINSISHED = CLEAR;
  uint16_t maxPulses = shift*pulse_rate;
  count_pulse_s1 = 0;
  count_pulse_s2 = 0;
  SetupExternalInterrupts(ENABLE);

  Extend();

  for (int i=0; i<30; i++)
    _delay_ms(1);
  
  while (((count_pulse_s1 < maxPulses) || (count_pulse_s2 < maxPulses)) && (ReadAdc() > THRESHOLD*2));
  
  Stop();
  
  FLAG_MOTION_FINSISHED = SET;
}

void AdvancedRefracted (uint8_t shift) {
  Stop();
  FLAG_MOTION_FINSISHED = CLEAR;
  uint16_t  maxPulses = shift*pulse_rate;
  count_pulse_s1 = 0;
  count_pulse_s2 = 0;
  
  SetupExternalInterrupts(ENABLE);
  
  Refract();

  for (int i=0; i<30; i++)
    _delay_ms(1);
  
  while (((count_pulse_s1 < maxPulses) || (count_pulse_s2 < maxPulses)) && (ReadAdc() > THRESHOLD*2));  
  Stop();
  
  FLAG_MOTION_FINSISHED = SET;
}

void Stopped (void) {
  Stop();
  FLAG_INITIAL_SETUP_FINISHED = CLEAR;
  FLAG_SAFE_RESET_FINISHED    = CLEAR;
  FLAG_RESET_COMMUNICATION    = CLEAR;
  FLAG_MOTION_FINSISHED       = CLEAR;
}

void Stop  (void) {
  FLAG_SOFT_START = CLEAR;
  Pwm(soft_start_stop, LOW);
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << INB  );
  PORTD &= ~( 1 << SEL0 );
}   

void Extend (void) {
  FLAG_SOFT_STOP = CLEAR;
  PORTD &= ~( 1 << INB  );
  PORTD |=  ( 1 << INA  );
  PORTD |=  ( 1 << SEL0 );
  _delay_us(20);
  Pwm(soft_start_stop, HIGH);
}

void Refract(void) {
  FLAG_SOFT_STOP = CLEAR;
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << SEL0 );
  PORTD |=  ( 1 << INB  );
  PORTB |=  ( 1 << PWM  );
  _delay_us(20);
  Pwm(soft_start_stop, HIGH);
}

void Pwm(uint8_t mode, uint8_t state) {
  if (mode == DIGITAL || (mode == SOFTSTARTSTOP && FLAG_SOFT_START == HIGH) || (mode == SOFTSTARTSTOP && FLAG_SOFT_STOP == HIGH)) {
    if (state == LOW)
      PORTB &= ~( 1 << PWM  ); 
    else if (state == HIGH)   
      PORTB |=  ( 1 << PWM  );  
  }

  else if ((mode == SOFTSTARTSTOP && FLAG_SOFT_START == LOW) || (mode == SOFTSTARTSTOP && FLAG_SOFT_STOP == LOW)) {
    if (state == LOW) {
      PORTB |=  ( 1 << PWM  );               
      for (int i=0; i<30; i++)
        _delay_ms(1);

      uint8_t duty_cycle = 99;
      while ( duty_cycle > 0 /*&& ReadAdc() > 2*THRESHOLD*/ ) {
        duty_cycle--;
        SetupPwm(duty_cycle);
        for (int i=0; i<accelaration_rate; i++)
          _delay_ms(1);
      }  
      DisablePwm();
      PORTB &= ~( 1 << PWM  );
      FLAG_SOFT_STOP = SET;
      aux = 1;
    }
    
    else if (state == HIGH) {      
      uint8_t duty_cycle = 1;
      SetupPwm(duty_cycle);
      
      for (int i=0; i<30; i++)
        _delay_ms(1);
   
      while ( duty_cycle < 100 /*&& ReadAdc() > 2*THRESHOLD*/ ) {
        duty_cycle++;
        SetupPwm(duty_cycle);
        for (int i=0; i<accelaration_rate; i++)
          _delay_ms(1);
      }
      FLAG_SOFT_START = SET;
      DisablePwm();
      PORTB |=  ( 1 << PWM  );  
    }
  }
}

uint8_t HallSensors (void) {
  if ( ( ( PIND & (1<<HALL_S1) ) == LOW ) && ( ( ( PIND & (1<<HALL_S2) ) == LOW ) ) )
    return LOW;
  else 
    return HIGH;  
}
