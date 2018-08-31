/**************************************************************************/
/*! 
    @file     Adafruit_APDS9960.cpp
    @author   Ladyada, Dean Miller (Adafruit Industries)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2017, Adafruit Industries
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "Adafruit_APDS9960.h"

//#include "nordic_common.h"
//#include "bsp.h"

//#include "app_error.h"
#include "nrf_delay.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
//#include "nrf_log_default_backends.h"

#include "i2c.h"


uint32_t millis(void)
{
	NRF_LOG_INFO("millis:%x",app_timer_cnt_get() / 32.768);
	NRF_LOG_FLUSH();
	return(app_timer_cnt_get() / 32.768); //todo
	//return 0;
}

uint8_t get_enable(struct enable setting) {
	return (setting.GEN << 6) | (setting.PIEN << 5) | (setting.AIEN << 4) | (setting.WEN << 3) | (setting.PEN << 2) | (setting.AEN << 1) | setting.PON;
}

uint8_t get_pers(struct pers setting){
	return (setting.PPERS << 4) | setting.APERS;
}

uint8_t get_config1(struct config1 setting){
	return setting.WLONG << 1;
}
uint8_t get_ppulse(struct ppulse setting){
	return (setting.PPLEN << 6) | setting.PPULSE;
}
uint8_t get_control(struct control setting){
	return (setting.LDRIVE << 6) | (setting.PGAIN << 2) | setting.AGAIN;
}
uint8_t get_config2(struct config2 setting){
	return (setting.PSIEN << 7) | (setting.CPSIEN << 6) | (setting.LED_BOOST << 4) | 1;
}
void set_status(uint8_t data, struct status setting){
	setting.AVALID = data & 0x01;
	setting.PVALID = (data >> 1) & 0x01;
	setting.GINT = (data >> 2) & 0x01;
	setting.AINT = (data >> 4) & 0x01;
	setting.PINT = (data >> 5) & 0x01;
	setting.PGSAT = (data >> 6) & 0x01;
	setting.CPSAT = (data >> 7) & 0x01;
}
uint8_t get_config3(struct config3 setting){
	return (setting.PCMP << 5) | (setting.SAI << 4) | (setting.PMASK_U << 3) | (setting.PMASK_D << 2) | (setting.PMASK_L << 1) | setting.PMASK_R;
}
uint8_t get_gconf1(struct gconf1 setting){
	return (setting.GFIFOTH << 6) | (setting.GEXMSK << 2) | setting.GEXPERS;
}
uint8_t get_gconf2(struct gconf2 setting){
	return (setting.GGAIN << 5) | (setting.GLDRIVE << 3) | setting.GWTIME;
}
uint8_t get_gpulse(struct gpulse setting){
	return (setting.GPLEN << 6) | setting.GPULSE;
}
uint8_t get_gconf3(struct gconf3 setting){
	return setting.GDIMS;
}
uint8_t get_gconf4(struct gconf4 setting){
	return (setting.GIEN << 1) | setting.GMODE;
}
void set_gconf4(uint8_t data,struct gconf4 setting){
	setting.GIEN = (data >> 1) & 0x01;
	setting.GMODE = data & 0x01;
}
void set_gstatus(uint8_t data, struct gstatus setting){
	setting.GFOV = (data >> 1) & 0x01;
	setting.GVALID = data & 0x01;
}
/*========================================================================*/
/*                          PRIVATE FUNCTIONS                             */
/*========================================================================*/

/**************************************************************************/
/*!
    @brief  Implements missing powf function
*/
/**************************************************************************/
float powf(const float x, const float y)
{
  return (float)(pow((double)x, (double)y));
}

