
#define TIM2_ARR_10KHz          1600
#define TIM2_ARR_20KHz          0x0320

#ifndef F_CPU
    #define F_CPU           16000000ul //16MHz
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