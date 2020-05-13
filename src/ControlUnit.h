#ifndef F_CPU
    #define F_CPU   16000000ul 
#endif

// Digital Outputs
    //  INA     
    //  INB    
    //  PWM      
    //  SEL0   
#define INA         PD4                       
#define INB         PD5
#define PWM         PB1                
#define SEL0        PD7                 

//  Digital Inputs
    //  HALL_S1  
    //  HALL_S2   
#define HALL_S1     PD2                 
#define HALL_S2     PD3     

//  Analogue Inputs
    //  CS  
#define CS          PC0

//  Spi Pins
#define SCK         PB5
#define MISO        PB4
#define MOSI        PB3
#define SS          PB2

//  ADC Channel
#define ADC_CHANNEL 0b0000

//  Reset Value for Timer 1
#define PWM_10KHZ   0x063F

 //  Auxiliar Masks       
#define ENABLE          1
#define DISABLE         0
#define HIGH            1
#define LOW             0
#define SET             1
#define CLEAR           0
#define SOFTSTARTSTOP   1
#define DIGITAL         0

//  SPI Masks
#define SHIFT               2
#define MODE                1
#define STROKE_LENGHT       2
#define PULSE_RATE          3
#define ACCELARATION_RATE   4
#define ACCELARATION_TIME   5
#define SOFT_START_STOP     6


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
#define INITIALSETUP()              0x01 
#define SAFERESET()                 0x02  
#define BASICEXTENDED()             0x03 
#define BASICREFRACTED()            0X04
#define ADVANCEDEXTENDED()          0x05
#define ADVANCEDREFRACTED()         0x06  
#define STOP()                      0x07
#define ENABLESPI()                 0x08
#define DISABLESPI()                0x09
#define SPICOMMANDINITIAL()         0xF0
#define SPICOMMANDFINISHED()        0xF1
#define SPIREADWITHSUCESS()         0xF2



