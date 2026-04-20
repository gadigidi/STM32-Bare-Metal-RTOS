#ifndef DWT_H_
#define DWT_H_

#include <stdint.h>

#define TRCEN           (1U<<24)
#define TRC_START       (1U<<0)

void dwt_init(void);
uint32_t dwt_count(void);

#endif /* DWT_H_ */
