#include "ControlUnit.h"
#include "Utils.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "printf_tools.h"

//  Auxiliar flags
uint8_t FLAG_BUFFER_SPI      = CLEAR;
uint8_t FLAG_INITIAL_SETUP   = CLEAR;
uint8_t FLAG_SAFE_RESET      = CLEAR;
uint8_t FLAG_ADVANCED        = CLEAR;

//  Counters for hall sensors
uint16_t count_pulse_s1     = 0x0000;
uint16_t count_pulse_s2     = 0x0000;
uint16_t count_pulse        = 0x0000;
uint8_t  conter_direction   = INCREMENTAL;
uint16_t current_pulses     = 0x0000;
uint16_t next_pulses        = 0x0000;

//  Status of actuator
uint16_t position               = 0x0000;
uint8_t  position_low           = 0x00;
uint8_t  position_high          = 0x00;
uint8_t  pulse_rate             = 0xFF;
uint16_t stroke_lenght          = 0x00;
uint8_t  stroke_lenght_low      = 0x00;
uint8_t  stroke_lenght_high     = 0x00;
uint8_t  accelaration_rate      = 0x00;
uint8_t  soft_start_stop        = 0x00;

//  SPI buffers
uint8_t spiCode             = 0x00;
uint8_t spiBuffer[256]      = {};
uint8_t posSpiBuffer        = 0x00;

uint8_t aux = 0;
//  Functions for operating modes
void    Sleep             (void);
void    InitialSetup      (void);    
void    SafeReset         (void); 
void    Stopped           (void);
void    BasicExtended     (void);
void    BasicRefracted    (void);
void    AdvancedExtended  (void);
void    AdvancedRefracted (void);

//  Basic functions
void    Stop              (void);     
void    Extend            (void);
void    Refract           (void);

//  Auxiliar functions
void    Pwm               (uint8_t _softStartStop, uint8_t _state );


ISR(SPI_STC_vect) {

  spiCode = SPDR;
  if (spiCode == START()) {
    posSpiBuffer = 0;
    FLAG_BUFFER_SPI = CLEAR;
    SPDR = READWITHSUCESS();
  }
  
  else if (spiCode == END()) {
    FLAG_BUFFER_SPI = SET; 
    SPDR = READWITHSUCESS();
  }

  else if ( posSpiBuffer == CMD) {

    if (spiCode == GETPOSITIONLOW())
      SPDR = (uint8_t) (position & 0xFF);
    else if(spiCode == GETPOSITIONHIGH())
      SPDR = (uint8_t) ((position >> 8) & 0xFF);
    else if (spiCode == GETPULSERATE())
      SPDR = pulse_rate;
    else 
        SPDR = READWITHSUCESS();
  }

  else
    SPDR = READWITHSUCESS();

  spiBuffer[posSpiBuffer] = spiCode;
  posSpiBuffer++;
}


ISR(INT0_vect) {
  if (conter_direction == INCREMENTAL) {
    if (count_pulse_s1 < stroke_lenght*pulse_rate)
      count_pulse_s1++;
    else
      count_pulse_s1 = stroke_lenght*pulse_rate;
  }

  else if (conter_direction == DECREMENTAL) {
    if (count_pulse_s1 > 0)
      count_pulse_s1--;
    else
      count_pulse_s1 = 0;
  }  
}

ISR(INT1_vect) {
if (conter_direction == INCREMENTAL) {
    if (count_pulse_s2 < stroke_lenght*pulse_rate)
      count_pulse_s2++;
    else
      count_pulse_s2 = stroke_lenght*pulse_rate;
  }
  
  else if (conter_direction == DECREMENTAL) {
    if (count_pulse_s2 > 0)
      count_pulse_s2--;
    else
      count_pulse_s2 = 0;
  }
    
}

