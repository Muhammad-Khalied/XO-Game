#include"led.h"
#include "utils.h"
int ledFlag=1;
void LedsEnDis(void){
	if(ledFlag){
		GPIO_PORTF_DATA_R=0x08;
	}
	if(!ledFlag){
		GPIO_PORTF_DATA_R=0x04;
	}
	if(!emptyFeild){
		GPIO_PORTF_DATA_R|=0x02;
		//delay
		Delay100ms(3);
		//...
		GPIO_PORTF_DATA_R&=~0x02;
		emptyFeild=1;
	}
}
void GPIOPortF_Init(void){
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x0E;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  //GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x0E;          // 7) enable digital pins PF4-PF0
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  //NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 (SW1) and 31 (SW2) in NVIC
	EnableInterrupts();
}

