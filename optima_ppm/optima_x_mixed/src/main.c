/*
* Copyright (c) 2012 Scott Driessens
* Licensed under the MIT License
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include <board/optima.h>
#include <core/usart.h>

/* number of ticks in 20 ms - clock/1024 */
#define FAILSAFE		144
/* number of ticks in 400 us ppm pulse */
#define PPM_PRE_PULSE	2949
/* number of ticks in 4600 us ppm sync period */
#define PPM_SYNC_PERIOD	(33915 - PPM_PRE_PULSE)

/* State machine states */
enum states {START, PREAMBLE, PACKET, VERIFY};

static uint16_t inputs[NUM_CHANNELS] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t buffer[2 * NUM_CHANNELS];
#if SELECTABLE
uint8_t ppm_flag = 0;
#endif
//static uint8_t failsafe = 0;

/* blocking delay */
#define delay(period) TIFR1 = (1 << OCF1A); TCNT1 = 0; OCR1A = period; while(!(TIFR1 & (1 << OCF1A)));
void ppm_handler(void);
void timer_init(void);

int main(void)
{
    uint8_t i;
    uint8_t j;
    uint8_t byte;
	/* Initialisation here */
	timer_init();
    
#if SELECTABLE
    *outputs[SELECTPIN].port |= outputs[SELECTPIN].mask;
    
    delay(737);
    for(i = 0; i < 250; ++i) {
        if(!(*outputs[SELECTPIN].pin & outputs[SELECTPIN].mask))
            ppm_flag = 1;
    }
    
    *outputs[SELECTPIN].port &= ~outputs[SELECTPIN].mask;
#endif
    
	usart_init();
		
	sei();  //interrupts ON
	
	/* State variable */
	enum states state = START;
	
	/* initialise output - only on those that are available*/
	for(i = 0; i < OPTIMA; ++i) {
		*outputs[i].ddr |= outputs[i].mask;
	}
	
	/* main loop */
	for (;;) {
		/* We have a character, do something */
		if(!q_empty(usart_rx)) {
			byte = q_take(usart_rx);
			switch(state)
			{
				case START: // A packet is arriving
					if(byte == 0xFF) {
						state = PREAMBLE;
					}		
					break;
				case PREAMBLE: // The packet begins on the next byte
					if(byte == 0xFF) {
						state = PACKET;
						j = 0;
					} else {
						state = START;
					}
					break;
				case PACKET: // Fill the buffer with the packet values
					buffer[j++] = byte;
					if(j >= 2 * NUM_CHANNELS) {
						// We have received the channel data
						state = VERIFY;
					}
					break;
				case VERIFY: // Verify the packet has finished
					if(byte == 0xEE) {
						// Fill the input values with the adjusted channel timing
						for(uint8_t i = 0; i < NUM_CHANNELS; ++i) {
							inputs[i] = ((uint16_t)(buffer[2*i] << 8) | (uint16_t)buffer[2*i+1]);
						}
						ppm_handler();
						//failsafe = 0;
					}
					// Fall through
				default:
					state = START;
			}
		} /*else if(failsafe > 49) { // failsafe if we have had no signal for 1s
			previous = failsafe;
			ppm_handler();
			failsafe = 50; // reset the counter
			
		}	*/
	}
}

void ppm_handler(void){
	uint8_t i;
#if SELECTABLE
    if(ppm_flag){
#endif
        
        for(i = 0; i < PPM_CHANNELS; ++i) {     //!! PPM_CHANNELS>OPTIMA !!
            /* ppm pre-pulse */
            *outputs[PPM].port |= outputs[PPM].mask;
            if(i != PPM-1 && i<OPTIMA)
                *outputs[i].port |= outputs[i].mask;
            delay(PPM_PRE_PULSE);
	
            /* end of pre-pulse */
            *outputs[PPM].port &= ~outputs[PPM].mask;
            delay(inputs[i]-PPM_PRE_PULSE);
	
            /* end of channel timing */
            if(i != PPM-1 && i<OPTIMA)
                *outputs[i].port &= ~outputs[i].mask;
        }
        /* ppm sync pulse */
        *outputs[PPM].port |= outputs[PPM].mask;
        delay(PPM_PRE_PULSE);

        /* ppm sync period */
        *outputs[PPM].port &= ~outputs[PPM].mask;
        delay(PPM_SYNC_PERIOD); //?
#if SELECTABLE
    } else{
        for(i = 0; i < OPTIMA; ++i) {
            *outputs[i].port |= outputs[i].mask;
            delay(inputs[i]);
            *outputs[i].port &= ~outputs[i].mask;
        }

    }
#endif
}


void timer_init(void)
{
	/* Output compare for PPM/PWM generation */
	OCR1A = 0xFFFF;
	
	/* start TIMER1 with no prescaler */
	TCCR1B = ( 1 << CS10 );
	
	/*
	// TIMER 0 - 20 ms intterupts
	// Timer 0 CTC mode
	TCCR0A |= (1 << WGM01);
	
	// Enable CTC interrupt
	TIMSK0 |= (1 << OCIE0A);
	
	// Output compare 20 ms
	OCR0A = FAILSAFE;
	
	// Start timer at clock/1024
	TCCR0B |= ((1 << CS02) | (1 << CS00));
	*/
}
/*
ISR(TIMER0_COMPA_vect)
{
	++failsafe;
}
*/
