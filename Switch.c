// Switch.c
// This software to input from switches or buttons
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/14/21
// Lab number: 10
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
int fallingedge;
int fallingedge2;
void Switch_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;
	fallingedge = 0;
	GPIO_PORTF_DIR_R &= ~0x10;
	GPIO_PORTF_DEN_R |= 0x10;
	GPIO_PORTF_PUR_R |= 0x10;
	GPIO_PORTF_IS_R &= ~0x10;
	GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;
}

void Switch_InitLang(void){
	SYSCTL_RCGCGPIO_R |= 0x20;
		__asm__{
	NOP
	NOP }
	GPIO_PORTF_DIR_R &= ~0x10;
	GPIO_PORTF_PDR_R |= 0x10;
	GPIO_PORTF_DEN_R |= 0x10;
	
	
}

void Switch_InitPause(void){
	SYSCTL_RCGCGPIO_R |= 0x10;
	fallingedge2 = 0;
	GPIO_PORTE_DIR_R &= ~0x01;
	GPIO_PORTE_DEN_R |= 0x01;
	//GPIO_PORTE_PUR_R |= 0x10;
	GPIO_PORTE_IS_R &= ~0x01;
	GPIO_PORTE_IBE_R &= ~0x01;    //     PE0 is not both edges
  GPIO_PORTE_IEV_R &= ~0x01;    //     PE0 falling edge event
  GPIO_PORTE_ICR_R = 0x01;      // (e) clear flag4
  GPIO_PORTE_IM_R |= 0x01;      // (f) arm interrupt on PF4
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF0F)|0x000000C0; // (g) priority 6
  NVIC_EN0_R = 0x00000010;
}
	
