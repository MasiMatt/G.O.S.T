/*************************************************
 * Title :      PWM Motor Controller             *
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
 * The PWM Motor Controller program controls two 
 * left side DC motors and two right side DC 
 * Motors with PWM output signals, and controls 
 * output LEDs. The output data for the PWM 
 * signals and LEDs are controlled by the input
 * data received through SPI communication and is 
 * triggered as an internal interrupt 
 */

/* === PREPROCESSOR DIRECTIVES === */

#include <avr/io.h> 
#include <avr/interrupt.h> 
#define F_CPU 8000000UL 
#include <util/delay.h>

/* === VOLATILE VARIABLE DECLARATION === */

volatile char data = 'a';       //SPI reception variable
volatile int on;                //on time of PWM signal
volatile int off;               //off time of PWM signal
volatile int var;               //port output value
volatile int ipt;               //check bit tags
volatile int led;               //enable pin for LEDs


int main (void)
{   
    DDRC = 0xff;                //enable PORTC as output
    DDRB = (1<<DDB4);           //set MISO output, all others input
    SPCR = (1<<SPE)|(1<<SPIE);  //enable SPI & SPI interrupt  
    SREG = 0x80;                //enable global interrupts

    var = 0;
    on = 10;
    off = 10;
    led = 2;
    int x = 0;
    
   
 sei();                      //set global interrupt enable
    
    while(1)
    {

    /* === PWM ON TIME === */   

        if(on != 10)            
        {   
            PORTC = var + led;  // LED and direction select
            for(x=0;x!=on;x++)  //on time duration
            {
                x=x;            //bogus instruction
            }
        }
        
    /* === PWM OFF TIME === */

        if(on != 20)
        {
            PORTC = 0 + led;    //LED and direction select
            for(x=0;x!=off;x++) //off time duration
            {
                x=x;            //bogus instruction
            }
    
        }
    }
}

/*************************************************************
 * Function : SPI Interrupt
 * ------------------------
 * This interrupt function is active whenever data is 
 * recieved by the ATMEGA through SPI. Recieved data is valued
 * between 0 and 10 which coresponds to the duty cycle of a 
 * pwm signal. All data is tagged with a bit to either enable 
 * the motor in reverse mode (64), enable the motor in forward
 * mode (32), or turn on the LEDs (128). 
 */

ISR(SPI_STC_vect)
{
    data = SPDR;            //read SPI data (char)
    ipt = data;             

    /* === ENABLING LEDs === */

    if(ipt >= 128)
    {       
        if(led == 2)        //disable LEDs
            led = 0;
        else
            led = 2;        //enable LEDs           
    }

   
 /* === REVERSE MOTOR === */

    else if(ipt >= 64)
    {   
        on = ipt - 64;      //removes tagged bit
        var = 5;            //PWM and reverse enabled

        on = on + 10;       //PWM on time
        off = 20 - on;      //PWM off time
    }

    /* === FORWARD MOTOR === */

    else 
    {       
        on = ipt - 32;      //removes tagged bit
        var = 9;            //PWM and forward enabled

        on = on + 10;       //PWM on time
        off = 20 - on;      //PWM off time
    }

    
} 
