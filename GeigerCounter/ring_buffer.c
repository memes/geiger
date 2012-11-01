/* Implementation of a very simple ring buffer for storing counter
 * values. Values are stored as 16-bit values but not using atomic
 * methods, so it is entirely possible for a value to be changed
 * during use.
 * 
 * All files are released under Creative Commons Attribution:
 * http://creativecommons.org/licenses/by/3.0/
 */

#include "ring_buffer.h"
#include <stdint.h>

static uint16_t counter_buffer[RING_BUFFER_SIZE];
static int offset = 0;

/* Reset the buffer, replacing all values with 0
 */
void reset_ring_buffer(void)
{
    offset = 0;
    for (int i = 0; i < RING_BUFFER_SIZE; i++) {
        counter_buffer[i] = 0;
    }
}

/* Add a counter value to the buffer
 */
void add_counter(uint16_t counter)
{
    counter_buffer[offset++] = counter;
    if (offset >= RING_BUFFER_SIZE) {
        offset = 0;
    }
}

/* Helper method to sum the contents of the buffer; 60 entries in
 * buffer so sum gives count/minute
 */
uint16_t get_sum_counters()
{
    uint16_t sum = 0;
    for (int i = 0; i < RING_BUFFER_SIZE; i++) {
        sum += counter_buffer[i];
    }
    return sum;
}
