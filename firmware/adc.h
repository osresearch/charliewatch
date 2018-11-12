/*! \file adc.h
  \brief ADC12 Driver for the GoodWatch.
  
  Mostly this uses the internal voltage reference in the ADC to
  measure the battery voltage.  For now, we aren't very interested in
  the temperature or other analog inputs.

*/


uint16_t adc12_single_conversion(uint16_t ref, uint16_t sht, uint16_t channel);
