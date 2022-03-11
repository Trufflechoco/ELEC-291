#include <stdio.h>
#include <at89lp51rd2.h>


// ~C51~ 
 
#define CLK 22118400L // SYSCLK frequency in Hz
#define BAUD 115200L // Baud rate of UART in bps
#define ONE_USEC (CLK/1000000L) // Timer reload for one microsecond delay
#define BRG_VAL (0x100-(CLK/(16L*BAUD)))

#define ADC_CE  P2_0
#define BB_MOSI P2_1
#define BB_MISO P2_2
#define BB_SCLK P2_3

//  AT89LP51RC2 with LCD in 4-bit interface mode
//  2008-2022 by Jesus Calvino-Fraga

#define LCD_RS P3_2
// #define LCD_RW PX_X // Not used in this code, connect the pin to GND
#define LCD_E  P3_3
#define LCD_D4 P3_4
#define LCD_D5 P3_5
#define LCD_D6 P3_6
#define LCD_D7 P3_7
#define CHARS_PER_LINE 16

unsigned char SPIWrite(unsigned char out_byte)
{
	// In the 8051 architecture both ACC and B are bit addressable!
	ACC=out_byte;
	
	BB_MOSI=ACC_7; BB_SCLK=1; B_7=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_6; BB_SCLK=1; B_6=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_5; BB_SCLK=1; B_5=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_4; BB_SCLK=1; B_4=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_3; BB_SCLK=1; B_3=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_2; BB_SCLK=1; B_2=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_1; BB_SCLK=1; B_1=BB_MISO; BB_SCLK=0;
	BB_MOSI=ACC_0; BB_SCLK=1; B_0=BB_MISO; BB_SCLK=0;
	
	return B;
}

unsigned char _c51_external_startup(void)
{
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	
	// If the ports are configured in compatibility mode, this is not needed.
	P0M0=0x00; P0M1=0x00;    
	P1M0=0x00; P1M1=0x00;    
	P2M0=0x00; P2M1=0x00;    
	P3M0=0x00; P3M1=0x00;
	
	// Initialize the pins used for SPI
	ADC_CE=0;  // Disable SPI access to MCP3008
	BB_SCLK=0; // Resting state of SPI clock is '0'
	BB_MISO=1; // Write '1' to MISO before using as input
	
	// Configure the serial port and baud rate
    PCON|=0x80;
	SCON = 0x52;
    BDRCON=0;
    #if (CLK/(16L*BAUD))>0x100
    #error Can not set baudrate
    #endif
    BRL=BRG_VAL;
    BDRCON=BRR|TBCK|RBCK|SPD;
    
	CLKREG=0x00; // TPS=0000B

    return 0;
}

void wait_us (unsigned char x)
{
	unsigned int j;
	
	TR0=0; // Stop timer 0
	TMOD&=0xf0; // Clear the configuration bits for timer 0
	TMOD|=0x01; // Mode 1: 16-bit timer
	
	if(x>5) x-=5; // Subtract the overhead
	else x=1;
	
	j=-ONE_USEC*x;
	TF0=0;
	TH0=j/0x100;
	TL0=j%0x100;
	TR0=1; // Start timer 0
	while(TF0==0); //Wait for overflow
}

void waitms (unsigned int ms)
{
	unsigned int j;
	unsigned char k;
	for(j=0; j<ms; j++)
		for (k=0; k<4; k++) wait_us(250);
}

/*Read 10 bits from the MCP3008 ADC converter*/
unsigned int volatile GetADC(unsigned char channel)
{
	unsigned int adc;
	unsigned char spid;

	ADC_CE=0; // Activate the MCP3008 ADC.
	
	SPIWrite(0x01);// Send the start bit.
	spid=SPIWrite((channel*0x10)|0x80);	//Send single/diff* bit, D2, D1, and D0 bits.
	adc=((spid & 0x03)*0x100);// spid has the two most significant bits of the result.
	spid=SPIWrite(0x00);// It doesn't matter what we send now.
	adc+=spid;// spid contains the low part of the result. 
	
	ADC_CE=1; // Deactivate the MCP3008 ADC.
		
	return adc;
}

void LCD_pulse (void)
{
	LCD_E=1;
	wait_us(40);
	LCD_E=0;
}

void LCD_byte (unsigned char x)
{
	// The accumulator in the 8051 is bit addressable!
	ACC=x; //Send high nible
	LCD_D7=ACC_7;
	LCD_D6=ACC_6;
	LCD_D5=ACC_5;
	LCD_D4=ACC_4;
	LCD_pulse();
	wait_us(40);
	ACC=x; //Send low nible
	LCD_D7=ACC_3;
	LCD_D6=ACC_2;
	LCD_D5=ACC_1;
	LCD_D4=ACC_0;
	LCD_pulse();
}

void WriteData (unsigned char x)
{
	LCD_RS=1;
	LCD_byte(x);
	waitms(2);
}

void WriteCommand (unsigned char x)
{
	LCD_RS=0;
	LCD_byte(x);
	waitms(5);
}

void LCD_4BIT (void)
{
	LCD_E=0; // Resting state of LCD's enable is zero
	//LCD_RW=0; // We are only writing to the LCD in this program
	waitms(20);
	// First make sure the LCD is in 8-bit mode and then change to 4-bit mode
	WriteCommand(0x33);
	WriteCommand(0x33);
	WriteCommand(0x32); // Change to 4-bit mode

	// Configure the LCD
	WriteCommand(0x28);
	WriteCommand(0x0c);
	WriteCommand(0x01); // Clear screen command (takes some time)
	waitms(20); // Wait for clear screen command to finsih.
}

