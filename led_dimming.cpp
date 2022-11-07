#include "led_dimming.h"

void LedDimming::_writeDebugMsg(String Msg)
{
#ifdef DEBUG_TIME
	if(Msg)
	{
		Serial.println(Msg);
	}
#endif	
}

uint16_t LedDimming::_percToAnalogWrite(uint8_t Perc)
{
	return ((Perc * _pwmRange) / 100);
}

LedDimming::LedDimming(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc, const char *LedStripeName)
{
	_pin = Pin;
	pinMode(_pin, OUTPUT);
	setDimmingTime(DimmingTime);
	setBrightness(MaxBrightnessPerc);
#ifdef ESP8266
	analogWriteRange(_pwmRange);
	analogWriteFreq(_pwmFrq);
#endif
	_engineTimer = millis();
	if(LedStripeName)
	{
		_ledStripeName = const_cast<char*>(LedStripeName);
	}
	setDimmingTime(NO_DIMMING);
}

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

void LedDimming::setStatus(stripe_status NewStatus, bool Fast)
{
	if(NewStatus != _targetStatus || Fast)
	{
		if(Fast)
		{
			if(NewStatus == off_status)
			{
				analogWrite(_pin, 0);
				_actualBrightness = 0;
				_actualStatus = off_status;
			}
			else
			{
				analogWrite(_pin, _brightnessTarget);
				_actualBrightness = _brightnessTarget;
				_actualStatus = on_status;
			}
			_stripeIsSwitching = false;
		}
		_targetStatus = NewStatus;
	}
}

LedDimming::stripe_status LedDimming::getStatus()
{
	return _actualStatus;
}

bool LedDimming::ledSwitching()
{
	return _stripeIsSwitching;
}

void LedDimming::setBrightness(uint8_t NewBrightnessPerc)
{
	uint16_t AnalogBright = _percToAnalogWrite(NewBrightnessPerc);
	if(AnalogBright != _brightnessTarget && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = AnalogBright;
	}
}

void LedDimming::ledStripeEngine()
{
	if(_engineTimer == 0)
	{
		_engineTimer = millis();
	}
	if(millis() - _engineTimer >= _DIMMING_CYCLE)
	{
		_engineTimer = 0;
		if(_actualStatus != _targetStatus)
		{
			if(_dimmingTime == NO_DIMMING)
			{
				if(_targetStatus == off_status)
				{
					analogWrite(_pin, 0);
					_actualBrightness = 0;
				}
				else
				{
					analogWrite(_pin, _brightnessTarget);
					_actualBrightness = _brightnessTarget;
				}
				_actualStatus = _targetStatus;
			}
			else
			{
				if(_targetStatus == off_status)
				{
					if(_actualBrightness - _brightnessIncrement > 0)
					{
						_actualBrightness -= _brightnessIncrement;
						_stripeIsSwitching = true;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = 0;
						_stripeIsSwitching = false;
						_writeDebugMsg("Led dimming OFF");
					}
				}
				else
				{
					if(_actualBrightness + _brightnessIncrement < _brightnessTarget)
					{
						_actualBrightness += _brightnessIncrement;
						_stripeIsSwitching = true;
					}
					else
					{
						_actualStatus = _targetStatus;
						_actualBrightness = _brightnessTarget;
						_stripeIsSwitching = false;
						_writeDebugMsg("Led dimming ON");
					}
				}
				analogWrite(_pin, _actualBrightness);
			}
		}
	}
}