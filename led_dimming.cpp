#include "led_stripes.h"
#include "../SerialDebug/serial_debug.h"

uint16_t LedStripe::_percToAnalogWrite(uint8_t Perc)
{
	return ((Perc * _pwmRange) / 100);
}

LedStripe::LedStripe(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPerc, const char *LedStripeName)
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

void LedStripe::setDimmingTime(uint16_t Time)
{
	if((Time <= _pwmRange * _DIMMING_CYCLE && Time >= _DIMMING_CYCLE) || 
		Time == NO_DIMMING)
	{
		_dimmingTime = Time;
		if(_dimmingTime == NO_DIMMING)
		{
			Debug.logInfo("Dimming impostato a \"NO_DIMMING\"");
		}
		else
		{
			_brightnessIncrement = _pwmRange / (_dimmingTime / _DIMMING_CYCLE);
		}
	}
}

void LedStripe::setStatus(stripe_status NewStatus, bool Fast)
{
	if(NewStatus != _targetStatus || Fast)
	{
		if(Fast)
		{
			if(NewStatus == off_status)
			{
				analogWrite(_pin, 0);
				_actualBrightness = 0;
				Debug.logInfo("Striscia spenta in Fast mode");
				_actualStatus = off_status;
			}
			else
			{
				analogWrite(_pin, _brightnessTarget);
				_actualBrightness = _brightnessTarget;
				Debug.logInfo("Striscia accesa in Fast mode");
				_actualStatus = on_status;
			}
			_stripeIsSwitching = false;
		}
		_targetStatus = NewStatus;
		if(_ledStripeName)
		{
			Debug.logDebug("Impostazione nuovo status della striscia: " + String(_ledStripeName));
		}
		Debug.logDebug("Settato nuovo stato della striscia led al valore: " + String(NewStatus));
	}
}

LedStripe::stripe_status LedStripe::getStatus()
{
	return _actualStatus;
}

bool LedStripe::ledSwitching()
{
	return _stripeIsSwitching;
}

void LedStripe::setBrightness(uint8_t NewBrightnessPerc)
{
	uint16_t AnalogBright = _percToAnalogWrite(NewBrightnessPerc);
	if(AnalogBright != _brightnessTarget && NewBrightnessPerc <= MAX_BRIGHTNESS)
	{
		_brightnessTarget = AnalogBright;
		if(_ledStripeName)
		{
			Debug.logDebug("Impostazione brightness striscia: " + String(_ledStripeName));
		}
		Debug.logInfo("Impostata nuova luminosita della striscia a: " + String(NewBrightnessPerc));
	}
}

void LedStripe::ledStripeEngine()
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
					Debug.logInfo("Striscia spenta senza dimming");
				}
				else
				{
					analogWrite(_pin, _brightnessTarget);
					_actualBrightness = _brightnessTarget;
					Debug.logInfo("Striscia accesa senza dimming");
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
						Debug.logInfo("Striscia spenta con dimming");
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
						Debug.logInfo("Striscia accesa con dimming");
					}
				}
				analogWrite(_pin, _actualBrightness);
			}
		}
	}
}