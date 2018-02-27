#include "ST7032.h"

#include "Arduino.h"

ST7032::ST7032(uint8_t rst, uint8_t rs, uint8_t cs, uint8_t scl, uint8_t sda) 
{
	init(rst, rs, cs, scl, sda);
} 

void ST7032::init(uint8_t rst, uint8_t rs, uint8_t cs, uint8_t scl, uint8_t sda)
{
	_rst=rst;
	_rs=rs;
	_cs=cs;
	_scl=scl;
	_sda=sda;
}

void ST7032::begin() 
{
	pinMode(_rst, OUTPUT);
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	pinMode(_scl, OUTPUT);
	pinMode(_sda, OUTPUT);
	
	digitalWrite(_rst, HIGH);
	delayMicroseconds(200);
	digitalWrite(_rst, LOW);
	delayMicroseconds(200);
	digitalWrite(_rst, HIGH);
	delayMicroseconds(4000);
	Write_Instruction(0x38); // FUNCTION SET 8 bit, N=1 2-line display mode, 5*7dot
	delayMicroseconds(WRITE_DELAY_MS);
	Write_Instruction(0x39); // FUNCTION SET 8 bit, N=1 2-line display mode, 5*7dot IS=1
	delayMicroseconds(WRITE_DELAY_MS);
	Write_Instruction(0x1C); // Internal OSC frequency adjustment 183HZ bias will be 1/4
	delayMicroseconds(WRITE_DELAY_MS);
	Write_Instruction(0x73); // Contrast control low byte
	delayMicroseconds(WRITE_DELAY_MS);
	Write_Instruction(0x57); // booster circuit is turn on. /ICON display off. /Contrast control high byte
	delayMicroseconds(WRITE_DELAY_MS);
	Write_Instruction(0x6c); // Follower control
	delayMicroseconds(5000);
	Write_Instruction(0x0f); // DISPLAY ON
	delayMicroseconds(WRITE_DELAY_MS);
	this->clear();
	Write_Instruction(0x06); // ENTRY MODE SET  CURSOR MOVES TO RIGHT
	delayMicroseconds(WRITE_DELAY_MS);	
}

size_t ST7032::write(uint8_t chr)
{
	// uint8_t addr;
	// if(_numlines==1)
		// addr = chr+0x80;
	// else
		// addr = chr+0x80;
	// Write_Instruction(addr);
	// delayMicroseconds(30);
	this->Write_Data(chr);
	return 1;
}

void ST7032::home()
{
	this->Write_Instruction(RETURN_HOME);
	delayMicroseconds(HOME_CLEAR_DELAY_MS);
}

void ST7032::setCursor(uint8_t line, uint8_t pos) 
{
	uint8_t p;
	if(pos > 15) pos = 0;
	if(line == 0) p = LINE_1_ADR + pos;
	else p = LINE_2_ADR + pos;
	this->Write_Instruction(SET_DDRAM_ADDRESS | p);
}

void ST7032::clear()
{
	this->Write_Instruction(CLEAR_DISPLAY);
	delayMicroseconds(HOME_CLEAR_DELAY_MS);
}

void ST7032::display()
{
	displayOnOffSetting |= DISPLAY_ON_OFF_D;
	Write_Instruction(displayOnOffSetting);
}

void ST7032::noDisplay()
{
	displayOnOffSetting &= ~DISPLAY_ON_OFF_D;
	Write_Instruction(displayOnOffSetting);
}

void ST7032::Write_Instruction(uint8_t com)
{
	unsigned int j;
	digitalWrite(_cs, LOW);
	digitalWrite(_rs, LOW);
	for (j = 0; j < 8; j++) {
		if (com & 0x80)
			digitalWrite(_sda, HIGH); //SDA = 1;
		else
			digitalWrite(_sda, LOW); //SDA = 0;
		digitalWrite(_scl, LOW); //SCLK = 0;
		com = com << 1;
		digitalWrite(_scl, HIGH);
	}
	digitalWrite(_cs, HIGH);
}

void ST7032::Write_Data(uint8_t dat)
{
	unsigned int j;
	digitalWrite(_cs, LOW);
	digitalWrite(_rs, LOW);
	for (j = 0; j < 8; j++) {
		if (dat & 0x80)
			digitalWrite(_sda, HIGH); //SDA = 1;
		else
			digitalWrite(_sda, LOW); //SDA = 0;
		digitalWrite(_scl, LOW); //SCLK = 0;
		dat = dat << 1;
		digitalWrite(_scl, HIGH);
	}
	digitalWrite(_cs, HIGH);
}