int main () {
  
  cli();
  
  init_USART(); 
  init_printf_tools();

  InitPorts();
  InitAdc();
  SetupSpi();

  count_pulse_s1              = 0x00;
  count_pulse_s2              = 0x00;
  count_pulse                 = 0x00;
  conter_direction            = INCREMENTAL;

  posSpiBuffer                = 0x00;
  spiCode                     = 0x00;

  soft_start_stop             = 0x00;
  accelaration_rate           = 0x00;
  pulse_rate                  = 0xFF;
  stroke_lenght               = 0x00;
  position                    = 0x00;
  position_low                = 0x00;
  position_high               = 0x00;
  stroke_lenght_low           = 0x00;
  stroke_lenght_high          = 0x00;

  FLAG_BUFFER_SPI             = CLEAR;
  FLAG_INITIAL_SETUP          = CLEAR;
  FLAG_SAFE_RESET             = CLEAR;
  FLAG_ADVANCED               = CLEAR;
 
  uint8_t current_state       = SLEEP;
  uint8_t next_state          = SLEEP;

  sei();
  printf("New State:%d; Previous State:%u CS:%u; Stroke:%u; Pulse rate:%u; Soft start and soft:%u; Accelaration Rate: %u; Position:%u\n", next_state, current_state, ReadAdc(), stroke_lenght, pulse_rate, soft_start_stop, accelaration_rate, position);

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
        AdvancedExtended();
        break;
      case ADVANCED_REFRACTED:
        AdvancedRefracted();
        break;    
      default:
        Sleep();
        break;
    }

    switch (current_state) {
      case SLEEP:
        if ((spiBuffer[MODE] == COMMUNICATION()) && (spiBuffer[CMD] == ENABLE()) && (FLAG_BUFFER_SPI== HIGH)) {
          FLAG_BUFFER_SPI = CLEAR;
          next_state      = STOPPED;
        }
      case INITIAL_SETUP:
        if (FLAG_INITIAL_SETUP == HIGH)
          next_state = STOPPED;
        break;
      case SAFE_RESET:
        if (FLAG_SAFE_RESET == HIGH)
          next_state = STOPPED;
        break;
      case STOPPED:
        if (FLAG_BUFFER_SPI == HIGH) {
          if ( (spiBuffer[CMD] == BASICEXTENDED()) && (spiBuffer[MODE] == BASIC()) ) 
            next_state = BASIC_EXTENDED;         
          
          else if ( (spiBuffer[CMD] == BASICREFRACTED()) && (spiBuffer[MODE] == BASIC()) ) 
            next_state = BASIC_REFRACTED;
          
          else if ( (spiBuffer[CMD] == ADVANCEDEXTENDED()) && (spiBuffer[MODE] == ADVANCED()) ) {
            current_pulses = count_pulse;
            next_pulses    = current_pulses + spiBuffer[DATA]*pulse_rate; 
            if (next_pulses > stroke_lenght*pulse_rate)
              next_pulses = stroke_lenght*pulse_rate;
            next_state = ADVANCED_EXTENDED;
          }

          else if ( (spiBuffer[CMD] == ADVANCEDREFRACTED()) && (spiBuffer[MODE] == ADVANCED()) ) {
            current_pulses = count_pulse;
            if (current_pulses >= spiBuffer[DATA]*pulse_rate)
              next_pulses = current_pulses - spiBuffer[DATA]*pulse_rate; 
            else
              next_pulses = 0;
            next_state = ADVANCED_REFRACTED;
          }

          else if ( (spiBuffer[CMD] == INITIALSETUP()) && (spiBuffer[MODE] == BASIC()) )
            next_state = INITIAL_SETUP;
          
          else if ( (spiBuffer[CMD] == SAFERESET()) && (spiBuffer[MODE] == BASIC()) ) 
            next_state = SAFE_RESET;
        
          else if ( (spiBuffer[CMD] == DISABLE()) && (spiBuffer[MODE] == COMMUNICATION()) ) 
            next_state = SLEEP;
          
          FLAG_BUFFER_SPI = CLEAR;
        }
        break;
      case BASIC_EXTENDED:
        if ( (spiBuffer[MODE] == BASIC()) && (spiBuffer[CMD] == STOP()) && (FLAG_BUFFER_SPI == HIGH) ) {
          FLAG_BUFFER_SPI = CLEAR;                   
          next_state = STOPPED;
        }
        else if ((count_pulse_s1 == stroke_lenght*pulse_rate) || (count_pulse_s2 == stroke_lenght*pulse_rate)) {
          next_state      = STOPPED;
          position        = stroke_lenght;
          count_pulse_s1  = stroke_lenght*pulse_rate;
          count_pulse_s2  = stroke_lenght*pulse_rate;
        }
        break;
      case BASIC_REFRACTED:
        if ( (spiBuffer[CMD] == STOP()) && (FLAG_BUFFER_SPI == HIGH) && (spiBuffer[MODE] == BASIC()) ) {
          FLAG_BUFFER_SPI = CLEAR;                   
          next_state = STOPPED;  
        }
        else if ((count_pulse_s1 == 0) || (count_pulse_s2 == 0)) {
          next_state      = STOPPED;
          position        = 0;
          count_pulse_s1  = 0;
          count_pulse_s2  = 0;
        }       
        break;
      case ADVANCED_EXTENDED:
        if (FLAG_ADVANCED == HIGH)
          next_state = STOPPED;
        break;
      case ADVANCED_REFRACTED:
        if (FLAG_ADVANCED == HIGH) 
          next_state = STOPPED;     
        break;    
      default:
        Sleep();
        break;
      }
    if (next_state != current_state) {
      printf("New State:%u; Previous State:%u\n", next_state, current_state);
      printf("Stroke:%u; Pulse rate:%u; Soft start and soft:%u; Accelaration Rate: %u; Position:%u\n", stroke_lenght, pulse_rate, soft_start_stop, accelaration_rate, position);

    }
    current_state = next_state;
  }
}

void Sleep (void) {
  SetupExternalInterrupts(LOW);
  Stop();
}

