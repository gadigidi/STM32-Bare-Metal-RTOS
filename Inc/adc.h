#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#define ADC1EN          (1U<<8)
#define CR2_ADON        (1U<<0)
#define CR2_SWSTART     (1U<<30)
#define SR_EOC          (1U<<1)
#define CR2_CONT        (1U<<1)

void adc1_pa0_pa1_init(void);
void adc1_enable_clock(void);
void adc1_config_sample_time(uint8_t channel, uint8_t sample_cycles);
void adc1_config_regular_sequence(uint8_t index, uint8_t channel);
void adc1_config_regular_sequence_length(uint8_t length);
void adc1_enable(void);
void adc1_start_conversion(void);
void adc1_start_continuous_conversion(void);
uint32_t adc1_read(void);

#endif /* ADC_H_ */

