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
#define THRESHOLD                   50

//  Reset Value for Timer 1
#define PWM_10KHZ                   0x063F


 //  Auxiliar Masks       
#define HIGH                        1
#define LOW                         0
#define SET                         1
#define CLEAR                       0
#define INCREMENTAL                 1
#define DECREMENTAL                 0

//  Operating Modes 
#define SLEEP                       0x00
#define INITIAL_SETUP               0x01
#define SAFE_RESET                  0x02
#define STOPPED                     0x03               
#define BASIC_EXTENDED              0x04
#define BASIC_REFRACTED             0x05
#define ADVANCED_EXTENDED           0x06
#define ADVANCED_REFRACTED          0x07


//  SPI Commands
#define ENABLE()                        (uint8_t)   0x01
#define DISABLE()                       (uint8_t)   0x02
#define INITIALSETUP()                  (uint8_t)   0x03
#define SAFERESET()                     (uint8_t)   0x04
#define BASICEXTENDED()                 (uint8_t)   0x05
#define BASICREFRACTED()                (uint8_t)   0X06
#define STOP()                          (uint8_t)   0x07
#define ADVANCEDEXTENDED()              (uint8_t)   0x08
#define ADVANCEDREFRACTED()             (uint8_t)   0x09
#define GETPOSITIONLOW()                (uint8_t)   0x0A
#define GETPOSITIONHIGH()               (uint8_t)   0x0B
#define GETPULSERATE()                  (uint8_t)   0x0C
#define SENDPOSITONLOW()                (uint8_t)   0x0D
#define SENDPOSITONHIGH()               (uint8_t)   0x0E
#define SENDPULSERATE()                 (uint8_t)   0x0F
#define SENDSTROKELENGHTLOW()           (uint8_t)   0x10
#define SENDSTROKELENGHTHIGH()          (uint8_t)   0x11
#define SENDSOFTSTART()                 (uint8_t)   0x12
#define SENDACCELARATIONRATE()          (uint8_t)   0x13

//  Auc Commands
#define START()                         (uint8_t)   0xFF
#define END()                           (uint8_t)   0XFE
#define READWITHSUCESS()                (uint8_t)   0XFD   

//  SPI Modes
#define COMMUNICATION()                 (uint8_t)   0xFC
#define BASIC()                         (uint8_t)   0xFB
#define ADVANCED()                      (uint8_t)   0xFA
#define UPDATE()                        (uint8_t)   0xF9
#define SETUP()                         (uint8_t)   0xF8


//  SPI Masks
#define MODE                            (uint8_t)   1
#define CMD                             (uint8_t)   2
#define DATA                            (uint8_t)   3
#define SIZE_BUFFER_COMMUNCATION_MODE   (uint8_t)   4
#define SIZE_BUFFER_SETUP_MODE          (uint8_t)   5
#define SIZE_BUFFER_UPDATE_MODE         (uint8_t)   4
#define SIZE_BUFFER_BASIC_MODE          (uint8_t)   4
#define SIZE_BUFFER_ADVANCED_MODE       (uint8_t)   5


