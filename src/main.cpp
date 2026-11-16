#include <Arduino.h>
#include <esp32-hal-log.h>
#include <Adafruit_NeoPixel.h>

const int Button_Pin = 5;
const int BUZZOR_OUTPUT_PIN = 2;

const int RED_RGB_LED_PIN = 15;
const int GREEN_RGB_LED_PIN = 7;
const int BLUE_RGB_LED_PIN = 6;

const int BUTTONS_PIN = 8;

// true, wenn die RGB-LED eine gemeinsame Anode hat (255 = aus, 0 = voll an)
const bool COMMON_ANODE = true;

void setColor(int r, int g, int b)
{
  if (COMMON_ANODE)
  {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  analogWrite(RED_RGB_LED_PIN, r);
  analogWrite(GREEN_RGB_LED_PIN, g);
  analogWrite(BLUE_RGB_LED_PIN, b);
}

void setup()
{
  pinMode(Button_Pin, INPUT);
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
  int ButtonValue = analogRead(BUTTONS_PIN);

  if (ButtonValue > 700 && ButtonValue < 800)
  {
    setColor(255, 0, 0); // Rot
  }
  else if (ButtonValue > 1500 && ButtonValue < 1620)
  {
    setColor(255, 180, 0); // Gelb (Grün etwas reduziert, da grüne LEDs heller sind)
  }
  else if (ButtonValue > 2350 && ButtonValue < 2450)
  {
    setColor(0, 255, 0); // Grün
  }
  else if (ButtonValue > 1850 && ButtonValue < 1950)
  {
    setColor(255, 0, 255); // Rot+Gelb -> Magenta
  }
  else if (ButtonValue > 2700 && ButtonValue < 2800)
  {
    setColor(0, 255, 255); // Gelb+Grün -> Cyan
  }
  else if (ButtonValue > 2500 && ButtonValue < 2600)
  {
    setColor(0, 0, 255); // Rot+Grün -> Blau
  }
  else
  {
    setColor(0, 0, 0); // kein Button gedrückt -> LED aus
  }
}

// ROT = Gemessener Wert : 761 GELB = Gemessener Wert : 1577 GRÜN = Gemessener Wert : 2407

// ROT + GELB Gemessener Wert: 1897   GELB+GRÜN = Gemessener Wert: 2749   ROT+GRÜN Gemessener Wert: 2551G
// Gemessener Wert: 1897              Gemessener Wert: 2750               Gemessener Wert: 2551