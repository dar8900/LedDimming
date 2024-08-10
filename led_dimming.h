#ifndef LED_STRIPES_H
#define LED_STRIPES_H
#include <Arduino.h>
#include <stdint.h>

// #define DEBUG_TIME

#define NO_DIMMING				0
#define FAST_SWITCH_ENABLED		true
#define FAST_SWITCH_DISABLED	false

class LedDimming
{
	public:
		typedef enum
		{
			off_status = 0,
			on_status
		}stripe_status;

	private:
		uint16_t _DIMMING_CYCLE = 10; // in ms
		char *_ledStripeName = NULL;
		int8_t _pin = -1;
		uint16_t _brightnessTarget = 0;
		uint16_t _oldBrightnessTarget = 0;
		uint16_t _actualBrightness = 0;
		uint16_t _brightnessIncrement = 0;
		uint16_t _dimmingTime = 0;
		uint32_t _engineTimer;
		bool _stripeIsSwitching = false;
#if defined(ESP8266)
	uint8_t _pwmResolution = 8;
    uint16_t _pwmRange = 1023;
    uint16_t _pwmFrq = 5000;
#elif defined(ESP32)
	uint8_t _pwmResolution = 8;
    uint16_t _pwmRange = 1023;
    uint16_t _pwmFrq = 5000;
#else
    const uint16_t _pwmRange = 255;
#endif
		/**
		 * @brief Funzione per la conversione da percentuale a valore analogico
		 * 
		 * @param uint8_t Perc 
		 * @return uint16_t analogVal
		 */
		uint16_t _percToAnalogWrite(uint8_t Perc);
		void _writeDebugMsg(String Msg);	

	public:
		const uint8_t MAX_BRIGHTNESS = 100; // Massima luminosità percentuale

#if defined(ESP8266)
		LedDimming(int8_t Pin, uint16_t DimmingTime, uint16_t MaxRange = 255, uint16_t MaxFrq = 1000, uint8_t MaxBrightnessPercent = 100, const char *LedStripeName = NULL);
#elif defined(ESP32)
		LedDimming(int8_t Pin, uint16_t DimmingTime, uint16_t MaxRange = 255, uint16_t MaxFrq = 1000,  uint8_t MaxBrightnessPercent = 100, const char *LedStripeName = NULL);
#else
		LedDimming(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPercent = 100, const char *LedStripeName = NULL);
#endif
		void setEngineCycle(uint16_t NewCyleTime);

		/**
		 * @brief Imposta il tempo di dimming tra accensione e spegnimento (in ms)
		 * 
		 * @param uint16_t Time 
		 */
		void setDimmingTime(uint16_t Time);


		/**
		 * @brief Imposta il nuovo stato da raggiungere dopo il dimming
		 * 			se Fast imposta subito il nuovo valore senza passare dall'engine
		 * 
		 * @param stripe_status NewStatus 
		 * @param bool Fast 
		 */
		void setStatus(stripe_status NewStatus, bool Fast = FAST_SWITCH_DISABLED);

		/**
		 * @brief Inverte lo stato del led se non sta dimmerando
		 * 
		 * @param bool Fast 
		 */
		void toggleStatus(bool Fast = FAST_SWITCH_DISABLED);

		/**
		 * @brief Restituisce lo stato attuale della striscia
		 * 
		 * @return stripe_status ActualStatus 
		 */
		stripe_status getStatus();

		/**
		 * @brief Informa se la striscia sta cambiando
		 * 
		 * @return bool
		 */
		bool ledSwitching();

		/**
		 * @brief Imposta la massima luminostà da raggiungere in percentuale
		 * 
		 * @param uint8_t NewBrightness 
		 * @param bool Fast
		 */
		void setBrightness(uint8_t NewBrightnessPercent, bool Fast = FAST_SWITCH_DISABLED);

		/**
		 * @brief Esegue il motore per la gestione del dimming e del cambio stato
		 * 
		 */
		void ledStripeEngine();

#if defined(ESP8266)
		void setPwmRange(uint16_t NewRange);
		void setPwmFrq(uint16_t NewFrq);
#elif defined(ESP32)
		void setPwmRange(uint16_t NewRange);
		void setPwmFrq(uint16_t NewFrq);
#endif

};

#endif