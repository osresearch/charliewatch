/*! \file rtc.c
  \brief RTC driver for the GoodWatch/Charliewatch.
 */

#include <msp430.h>
#include <string.h>
#include <stdio.h>

#include "rtc.h"
#include "rtcasm.h"

//#include "api.h"
//#include "apps/calibrate.h"

//Automatically generated, not a part of the git repo.
//#include "buildtime.h"

//! ROM copy of the manufacturing time is generated at build time
extern const uint8_t romsavetime[];
extern const uint32_t buildtime;

//! If this is 0xdeadbeef, the ram time is good.
static unsigned long magicword __attribute__ ((section (".noinit")));
//! Time and date, in case of a reboot.
static unsigned char ramsavetime[8] __attribute__ ((section (".noinit")));

#ifdef CONFIG_ALARM
// Alarm tone status
static unsigned int alarm_ringing = 0;
#endif

//! Save the times to RAM.  Must be fast.
static void rtc_savetime(){
  ramsavetime[0]=RTCYEAR - 2000;
  ramsavetime[1]=RTCMON;
  ramsavetime[2]=RTCDAY;
  ramsavetime[3]=RTCHOUR;
  ramsavetime[4]=RTCMIN;
  ramsavetime[5]=RTCSEC;
  
  //Set the magic word, so we'll know the time is good.
  magicword=buildtime;
}

//! Load the time from RAM or ROM
static void rtc_loadtime(){
  //Use the RAM copy if it is reasonable.
  if(magicword!=buildtime){
    memcpy(ramsavetime,romsavetime,sizeof(ramsavetime));
  }
  
  /* We need to call these functions for safety, as there are some
     awful RTC errata to work around. */
  SetRTCHOUR(ramsavetime[3]   %24);
  SetRTCMIN(ramsavetime[4]   %60);
  SetRTCSEC(ramsavetime[5]   %60);
  
  SetRTCYEAR(ramsavetime[0] + 2000);
  SetRTCMON(ramsavetime[1]);
  SetRTCDAY(ramsavetime[2]);
  //printf("Setting RTCDAY to %d yielded %d.\n",
  //ramsavetime[7], RTCDAY);

  // opposite time
  //SetRTCHOUR(13);
  //SetRTCMIN(34);
  //SetRTCSEC(58);

  // align test
  if(0) {
    SetRTCHOUR(17);
    SetRTCMIN(25);
    SetRTCSEC(5);
  }

  //SetRTCHOUR(5);
  //SetRTCMIN(59);
  //SetRTCSEC(45);
}


//! Initializes the clock with the timestamp from memory.
void rtc_init(){
  // Setup RTC Timer

  // Calendar Mode, RTC1PS, 8-bit ovf
  // overflow interrupt enable
  // alarm interrupt enable
  RTCCTL01 = RTCTEVIE + RTCSSEL_2 + RTCTEV_0 + RTCMODE + RTCAIE;
  RTCPS0CTL = RT0PSDIV_2;                   // ACLK, /8, start timer
  RTCPS1CTL = RT1SSEL_2 + RT1PSDIV_3;       // out from RT0PS, /16, start timer

  #ifdef CALIBRATE_APP
  //Load the calibration routines.
  calibrate_enforce();
  #endif

  rtc_loadtime();
  rtc_setdow();
}


//! Years div by 4, but not by 100 unless also by 400 are leap years.
#define IS_LEAP_YEAR(year) ((year%400==0) || (year%4==0 && year%100!=0))
//! Number of leap years since 1984.  Accurate until 2200, then off by one.
#define LEAPS_SINCE_YEAR(year) (((year) - 1984) \
				+ ((year) - 1984) / 4 \
				+ (year>2100?-1:0)    \
				);


//! Returns the number of days for a given month.
static unsigned int rtc_get_max_days(unsigned int month, unsigned int year) {
  switch (month) {
    //Thirty days hath November, April, June, and September.
  case 4: case 6: case 9: case 11:
    return 30;

    //With 28 or 29 there is but one
  case 2:
    if (IS_LEAP_YEAR(year))
      return 29;
    else
      return 28;
    
    //All the rest have thirty one
  case 1: case 3: case 5: case 7: case 8: case 10: case 12:
    return 31;

  default:
    return 0;
  }
}

//! Sets the DOW from the calendar date.
void rtc_setdow(){
  unsigned int dow;

  //Begin with the offset for the current year.
  dow = LEAPS_SINCE_YEAR(RTCYEAR);

  //Subtract a day if this year is a leap year, but we haven't reached
  //the leap day yet.
  if ((29 == rtc_get_max_days(2, RTCYEAR)) && (RTCMON < 3))
    dow--;
    
  //Add this month's offset.
  switch (RTCMON) {
  case 5:
    dow += 1;
    break;
    
  case 8:
    dow += 2;
    break;
    
  case 2: case 3: case 11:
    dow += 3;
    break;
    
  case 6:
    dow += 4;
    break;
    
  case 9: case 12:
    dow += 5;
    break;
    
  case 4: case 7:
    dow += 6;
    break;
  }

  //Add the day of the current month.
  dow += RTCDAY;
  
  //Write the result to the register.
  dow = dow % 7;
  SetRTCDOW(dow);
}

//! Real Time Clock interrupt handler.
void __attribute__ ((interrupt(RTC_VECTOR))) RTC_ISR (void){
  //Save the time once a minute, so that when we reboot, we loose just
  //a few seconds.  We might later decide to call this in the
  //rendering loop.
  rtc_savetime();
  
  //We don't really handly these, but might want to in the future.
  switch(RTCIV&~1){
    case 0: break;                          // No interrupts
    case 2: break;                          // RTCRDYIFG
    case 4: break;                          // RTCTEVIFG
    case 6:                                 // RTCAIFG Alarm
#ifdef CONFIG_ALARM
      if (!alarm_ringing) {
	alarm_ringing = 1;
	printf("Sounding the alarm.\n");
	//! Sound the alarm
	tone(NOTE_C6, 500);
	tone(NOTE_E6, 500);
	tone(NOTE_G6, 500);
	tone(NOTE_B7, 500);
	tone(NOTE_C7, 500);
	alarm_ringing = 0;
      }
#endif
      break;
    case 8: break;                          // RT0PSIFG
    case 10: break;                         // RT1PSIFG
    case 12: break;                         // Reserved
    case 14: break;                         // Reserved
    case 16: break;                         // Reserved
    default: break;
  }
}
