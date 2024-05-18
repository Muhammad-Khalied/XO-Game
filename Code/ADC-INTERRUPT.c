#include "..//tm4c123gh6pm.h"

#include "TExaS.h"
#include <stdio.h>
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);
void Timer2_Init(unsigned long period);
void 	ADC_Init(void);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
unsigned long TimerCount;
unsigned long Semaphore;

volatile int adcData=1000,delay;

void GpioPortF_Init(){
	// intializing PF as digital output 
	GPIO_PORTF_DEN_R|=0x0E; // enable PF1,PF2,PF3 as a digital 
	GPIO_PORTF_DIR_R|=0x0E; // set PF1,PF2,PF3 as a digital output
}
//  ADC 
void ADC1Seq3_Handler(void){	
	if((ADC1_RIS_R&(1<<3))){
		ADC1_ISC_R|=(1<<3);
		adcData = ADC1_SSFIFO3_R;
		
		if(adcData<500){
				GPIO_PORTF_DATA_R=0x00;
			}else if(adcData>=500&&adcData<1000){
				GPIO_PORTF_DATA_R=0x02;
			}else if(adcData>=1000&&adcData<1500){
				GPIO_PORTF_DATA_R=0x04;
			}else if(adcData<1500&&adcData>=2000){
				GPIO_PORTF_DATA_R=0x06;
			}else if(adcData>=2000&&adcData<2500){
				GPIO_PORTF_DATA_R=0x08;
			}else if(adcData>=2500&&adcData<3000){
				GPIO_PORTF_DATA_R=0x0A;
			}else if(adcData>=3000&&adcData<3500){
				GPIO_PORTF_DATA_R=0x0C;
			}else if(adcData>=3500){
				GPIO_PORTF_DATA_R=0x0E;
			}
	}
		
}
int main(void){
	ADC_Init();
	GpioPortF_Init();
	EnableInterrupts();
 while(1){
		WaitForInterrupt();
  }

}



void 	ADC_Init(void){
	SYSCTL_RCGCADC_R = (1<<1);  //ENABLE ADC MODULE 1
	
	SYSCTL_RCGCGPIO_R=(1<<5)   // ENABLE PORT F CLOCK
									 |(1<<4);  // ENABLE PORT E CLOCK
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
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
// pink     R-B    0x06