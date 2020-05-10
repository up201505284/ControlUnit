
#define TIM2_ARR_10KHz          1600
#define TIM2_ARR_20KHz          0x0320

#ifndef F_CPU
    #define F_CPU               16000000ul //16MHz
#endif

// Digital outputs
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

//  Analogue inputs.
    //  CS  
#define CS          PC0

//  Spi pins.
#define SCK         PB5
#define MISO        PB4
#define MOSI        PB3
#define SS          PB2

//  ADC Channel
#define ADC_CHANNEL 0b0000

//  Reset counter of timer0
#define PWM_10KHZ   0x063F
#define PWM_20KHZ   0x031F
#define PWM_105HZ   0x4A67

//  State for external interrupts
#define ENABLE      1
#define DISABLE     0
 //  Auxiliar Masks       
#define LOW         0
#define HIGH        1
#define CLEAR       0
#define SET         1


//  Operating Modes 
#define WAIT                        0x00
#define INITIAL_SETUP               0x01
#define SAFE_RESET                  0x02
#define BASIC_EXTENDED              0x03
#define BASIC_REFRACTED             0x04
#define ADVANCED_EXTENDED           0x06
#define ADVANCED_REFRACTED          0x07

//  States for initial setup mode
#define IS_INIT                     0x00                                      
#define IS_EXTENDED                 0x01
#define IS_STOP                     0x02                
#define IS_REFRACTED                0x03
#define IS_SEND_PULSE_RATE          0x04
#define IS_FINISHED                 0x05

#define SR_STOP                     0x00
#define SR_EXTENDED                 0x00
#define SR_REFRACTED                0x00
#define SR_FINISHED                 0x00    


//  SPI Commands      
#define DISCONNECTED()              0x00 
#define INITIALSETUP()              0x01 
#define SAFERESET()                 0x02  
#define BASICEXTENDED()             0x03 
#define BASICREFRACTED()            0X04
#define ADVANCEDEXTENDED()          0x05
#define ADVANCEDREFRACTED()         0x06  
#define STOP()                      0x07
#define SUCESS()                    0x08  
