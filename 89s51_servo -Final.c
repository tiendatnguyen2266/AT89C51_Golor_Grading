#include<REGX51.h>

unsigned char code Code7Seg[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
unsigned char low, high;
unsigned int F_do, F_xanhlam, F_xanhluc;
unsigned int number, count=0, t = 0;

// Thiet lap chan dieu khien dong co DC
sbit	EnA = P2^3; 
sbit	In1 = P2^4; 
sbit 	In2 = P2^5;	

// Thiet lap chan dieu khien servo
sbit In3 = P2^6; 
sbit In4 = P2^7; 
sbit control_pin = P1^7;

// Thiet lap chan dieu khien TCS3200
sbit S0 = P1^2;  
sbit S1 = P1^3;
sbit S2 = P1^0;
sbit S3 = P1^1;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Thiet lap ham delay hien thi
void Delay(int time) {
	while(time--);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Thiet lap ham dieu khien servo
void Delay_servo(unsigned int ds) {
	TMOD = 0x15;
  TH1=0xFF - (ds>>8) & 0xFF;
  TL1=0xFF- ds & 0xFF;
  ET1=1;
  EA=0;
  TR1=1;
  while(TF1==0);
  TR1=0;
  TF1=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Thiet lap ham hien thi bang Led 7 doan
void Display(unsigned int n) {
	unsigned char tram, chuc, dvi;
	tram = n/100;
	n %= 100;
	chuc = n/10;	
  dvi = n%10;

	P0 = Code7Seg[tram];
	P2_2 = 0;
	Delay(1);
	P2_2 = 1;

	P0 = Code7Seg[chuc];
	P2_1 = 0;
	Delay(1);
	P2_1 = 1;

	P0 = Code7Seg[dvi];
	P2_0 = 0;
	Delay(1);
	P2_0 = 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void main() {
	//Thiet lap cac chan dieu khien dong co DC
	EnA = 1; 
	In1 = 0; 
	In2 = 1;	
	// Thiet lap chan dieu khien servo
	control_pin=0;
	In3 = 0; 
	In4 = 1; 
	//Thiet lap tan so TCS3200 20%
	S0 = 1;  
	S1 = 0;

	TMOD = 0x15;
	ET1 = 1;
	TR1 = 1;
	EA = 1;
	TR0 = 1;	
	
	while(1) {
		switch(count) {
			case 0: // Bat photodiode Red
				S2 = 0;
				S3 = 0;
			break;
			case 1: // Bat photodiode Blue
				S2 = 0;
				S3 = 1;
			break;
			case 2: // Bat photodiode Green
				S2 = 1;
				S3 = 1;
			break;
		}
		
		do {
			high = TH0;
			low = TL0;
		}	while(high!=TH0);

		number = high;
		number <<= 8;
		number = low;
		Display(number);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ngat() interrupt 3{
	long a = 50000;
	t++;
	TH1 = 0xfc;
	TL1 = 0x18;
	TR1 = 1;
	
	if(t>=2000){
		if (S2 == 0 && S3 == 0) {    
			F_do=number;
		}
		if (S2 == 0 && S3 == 1) {    
			F_xanhlam=number;
		}
		if (S2 == 1 && S3 == 1) {    
			F_xanhluc=number;
		}
		t = 0;
		if(count<2){
			count++;
		}
		else {
			if ( ((F_do < 35) && (F_do > 25)) && ((F_xanhlam < 25) && (F_xanhlam > 15)) && ((F_xanhluc < 15) && (F_xanhluc > 5)) )	{
			control_pin=1;
			Delay_servo(1000);
			control_pin=0;
			}
			if ( ((F_do < 15) && (F_do > 5)) && ((F_xanhlam < 35) && (F_xanhlam > 25)) && ((F_xanhluc < 25) && (F_xanhluc > 15)) )	{
			control_pin=1;
			Delay_servo(1500);
			control_pin=0;
			}
			if ( ((F_do < 25) && (F_do > 15)) && ((F_xanhlam < 15) && (F_xanhlam > 5)) && ((F_xanhluc < 35) && (F_xanhluc > 25)) )	{
			control_pin=1;
			Delay_servo(2000);
			control_pin=0;
			}
			count=0;
			TH1 = 0xfc;
			TL1 = 0x18;
			ET1 = 1;
			TR1 = 1;
			EA = 1;
			//Tra ve trang thai clear
			S2 = 1;
			S3 = 0;
			Delay(1);
		}		

		while(a--){};
		TH0 = 0;
		TL0 = 0;
	}
}
