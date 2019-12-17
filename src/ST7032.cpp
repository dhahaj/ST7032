#include "Arduino.h"
#include <SPI.h>

#include "ST7032.h"


ST7032::ST7032(uint8_t rst, uint8_t rs, uint8_t cs, uint8_t sclk, uint8_t sda) : _rst(rst),_rs(rs),_cs(cs),_sclk(sclk),_sda(sda)
{
	// init(rst, rs, cs, sclk, sda);
	// this->begin();
	 _spi_settings = SPISettings(5000000, MSBFIRST, SPI_MODE0);
} 

/*
void ST7032::init(uint8_t rst, uint8_t rs, uint8_t cs, uint8_t sclk, uint8_t sda)
{
	_rst=rst;
	_rs=rs;
	_cs=cs;
	_sclk=sclk;
	_sda=sda;
}*/

void ST7032::begin() 
{
	SPI.begin();
	pinMode(_rst, OUTPUT);
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	
	digitalWrite(_cs, HIGH);
	digitalWrite(_rs, HIGH);
	
	digitalWrite(_rst, HIGH);
	delayMicroseconds(200);
	digitalWrite(_rst, LOW);
	delayMicroseconds(200);
	digitalWrite(_rst, HIGH);
	
	delayMicroseconds(4000);
	// Write_Instruction(0x38); // FUNCTION SET 8 bit, N=1 2-line display mode, 5*7dot
	send(0x38, COMMAND);
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Instruction(0x39); // FUNCTION SET 8 bit, N=1 2-line display mode, 5*7dot IS=1
	send(0x39, COMMAND); // FUNCTION SET 8 bit, N=1 2-line display mode, 5*7dot IS=1
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Instruction(0x1C); // Internal OSC frequency adjustment 183HZ bias will be 1/4
	send(0x1C, COMMAND); // Internal OSC frequency adjustment 183HZ bias will be 1/4
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Instruction(0x73); // Contrast control low byte
	send(0x73, COMMAND); // Contrast control low byte
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Instruction(0x57); // booster circuit is turn on. /ICON display off. /Contrast control high byte
	send(0x57, COMMAND); // booster circuit is turn on. /ICON display off. /Contrast control high byte
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Instruction(0x6c); // Follower control
	send(0x6c, COMMAND); // Follower control
	delayMicroseconds(5000);
	// Write_Instruction(0x0f); // DISPLAY ON
	send(0x0f, COMMAND); // DISPLAY ON
	delayMicroseconds(WRITE_DELAY_MS);
	// this->clear();
	delayMicroseconds(WRITE_DELAY_MS);	
	// Write_Instruction(0x06); // ENTRY MODE SET  CURSOR MOVES TO RIGHT
	send(0x06, COMMAND); // ENTRY MODE SET  CURSOR MOVES TO RIGHT
	delayMicroseconds(WRITE_DELAY_MS);	

	 send(0x01, COMMAND);   // clear
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
	// this->Write_Instruction(RETURN_HOME);
	this->send(RETURN_HOME, COMMAND);
	delayMicroseconds(HOME_CLEAR_DELAY_MS);
}

void ST7032::setCursor(uint8_t line, uint8_t pos) 
{
	uint8_t p;
	if(pos > 15) pos = 0;
	if(line == 0) p = LINE_1_ADR + pos;
	else p = LINE_2_ADR + pos;
	// this->Write_Instruction(SET_DDRAM_ADDRESS | p);
	send((SET_DDRAM_ADDRESS | p), COMMAND);
	// delayMicroseconds(WRITE_DELAY_MS);
}

void ST7032::clear()
{
	// this->Write_Instruction(CLEAR_DISPLAY);
	send(CLEAR_DISPLAY, COMMAND);
	delayMicroseconds(HOME_CLEAR_DELAY_MS);
}

void ST7032::display()
{
	displayOnOffSetting |= DISPLAY_ON_OFF_D;
	send(displayOnOffSetting,COMMAND);
}

void ST7032::noDisplay()
{
	displayOnOffSetting &= ~DISPLAY_ON_OFF_D;
	send(displayOnOffSetting,COMMAND);
}

void ST7032::cursor() //display underline cursor
{ 
	displayOnOffSetting |= DISPLAY_ON_OFF_C;
	// this->Write_Instruction(displayOnOffSetting);
	send(displayOnOffSetting, COMMAND);
}

void ST7032::noCursor() //stop display underline cursor
{ 
	displayOnOffSetting &= ~DISPLAY_ON_OFF_C;
	// this->Write_Instruction(displayOnOffSetting);
	this->send(displayOnOffSetting, COMMAND);
}

void ST7032::blink() //cursor block blink
{ 
	displayOnOffSetting |= DISPLAY_ON_OFF_B;
	// this->Write_Instruction(displayOnOffSetting);
	send(displayOnOffSetting, COMMAND);
}

