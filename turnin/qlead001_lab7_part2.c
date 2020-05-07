/*	Author: Quinn Leader qlead001@ucr.edu
 *  Partner(s) Name: NA
 *	Lab Section: 026
 *	Assignment: Lab 7  Exercise 2
 *	Exercise Description: Lab 6 Exercise 2 but with LCD and a score
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR sets to 1, user code sets to 0

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1 ms ticks

void TimerOn() {
    // See https://docs.google.com/document/d/151t_PpK_cezrEfeSML2oEYGC0BedWDxp3f0Avicnptc/
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR () {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum States {
    Start,
    One,
    Two,
    Three,
    Two_2,
    Stop,
    WaitButton,
    WaitButton2,
    End,
    End2,
    End3,
} state;

unsigned char count = 0, score;

void Tick() {
    switch (state) {
        case Start:
            state = One;
            score = 5;
            break;
        case One:
            if ((~PINA)&0x01) {
                state = Stop;
                score--;
            }
            //else if (count < 3) count++;
            else {
                state = Two;
                count = 0;
            }
            break;
        case Two:
            if ((~PINA)&0x01) {
                state = Stop;
                score++;
            }
            //else if (count < 3) count++;
            else {
                state = Three;
                count = 0;
            }
            break;
        case Three:
            if ((~PINA)&0x01) {
                state = Stop;
                score--;
            }
            //else if (count < 3) count++;
            else {
                state = Two_2;
                count = 0;
            }
            break;
        case Two_2:
            if ((~PINA)&0x01) {
                state = Stop;
                score++;
            }
            //else if (count < 3) count++;
            else {
                state = One;
                count = 0;
            }
            break;
        case Stop:
            if (score >= 9) {
                state = End;
                LCD_DisplayString(1, "WINNER");
                PORTB = 8;
            } else if (score < 0) {
                state = End;
                LCD_DisplayString(1, "Game Over");
                PORTB = 0;
            } else if (!((~PINA)&0x01)) state = WaitButton;
            count = 0;
            break;
        case WaitButton:
            if ((~PINA)&0x01) state = WaitButton2;
            break;
        case WaitButton2:
            if (!((~PINA)&0x01)) state = One;
            break;
        case End:
            if (!((~PINA)&0x01)) state = End2;
            break;
        case End2:
            if ((~PINA)&0x01) state = End3;
            break;
        case End3:
            if (!((~PINA)&0x01)) state = Start;
            break;
        default:
            state = Start;
            break;
    }
    switch (state) {
        case Start:
            break;
        case One:
            PORTB = 1;
            break;
        case Two:
            PORTB = 2;
            break;
        case Three:
            PORTB = 4;
            break;
        case Two_2:
            PORTB = 2;
            break;
        default:
            break;
    }
    if (state != End && state != End2 && state != End3) {
        LCD_Cursor(1);
        LCD_WriteData(score+'0');
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
    LCD_init();
    TimerSet(300);
    TimerOn();
    state = Start;
    while (1) {
        Tick();
        while (!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
