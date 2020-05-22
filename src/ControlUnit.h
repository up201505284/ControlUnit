#ifndef F_CPU
    #define F_CPU   16000000ul 
#endif

// Digital Outputs
    //  INA     
    //  INB    
    //  PWM      
    //  SEL0   
#define INA                         PD4                       
#define INB                         PD5
#define PWM                         PB1                
#define SEL0                        PD7                 

//  Digital Inputs
    //  HALL_S1  
    //  HALL_S2   
#define HALL_S1                     PD2                 
#define HALL_S2                     PD3     

//  Analogue Inputs
    //  CS  
#define CS                          PC0

//  Spi Pins
#define SCK                         PB5
#define MISO                        PB4
#define MOSI                        PB3
#define SS                          PB2

//  ADC Channel
#define ADC_CHANNEL                 0b0000
#define THRESHOLD_STOP              10
#define THRESHOLD_START             10  
#define THRESHOLD                   12

//  Reset Value for Timer 1
#define PWM_10KHZ                   0x063F

//  Value for delay of 1ms
#define OCR_1MS                     0xF9

 //  Auxiliar Masks       
#define ENABLE                      1
#define DISABLE                     0
#define HIGH                        1
#define LOW                         0
#define SET                         1
#define CLEAR                       0
#define SOFTSTARTSTOP               1
#define DIGITAL                     0

//  SPI Commands
#define INITIALSETUP()              (uint8_t)   0x01
#define SAFERESET()                 (uint8_t)   0x02
#define BASICEXTENDED()             (uint8_t)   0x03
#define BASICREFRACTED()            (uint8_t)   0X04
#define ADVANCEDEXTENDED()          (uint8_t)   0x05
#define ADVANCEDREFRACTED()         (uint8_t)   0x06
#define STOP()                      (uint8_t)   0x07
#define ENABLESPI()                 (uint8_t)   0x08
#define DISABLESPI()                (uint8_t)   0x09
#define UPDATE()                    (uint8_t)   0x0A
#define SPICOMMANDINITIAL()         (uint8_t)   0xF0
#define SPICOMMANDFINISHED()        (uint8_t)   0xF1
#define SPIREADWITHSUCESS()         (uint8_t)   0xF2

//  SPI Masks
#define INITIAL                     (uint8_t)   0
#define MODE                        (uint8_t)   1
#define SHIFT                       (uint8_t)   2
#define PULSE_RATE_DISABLE          (uint8_t)   2
#define STROKE_LENGHT_HIGH          (uint8_t)   2
#define STROKE_LENGHT_LOW           (uint8_t)   3
#define PULSE_RATE_ENABLE           (uint8_t)   4
#define ACCELARATION_TIME_HIGH      (uint8_t)   5
#define ACCELARATION_TIME_LOW       (uint8_t)   6
#define SOFT_START_STOP             (uint8_t)   7
#define SIZE_BUFFER_ENABLE_SPI      (uint8_t)   9
#define SIZE_BUFFER_UPDATE          (uint8_t)   9
#define SIZE_BUFFER_DISABLE_SPI     (uint8_t)   3
#define SIZE_BUFFER_BASIC           (uint8_t)   3
#define SIZE_BUFFER_ADVANCED        (uint8_t)   4


//  Operating Modes 
#define SLEEP                       0x00
#define INITIAL_SETUP               0x01
#define SAFE_RESET                  0x02
#define STOPPED                     0x03               
#define BASIC_EXTENDED              0x04
#define BASIC_REFRACTED             0x05
#define ADVANCED_EXTENDED           0x06
#define ADVANCED_REFRACTED          0x07





