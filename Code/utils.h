#ifndef _UTILS_H
#define _UTILS_H
#include "..//tm4c123gh6pm.h"
extern int lsw,rsw,usw,dsw,ensw;
extern int xl,yl,dx,dy,xr,endGame,playAgain,selectEnd;
extern int data[3][3] ;
extern int delay;
void saveData(int x,int y,int round);
extern int com,won,end,closeGame,adcData;
int win();
int complete();
void Timer2_Init(unsigned long period);
void WaitForInterrupt(void);
void EnableInterrupts(void);
void DisableInterrupts(void);
void playXO();
void LedsEnDis();
void Delay100ms(unsigned long count);
extern int ledFlag,emptyFeild;
void ADC1Seq3_Handler(void);
void 	ADC_Init(void);
#endif
