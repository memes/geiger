/* Simple geiger counter program as a replacement for stock Sparkfun
 * geiger counter firmware. Designed to be used with a 16x2 LCD panel
 * attached to TX line, and 
 *
 * - displays count/sec with maximum since reset
 * - displays running count/minute based on last 60 seconds of
 *   samples, and a maximum since reset
 * 
 * All files are released under Creative Commons Attribution:
 * http://creativecommons.org/licenses/by/3.0/
 */

#include "geiger.h"
#include "ring_buffer.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <util/setbaud.h>

/* Output buffer; sized for two 16 character LCD lines + 2 control
 * characters per line
 */
static char buf[LCD_LINE_SIZE * LCD_LINE_COUNT];

int main(void)
{
    initialise();

    while (1) {
        /* Allow the CPU to sleep; will be woken on an interrupt, so
         * clear the LED and repeat.
         */
        sleep_cpu();
        _delay_ms(LED_DELAY);
        PORTC |= LED_OFF_MASK;
    }
}

void initialise(void)
{
    /* Disable interrupts during initialisation
     */
    cli();

    /* Setup i/o pins
     * PORTB - all output except input MISO on PB4
     * PORTC - all input except outputs PC0(?), PC4 (?) and PC5 (Led)
     * PORTD - all output except inputs PD0 (RX), PD2 (signal) and
     *         PD3(int1)
     */
    DDRB = ~(1 << DDB4);
    DDRC = (1 << ACTIVITY_LED) | (1 << DDC4) | (1 << DDC0);
    DDRD = ~((1 << DDD3) | (1 << DDD2) | (1 << DDD0));

    /* Enable external interrupt on INT0 (PD2) - falling edge trigger
     */
    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);

    /* Setup a 16 bit timer to generate 1s interrupts for display updates
     */
    TCCR1B |= (1 << CS12) | (1 << WGM12);
    OCR1A = (F_CPU / 256) - 1;
    TIMSK1 |= (1 << OCIE1A);

    /* Setup UART0
     */
    UCSR0B |= (GEIGER_XFER);
    UCSR0C |= (GEIGER_XFER_CFG);
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    PORTC |= LED_OFF_MASK;

    /* Allow the CPU to sleep and be woken on timer and external
     * interrupts
     */ 
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
}

/* Reset all internal counters, etc
 */
void reset(void)
{
    counter = peak_cps = peak_cpm = 0;
    reset_ring_buffer();
}

/* Helper to write the contents of a char buffer to UART; the number
 *  of characters written will be exactly 'size'. No validation is
 * performed.
 */
void write_buffer(const char* buffer, int size)
{
    for (int i = 0; i < size; i++) {
        /* Busy wait for UART ready to TX
         */
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = buf[i];
    }
}

/* Interrupt handler for UART byte receiving:-
 *  t - test by flashing LED 10 times
 *  z - reset all counters
 */
ISR(USART_RX_vect)
{
    uint8_t rx = UDR0;
    switch (rx) {
        case 't':
            for (int i = 0; i < 10; i++) {
                PORTC ^= LED_OFF_MASK;
                _delay_ms(100);
            }
            break;
        case 'z':
            reset();
            break;
    }
}

/* Handler for interrupt 0, triggered when a ionising event happens
 */
ISR(INT0_vect)
{
    counter++;
    PORTC &= LED_ON_MASK;
}

/* Timer interrupt; every second prepare to update the LCD display
 */
ISR(TIMER1_COMPA_vect)
{
    /* Store the counter and then reset it. Update current peak values
     * for count/sec and count/min
     */
    uint16_t tmp_counter = counter;
    counter = 0;
    add_counter(tmp_counter);
    if (tmp_counter > peak_cps) {
        peak_cps = tmp_counter;
    }
    uint16_t cpm = get_sum_counters();
    if (cpm > peak_cpm) {
        peak_cpm = cpm;
    }

    /* Prepare the buffer with contents to display on the LCD
     * - each line is limited to a fixed width set in geiger.h
     * - each line is right padded with spaces to provide a quick and
     *   dirty clean-up of trailing characters
     */
    int count = snprintf(buf, LCD_LINE_SIZE, LCD_LINE_1_FORMAT, tmp_counter,
                         peak_cps);
    while (count < LCD_LINE_SIZE) {
        buf[count++] = ' ';
    }
    count = snprintf(buf + LCD_LINE_SIZE, LCD_LINE_SIZE, LCD_LINE_2_FORMAT,
                     cpm, peak_cpm);
    while (count < LCD_LINE_SIZE) {
        buf[LCD_LINE_SIZE + count++] = ' ';
    }

    /* Re-enable UART buffer empty interrupt; the display will be
     * updated in that interrupt handler
     */
    UCSR0B |= (uint8_t) (1 << UDRE0);
}

/* Interrupt is triggered when the UDRE0 flag is set (see timer
 * interrupt) and the UART is ready to transmit data to update
 * listening devices.
 */
ISR(USART_UDRE_vect)
{
    /* Disable UART empty interrupt and write to UART
     */
    UCSR0B &= (uint8_t) ~(1 << UDRE0);
    write_buffer(buf, LCD_LINE_SIZE * LCD_LINE_COUNT);
}
