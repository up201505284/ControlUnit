#ifndef	_PRINTF_TOOLS_H
#define	_PRINTF_TOOLS_H	

#include <stdio.h>
#include <avr/io.h>

// baud rate
#define	baud              57600  

// baud divider
#define baudgen            ( ( F_CPU /  (16*baud ) ) - 1 )  

static int uart_putchar( char c, FILE *stream );

static FILE mystdout = FDEV_SETUP_STREAM( uart_putchar, NULL, _FDEV_SETUP_WRITE );

static int uart_putchar( char c, FILE *stream ) {
    
    // loop until bit is set ( UCSR0A, UDRE0 )
    while ( !( UCSR0A & ( 1<<UDRE0 ) ) );
    UDR0 = c;
    return 0;
    
}


static void init_printf_tools( void ) {
	
    // init stdout
    stdout = &mystdout; 

}

void init_USART( void ) {

   UBRR0 = baudgen;
   UCSR0B = ( 1 << RXEN0 ) | ( 1 << TXEN0 );
   UCSR0C = ( 1 << USBS0 ) | ( 3 << UCSZ00 );

}

#endif
