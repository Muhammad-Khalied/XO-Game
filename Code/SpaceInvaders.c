// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "switch.h"
#include "led.h"
#include "utils.h"

void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;
char xRes='0',oRes='0';
int end=0,closeGame=0,playAgain=1,selectEnd=1,adcData;
extern int xl,yl,xr;
extern int com,won;
int t=5;
int data[3][3]={0,0,0,0,0,0,0,0,0};
char graph[5][12]={"  |  |      ","-------- x|o","  |  |   0|0","--------    ","  |  |      "};

void ADC1Seq3_Handler(void){	
	if((ADC1_RIS_R&(1<<3))){
		ADC1_ISC_R|=(1<<3);
		adcData = ADC1_SSFIFO3_R;		
	}
		
}

int complete(){   
	//if all feilds complete
	if(data[0][0]!=0&&data[0][1]!=0&&data[0][2]!=0&&data[1][0]!=0&&data[1][1]!=0&&data[1][2]!=0&&data[2][0]!=0&&data[2][1]!=0&&data[2][2]!=0){
		return 1;
	}else{
		return 0;
	}
}
void saveData(int x,int y,int round){ //record data to the array of data 
	/*x=x/3;   // x values are 1 , 4 ,7 and maps to 0 ,1 ,2 in data[][]
	y=y/2;	 // y values are 1 , 3 ,5 and maps to 0 ,1 ,2 in data[][]*/
	data[x][y]=round;  
}
int win(){
	if( (data[0][0]==data[0][1]&&data[0][1]==data[0][2]&&data[0][1]!=0)||
		  (data[1][0]==data[1][1]&&data[1][1]==data[1][2]&&data[1][1]!=0)||
		  (data[2][0]==data[2][1]&&data[2][1]==data[2][2]&&data[2][1]!=0)||
		  (data[0][0]==data[1][0]&&data[1][0]==data[2][0]&&data[1][0]!=0)||
			(data[0][1]==data[1][1]&&data[1][1]==data[2][1]&&data[1][1]!=0)||
			(data[0][2]==data[1][2]&&data[1][2]==data[2][2]&&data[1][2]!=0)||
			(data[0][0]==data[1][1]&&data[1][1]==data[2][2]&&data[1][1]!=0)||
			(data[0][2]==data[1][1]&&data[1][1]==data[2][0]&&data[1][1]!=0)){
				
				return 1;
		
	}
	else{
		return 0;
	}
}
void playXO(){
	data[0][0]= 0,data[1][0]= 0,data[2][0]= 0;
	data[0][1]= 0,data[1][1]= 0,data[2][1]= 0;
	data[0][2]= 0,data[1][2]= 0,data[2][2]= 0;
	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 2);
	Nokia5110_OutString("Loading...");
	//Timer2_Init(100000);
	Delay100ms(30);
	LedsEnDis();
	xl=1,yl=1;
	com=0,won=0;
	Nokia5110_Clear();
  Nokia5110_SetCursor(0, 1);
	graph[2][9]=xRes;
	graph[2][11]=oRes;
	Nokia5110_OutString(graph[0]);
	Nokia5110_SetCursor(xl, yl);
	Nokia5110_OutChar('.');
	EnableInterrupts();
	while(!(won||com)){ // play until a gamer win or the fields completed 
		
	}

	Nokia5110_Clear();
	Nokia5110_SetCursor(3, 1);
	//xr^=1;
	if(won){
		if(xr){
			xRes++;
			xr=1;
			ledFlag=1;
			Nokia5110_OutChar('x');
			Nokia5110_OutString(" won!");
			Nokia5110_SetCursor(3, 4);
			Nokia5110_OutChar(xRes);
			Nokia5110_OutChar(':');
			Nokia5110_OutChar(oRes);
			t=12;
			while(t--){
			  Delay100ms(1);
				GPIO_PORTF_DATA_R ^=0x08;
				Delay100ms(1);
			}
		}else{
			oRes++;
			xr=0;
			ledFlag=0;
			Nokia5110_OutChar('o');
			Nokia5110_OutString(" won!");
			Nokia5110_SetCursor(3, 4);
			Nokia5110_OutChar(xRes);
			Nokia5110_OutChar(':');
			Nokia5110_OutChar(oRes);
			t=12;
			while(t--){
			Delay100ms(1);
				GPIO_PORTF_DATA_R ^=0x04;
				Delay100ms(1);
			}
		}
		GPIO_PORTF_DATA_R=0x00;
	  Delay100ms(30);
	}else{
		Nokia5110_OutString("Gameover");
	}
	selectEnd=1;
	while(selectEnd)
	{
		Nokia5110_Clear();
				//1-continue
		Nokia5110_SetCursor(1,2);
		Nokia5110_OutString("1-continue");
		Nokia5110_SetCursor(1,4);
		Nokia5110_OutString("2-end game");
	//	EnableInterrupts();
		adcData = ADC1_SSFIFO3_R;	
		if(adcData<2047){
		Nokia5110_SetCursor(0, 3);
		Nokia5110_OutString("            ");
		Nokia5110_SetCursor(0, 4);
		Nokia5110_OutString(" ");
		Nokia5110_SetCursor(0, 5);
		Nokia5110_OutString("            ");
		Nokia5110_SetCursor(11,4);
		Nokia5110_OutString(" ");
		Nokia5110_SetCursor(0, 1);
		Nokia5110_OutString("************");
		Nokia5110_SetCursor(0, 2);
		Nokia5110_OutString("#");
		Nokia5110_SetCursor(0, 3);
		Nokia5110_OutString("************");
		Nokia5110_SetCursor(11,2);
		Nokia5110_OutString("#");
	}else{
		Nokia5110_SetCursor(0, 1);
		Nokia5110_OutString("            ");
		Nokia5110_SetCursor(0, 2);
		Nokia5110_OutString(" ");
		Nokia5110_SetCursor(0, 3);
		Nokia5110_OutString("            ");
		Nokia5110_SetCursor(11,2);
		Nokia5110_OutString(" ");
		
		Nokia5110_SetCursor(0, 3);
		Nokia5110_OutString("************");
		Nokia5110_SetCursor(0, 4);
		Nokia5110_OutString("#");
		Nokia5110_SetCursor(0, 5);
		Nokia5110_OutString("************");
		Nokia5110_SetCursor(11,4);
		Nokia5110_OutString("#"); 
	}
	while(!(GPIO_PORTA_DATA_R&0x10))
	{
		if(adcData<2047){
		 playAgain=1;
  	}else{
		 playAgain=0;
	  }
	 selectEnd=0;
	 endGame=0;

	}
	Delay100ms(1);
	Nokia5110_Clear();
	
	}	


}

