#include "switch.h"
#include "Nokia5110.h"
#include "utils.h"
#include "led.h"

int lsw=1,rsw=1,usw=1,dsw=1,ensw=1;
int xl=1,yl=1,dx=3,dy=2,xr=1;
int com=0,won=0;
int delay ;
int endGame=0,playing=1,emptyFeild=1;
void Switch_Init(void){
	SYSCTL_RCGC2_R |= 0x00000004; // (a) activate clock for port F
	 delay = 0;
	GPIO_PORTC_LOCK_R = 0x4C4F434B;
	GPIO_PORTC_CR_R = 0xf0;           // allow changes to PF4-0
  GPIO_PORTC_DIR_R &= ~0xf0;    // (c) make PF4 in (built-in button)
	GPIO_PORTC_AFSEL_R &= ~0xf0;  //     disable alt funct on PF4
	GPIO_PORTC_DEN_R |= 0xf0;     //     enable digital I/O on PF4   
	GPIO_PORTC_PCTL_R &= ~0xffff0000; // configure PF4 as GPIO
	GPIO_PORTC_AMSEL_R = 0x00;       //     disable analog functionality on PF
	GPIO_PORTC_PUR_R |= 0xf0;     //     enable weak pull-up on PF4
	GPIO_PORTC_IS_R &= ~0xf0;     // (d) PF4 is edge-sensitive
	GPIO_PORTC_IBE_R &= ~0xf0;    //     PF4 is not both edges
	GPIO_PORTC_IEV_R &= ~0xf0;    //     PF4 falling edge event
	GPIO_PORTC_ICR_R = 0xf0;      // (e) clear flag4
	GPIO_PORTC_IM_R |= 0xf0;      // (f) arm interrupt on PF4 (SW1) and PF0 (SW2)
	
	GPIO_PORTA_LOCK_R = 0x4C4F434B;
	GPIO_PORTA_CR_R = 0x10;
  GPIO_PORTA_DIR_R &= ~0x10;
  GPIO_PORTA_AFSEL_R &= ~0x10;
  GPIO_PORTA_DEN_R |= 0x10;
  GPIO_PORTA_PCTL_R &= ~0x000f0000;
  GPIO_PORTA_AMSEL_R = 0x00;
  GPIO_PORTA_PUR_R |= 0x10;
  GPIO_PORTA_IS_R &= ~0x10;
  GPIO_PORTA_IBE_R &= ~0x10;
  GPIO_PORTA_IEV_R &= ~0x10;
  GPIO_PORTA_ICR_R = 0x10;
  GPIO_PORTA_IM_R |= 0x10;
	
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x00000005;      // (h) enable interrupt 30 (SW1) and 31 (SW2) in NVIC
	EnableInterrupts();

}

void GPIOPortC_Handler(void){
		//GPIO_PORTC_ICR_R=0xf0;
	 if(!(GPIO_PORTC_DATA_R&0x80)&&!endGame){
		 Delay100ms(1);
		 if(!(GPIO_PORTC_DATA_R&0x80)&&!endGame)
		 {
		 GPIO_PORTC_ICR_R = 0x80; 
		  Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar(' ');
		  if(xl==1){
				xl=7;
			}else{ xl=(xl-dx);}
			Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar('.');
		}
	 } 
	 else if(!(GPIO_PORTC_DATA_R&0x20)&&!endGame){
			Delay100ms(1);
			if(!(GPIO_PORTC_DATA_R&0x20)&&!endGame)
			{
		  GPIO_PORTC_ICR_R = 0x20;
		  Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar(' ');
			yl=(yl+dy)%6;
			Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar('.');
		  }
	 } 
	 else if(!(GPIO_PORTC_DATA_R&0x40)&&!endGame){
			Delay100ms(1);
		 if(!(GPIO_PORTC_DATA_R&0x40)&&!endGame)
		 {
			GPIO_PORTC_ICR_R = 0x40;
		  Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar(' ');
			yl=(yl-dy+6)%6;
			Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar('.');
		 }
	 } 
	 else if(!(GPIO_PORTC_DATA_R&0x10)&&!endGame){
		 Delay100ms(1);
		 if(!(GPIO_PORTC_DATA_R&0x10)&&!endGame)
		 {
		 GPIO_PORTC_ICR_R = 0x10;
		 	Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar(' ');
			xl=(xl+dx)%9;
			Nokia5110_SetCursor(xl, yl);
			Nokia5110_OutChar('.');
		 }
	 }

}
void GPIOPortA_Handler(void){
	//GPIO_PORTA_ICR_R = 0x10; 
	if(!(GPIO_PORTA_DATA_R&0x10)&&!endGame){
		Delay100ms(1);
		if(!(GPIO_PORTA_DATA_R&0x10)&&!endGame)
		{
		//Timer2_Init(10);
		GPIO_PORTA_ICR_R = 0x10; 
		if(data[xl/3][yl/2]==0){
			if(xr){
				Nokia5110_SetCursor(xl-1, yl);
				Nokia5110_OutChar('x');
				saveData(xl/3,yl/2,1);
			}else{
				Nokia5110_SetCursor(xl-1, yl);
				Nokia5110_OutChar('o');
				saveData(xl/3,yl/2,2);
			}
			
			won= win();
			com=complete();
			if(won||com){
				endGame=1;
				DisableInterrupts();
				return;
			}
			ensw=0;
			xr^=1;
			ledFlag^=1;
			LedsEnDis();
		}
		else if(data[xl/3][yl/2]!=0){
			emptyFeild=0;
			LedsEnDis();
		}
	}
	}
	
	
}