void LCDprint(char * string, unsigned char line, bit clear)
{
	int j;

	WriteCommand(line==2?0xc0:0x80);
	waitms(5);
	for(j=0; string[j]!=0; j++)	WriteData(string[j]);// Write the message
	if(clear) for(; j<CHARS_PER_LINE; j++) WriteData(' '); // Clear the rest of the line
}

#define VREF 4.096
char mystr[CHARS_PER_LINE+1];
void main (void)
{
	float y;
	unsigned int myof;
	float refVolt = 0.4;
	float Period1;
	float Period2;
	float period;
	float period2;
	float freq1;
	float freq2;
	float peakVoltage = 0.0;
	float peakVoltage2 = 0.0;
	float PPvoltage=0.0;
	float rmsVoltage=0.0;
	float rmsVoltage2=0.0;
	float timeDiff;
	float degree =0.0;
	float x;
	unsigned int i=0;

	waitms(500);	
	printf("\n\nAT89LP51Rx2 SPI ADC test program.\n");
	
	// Measure half period at pin P1.0 using timer 0
	TR0=0; // Stop timer 0
	TMOD&=0B_1111_0000; // Set timer 0 as 16-bit timer (step 1)
	TMOD|=0B_0000_0001; // Set timer 0 as 16-bit timer (step 2)
	TH0=0; TL0=0; myof=0; // Reset the timer and overflow counter
	TF0=0; // Clear overflow flag
	while (P1_0==1); // Wait for the signal to be zero
	while (P1_0==0); // Wait for the signal to be one
	TR0=1; // Start timing
	while (P1_0==1) // Wait for the signal to be zero
	{
		if (TF0) {TF0=0; myof++; }
	}
	TR0=0; // Stop timer 0
	// [myof,TH0,TL0] is half the period in multiples of 1/CLK, so:
	// (Assume Period is float)
	Period1=(myof*65536.0+TH0*256.0+TL0)*2.0;
	
	// Measure half period at pin P1.1 using timer 0
	TH0=0; TL0=0; myof=0; // Reset the timer and overflow counter
	TF0=0; // Clear overflow flag
	while (P1_1==1); // Wait for the signal to be zero
	while (P1_1==0); // Wait for the signal to be one
	TR0=1; // Start timing
	while (P1_1==1) // Wait for the signal to be zero
	{
		if (TF0) {TF0=0; myof++; }
	}
	TR0=0; // Stop timer 0
	Period2=(myof*65536.0+TH0*256.0+TL0)*2.0;
	
	TH0=0; TL0=0; myof=0; // Reset the timer and overflow counter
	TF0=0; // Clear overflow flag
	while (P1_1==1); // Wait for the signal to be zero
	while (P1_1==0); // Wait for the signal to be one
	TR0 = 1;
	while (P1_0 == 0);// Wait for second signal to be one
	TR0 = 0;
	timeDiff =(myof*65536.0+TH0*256.0+TL0);
	timeDiff = timeDiff / 22118400.0;
	
	period = Period1/22118400.0;
	freq1 = 1/period;
	period2 = Period2/22118400.0;
	freq2 = 1/period2;
	degree = (360.0 / period)* timeDiff;
	
		for(i=0; i < 100; i++)
		{
			// Convert the 10-bit integer from the ADC to voltage
			y=(GetADC(0)*VREF)/1023.0;
			if (y>peakVoltage){
				peakVoltage = y;
				rmsVoltage = peakVoltage * 0.7071;
			}
		
			// Convert the 10-bit integer from the ADC to voltage
			x = (GetADC(1) *VREF)/1023.0;
			if (x>peakVoltage2){
				peakVoltage2 = x;
				rmsVoltage2 = peakVoltage2 * 0.7071;
			}
		}
	
	//Printing to LCD
	while(1)
	{	
		//LCD section
		waitms(500); // Gives time to putty to start before sending text
	
		// Configure the LCD
		LCD_4BIT();
		
   		// Display data in the LCD
		sprintf(mystr, "Period1: %.4fs", period);
		LCDprint(mystr, 1, 1);
	
		sprintf(mystr, "Period2: %.4fs", period2);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);
		
		sprintf(mystr, "Freq1: %.4fHz", freq1);
		LCDprint(mystr, 1, 1);
		
		sprintf(mystr, "Freq2: %.4fHz", freq2);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);
		
		sprintf(mystr, "PeakV1: %.4fV", peakVoltage);
		LCDprint(mystr, 1, 1);
		
		sprintf(mystr, "PeakV2: %.4fV", peakVoltage2);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);
		
		sprintf(mystr, "PPV1: %.4fV", peakVoltage*2);
		LCDprint(mystr, 1, 1);
		
		sprintf(mystr, "PPV2: %.4fV", peakVoltage2*2);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);
		
		sprintf(mystr, "rmsV1: %.4fV", rmsVoltage);
		LCDprint(mystr, 1, 1);
		
		sprintf(mystr, "rmsV2: %.4fV", rmsVoltage2);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);
		
		sprintf(mystr, "tDiff: %.4fms", timeDiff*1000);
		LCDprint(mystr, 1, 1);
		
		sprintf(mystr, "Degree: %.2fdeg", degree);
		LCDprint(mystr, 2, 1);
		
		waitms(5000);

	}
}