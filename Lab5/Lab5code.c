#include <stdio.h>
#include <at89lp51rd2.h>


// ~C51~ 
 
#define CLK 22118400L
#define BAUD 115200L
#define ONE_USEC (CLK/1000000L) // Timer reload for one microsecond delay
#define BRG_VAL (0x100-(CLK/(16L*BAUD)))

#define ADC_CE  P2_0
#define BB_MOSI P2_1
#define BB_MISO P2_2
#define BB_SCLK P2_3

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

#define VREF 4.096

void main (void)
{
	float y;
	unsigned char i;
	unsigned int myof;
	float refVolt = 0.4;
	float Period1;
	float Period2;
	float period;
	float period2;
	float peakVoltage = 0.0;
	float peakVoltage2 = 0.0;
	float timeDiff;
	float degree =0.0;
	float x;

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
//	TR0=0; // Stop timer 0
//	TMOD&=0B_1111_0000; // Set timer 0 as 16-bit timer (step 1)
//	TMOD|=0B_0000_0001; // Set timer 0 as 16-bit timer (step 2)
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
	// [myof,TH0,TL0] is half the period in multiples of 1/CLK, so:
	// (Assume Period is float)
	Period2=(myof*65536.0+TH0*256.0+TL0)*2.0;
	
	TH0=0; TL0=0; myof=0; // Reset the timer and overflow counter
	TF0=0; // Clear overflow flag
	while (P1_1==1); // Wait for the signal to be zero
	while (P1_1==0); // Wait for the signal to be one
	TR0 = 1;
	while (P1_0 == 0);// Wait for second signal to be one
	TR0 = 0;
	timeDiff =(myof*65536.0+TH0*256.0+TL0)*2.0;
	timeDiff = timeDiff / 22118400.0;
	
	
	while(1)
	{	
		period = Period1/22118400.0;
		period2 = Period2/22118400.0;
		degree = (360.0 / period)* timeDiff;
		for(i=0; i<1; i++)
		{
			y=(GetADC(i)*VREF)/1023.0;
			if (y>peakVoltage){
			peakVoltage = y;
			peakVoltage = peakVoltage * 0.7071;
			} // Convert the 10-bit integer from the ADC to voltage
			
			x = (GetADC(1) *VREF)/1023.0;
			if (x>peakVoltage2){
			peakVoltage2 = x;
			peakVoltage2 = peakVoltage2 * 0.7071;
			}
	
			printf("time diff = %f", timeDiff);
			
			printf("degree = %f", degree);
			
			printf("Period=%f", period);
			
			printf("Period2 = %f" , period2);
			
			printf("Peak Voltage = %f", peakVoltage);
			
			printf("Peak Voltage2 = %f", peakVoltage2);
		}
	}
}
