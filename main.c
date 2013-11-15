/**
 * Number Board Console
 */

#include <msp430.h>
#define SFR_8BIT(address)   extern volatile unsigned char address

#define		SHIFT_SER			(unsigned char)BIT0
#define		SHIFT_CLK			(unsigned char)BIT6
#define		SHIFT_EN			(unsigned char)BIT7

#define 	SHIFT_DIR			P1DIR
#define		SHIFT_OUT			P1OUT

#define 	KEYPAD_Y0			(unsigned char)BIT3
#define		KEYPAD_Y1			(unsigned char)BIT4
#define		KEYPAD_Y2			(unsigned char)BIT5
#define 	KEYPAD_Y			(unsigned char)(KEYPAD_Y0 + KEYPAD_Y1 + KEYPAD_Y2)

#define		KEYPAD_X0			(unsigned char)BIT0
#define		KEYPAD_X1			(unsigned char)BIT1
#define 	KEYPAD_X2			(unsigned char)BIT2
#define		KEYPAD_X3			(unsigned char)BIT3
#define		KEYPAD_X			(unsigned char)(KEYPAD_X0 + KEYPAD_X1 + KEYPAD_X2 + KEYPAD_X3)



#define     KEYPAD_X_OUT        P2OUT
#define     KEYPAD_X_DIR        P2DIR
#define     KEYPAD_X_IN         P2IN
#define     KEYPAD_X_IE         P2IE    // Interrupt Enable
#define     KEYPAD_X_IES        P2IES   // Interrupt Edge Select
#define     KEYPAD_X_IFG        P2IFG   // Interrupt flag
#define     KEYPAD_X_REN        P2REN   // Resistor Enable

#define     KEYPAD_Y_REN        P1REN   // Resistor Enable

void InitializeLeds(void);
void InitializeKeypad(void);
void delay(int time);

short bar[32];
short * shift[32];
short seven[12];

int keypad_inputs, debounce;
unsigned char key_pressed, last_key, y0_key, y1_key, y2_key;
unsigned char display_buffer[4];

enum Segments { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP };

void set(short digit, unsigned char value)
{
	volatile int ii;
	short *d = &bar[8 * digit];

	for ( ii = 0; ii < 8; ++ii )
	{
		d[ii] = value > 11 ? 0 : ((seven[value] >> ii) & 0x01);
	}

}

/*
 * main.c
 */
