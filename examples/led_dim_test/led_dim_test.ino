#include "led_dimming.h"

LedDimming dim(4, 500, 100);

void setup()
{
	dim.setStatus(LedDimming::on_status);
}

void loop()
{
	dim.ledStripeEngine();
}