void Stopped (void) {
  Stop();
  
  count_pulse         = (uint16_t) ((count_pulse_s1 + count_pulse_s2) / 2);
  position            = (uint16_t) (count_pulse / pulse_rate);


  FLAG_INITIAL_SETUP  = CLEAR;
  FLAG_SAFE_RESET     = CLEAR;
  FLAG_ADVANCED       = CLEAR;

  if ((FLAG_BUFFER_SPI == HIGH) && (spiBuffer[MODE] == SETUP())) {
    
    if (spiBuffer[CMD] == SENDSTROKELENGHTLOW()) 
      stroke_lenght_low = spiBuffer[DATA];

    else if (spiBuffer[CMD] == SENDSTROKELENGHTHIGH()) 
      stroke_lenght_high = spiBuffer[DATA];
    
    //else if (spiBuffer[CMD] == SENDPOSITONLOW()) 
    //  position_low = spiBuffer[DATA];

    //else if (spiBuffer[CMD] == SENDPOSITONHIGH()) 
    //  position_high = spiBuffer[DATA];
    

    else if (spiBuffer[CMD] == SENDPULSERATE()) 
      pulse_rate = spiBuffer[DATA]; 
    
    else if (spiBuffer[CMD] == SENDACCELARATIONRATE())
      accelaration_rate = spiBuffer[DATA]; 
    
    else if (spiBuffer[CMD] == SENDSOFTSTART()) {
      soft_start_stop = spiBuffer[DATA]; 
    //  printf("%u\n", soft_start_stop);
    }

    FLAG_BUFFER_SPI = CLEAR;
    stroke_lenght = (stroke_lenght_low & 0xFF) | ((stroke_lenght_high << 8) & 0xFF00);
  }
}

void SafeReset (void) {
  SetupExternalInterrupts(LOW);
  conter_direction = INCREMENTAL;

  Refract();
  for (int i=0; i<30; i++)
    _delay_ms(1);
  while (ReadAdc() > THRESHOLD*2);
  
  Stop();
  
  count_pulse_s1  = 0;
  count_pulse_s2  = 0;
  count_pulse     = 0;
  position        = 0;
  SetupExternalInterrupts(HIGH);
  
  FLAG_SAFE_RESET = SET;
}

void  InitialSetup (void) {
  SetupExternalInterrupts(LOW);
  
  Extend();
  
  for (int i=0; i<30; i++)
    _delay_ms(1);
  while (ReadAdc() > THRESHOLD*2);
  
  Stop();
  count_pulse_s1    = 0;
  count_pulse_s2    = 0;
  count_pulse       = 0;
  conter_direction  = INCREMENTAL;

  SetupExternalInterrupts(HIGH);  
  Refract();
  
  for (int i=0; i<30; i++)
    _delay_ms(1);  
  while (ReadAdc() > THRESHOLD*2);
  
  Stop();

  SetupExternalInterrupts(LOW);  
  uint8_t pulse_rate_s1 = (uint8_t)((count_pulse_s1)/stroke_lenght);
  uint8_t pulse_rate_s2 = (uint8_t)((count_pulse_s2)/stroke_lenght);            
  pulse_rate            = (uint8_t) ((pulse_rate_s1 + pulse_rate_s2) / 2);
  count_pulse_s1        = 0;
  count_pulse_s2        = 0;
  count_pulse           = 0;
  position              = 0;

  FLAG_INITIAL_SETUP = SET;
}

void BasicRefracted (void) {
  conter_direction = DECREMENTAL;
  Refract();
}

void BasicExtended (void) {
  conter_direction = INCREMENTAL;
  Extend();
}

void AdvancedExtended (void) {
  conter_direction = INCREMENTAL;
  Extend();
  while (count_pulse < next_pulses) {
    count_pulse = (uint16_t) ((count_pulse_s1 + count_pulse_s2) / 2);
    _delay_us(5);
  }

  Stop();
  FLAG_ADVANCED = SET;
}

void AdvancedRefracted (void) {
  conter_direction = DECREMENTAL;
  Refract();

  while (count_pulse > next_pulses) {
    count_pulse = (uint16_t) ((count_pulse_s1 + count_pulse_s2) / 2);
    _delay_us(5);
  }
  
  Stop();
  FLAG_ADVANCED = SET;

}

void Stop  (void) {
  Pwm(soft_start_stop, LOW);
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << INB  );
  PORTD &= ~( 1 << SEL0 );
}   

void Extend (void) {
  PORTD &= ~( 1 << INB  );
  PORTD |=  ( 1 << INA  );
  PORTD |=  ( 1 << SEL0 );
  _delay_us(20);
  Pwm(soft_start_stop, HIGH);
}

void Refract(void) {
  PORTD &= ~( 1 << INA  );
  PORTD &= ~( 1 << SEL0 );
  PORTD |=  ( 1 << INB  );
  PORTB |=  ( 1 << PWM  );
  _delay_us(20);
  Pwm(soft_start_stop, HIGH);
}

void Pwm(uint8_t _softStartStop, uint8_t _state) {
  if (_softStartStop == LOW ) {
    if (_state == LOW)
      PORTB &= ~( 1 << PWM  ); 
    else if (_state == HIGH)   
      PORTB |=  ( 1 << PWM  );  
  }

  else if (_softStartStop == HIGH) {
    if (_state == LOW) {
      DisablePwm();
      PORTB &= ~( 1 << PWM  ); 
    }
    else if (_state == HIGH)   
      SetupPwm(90);
  }
  
  else
    PORTB &= ~( 1 << PWM  ); 
}

