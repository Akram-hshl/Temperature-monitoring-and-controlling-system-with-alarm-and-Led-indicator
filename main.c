#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define LCD PORTB		// LCD data port connected to the PORTB
#define EN 7			// En pin atmega ar jekono port ar 7 number a connect korte hobe but nice main program a seta define korte hobe
#define  RW 6			// read write pin 6 or grownd a set korete paro
#define RS 5
unsigned char data;


void lcdcmd (unsigned char cmd)
{
	PORTD &=~(1<<RS);		//RS=0 for command (it means 1 shifted 5 time left and 5th position resister value will be 1 and negation of 1=0 so it ready for command and will be change 1 after command
	PORTD &=~(1<<RW);		//RW=0 for write
	LCD = cmd & 0xF0	;		// send upper nibble (amra 4bit mode a lcd set korci tai 8 bit 2bage bag howe first 4bit upper nibble a and second 4bit lowe nibble a jabe
	PORTD |=(1<<EN);		// EN =1 for Higher to Lower shift next 4bit
	_delay_ms(1);
	PORTD &=~(1<<EN);		// EN=0 for H to L pulse
	
	LCD = cmd<<4;			// send lower nibble
	PORTD |=(1<<EN);		// EN =1 for Higher to Lower shift next 4bit
	_delay_ms(1);
	PORTD &=~(1<<EN);		// EN=0 for H to L pulse
}


void lcddata(unsigned char data)
{
	PORTD |= (1<<RS);	// RS = 1 for data (ar purbe command a RS = 0 cilo but amra data sent korteci akhon
	PORTD &=~(1<<RW);
	LCD = data & 0xF0 ;	// send upper nibble (amra 4bit mode a lcd set korci tai 8 bit 2bage bag howe first 4bit upper nibble a and second 4bit lowe nibble a jabe
	PORTD |=(1<<EN);		// EN =1 for Higher to Lower shift next 4bit
	_delay_ms(1);
	PORTD &=~(1<<EN);		// EN=0 for H to L pulse
	
	LCD = data<<4;			// send lower nibble
	PORTD |=(1<<EN);		// EN =1 for Higher to Lower shift next 4bit
	_delay_ms(1);
	PORTD &=~(1<<EN);		// EN=0 for H to L pulse
}

void lcd_int()
{
	DDRB = 0XFF;		//define output LCD port  (jehetu LCD ar data pin Port F te set korbo tai port F output
	DDRD = 0xFF;		// define RS, EN and RW pin as output agula control pin(jehetu amra RS, RW, EN pin port D te set korbo tai port D output
	PORTD &=~(1<<EN);		// initialize EN = 0 because ata o hole amra data sent kore one korte parbo
	lcdcmd(0x33);
	lcdcmd(0x32);
	lcdcmd(0x28);			// LCD in 4 bit mode amra ei 3ta command diye lcd k 4bit mode set korbo
	lcdcmd(0x0E);			// display cursor on korar jonno ei command
	lcdcmd(0x01);			// clear LCD korte ei command
	_delay_ms(2);
}

void lcd_print(char *str)
{
	unsigned char i = 0;
	while(str[i]!=0)			//i ar value print koro jothokhon porjonto na i = 0 hoi.   ses a null value 0 = 0 hoi r ei loop break hoi
	{
		lcddata(str[i]);
		i++;
		_delay_ms(25);		// letter gulu koto sec por por display korbe ata sei delay
	}
}


int main(void)
{
	/* Replace with your application code */
	lcd_int();				// lcd initialize korbe
	lcd_print("Temperature:" );		// lcd_print k call korbe r protivar i = T thake suru kore ses a 1ta null data niye break korbe
	
	DDRA &=~(1<<0);				// make PA0 an input for ADC, ADC voltage k raferance dore voltage thake temperature count kore
	ADCSRA = 0x87;				// make ADC enable and ck/128
	ADMUX = 0xE0;				// 2.56 V voltage referance and ABC0 signal ended data will be left justified
	
	while (1)
	{
		ADCSRA |=(1<<ADSC);
		while((ADCSRA&(1<<ADIF))==0);
		data= ADCH;
		convert_Display(data);
		_delay_ms(100);		// koto sec por por left to right a jave
	}
	

}

void convert_Display(unsigned char value)
{
	unsigned char x, d1, d2;		//3ta unsigned char variable nilam calculation ar subadar jonno
	x=value/10;					//value sorasori show hobe na tai ata k 2babe bag kore nibe value=32 hole x=32/10 = 3
	d1=x;						// d1=x ar value mani 3
	d2=value%10;				// d2=32%10 = 2 ata ditiyo bag		orthat value 32 ar prothom bag 3 porer bag 2 bag kora holo 
	
	lcdcmd(0x8D);			// move the cursor to the 13th position because we already use first 12 position for writing "Temperature:"
	lcddata(d1+0x30);		// move 0x33 ASCII to LCD that will display 3 on LCD orthat decimal number a show korte valu ar prothom bag ar sate ox30 ar or operation kora lagbe
	lcddata(d2+0x30);		//move 0x32 ASCII to LCD that will display 2 on LCD 
	
}