int main(void){
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
	Switch_Init();
	GPIOPortF_Init();
	ADC_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	//Delay100ms(50);
	xr = 1;
	while(playAgain){
		playXO();
	}
	
	Nokia5110_Clear();
	Nokia5110_SetCursor(2, 2);
	Nokia5110_OutString("Thanks..");
 /*
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Nice try,");
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("Earthling!");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(1234);
*/
  while(1){
		
  }

}


// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
  Semaphore = 1; // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

void 	ADC_Init(void){
	SYSCTL_RCGCADC_R = (1<<1);  //ENABLE ADC MODULE 1
	
	SYSCTL_RCGCGPIO_R|=(1<<4);  // ENABLE PORT E CLOCK
	delay=0;
	GPIO_PORTE_AFSEL_R =(1<<1); //ENABLE ALTERNATE FUNCTION FOR PE1
	GPIO_PORTE_DEN_R  &=~(1<<1); // DISABLE DIGITAL FUNTION FOR PE1
	GPIO_PORTE_AMSEL_R =(1<<1);  //DISABLE THE ANALOG ISOLATION  //ANALOG MODE SELECT
	ADC1_ACTSS_R&=~(1<<3);  // disable sample sequencer 3
	ADC1_EMUX_R = (0xf<<12);// sample sequencer 3 -> always
	ADC1_SSMUX3_R=(2);      // sample sequencer 3 READS FROM AIN2->PE1
	ADC1_SSCTL3_R=(1<<1)    // End bit must be set
							 |(1<<2);		// Enable interrupt for sample sequencer 3
	ADC1_IM_R |=(1<<3);			// Enable mask for sample sequencer 3
	ADC1_ISC_R |=(1<<3);    // CLEAR ADC1 INTERRUPT FLAG
	ADC1_RIS_R |=(1<<3);		// clear interrupt status for sample sequencer 3
	ADC1_ACTSS_R |=(1<<3);  // Enable sample sequencer 3
	NVIC_EN1_R |=(1<<19);    //INT_ADC1SS3_R

}