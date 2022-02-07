// Lab10.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/16/2021 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* 
 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// VCC   3.3V power to OLED
// GND   ground
// SCL   PD0 I2C clock (add 1.5k resistor from SCL to 3.3V)
// SDA   PD1 I2C data

//************WARNING***********
// The LaunchPad has PB7 connected to PD1, PB6 connected to PD0
// Option 1) do not use PB7 and PB6
// Option 2) remove 0-ohm resistors R9 R10 on LaunchPad
//******************************

#include <stdint.h>
#include <stdlib.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include "SSD1306.h"
#include "Print.h"
#include "Random.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "TExaS.h"
#include "Switch.h"
//********************************************************************************
// debuging profile, pick up to 7 unused bits and send to Logic Analyzer
#define PB54                  (*((volatile uint32_t *)0x400050C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// use for debugging profile
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define PB5       (*((volatile uint32_t *)0x40005080)) 
#define PB4       (*((volatile uint32_t *)0x40005040)) 
// TExaSdisplay logic analyzer shows 7 bits 0,PB5,PB4,PF3,PF2,PF1,0 
// edit this to output which pins you use for profiling
// you can output up to 7 pins
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PB54; // sends at 10kHz
}
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// edit this to initialize which pins you use for profiling
void Profile_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x22;      // activate port B,F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 
  GPIO_PORTF_DEN_R |=  0x0E;   // enable digital I/O on PF3,2,1
  GPIO_PORTB_DIR_R |=  0x30;   // output on PB4 PB5
  GPIO_PORTB_DEN_R |=  0x30;   // enable on PB4 PB5  
}
//********************************************************************************
 typedef enum {dead,alive} status_t;
struct sprite {
	int32_t x; //x coordinate
	int32_t y; //y coordinate
	int32_t vx;
	int32_t vy;
	const uint8_t *image;
	status_t life;
};
typedef struct sprite sprite_t;
sprite_t Enemies[6];
sprite_t Player;
sprite_t Missles[20];
int NeedtoDraw; //1 means to draw 
void Init(void) {
	//int i;
	Player.x=50;
	Player.y=60;
	Player.image =PlayerShip0 ;
	Player.life =alive;
	//NeedtoDraw = 1;
	for(int i=0;i<6;i++) {
		Enemies[i].x=20*i;
		Enemies[i].y=10;
		if(i<3) {
			Enemies[i].vx=0; 
			Enemies[i].vy=1;
			Enemies[i].image=Alien20pointA;
		}
		if(i>=3) {
			Enemies[i].vx=0;
			Enemies[i].vy=1;
			Enemies[i].image=Alien30pointA;
		}
			Enemies[i].life=alive;
	}
	//for(i=0;i<18;i++) {
		//Enemies[i].life=dead;
	//}
	for(int i = 0;i<20;i++){
		Missles[i].life = dead;
	}
}

int count =0;
void Move(void) {
	//int i;
	count ++;
	if(Player.life==alive) {
		NeedtoDraw =1;
	uint32_t adcData=ADC_In();
		Player.x=((127-16)*adcData)/4096;
	}
	for(int i=0;i<18;i++) {
		if(Missles[i].life==alive) {
			NeedtoDraw =1;
			if(Missles[i].x <0) {
				Missles[i].x =2;
				Missles[i].vx =-Missles[i].vx;
			}
			if(Missles[i].x >126) {
				Missles[i].x =124;
				Missles[i].vx =-Missles[i].vx;
			}
			if(Missles[i].y<62&&Missles[i].y>0) {
				Missles[i].x+=	Missles[i].vx;	
				Missles[i].y+=	Missles[i].vy; }
			else {
				Missles[i].life=dead;
			}
		}		
	}
if(count == 10){
	for(int i=0;i<6;i++) {
		if(Enemies[i].life==alive) {
			NeedtoDraw =1;
			if(Enemies[i].x <0) {
				Enemies[i].x =2;
				Enemies[i].vx =-Enemies[i].vx;
			}
			if(Enemies[i].x >126) {
				Enemies[i].x =124;
				Enemies[i].vx =-Enemies[i].vx;
			}
			if(Enemies[i].y<62&&Enemies[i].y>0) {
				Enemies[i].x+=	Enemies[i].vx;
				Enemies[i].y+=	Enemies[i].vy; }
			else {
				Enemies[i].life=dead;
			}
		}		
	}
	count = 0;
}
	
}