int main(void)
{
	short * const d1 = &bar[0];
	short * const d2 = &bar[8];
	short * const d3 = &bar[16];
	short * const d4 = &bar[24];

	shift[0] = &(d1[SEG_E]);		// pin 1
	shift[1] = &(d1[SEG_D]);		// pin 2
	shift[2] = &(d1[SEG_C]);		// pin 3
	shift[3] = &(d1[SEG_DP]);		// pin 4
	shift[4] = &d2[SEG_E];		// pin 5
	shift[5] = &d2[SEG_D];		// pin 6
	shift[6] = &d2[SEG_G];		// pin 7
	shift[7] = &d2[SEG_C];		// pin 8
	shift[8] = &d2[SEG_DP];		// pin 9
	shift[9] = &(d3[SEG_E]);		// pin 10
	shift[10] = &(d3[SEG_D]);		// pin 11
	shift[11] = &(d3[SEG_C]);		// pin 12
	shift[12] = &d4[SEG_E];		// pin 15
	shift[13] = &d4[SEG_D];		// pin 16
	shift[14] = &d4[SEG_G];		// pin 17
	shift[15] = &d4[SEG_C];		// pin 18
	shift[16] = &d4[SEG_B];		// pin 19
	shift[17] = &d4[SEG_A];		// pin 20
	shift[18] = &d4[SEG_F];		// pin 21
	shift[19] = &d4[SEG_DP];	// pin 22
	shift[20] = &(d3[SEG_B]);		// pin 23
	shift[21] = &(d3[SEG_A]);		// pin 24
	shift[22] = &(d3[SEG_G]);		// pin 25
	shift[23] = &(d3[SEG_F]);		// pin 26
	shift[24] = &(d3[SEG_DP]);	// pin 27
	shift[25] = &d2[SEG_B];		// pin 28
	shift[26] = &d2[SEG_A];		// pin 29
	shift[27] = &d2[SEG_F];		// pin 30
	shift[28] = &d1[SEG_B];		// pin 33
	shift[29] = &d1[SEG_A];		// pin 34
	shift[30] = &d1[SEG_G];		// pin 35
	shift[31] = &d1[SEG_F];		// pin 36

	// 7-segment codes 	   PGFE DCBA
	seven[0] = 0x3F;  	// 0011 1111
	seven[1] = 0x06;  	// 0000 0110
	seven[2] = 0x5B;	// 0101 1011
	seven[3] = 0x4F;	// 0100 1111
	seven[4] = 0x66;	// 0110 0110
	seven[5] = 0x6D;	// 0110 1101
	seven[6] = 0x7D;	// 0111 1101
	seven[7] = 0x07;	// 0000 0111
	seven[8] = 0x7F;	// 0111 1111
	seven[9] = 0x6F;	// 0110 1111
	seven[10] = 0x76;	// 0111 0110
	seven[11] = 0x5C;	// 0101 1100





  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT



  BCSCTL1 |= DIVA_1;                        // ACLK/2
  BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO

  InitializeLeds();
  InitializeKeypad();

  unsigned int ii;

	// initialize display
	for ( ii = 0; ii < 32; ++ii )
	{
		*shift[ii] = 0;
	}

  //__enable_interrupt();                     // Enable interrupts.

	  display_buffer[0] = 15;
	  display_buffer[1] = 15;
	  display_buffer[2] = 15;
	  display_buffer[3] = 15;

	  set(0, display_buffer[0]);
	  set(1, display_buffer[1]);
	  set(2, display_buffer[2]);
	  set(3, display_buffer[3]);

	  debounce = 100;


  /* Main Application Loop */
  while(1)
  {
	  unsigned int jj;

	  // disable outputs while shifting
	  SHIFT_OUT |= SHIFT_EN;

	  // shift in digits
	  for ( jj = 0; jj < 32; ++jj )
	  {

		  if ( *shift[jj] == 1 )
		  {
			  SHIFT_OUT &= ~SHIFT_SER;
		  }
		  else
		  {
			  SHIFT_OUT |= SHIFT_SER;
		  }

		  // Toggle clock
		  SHIFT_OUT &= ~SHIFT_CLK;
		  SHIFT_OUT |= SHIFT_CLK;
	  }

	  SHIFT_OUT |= SHIFT_SER;

	  // Toggle clock
	  SHIFT_OUT &= ~SHIFT_CLK;
	  SHIFT_OUT |= SHIFT_CLK;

	  SHIFT_OUT &= ~SHIFT_EN;


	  for ( jj = 0; jj < 50; jj++ )
	  {
		  last_key = key_pressed;
		  key_pressed = y0_key = y1_key = y2_key = 15;


		  // scan column 0
		  P1OUT |= KEYPAD_Y0;
		  if ( KEYPAD_X_IN == KEYPAD_X0 ) y0_key = 1;
		  if ( KEYPAD_X_IN == KEYPAD_X1 ) y0_key = 4;
		  if ( KEYPAD_X_IN == KEYPAD_X2 ) y0_key = 7;
		  if ( KEYPAD_X_IN == KEYPAD_X3 ) y0_key = 10;
		  P1OUT &= ~KEYPAD_Y0;

		  // scan column 1
		  P1OUT |= KEYPAD_Y1;
		  if ( KEYPAD_X_IN == KEYPAD_X0 ) y1_key = 2;
		  if ( KEYPAD_X_IN == KEYPAD_X1 ) y1_key = 5;
		  if ( KEYPAD_X_IN == KEYPAD_X2 ) y1_key = 8;
		  if ( KEYPAD_X_IN == KEYPAD_X3 ) y1_key = 11;
		  P1OUT &= ~KEYPAD_Y1;

		  // scan column 2
		  P1OUT |= KEYPAD_Y2;
		  if ( KEYPAD_X_IN == KEYPAD_X0 ) y2_key = 3;
		  if ( KEYPAD_X_IN == KEYPAD_X1 ) y2_key = 6;
		  if ( KEYPAD_X_IN == KEYPAD_X2 ) y2_key = 0;
		  if ( KEYPAD_X_IN == KEYPAD_X3 ) y2_key = 9;
		  P1OUT &= ~KEYPAD_Y;

		  // check for multiple keys
		  if ( y0_key < 15 && y1_key == 15 && y2_key == 15 ) key_pressed = y0_key;
		  if ( y1_key < 15 && y0_key == 15 && y2_key == 15 ) key_pressed = y1_key;
		  if ( y2_key < 15 && y0_key == 15 && y1_key == 15 ) key_pressed = y2_key;

		  if ( key_pressed != last_key )
		  {
			  debounce = 100;
		  }
		  else if ( debounce > 0 && key_pressed < 12 )
		  {
			  --debounce;
		  }

		  if ( debounce == 0 )
		  {
			  --debounce;

			  // normal number button pressed
			  if ( key_pressed < 10 )
			  {
				  // shift buffer down one
				  display_buffer[0] = display_buffer[1];
				  display_buffer[1] = display_buffer[2];
				  display_buffer[2] = display_buffer[3];

				  // insert new value into buffer
				  display_buffer[3] = key_pressed;
			  }
			  // clear (star) button pressed
			  else if ( key_pressed == 10 )
			  {
				  display_buffer[0] = 15;
				  display_buffer[1] = 15;
				  display_buffer[2] = 15;
				  display_buffer[3] = 15;
			  }

			  // backspace (pound) button pressed
			  else if ( key_pressed == 11 )
			  {
				  display_buffer[3] = display_buffer[2];
				  display_buffer[2] = display_buffer[1];
				  display_buffer[1] = display_buffer[0];
				  display_buffer[0] = 15;
			  }

			  // update display
			  for ( jj = 0; jj < 4; jj++ )
			  {
				  set(jj, display_buffer[jj]);
			  }

		  }
	  }

	  //__bis_SR_register(LPM3_bits + GIE);        // LPM0 with interrupts enabled

   }

  return 0;
}

void delay(int time)
{
	int i, j, k;
	for ( i = time; i == 0; --i )
	{
		for ( j = 1000; j == 0; --j ) { k = i + j; }
	}
}

void InitializeLeds(void)
{
  SHIFT_DIR |= SHIFT_SER + SHIFT_CLK + SHIFT_EN;
}


void InitializeKeypad(void)
{
	// Inputs
	KEYPAD_X_DIR &= ~KEYPAD_X;
	KEYPAD_X_OUT = 0;				// pull down resistors
	KEYPAD_X_REN |= KEYPAD_X;		// enable internal resistor

	// Outputs
	P1DIR |= KEYPAD_Y;
	P1OUT &= ~KEYPAD_Y;
}

