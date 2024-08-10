#include "led_dimming.h"


void LedDimming::_writeDebugMsg(String Msg)
{
#ifdef DEBUG_TIME
	if(Msg != "")
	{
		if(_ledStripeName){
			Serial.print(_ledStripeName);
			Serial.print("\t");
		}
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
	analogWriteResolution(_pmwResolution);
	analogWriteFrequency(_pwmFrq);
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
	analogWriteResolution(_pmwResolution);
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
	if((Time <= _pwmRange * _DIMMING_CYCLE && Time >= _DIMMING_CYCLE) || 
		Time == NO_DIMMING)
	{
		_dimmingTime = Time;
		if(_dimmingTime != NO_DIMMING)
		{
			_brightnessIncrement = _pwmRange / (_dimmingTime / _DIMMING_CYCLE);
			_writeDebugMsg("_brightnessIncrement = " + String(_brightnessIncrement));
		}
	}
}

void LedDimming::toggleStatus(bool Fast)
{
	if(!_stripeIsSwitching && _brightnessTarget == _actualBrightness){
		_writeDebugMsg("Toggle status");
		if(_brightnessTarget > 0){
			setStatus(off_status, Fast);
		} else {
			setStatus(on_status, Fast);
		}
	}
}

void LedDimming::setStatus(stripe_status NewStatus, bool Fast)
{
	if(NewStatus == off_status)
	{
		_actualBrightness = 0;
	}
	else
	{
		_actualBrightness = _brightnessTarget;
	}
	if(Fast == FAST_SWITCH_ENABLED)
	{
		analogWrite(_pin, _actualBrightness);
		_brightnessTarget = _actualBrightness;
		_stripeIsSwitching = false;
	}

}

LedDimming::stripe_status LedDimming::getStatus()
{
	return _actualBrightness > 0 ? on_status : off_status;
}

bool LedDimming::ledSwitching()
{
	return _stripeIsSwitching;
}

void LedDimming::setBrightness(uint8_t NewBrightnessPerc, bool Fast)
{
	uint16_t AnalogBright = _percToAnalogWrite(NewBrightnessPerc);
	if(AnalogBright != _brightnessTarget && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = AnalogBright;
		if(Fast)
		{
			if(NewBrightnessPerc == 0){
				setStatus(off_status, Fast);
			} else {
				setStatus(on_status, Fast);
			}
		}
	}
}

#if defined(ESP8266)
void LedDimming::setPwmRange(uint16_t NewRange)
{
	_pwmRange = NewRange;
	_pmwResolution = calcResolution(_pwmRange);
	analogWriteResolution(_pmwResolution);
	setDimmingTime(_dimmingTime);
}

void LedDimming::setPwmFrq(uint16_t NewFrq)
{
	if(_pwmFrq != NewFrq){
		_pwmFrq = NewFrq;
		analogWriteFrequency(_pwmFrq);
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
			if(_dimmingTime == NO_DIMMING){
				setStatus(getStatus(), FAST_SWITCH_ENABLED);
			}
			else
			{
				if(BrightnessDelta > 0)
				{
					if(BrightnessDelta > _brightnessIncrement)
					{
						_actualBrightness -= _brightnessIncrement;
						_stripeIsSwitching = true;
						_writeDebugMsg("Brightness DECREMENT");
					}
					else
					{
						_actualBrightness = _brightnessTarget;
						_stripeIsSwitching = false;
					}
				}
				else
				{
					if(BrightnessDelta < (-1 * _brightnessIncrement))
					{
						_actualBrightness += _brightnessIncrement;
						_stripeIsSwitching = true;
						_writeDebugMsg("Brightness INCREMENT");
					}
					else
					{
						_actualBrightness = _brightnessTarget;
						_stripeIsSwitching = false;
					}
				}
				analogWrite(_pin, _actualBrightness);
			}
		}
	}
}
