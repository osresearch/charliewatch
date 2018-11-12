/*! \file adc.c
  \brief ADC12 one-shot reading

  Based on the TI Chronos example, modified to work without the interrupt.

*/
// *************************************************************************************************
//
//      Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//        Redistribution and use in source and binary forms, with or without
//        modification, are permitted provided that the following conditions
//        are met:
//
//          Redistributions of source code must retain the above copyright
//          notice, this list of conditions and the following disclaimer.
//
//          Redistributions in binary form must reproduce the above copyright
//          notice, this list of conditions and the following disclaimer in the
//          documentation and/or other materials provided with the
//          distribution.
//
//          Neither the name of Texas Instruments Incorporated nor the names of
//          its contributors may be used to endorse or promote products derived
//          from this software without specific prior written permission.
//
//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <msp430.h>
#include <stdio.h>
#include "adc.h"
#include "led.h"

// *************************************************************************************************
// @fn          adc12_single_conversion
// @brief       Init ADC12.      Do single conversion. Turn off ADC12.
// @param       uint16_t ref                  Select reference
//                              uint16_t sht          Sample-and-hold time
//                              uint16_t channel      Channel of the conversion
// @return      uint16_t adc12_result Return ADC result
// *************************************************************************************************
uint16_t adc12_single_conversion(uint16_t ref, uint16_t sht, uint16_t channel)
{
    // Initialize the shared reference module
    REFCTL0 |= REFMSTR + ref + REFON;   // Enable internal reference (1.5V or 2.5V)

    // Initialize ADC12_A
    ADC12CTL0 = sht + ADC12ON;          // Set sample time
    ADC12CTL1 = ADC12SHP;               // Enable sample timer
    ADC12MCTL0 = ADC12SREF_1 + channel; // ADC input channel
    ADC12IE = 0x001;                    // ADC_IFG upon conv result-ADCMEMO

    // Wait 2 ticks (66us) to allow internal reference to settle
    //Timer0_A4_Delay(2);
    //delay(1000);

    // Start ADC12
    ADC12CTL0 |= ADC12ENC;

/*
    // Clear data ready flag
    adc12_data_ready = 0;
*/

    // Sampling and conversion start
    ADC12CTL0 |= ADC12SC;

    //Start the conversion and fetch the result when ready.

    // Delay to get next ADC value
    //Timer0_A4_Delay(5);
    //delay(30);
    //delay(1000);
    //while (!adc12_data_ready) ;
    while ((ADC12IFG & BIT0)==0);
    uint16_t adc12_result = ADC12MEM0;

    // Shut down ADC12
    ADC12CTL0 &= ~(ADC12ENC | ADC12SC | sht);
    ADC12CTL0 &= ~ADC12ON;

    // Shut down reference voltage
    REFCTL0 &= ~(REFMSTR + ref + REFON);

    ADC12IE = 0;

    // Return ADC result
    return adc12_result;
}
