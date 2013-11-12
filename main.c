/*
 * main.c
 */

#include  "msp430g2231.h"

#define     LED1                  0
#define     LED2                  0
#define     LED_DIR               P1DIR
#define     LED_OUT               P1OUT



#define     BUTTON                BIT3
#define     BUTTON_OUT            P1OUT
#define     BUTTON_DIR            P1DIR
#define     BUTTON_IN             P1IN
#define     BUTTON_IE             P1IE    // Interrupt Enable
#define     BUTTON_IES            P1IES   // Interrupt Edge Select
#define     BUTTON_IFG            P1IFG   // Interrupt flag
#define     BUTTON_REN            P1REN   // Resistor Enable



void InitializeLeds(void);
void InitializeButton(void);


void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT



  BCSCTL1 |= DIVA_1;                        // ACLK/2
  BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO

  InitializeButton();
  InitializeLeds();

  __enable_interrupt();                     // Enable interrupts.

  /* Main Application Loop */
  while(1)
  {

	  __bis_SR_register(LPM3_bits + GIE);        // LPM0 with interrupts enabled

   }
}

void InitializeButton(void)                 // Configure Push Button
{
  BUTTON_DIR &= ~BUTTON;
  BUTTON_OUT |= BUTTON;
  BUTTON_REN |= BUTTON;
  BUTTON_IES &= ~BUTTON;
  BUTTON_IFG &= ~BUTTON;
  BUTTON_IE |= BUTTON;
}


void InitializeLeds(void)
{
  LED_DIR |= LED1 + LED2;
  LED_OUT |= LED1;                          // To enable the LED toggling effect
  LED_OUT &= ~LED2;
}

/* *************************************************************
 * Port Interrupt for Button Press
 * *********************************************************** */
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  BUTTON_IFG = 0;
  BUTTON_IE &= ~BUTTON;            /* Debounce */
  WDTCTL = WDT_ADLY_16;    /* (WDTPW+WDTTMSEL+WDTCNTCL+WDTSSEL+WDTIS0) */
  IFG1 &= ~WDTIFG;                 /* clear interrupt flag */
  IE1 |= WDTIE;

  LED_OUT ^= (LED1 + LED2);

}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    IE1 &= ~WDTIE;                   /* disable interrupt */
    IFG1 &= ~WDTIFG;                 /* clear interrupt flag */
    WDTCTL = WDTPW + WDTHOLD;        /* put WDT back in hold state */
    BUTTON_IE |= BUTTON;             /* Debouncing complete */
}








