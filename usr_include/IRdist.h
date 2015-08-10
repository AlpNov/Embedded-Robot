/*
 * IRdist.h
 *
 *  Created on: Oct 10, 2013
 *      Author: ppovolot
 */

#ifndef IRDIST_H_
#define IRDIST_H_

// Initializes PE3 for Analog ADC Function
// Initializes the ADC for a single sample sequence
void IRdsitinit(void);
// Returns the Raw distance value
unsigned long IRdist(void);

#endif /* IRDIST_H_ */

