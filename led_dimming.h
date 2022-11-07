#ifndef LED_STRIPES_H
#define LED_STRIPES_H
#include <Arduino.h>
#include <stdint.h>

#define NO_DIMMING			0

class LedDimming
{
	public:
		typedef enum
		{
			off_status = 0,
			on_status
		}stripe_status;

	private:
		const uint16_t _DIMMING_CYCLE = 10; // in ms
		char *_ledStripeName = NULL;
		int8_t _pin = -1;
		uint16_t _brightnessTarget = 0;
		uint16_t _actualBrightness = 0;
		uint16_t _brightnessIncrement = 0;
		uint16_t _dimmingTime = 0;
		stripe_status _actualStatus = off_status;
		stripe_status _targetStatus = off_status;
		uint32_t _engineTimer;
		bool _stripeIsSwitching = false;
#ifdef ESP8266
		const uint16_t _pwmRange = 1023;
		const uint16_t _pwmFrq = 5000;
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

	public:
		const uint8_t MAX_BRIGHTNESS = 100; // Massima luminosità percentuale

		LedDimming(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPercent, const char *LedStripeName = NULL);
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
		void setStatus(stripe_status NewStatus, bool Fast = false);

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
		 */
		void setBrightness(uint8_t NewBrightnessPercent);

		/**
		 * @brief Esegue il motore per la gestione del dimming e del cambio stato
		 * 
		 */
		void ledStripeEngine();

};

#endif