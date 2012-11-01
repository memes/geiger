/* Geiger counter declarations
 * 
 * All files are released under Creative Commons Attribution:
 * http://creativecommons.org/licenses/by/3.0/
 */

#include <avr/io.h>

#ifndef _GEIGER_H
#define _GEIGER_H

/* The activity led is on pin 5 by default, and be turned off after 30ms
 */
#ifndef ACTIVITY_LED
#define ACTIVITY_LED 5
#endif
#ifndef LED_DELAY
#define LED_DELAY 30
#endif

#define LED_OFF_MASK (1 << ACTIVITY_LED);
#define LED_ON_MASK ~(1 << ACTIVITY_LED);

/* Default to 8Mhz if not overridden
 */
#ifndef F_CPU
#define F_CPU 8000000
#endif

/* UART serial configuration: 9600, 8, N, 1, full duplex using
 * interrupt for incoming data
 */
#ifndef BAUD
#define BAUD 9600
#endif
#define GEIGER_XFER (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0)
#define GEIGER_XFER_CFG (1 << UCSZ00) | (1 << UCSZ01)

/* Number of characters per LCD line, 16 + 2 chars for cursor control,
 * and number of LCD lines
 */
#define LCD_LINE_SIZE 18
#define LCD_LINE_COUNT 2

/* Display strings to use; note LCD control characters at start to
 * position cursor correctly.
 * These control characters are specifically for LCDs using Sparkfun
 * SerLCD 2.5 firmware as documented at
 * www.sparkfun.com/datasheets/LCD/SerLCD_V2_5.PDF
 */
const char* const LCD_LINE_1_FORMAT = "\376\200%u c/s (max %u)";
const char* const LCD_LINE_2_FORMAT = "\376\300%u c/m (max %u)";

/* Global counters
 */
volatile uint16_t counter = 0;
volatile uint16_t peak_cps = 0;
volatile uint16_t peak_cpm = 0;

/* Function declarations
 */
static void initialise(void);
static void reset(void);
static void write_buffer(const char* buffer, int size);

#endif
