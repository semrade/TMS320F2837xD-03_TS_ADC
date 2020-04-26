/***********************************************************************************
 * File              :TS_SysMng_ADC.c
 *
 * Title             :
 *
 * Author            :Tarik SEMRADE
 *
 * Created on        :Mar 19, 2020
 *
 * Version           :
 *
 * Description       : ADC Reset and management
 *
 * Copyright (c) 2020 Tarik SEMRADE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************/
#include "TS_SysMng_Adc.h"
#include "F28x_Project.h"
#include "F2837xD_Adc_defines.h"
#include "TS_SysMng/TS_SysMng_X.h"
/**********************************************************************************
 * \function:       TS_SysMng_AdcConfig
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/

void
TS_SysMng_AdcConfig(void)
{
    /* Enable EALLOW protected register access */
    EALLOW;

    /* ADC reset*/
    DevCfgRegs.SOFTPRES13.bit.ADC_A = 1;

    /* ADC is released from reset */
    DevCfgRegs.SOFTPRES13.bit.ADC_A = 0;

    /* Interrupt pulse position when end of conversion occurs */
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 0x1;

    /* Adc clock configuration is 50Mhz */
    AdcaRegs.ADCCTL2.bit.PRESCALE = 0b0110;

    /* Call AdcSetMode() to configure the resolution and signal mode. */
    /* This also performs the correct ADC calibration for the configured mode. */
    AdcSetMode(ADC_ADCA, ADC_BITRESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    /*
     * => SOC0 configuration
     * => Trigger using Timer0
     * => Convert channel ADCINA0 (Ch. 0)
     * => Acquisition window set to (19+1)=20 cycles (100 ns with 200 MHz SYSCLK)
     * */
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0x01;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 19;

    /* No ADC interrupt triggers SOC0 (TRIGSEL field alone determines SOC0 trigger) */
    AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 0;

    /* All SOCs handled in round-robin mode, no priority */
    AdcaRegs.ADCSOCPRICTL.bit.SOCPRIORITY = 0;

    /*
     * => ADCA1 interrupt configuration
     * => Interrupt pulses regardless of flag state
     * => Enable the interrupt in the ADC
     * => EOC0 triggers the interrupt
     * */
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    /*
     * => Power up ADC-A
     * => Wait 1 ms after power-up before using the ADC
     * => Disable EALLOW protected register access
     * */
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1000);
    EDIS;

}
/**********************************************************************************
 * \function:       TS_SysMng_AdcAcqEnding
 * \brief           main `0` numbers
 * \param[in]       void
 * \return          void
 **********************************************************************************/
void
TS_SysMng_AdcAcqEnding(void)
{
    /* Wait for end of conversion event */
    while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT1);

    /* Clear event ADC-A flag interrupt acquisition */
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

}