void Draw(void) { 
	//int i;
	SSD1306_ClearBuffer ();
	if(Player.life==alive) {
	SSD1306_DrawBMP(Player.x,Player.y,
			Player.image, 0, SSD1306_INVERSE);	
	}
	for(int i=0;i<6;i++) {
		if(Enemies[i].life!=dead) {
		SSD1306_DrawBMP(Enemies[i].x,Enemies[i].y,
			Enemies[i].image, 0, SSD1306_INVERSE);
		}
	}
	for(int i=0;i<20;i++) {
		if(Missles[i].life!=dead) {
		SSD1306_DrawBMP(Missles[i].x,Missles[i].y,
			Missles[i].image, 0, SSD1306_INVERSE);
		}
	}
	SSD1306_OutBuffer ();
	NeedtoDraw =0;
}

void Fire(int vx1, int vy1){
	int i =0;
	while(Missles[i].life == alive){
		i++;
		if(i==20){
			return;
		}
	}
	Missles[i].x = Player.x+7;
	Missles[i].y = Player.y-4;
	Missles[i].vx = vx1;
	Missles[i].vy = vy1;
	Missles[i].image = Laser0;
	Missles[i].life = alive;

	
}
int check = 0;
int score = 0;
int win = 0;
int GamePaused = 0;
int language = 0; 
int langset = 0;
void Collisions(void) {
	//int i;
	int32_t x1,y1,x2,y2;
	for(int i = 0;i<6;i++){
		x2= Enemies[i].x +6;
		y2=Enemies[i].y -4;
		for( int j=0; j<20; j++) {
			if(Missles[j].life == alive) {
				x1 = Missles[j].x;
				y1 = Missles[j].y;
				if((abs(x1-x2)<7)&&(abs(y1-y2)<2)) {
						Enemies[i].life=dead; 
						Missles[j].life = dead;
						//check++;
						return;
					}
				
			}		
		}
	}

}

void Outcome(void){
	//int i = 0;
	if((Enemies[0].life==dead)&&(Enemies[1].life==dead)&&(Enemies[2].life==dead)&&(Enemies[3].life==dead)&&(Enemies[4].life==dead)&&(Enemies[5].life==dead)){
		check = 6;
		win=1; 
		score = 100; 
	
	}
	for(int i = 0;i<6;i++){
		if(Enemies[i].y==50){
			check =6;
			win = 0;
		}
	}
}

//int scoreDisplay[2];

//void Caclulate(void){
	//scoreDisplay[0] = score%10;
	//scoreDisplay[1] = score/10;
//}
			
void SysTick_Init(void);
	
void Delay100ms(uint32_t count){uint32_t volatile time;
 
	while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
	Move();
}