void ST7032::noBlink() //stop cursor block blink
{ 
	displayOnOffSetting &= ~DISPLAY_ON_OFF_B;
	// this->Write_Instruction(displayOnOffSetting);
	send(displayOnOffSetting, COMMAND);
}

void ST7032::setcontrast(int val) 
{
	if (val > CONTRAST_MAX) val = CONTRAST_MIN;
	else if (val < CONTRAST_MIN) val = CONTRAST_MAX;
	// Write_Instruction(CONTRAST_SET | (val & B00001111));
	send(CONTRAST_SET | (val & B00001111), COMMAND);
	// Write_Instruction((val >> 4) | POWER_ICON_BOST_CONTR | POWER_ICON_BOST_CONTR_Bon);
	send((val >> 4) | POWER_ICON_BOST_CONTR | POWER_ICON_BOST_CONTR_Bon, COMMAND);
	contrast = val;
}

void ST7032::adjcontrast(int val) 
{
	setcontrast(val + contrast);
}

uint8_t ST7032::getcontrast() 
{
	return contrast;
}

void ST7032::printString(char chr[])
{
  //Write_Instruction(LINE_1_ADR);  // 1 LINE ADDRESS
  // delayMicroseconds(WRITE_DELAY_MS);
  // this->clear();
  // this->home();
  for (int m = 0; m < 16; m++)
    // Write_Data(chr[m]);
    send(chr[m], DATA);

  // Write_Instruction(LINE_2_ADR);  // 2 LINE ADDRESS
  //send(LINE_2_ADR, COMMAND);  // 2 LINE ADDRESS
 // delayMicroseconds(WRITE_DELAY_MS);
  for (int m = 16; m < 32; m++){
    // Write_Data(chr[m]);
    send(chr[m], DATA);
	delayMicroseconds(WRITE_DELAY_MS);
  }
}

void ST7032::printString(const char* chr)
{
	for (int m = 0; m < 16; m++)
    // Write_Data(chr[m]);
    	send(chr[m], DATA);
  send(LINE_2_ADR, COMMAND);  // 2 LINE ADDRESS
  delayMicroseconds(WRITE_DELAY_MS);
	for (int m = 16; m < 32; m++){
       // Write_Data(chr[m]);
       send(chr[m], DATA);
	   delayMicroseconds(WRITE_DELAY_MS);
  }
}

void ST7032::show(unsigned char dat, unsigned char add, unsigned char nline)
{
	// Write_Instruction(0xC0);  //2 LINE ADDRESS
	// delayMicroseconds(30);
	if (nline == 1)
		add = LINE_1_ADR + add;
	if (nline == 2)
		add = LINE_2_ADR + add;
	// Write_Instruction(add);
	send(add, COMMAND);
	delayMicroseconds(WRITE_DELAY_MS);
	// Write_Data(dat);
	send(dat, DATA);
}

void ST7032::Write_Instruction(uint8_t com)
{
	uint8_t j;
	digitalWrite(_cs, LOW);
	digitalWrite(_rs, LOW);
	for (j = 0; j < 8; j++) {
		// if (com & 0x80)
		// digitalWrite(_sda, HIGH); //SDA = 1;
		// else
		// digitalWrite(_sda, LOW); //SDA = 0;
		digitalWrite(_sda, (com & 0x80)); //SDA = 0;
		digitalWrite(_sclk, LOW); //SCLK = 0;
		com = com << 1;
		digitalWrite(_sclk, HIGH);
	}
	digitalWrite(_cs, HIGH);
}

void ST7032::Write_Data(uint8_t dat)
{
	unsigned int j;
	digitalWrite(_cs, LOW);
	digitalWrite(_rs, HIGH);
	for (j = 0; j < 8; j++) {
		// if (dat & 0x80)
		// digitalWrite(_sda, HIGH); //SDA = 1;
		// else
		// digitalWrite(_sda, LOW); //SDA = 0;
		digitalWrite(_sda, (dat & 0x80));
		digitalWrite(_sclk, LOW); //SCLK = 0;
		dat = dat << 1;
		digitalWrite(_sclk, HIGH);
	}
	digitalWrite(_cs, HIGH);
}

void ST7032::send(uint8_t value, uint8_t mode){
  // send mode => RS pin
  // LOW: command; HIGH: data
  bool send_mode = ( mode == DATA );

  noInterrupts();
  SPI.beginTransaction(_spi_settings);
  digitalWrite(_cs, LOW);
  digitalWrite(_rs, send_mode);
  delayMicroseconds(150); // do we need this?
  SPI.transfer(value);
  delayMicroseconds(150); // do we need this?
  SPI.endTransaction();
  digitalWrite(_cs, HIGH);
  interrupts();
}
