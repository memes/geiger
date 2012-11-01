/* Simple ring buffer to store fixed number of counter values to allow
 * for rolling counts/minute calculation.
 *
 * Does not use atomic methods to protect the values.
 * 
 * All files are released under Creative Commons Attribution:
 * http://creativecommons.org/licenses/by/3.0/
 */

#include <stdint.h>

#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

/* Allow up to 60 samples to be stored
 */
#define RING_BUFFER_SIZE 60

void reset_ring_buffer(void);
void add_counter(uint16_t counter);
uint16_t get_sum_counters(void);

#endif