int main(void){
	//int i;
  DisableInterrupts();
  // pick one of the following three lines, all three set to 80 MHz
 // PLL_Init();                   // 1) call to have no TExaS debugging
  TExaS_Init(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_Init(&ScopeTask);       // or 3) call to activate analog scope PD2
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear();   
  Random_Init(1);
  Profile_Init(); // PB5,PB4,PF3,PF2,PF1 
  SSD1306_ClearBuffer();
  SSD1306_DrawBMP(2, 62, SpaceInvadersMarquee, 0, SSD1306_WHITE);
  SSD1306_OutBuffer();
	Delay100ms(20);
	SSD1306_OutClear(); 
	int counter;
	int timer;
	Switch_Init();
	EnableInterrupts();
	while (counter <1000){
		//int data = GPIO_PORTF_DATA_R;
		//if((GPIO_PORTF_DATA_R&0x10) == 0x10){
		//	language = 1;
			//SSD1306_OutString("in if");
			//SSD1306_SetCursor(1, 2);
			
		//}
		SSD1306_SetCursor(1, 1);
		SSD1306_OutString("Welcome(bienvenidas)");
		SSD1306_SetCursor(1, 2);
		SSD1306_OutString("Pulsa SW1:Espanol");
		counter++;
		if(counter%100==0){
			if(language == 0){
			SSD1306_SetCursor(1, 4);
				SSD1306_OutString("Game starts in 10s:");
			SSD1306_SetCursor(1, 5);
			timer = counter/100;
			LCD_OutDec(timer);
			}
		
		if(language == 1){
			SSD1306_SetCursor(1, 4);
			SSD1306_OutString("El juego empieza en 10 segundos:");
			SSD1306_SetCursor(1, 9);
			timer = counter/100;
			LCD_OutDec(timer);
			
			}
		}
		
	}
	langset=1;
	
	//Delay100ms(20);
	DisableInterrupts();
	SysTick_Init();
	ADC_Init();
	Init();
	Switch_Init();
	Switch_InitPause();
	EnableInterrupts();
	//Move();
 // SSD1306_ClearBuffer();
  //SSD1306_DrawBMP(47, 63, PlayerShip0, 0, SSD1306_WHITE); // player ship bottom
  //SSD1306_DrawBMP(53, 55, Bunker0, 0, SSD1306_WHITE);

 //SSD1306_DrawBMP(0, 9, Alien10pointA, 0, SSD1306_WHITE);
  //SSD1306_DrawBMP(20,9, Alien10pointB, 0, SSD1306_WHITE);
  //SSD1306_DrawBMP(40, 9, Alien20pointA, 0, SSD1306_WHITE);
  //SSD1306_DrawBMP(60, 9, Alien20pointB, 0, SSD1306_WHITE);
  //SSD1306_DrawBMP(80, 9, Alien30pointA, 0, SSD1306_WHITE);
  //SSD1306_DrawBMP(50, 19, AlienBossA, 0, SSD1306_WHITE);
  //SSD1306_OutBuffer();
  //Delay100ms(30);

  //SSD1306_OutClear();  
  //SSD1306_SetCursor(1, 1);
  //SSD1306_OutString("GAME OVER");
  //SSD1306_SetCursor(1, 2);
  //SSD1306_OutString("Nice try,");
  //SSD1306_SetCursor(1, 3);
  //SSD1306_OutString("Earthling!");
  //SSD1306_SetCursor(2, 4);
	while(check<6){
  while((GamePaused==0)&&(check<6)){
	//	uint32_t time;
		if(NeedtoDraw==1) 
       {Draw();}
	// for(int i=0;i<6;i++){
		//if(Enemies[i].life==dead){
			//check++;
			//if(i<3){
				//score = score + 10;
			//}
			//else{
				//score = score + 20;
			//}
		//}
	//}

    //Delay100ms(10);
    //SSD1306_SetCursor(19,0);
    //SSD1306_OutUDec2(time);
    //time++;
   // PF1 ^= 0x02;
  }
	SSD1306_OutClear();
	while(GamePaused==1){
		if(language == 0){
			SSD1306_SetCursor(1, 1);
			SSD1306_OutString("Paused");
			SSD1306_SetCursor(1, 2);
			SSD1306_OutString("Press SW1 to resume");
		}
		else{
			SSD1306_SetCursor(1, 1);
			SSD1306_OutString("Se Diente El Juego");
		}
	}
}
	
	DisableInterrupts();
	SSD1306_OutClear();
	if(language ==0){
		if(win == 1){
		SSD1306_SetCursor(1, 1);
		SSD1306_OutString("You win!");
		SSD1306_SetCursor(1, 2);
		SSD1306_OutString("Score:");
		SSD1306_SetCursor(8,2);
		LCD_OutDec(score);
		}
		else{
			for(int i = 0;i<6;i++){
				if(Enemies[i].life==dead){
					if(i<3){
					score = score + 10;
					}
					else{
						score = score + 20;
					}
				}
			}
			SSD1306_SetCursor(1, 1);
			SSD1306_OutString("Game Over! You lost");
			SSD1306_SetCursor(1, 2);
			SSD1306_OutString("Score:");
			SSD1306_SetCursor(8,2);
			LCD_OutDec(score);
		}
	}
	
	else{
		if(win == 1){
		SSD1306_SetCursor(1, 1);
		SSD1306_OutString("Tu Ganas!");
		SSD1306_SetCursor(1, 2);
		SSD1306_OutString("Puntas:");
		SSD1306_SetCursor(8,2);
		LCD_OutDec(score);
		}
		else{
			for(int i = 0;i<6;i++){
				if(Enemies[i].life==dead){
					score = score + 10;
				}
			}
			SSD1306_SetCursor(1, 1);
			SSD1306_OutString("Perdiste!");
			SSD1306_SetCursor(1, 2);
			SSD1306_OutString("Puntas:");
			SSD1306_SetCursor(8,2);
			LCD_OutDec(score);
		}
	}
	while(1){};

	
	
}

// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays


void SysTick_Init(void) { 

	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R &0x00FFFFFF) | 0x40000000;
	NVIC_ST_RELOAD_R =(4000000)-1;
	NVIC_ST_CTRL_R =7;
}

void SysTick_Handler(void) {
	Move();
	Collisions();
	Outcome();
	//check = 1;
}

void GPIOPortF_Handler(void){
	GPIO_PORTF_ICR_R = 0x10;
	if(langset==1){
	playsound(Shoot);
	Fire(0,-2);
	}
	if(langset==0){
		language = 1;
	}
}

void GPIOPortE_Handler(void){
	GPIO_PORTE_ICR_R = 0x01;
	GamePaused++;
	if (GamePaused == 2){
		GamePaused = 0;
	}
	//SSD1306_OutString("Paused");
	//Fire(0,-2);
}