/**************************************************************************/
/*!
    Enables the device
    Disables the device (putting it in lower power sleep mode)
*/
/**************************************************************************/
//void Adafruit_APDS9960_enable(boolean en)
void Adafruit_APDS9960_enable(bool en)
{
  _enable.PON = en;
  //this->write8(APDS9960_ENABLE, _enable.get());
  Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

/*========================================================================*/
/*                           PUBLIC FUNCTIONS                             */
/*========================================================================*/

/**************************************************************************/
/*!
    Initializes I2C and configures the sensor (call this function before
    doing anything else)
*/
/**************************************************************************/
//boolean Adafruit_APDS9960_begin(uint16_t iTimeMS, apds9960AGain_t aGain, uint8_t addr)
bool Adafruit_APDS9960_begin(uint16_t iTimeMS, apds9960AGain_t aGain, uint8_t addr)
{
	Adafruit_APDS9960_i2c_init();
  _i2caddr = addr;
   
  /* Make sure we're actually connected */
  uint8_t x = Adafruit_APDS9960_read8(APDS9960_ID);
  if (x != 0xAB)
  {
    return false;
  }

  /* Set default integration time and gain */
  Adafruit_APDS9960_setADCIntegrationTime(iTimeMS);
  Adafruit_APDS9960_setADCGain(aGain);

  // disable everything to start
  Adafruit_APDS9960_enableGesture(false);
  Adafruit_APDS9960_enableProximity(false);
  Adafruit_APDS9960_enableColor(false);
  
  Adafruit_APDS9960_disableColorInterrupt();
  Adafruit_APDS9960_disableProximityInterrupt();
  Adafruit_APDS9960_clearInterrupt();

  /* Note: by default, the device is in power down mode on bootup */
  Adafruit_APDS9960_enable(false);
  //delay(10);
  nrf_delay_ms(10);
  Adafruit_APDS9960_enable(true);
  //delay(10);
  nrf_delay_ms(10);
  
  //default to all gesture dimensions
  Adafruit_APDS9960_setGestureDimensions(APDS9960_DIMENSIONS_ALL);
  Adafruit_APDS9960_setGestureFIFOThreshold(APDS9960_GFIFO_4);
  Adafruit_APDS9960_setGestureGain(APDS9960_GGAIN_4);
  Adafruit_APDS9960_setGestureProximityThreshold(50);
  Adafruit_APDS9960_resetCounts();
  
  _gpulse.GPLEN = APDS9960_GPULSE_32US;
  _gpulse.GPULSE = 9; //10 pulses
  //this->write8(APDS9960_GPULSE, _gpulse.get());
  Adafruit_APDS9960_write8(APDS9960_GPULSE, get_gpulse(_gpulse));
  return true;
}
  
/**************************************************************************/
/*!
    Sets the integration time for the ADC of the APDS9960, in millis
*/
/**************************************************************************/
void Adafruit_APDS9960_setADCIntegrationTime(uint16_t iTimeMS)
{
  float temp;

  // convert ms into 2.78ms increments
  temp = iTimeMS;
  temp /= 2.78;
  temp = 256 - temp;
  if (temp > 255) temp = 255;
  if (temp < 0)   temp = 0;
  
  /* Update the timing register */
  Adafruit_APDS9960_write8(APDS9960_ATIME, (uint8_t)temp);
}

float Adafruit_APDS9960_getADCIntegrationTime(void)
{
  float temp;

  temp = Adafruit_APDS9960_read8(APDS9960_ATIME);

  // convert to units of 2.78 ms
  temp = 256 - temp;
  temp *= 2.78;
  return temp;
}

/**************************************************************************/
/*!
    Adjusts the color/ALS gain on the APDS9960 (adjusts the sensitivity to light)
*/
/**************************************************************************/
void Adafruit_APDS9960_setADCGain(apds9960AGain_t aGain)
{
  _control.AGAIN = aGain;

  /* Update the timing register */
  //write8(APDS9960_CONTROL, _control.get());
  Adafruit_APDS9960_write8(APDS9960_CONTROL, get_control(_control));
}

apds9960AGain_t Adafruit_APDS9960_getADCGain(void)
{
  return (apds9960AGain_t) ( Adafruit_APDS9960_read8(APDS9960_CONTROL) & 0x03 );
}


/**************************************************************************/
/*!
    Adjusts the Proximity gain on the APDS9960
*/
/**************************************************************************/
void Adafruit_APDS9960_setProxGain(apds9960PGain_t pGain)
{
  _control.PGAIN = pGain;

  /* Update the timing register */
  //write8(APDS9960_CONTROL, _control.get());
  Adafruit_APDS9960_write8(APDS9960_CONTROL, get_control(_control));
}

apds9960PGain_t Adafruit_APDS9960_getProxGain(void)
{
  return (apds9960PGain_t) ( Adafruit_APDS9960_read8(APDS9960_CONTROL) & 0x0C );
}

void Adafruit_APDS9960_setProxPulse(apds9960PPulseLen_t pLen, uint8_t pulses) {
  if (pulses < 1)  pulses = 1;
  if (pulses > 64) pulses = 64;
  pulses--;
  
  _ppulse.PPLEN = pLen;
  _ppulse.PPULSE = pulses;
  
  //write8(APDS9960_PPULSE, _ppulse.get());
  Adafruit_APDS9960_write8(APDS9960_PPULSE, get_ppulse(_ppulse));
} 

/**************************************************************************/
/*!
    Enable proximity readings on APDS9960
*/
/**************************************************************************/
//void Adafruit_APDS9960_enableProximity(boolean en)
void Adafruit_APDS9960_enableProximity(bool en)
{
  _enable.PEN = en;

  //write8(APDS9960_ENABLE, _enable.get());
  Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

void Adafruit_APDS9960_enableProximityInterrupt() {
	_enable.PIEN = 1;
	//write8(APDS9960_ENABLE, _enable.get());
	Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
	Adafruit_APDS9960_clearInterrupt();
}

void Adafruit_APDS9960_disableProximityInterrupt() {
	_enable.PIEN = 0;
	//write8(APDS9960_ENABLE, _enable.get());
	Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

void Adafruit_APDS9960_setProximityInterruptThreshold(uint8_t low, uint8_t high, uint8_t persistance){
	Adafruit_APDS9960_write8(APDS9960_PILT, low);
	Adafruit_APDS9960_write8(APDS9960_PIHT, high);
  
	if (persistance > 7) persistance = 7;
	_pers.PPERS = persistance;
	//write8(APDS9960_PERS,_pers.get());
	Adafruit_APDS9960_write8(APDS9960_PERS, get_pers(_pers));
}

bool Adafruit_APDS9960_getProximityInterrupt()
{
	//_status.set(this->read8(APDS9960_STATUS));
	set_status(Adafruit_APDS9960_read8(APDS9960_STATUS),_status);
	return _status.PINT;
};

/**************************************************************************/
/*!
    Read proximity data
*/
/**************************************************************************/
uint8_t Adafruit_APDS9960_readProximity(void)
{
  return Adafruit_APDS9960_read8(APDS9960_PDATA);
}


bool Adafruit_APDS9960_gestureValid()
{
	//_gstatus.set(this->read8(APDS9960_GSTATUS));
	set_gstatus(Adafruit_APDS9960_read8(APDS9960_GSTATUS),_gstatus);
	return _gstatus.GVALID;
}

void Adafruit_APDS9960_setGestureDimensions(uint8_t dims)
{
	_gconf3.GDIMS = dims;
	//this->write8(APDS9960_GCONF3, _gconf3.get());
	Adafruit_APDS9960_write8(APDS9960_GCONF3, get_gconf3(_gconf3));
}

void Adafruit_APDS9960_setGestureFIFOThreshold(uint8_t thresh)
{
	_gconf1.GFIFOTH = thresh;
	//this->write8(APDS9960_GCONF1, _gconf1.get());
	Adafruit_APDS9960_write8(APDS9960_GCONF1, get_gconf1(_gconf1));
}

void Adafruit_APDS9960_setGestureGain(uint8_t gain)
{
	_gconf2.GGAIN = gain;
	//this->write8(APDS9960_GCONF2, _gconf2.get());
	Adafruit_APDS9960_write8(APDS9960_GCONF2, get_gconf2(_gconf2));
}

void Adafruit_APDS9960_setGestureProximityThreshold(uint8_t thresh)
{
	//this->write8(APDS9960_GPENTH, thresh);
	Adafruit_APDS9960_write8(APDS9960_GPENTH, thresh);
}

void Adafruit_APDS9960_setGestureOffset(uint8_t offset_up, uint8_t offset_down, uint8_t offset_left, uint8_t offset_right)
{
	//this->write8(APDS9960_GOFFSET_U, offset_up);
	//this->write8(APDS9960_GOFFSET_D, offset_down);
	//this->write8(APDS9960_GOFFSET_L, offset_left);
	//this->write8(APDS9960_GOFFSET_R, offset_right);
	Adafruit_APDS9960_write8(APDS9960_GOFFSET_U, offset_up);
	Adafruit_APDS9960_write8(APDS9960_GOFFSET_D, offset_down);
	Adafruit_APDS9960_write8(APDS9960_GOFFSET_L, offset_left);
	Adafruit_APDS9960_write8(APDS9960_GOFFSET_R, offset_right);
}

/**************************************************************************/
/*!
    Enable gesture readings on APDS9960
*/
/**************************************************************************/
//void Adafruit_APDS9960_enableGesture(boolean en)
void Adafruit_APDS9960_enableGesture(bool en)
{
  if(!en){
	  _gconf4.GMODE = 0;
	  //write8(APDS9960_GCONF4, _gconf4.get());
	  Adafruit_APDS9960_write8(APDS9960_GCONF4, get_gconf4(_gconf4));
  }
  _enable.GEN = en;
  //write8(APDS9960_ENABLE, _enable.get());
  Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
  Adafruit_APDS9960_resetCounts();
}

void Adafruit_APDS9960_resetCounts()
{
  gestCnt = 0;
  UCount = 0;
  DCount = 0;
  LCount = 0;
  RCount = 0;
}

uint8_t Adafruit_APDS9960_readGesture(void)
{
	uint8_t toRead;
	//uint8_t bytesRead;
	uint8_t buf[256];
	//unsigned long t;
	unsigned long t = 0;
	uint8_t gestureReceived;
	while(1){
		int up_down_diff = 0;
		int left_right_diff = 0;
		gestureReceived = 0;
		if(!Adafruit_APDS9960_gestureValid()) return 10; //TODO: check
		
		//delay(30);
		nrf_delay_ms(30);
		//toRead = this->read8(APDS9960_GFLVL);
		toRead = Adafruit_APDS9960_read8(APDS9960_GFLVL);
		
		//bytesRead = this->read(APDS9960_GFIFO_U, buf, toRead);
		Adafruit_APDS9960_read(APDS9960_GFIFO_U, buf, toRead);

    if(abs((int)buf[0] - (int)buf[1]) > 13)
      up_down_diff += (int)buf[0] - (int)buf[1];

    if(abs((int)buf[2] - (int)buf[3]) > 13)
      left_right_diff += (int)buf[2] - (int)buf[3];

		if(up_down_diff != 0){
			if(up_down_diff < 0){
				 if( DCount > 0){
					 gestureReceived = APDS9960_UP;
				 }
				 else UCount++;
			}
			else if(up_down_diff > 0){
				 if( UCount > 0){
					 gestureReceived = APDS9960_DOWN;
				 }
				 else DCount++;
			}
		}
		
		if(left_right_diff != 0){
			if(left_right_diff < 0){
				 if( RCount > 0){
					 gestureReceived = APDS9960_LEFT;
				 }
				 else LCount++;
			}
			else if(left_right_diff > 0){
				 if( LCount > 0){
					 gestureReceived = APDS9960_RIGHT;
				 }
				 else RCount++;
			}
		}
		
		if(up_down_diff != 0 || left_right_diff != 0) t = millis();
		
		if(gestureReceived || millis() - t > 300){
			 Adafruit_APDS9960_resetCounts();
			 return gestureReceived;
		}
	}
}

/**************************************************************************/
/*!
    Set LED brightness for proximity/gesture
*/
/**************************************************************************/
void Adafruit_APDS9960_setLED(apds9960LedDrive_t drive, apds9960LedBoost_t boost) {
  // set BOOST
  _config2.LED_BOOST = boost;
  //write8(APDS9960_CONFIG2, _config2.get());
  Adafruit_APDS9960_write8(APDS9960_CONFIG2, get_config2(_config2));
  
  _control.LDRIVE = drive;
  //write8(APDS9960_CONTROL, _control.get());
  Adafruit_APDS9960_write8(APDS9960_CONTROL, get_control(_control));
}

/**************************************************************************/
/*!
    Enable proximity readings on APDS9960
*/
/**************************************************************************/
//void Adafruit_APDS9960_enableColor(boolean en)
void Adafruit_APDS9960_enableColor(bool en)
{
  _enable.AEN = en;
  //write8(APDS9960_ENABLE, _enable.get());
  Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

bool Adafruit_APDS9960_colorDataReady()
{
	//_status.set(this->read8(APDS9960_STATUS));
	set_status(Adafruit_APDS9960_read8(APDS9960_STATUS),_status);
	return _status.AVALID;
}







uint8_t Adafruit_APDS9960_read(uint8_t reg, uint8_t *buf, uint8_t num)
{
	user_i2c_read(APDS9960_ADDRESS, reg, buf, num);
	return 1;
	/*
	uint8_t value;
	uint8_t pos = 0;
	bool eof = false;

	//on arduino we need to read in 32 byte chunks
	while(pos < num && !eof){

		uint8_t read_now = min(32, num - pos);
		Wire.beginTransmission((uint8_t)_i2caddr);
		Wire.write((uint8_t)reg + pos);
		Wire.endTransmission();

		Wire.requestFrom((uint8_t)_i2caddr, read_now);

		for(int i=0; i<read_now; i++){
			if(!Wire.available()){
				eof = true;
				break;
			}
			buf[pos] = Wire.read();
			pos++;
		}
	}
	return pos;
	*/
}

void Adafruit_APDS9960_write(uint8_t reg, uint8_t *buf, uint8_t num)
{
	user_i2c_write(APDS9960_ADDRESS, reg, buf, num);
	/*
	Wire.beginTransmission((uint8_t)_i2caddr);
	Wire.write((uint8_t)reg);
	Wire.write((uint8_t *)buf, num);
	Wire.endTransmission();
	*/
}
uint16_t Adafruit_APDS9960_read16R(uint8_t reg)
{
  uint8_t ret[2];
  //this->read(reg, ret, 2);
  Adafruit_APDS9960_read(reg, ret, 2);

  return (ret[1] << 8) | ret[0];
}
/**************************************************************************/
/*!
    @brief  Reads the raw red, green, blue and clear channel values
*/
/**************************************************************************/
void Adafruit_APDS9960_getColorData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  
  *c = Adafruit_APDS9960_read16R(APDS9960_CDATAL);
  *r = Adafruit_APDS9960_read16R(APDS9960_RDATAL);
  *g = Adafruit_APDS9960_read16R(APDS9960_GDATAL);
  *b = Adafruit_APDS9960_read16R(APDS9960_BDATAL);
  //c = Adafruit_APDS9960_read16R(APDS9960_CDATAL);
  //r = Adafruit_APDS9960_read16R(APDS9960_RDATAL);
  //g = Adafruit_APDS9960_read16R(APDS9960_GDATAL);
  //b = Adafruit_APDS9960_read16R(APDS9960_BDATAL);

}

/**************************************************************************/
/*!
    @brief  Converts the raw R/G/B values to color temperature in degrees
            Kelvin
*/
/**************************************************************************/
uint16_t Adafruit_APDS9960_calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b)
{
  float X, Y, Z;      /* RGB to XYZ correlation      */
  float xc, yc;       /* Chromaticity co-ordinates   */
  float n;            /* McCamy's formula            */
  float cct;

  /* 1. Map RGB values to their XYZ counterparts.    */
  /* Based on 6500K fluorescent, 3000K fluorescent   */
  /* and 60W incandescent values for a wide range.   */
  /* Note: Y = Illuminance or lux                    */
  X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
  Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
  Z = (-0.68202F * r) + (0.77073F * g) + ( 0.56332F * b);

  /* 2. Calculate the chromaticity co-ordinates      */
  xc = (X) / (X + Y + Z);
  yc = (Y) / (X + Y + Z);

  /* 3. Use McCamy's formula to determine the CCT    */
  n = (xc - 0.3320F) / (0.1858F - yc);

  /* Calculate the final CCT */
  cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) + 5520.33F;


  /* Return the results in degrees Kelvin */
  return (uint16_t)cct;
  /* Return the results in degrees Celsius */
  //return (uint16_t)(cct-273.15F);
}

/**************************************************************************/
/*!
    @brief  Calculate ambient light values
*/
/**************************************************************************/

uint16_t Adafruit_APDS9960_calculateLux(uint16_t r, uint16_t g, uint16_t b)
{
  float illuminance;

  /* This only uses RGB ... how can we integrate clear or calculate lux */
  /* based exclusively on clear since this might be more reliable?      */
  illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

  return (uint16_t)illuminance;
}

void Adafruit_APDS9960_enableColorInterrupt() {
  _enable.AIEN = 1;
  //write8(APDS9960_ENABLE, _enable.get());
  Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

void Adafruit_APDS9960_disableColorInterrupt() {
	_enable.AIEN = 0;
	//write8(APDS9960_ENABLE, _enable.get());
	Adafruit_APDS9960_write8(APDS9960_ENABLE, get_enable(_enable));
}

void Adafruit_APDS9960_clearInterrupt(void) {
  //this->write(APDS9960_AICLEAR, NULL, 0);
	uint8_t asd = 1;
	Adafruit_APDS9960_write(APDS9960_AICLEAR, &asd, 1);
}


void Adafruit_APDS9960_setIntLimits(uint16_t low, uint16_t high) {
	Adafruit_APDS9960_write8(APDS9960_AILTIL, low & 0xFF);
	Adafruit_APDS9960_write8(APDS9960_AILTH, low >> 8);
	Adafruit_APDS9960_write8(APDS9960_AIHTL, high & 0xFF);
	Adafruit_APDS9960_write8(APDS9960_AIHTH, high >> 8);
}

//void Adafruit_APDS9960_write8(byte reg, byte value)
void Adafruit_APDS9960_write8(uint8_t reg, uint8_t value)
{
	//this->write(reg, &value, 1);
	Adafruit_APDS9960_write(reg, &value, 1);
}

//uint8_t Adafruit_APDS9960_read8(byte reg)
uint8_t Adafruit_APDS9960_read8(uint8_t reg)
{
	uint8_t ret;
	//this->read(reg, &ret, 1);
	Adafruit_APDS9960_read(reg, &ret, 1);
	
	return ret;
}

uint32_t Adafruit_APDS9960_read32(uint8_t reg)
{
	uint8_t ret[4];
	//this->read(reg, ret, 4);
	Adafruit_APDS9960_read(reg, ret, 4);
	
	return (ret[0] << 24) | (ret[1] << 16) | (ret[2] << 8) | ret[3];
}

uint16_t Adafruit_APDS9960_read16(uint8_t reg)
{
	uint8_t ret[2];
	//this->read(reg, ret, 2);
	Adafruit_APDS9960_read(reg, ret, 2);
	
	return (ret[0] << 8) | ret[1];
}


void Adafruit_APDS9960_i2c_init()
{
	//Wire.begin(); //todo
}


