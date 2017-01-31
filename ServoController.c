/*************************************************
 * Title :      Servomotor Controller            *
 *************************************************
 * Author :     Matthew Masi & Michael Scalera   *
 *************************************************
 * Chip :       ATMEGA8L                         *
 *************************************************                  
 * Created :    12/02/13                         *
 *************************************************                  
 * Modified :   24/04/13                         *
 *************************************************/
 

/*************************************************
 * The Servomotor Controller program controls a 
 * servomotor with different length pulses. The
 * duration of each pulse will put the servomotor
 * at a different angle. These pulses are affected
 * by the input data received through SPI 
 * communication and are triggered as an internal 
 * interrupt.  
 */ 

/* === PREPROCESSOR DIRECTIVES === */
 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#define F_CPU 8000000UL 
#include <util/delay.h>

/* === VOLATILE VARIABLE DECLARATION === */

volatile char data = 'a';
volatile int on;
volatile int y;
volatile int x;
volatile int z;

int main (void)
{   
    DDRC = 0xff;                    //enable PORTC as output
    DDRB = (1<<DDB4);               //set MISO output, all others input             
    SPCR = (1<<SPE) | (1<<SPIE);    //enable SPI & SPI interrupt 
    SREG = 0x80;                    //enable global interrupts

    on = 0;
    y = 0;
    x = 0;
    z = 0;

    sei();                          //set global interrupt enable
    
    while(1)
    {
    
        /* === X-AXIS ON-TIME PULSE LENGTH === */
    
        PORTC = 1;                  //pin connected to x-axis servo
        _delay_us(550);             //550us buffer between servomotors

        for(x=0;x<=y;x++)
        {
            _delay_us(180);         //pulse on-time for x-axis
        }
        
        /* === Y-AXIS ON-TIME PULSE LENGTH === */
        
        PORTC = 2;                  //pin connected to y-axis servo
        _delay_us(550);             //550us buffer between servomotors

        for(x=0;x<=z;x++)
        {
            _delay_us(180);         //pulse on-time for y-axis
        }

        /* === X & Y-AXIS OFF-TIME PULSE LENGTH === */
            
        PORTC = 0;
        _delay_ms(10);              //10ms pulse off-time
    }
}

/*************************************************************
 * Function : SPI Interrupt
 * ------------------------
 * This interrupt function is active whenever data is 
 * recieved by the ATMEGA through SPI. Recieved data is valued
 * between 0 and 10 which coresponds to the duration of a 
 * signal pulse. All data is tagged with a bit to either enable 
 * the y-axis motor (32), or enable the x-axis motor (64). 
 */

ISR(SPI_STC_vect)
{
    data = SPDR;                    //read SPI data (char)
    on = data;                      
    
    /* === X-AXIS ROTATION OF SERVOMOTOR === */
    
    if(on >= 64)
    {
        on = on -64;                //removes bit tag
        z = 10 - on;                //on-time pulse length
    }

    /* === Y-AXIS ROTATION OF SERVOMOTOR === */

    else if(on >=32 && on <= 63)
    {
        on = on -32;                //removes bit tag
        y = 10 - on;                //on-time pulse length

    } 
}
