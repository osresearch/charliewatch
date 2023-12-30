/*! \file rtc.h
  \brief Real Time Clock library.
*/

//! Flash buffer containing the manufacturing time.
extern const unsigned char romsavetime[];

//! Configure the Real Time Clock
void rtc_init();

//! Sets the DOW from the calendar date.
void rtc_setdow();

// msp430g2553 has no RTC
extern uint8_t RTCHOUR;
extern uint8_t RTCMIN;
extern uint8_t RTCSEC;

#include "rtcasm.h"

