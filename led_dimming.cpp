#include "led_dimming.h"


void LedDimming::_writeDebugMsg(String Msg)
{
#ifdef DEBUG_TIME
	if(Msg != "")
	{
		Serial.println(Msg);
	}
#endif	
}

uint16_t LedDimming::_percToAnalogWrite(uint8_t Perc)
{
	return ((Perc * _pwmRange) / 100);
}

#if defined(ESP8266)
int calcResolution(int Range) {
    int res = 0;

    // Trova il massimo res tale che 2^res <= Range
    while ((1 << (res + 1)) <= Range) {
        res++;
    }

    int potenzaBassa = 1 << res;      // 2^res
    int potenzaAlta = 1 << (res + 1); // 2^(res + 1)

    // Determina quale delle due potenze è più vicina al Range
    if ((Range - potenzaBassa) <= (potenzaAlta - Range)) {
        return res;
    } else {
        return res + 1;
    }
}

LedDimming::LedDimming(int8_t Pin, uint16_t DimmingTime, uint16_t MaxRange, uint16_t MaxFrq, uint8_t MaxBrightnessPercent, const char *LedStripeName)
{
	_pin = Pin;
	_pwmRange = MaxRange;
	_pwmFrq = MaxFrq;
	_pwmResolution = calcResolution(_pwmRange);
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPercent);
	analogWriteResolution(_pwmResolution);
	analogWriteFreq(_pwmFrq);
	_engineTimer = millis();
	if(LedStripeName){
		_ledStripeName = const_cast<char*>(LedStripeName);
	}
}
#elif defined(ESP32)
int calcResolution(int Range) {
    int res = 0;

    // Trova il massimo res tale che 2^res <= Range
    while ((1 << (res + 1)) <= Range) {
        res++;
    }

    int potenzaBassa = 1 << res;      // 2^res
    int potenzaAlta = 1 << (res + 1); // 2^(res + 1)

    // Determina quale delle due potenze è più vicina al Range
    if ((Range - potenzaBassa) <= (potenzaAlta - Range)) {
        return res;
    } else {
        return res + 1;
    }
}

LedDimming::LedDimming(int8_t Pin, uint16_t DimmingTime, uint16_t MaxRange, uint16_t MaxFrq,  uint8_t MaxBrightnessPercent, const char *LedStripeName)
{
	_pin = Pin;
	_pwmRange = MaxRange;
	_pwmFrq = MaxFrq;
	_pwmResolution = calcResolution(_pwmRange);
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPercent);
	analogWriteResolution(_pwmResolution);
	analogWriteFrequency(_pwmFrq);
	_engineTimer = millis();
	if(LedStripeName){
		_ledStripeName = const_cast<char*>(LedStripeName);
	}
}
#else
LedDimming::LedDimming(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc, const char *LedStripeName)
{
	_pin = Pin;
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPerc);
	_engineTimer = millis();
	if(LedStripeName){
		_ledStripeName = const_cast<char*>(LedStripeName);
	}
}
#endif

void LedDimming::setEngineCycle(uint16_t NewCyleTime)
{
	if(NewCyleTime > 0)
	{
		_DIMMING_CYCLE = NewCyleTime;
	}
}

void LedDimming::setDimmingTime(uint16_t Time)
{
	if((Time <= _pwmRange * _DIMMING_CYCLE && Time >= 0))
	{
		if(Time == 0){
			Time = _DIMMING_CYCLE;
		}
		_dimmingTime = Time;
		if(_dimmingTime != 0)
		{
			_brightnessIncrement = _pwmRange / (_dimmingTime / _DIMMING_CYCLE);
			_writeDebugMsg("setDimmingTIme -> _brightnessIncrement = " + String(_brightnessIncrement));
		}
	}
}



void LedDimming::_toggle()
{
	_writeDebugMsg("_toggle -> ActualBrightness: " + String(_actualBrightness) + " _brightnessTarget: " + String(_brightnessTarget));
	if(_actualBrightness > 0){
		_writeDebugMsg("_toggle -> Toggle to OFF");
		setStatus(off_status);
	} else {
		_writeDebugMsg("_toggle -> Toggle to ON");
		setStatus(on_status);
	}
}


