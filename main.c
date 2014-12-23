#include <REG_MPC82G516.h>

//Global variables and definition
#define CW P20
#define CCW P21
#define LED_PLUS P37
#define LED_MINUS P36
#define SEG_SELECT P1
#define SEG_VALUE P0
 
signed short speed;

void timer0_init();
void btn_init();

void main() {
	speed = 0;
	
	TMOD = 0;
	timer0_init();
	btn_init();
	
	CW = 1;
	CCW = 0;
	LED_PLUS = 0;
	LED_MINUS = 1;
	
	while(1) {
		// Display in 7-seg
		unsigned char spd = (unsigned char) (speed < 0 ? -speed : speed); 
		unsigned char sel;
		unsigned char delay;
		for(sel = 1 << 2; sel != 0; sel >>= 1) {
			SEG_SELECT = ~sel;
			SEG_VALUE = spd % 10;
			spd /= 10;
			
			for(delay=0; delay<250; delay++);
		}
	}
}

void timer0_up() interrupt 1 {
	static bit pwm_flag = 0;
	unsigned char pwm_width = (unsigned char) (speed < 0 ? -speed : speed) >> 1;
	
	if(!pwm_flag) {	//Start of High level
		pwm_flag = 1;	//Set flag
		TH0 = 255 - pwm_width;	//Load timer
		
		if(speed >= 0)
			CW = 1;	//Set PWM o/p pin
		else
			CCW = 1;
	}	else {	//Start of Low level
		pwm_flag = 0;	//Clear flag
		CW = 0;
		CCW = 0;	//Clear PWM o/p pin
		TH0 = pwm_width;	//Load timer
	}
}

void btnAdd() interrupt 0 {
	if(speed < 255)
		speed++;
	if(speed > 0) {
		LED_PLUS = 0;
		LED_MINUS = 1;
	}
}

void btnSub() interrupt 2 {
	if(speed > -255)
		speed--;
	if(speed < 0) {		
		LED_PLUS = 1;
		LED_MINUS = 0;
	}
}

void btn_init() {
	IE |= 0x85;
	IT0 = 1;
	IT1 = 1;
}

void timer0_init() {
	IE |= 0x82;
	TMOD &= 0xF0;
	TMOD |= 0x01;
	
	TH0 = (65536-50000) >> 8;
	TL0 = (65536-50000) & 0x00FF;
	TR0 = 1;
}
