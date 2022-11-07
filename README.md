# Libreria per la gestione del dimming di un led collegato ad una board dell'arduino ecosystem

# Utilizzo

#### Inizializza l'oggetto LedDimming con il pin pwm associato, il tempo di dimming, la massima luminosità percentuale e opzionalmente il nome associato
```c++
LedDimming(int8_t Pin, uint16_t DimmingTime, uint8_t MaxBrightnessPercent, const char 
*LedStripeName = NULL);
```
#### Imposta il tempo di dimming tra accensione e spegnimento (in ms)
```c++
void setDimmingTime(uint16_t Time);
```

#### Imposta il nuovo stato da raggiungere dopo il dimming se Fast imposta subito il nuovo valore senza passare dall'engine
```c++
void setStatus(stripe_status NewStatus, bool Fast = false);
```

#### Restituisce lo stato attuale della striscia
```c++
stripe_status getStatus();
```

#### Informa se la striscia sta cambiando
```c++
bool ledSwitching();
```

#### Imposta la massima luminostà da raggiungere in percentuale
```c++
void setBrightness(uint8_t NewBrightnessPercent);
```

#### Esegue il motore per la gestione del dimming e del cambio stato
```c++
void ledStripeEngine();
```