void LedDimming::toggle(bool Enable)
{
	if(!_toggleEnabled && Enable){
		_toggle();
	}
	_toggleEnabled = Enable;
}


void LedDimming::setStatus(stripe_status NewStatus)
{
	if(NewStatus == off_status)
	{
		_brightnessTarget = 0;
	}
	else
	{
		_brightnessTarget = _maxBrightness;
	}
	_writeDebugMsg("setStatus -> _brightnessTarget: " + String(_brightnessTarget));
}

LedDimming::stripe_status LedDimming::getStatus()
{
	if(!_stripeIsSwitching){
		return _brightnessTarget > 0 ? on_status : off_status;
	}
	return _actualBrightness > 0 ? on_status : off_status;
}

bool LedDimming::ledSwitching()
{
	return _stripeIsSwitching;
}

void LedDimming::setBrightness(uint8_t NewBrightnessPerc)
{
	uint16_t AnalogBright = _percToAnalogWrite(NewBrightnessPerc);
	if(AnalogBright != _maxBrightness && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_writeDebugMsg("setBrightness -> brightnessTarget to: " + String(AnalogBright));
		_maxBrightness = AnalogBright;
	}
}

#if defined(ESP8266)
void LedDimming::setPwmRange(uint16_t NewRange)
{
	_pwmRange = NewRange;
	_pwmResolution = calcResolution(_pwmRange);
	analogWriteResolution(_pwmResolution);
	setDimmingTime(_dimmingTime);
}

void LedDimming::setPwmFrq(uint16_t NewFrq)
{
	if(_pwmFrq != NewFrq){
		_pwmFrq = NewFrq;
		analogWriteFreq(_pwmFrq);
	}
}
#elif defined(ESP32)
void LedDimming::setPwmRange(uint16_t NewRange)
{
	_pwmRange = NewRange;
	_pwmResolution = calcResolution(_pwmRange);
	analogWriteResolution(_pwmResolution);
	setDimmingTime(_dimmingTime);
}

void LedDimming::setPwmFrq(uint16_t NewFrq)
{
	if(_pwmFrq != NewFrq){
		_pwmFrq = NewFrq;
		analogWriteFrequency(_pwmFrq);
	}
}
#endif

void LedDimming::ledStripeEngine()
{
	if(_engineTimer == 0)
	{
		_engineTimer = millis();
	}
	if(millis() - _engineTimer >= _DIMMING_CYCLE)
	{
		_engineTimer = 0;
		int16_t BrightnessDelta = _actualBrightness - _brightnessTarget;
		if(BrightnessDelta != 0)
		{
			if(BrightnessDelta > 0)
			{
				if(BrightnessDelta > _brightnessIncrement)
				{
					_actualBrightness -= _brightnessIncrement;
					_stripeIsSwitching = true;
					_writeDebugMsg("ledStripeEngine -> Brightness DECREMENT");
				}
				else
				{
					_actualBrightness = _brightnessTarget;
					_stripeIsSwitching = false;
					if(_toggleEnabled){
						_writeDebugMsg("ledStripeEngine -> Toggle dopo ultimo decremento");
						_toggle();
					}
				}
			}
			else
			{
				if(BrightnessDelta < (-1 * _brightnessIncrement))
				{
					_actualBrightness += _brightnessIncrement;
					_stripeIsSwitching = true;
					_writeDebugMsg("ledStripeEngine -> Brightness INCREMENT");
				}
				else
				{
					_actualBrightness = _brightnessTarget;
					_stripeIsSwitching = false;
					if(_toggleEnabled){
						_writeDebugMsg("ledStripeEngine -> Toggle dopo ultimo incremento");
						_toggle();
					}
				}
			}
			analogWrite(_pin, _actualBrightness);
		}
	}
}
