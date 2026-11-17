#include <Arduino.h>

#define CLK 12
#define DT 10
#define SW 16
int counter = 0;
int currentStateCLK;
int lastStateCLK;

int sw_default = 0;

struct Button
{
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button1 = {SW, 0, false};

void IRAM_ATTR isr()
{
  button1.numberKeyPresses++;
  button1.pressed = true;
}

void setup_ISR()
{

  pinMode(button1.PIN, INPUT);
  attachInterrupt(button1.PIN, isr, FALLING);
}
unsigned long m;
unsigned long interval = 2000;

void setup()
{

  setup_ISR();

  pinMode(CLK, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(DT, INPUT);
  lastStateCLK = digitalRead(CLK);
  Serial.begin(115200);
  sw_default = digitalRead(SW);
  Serial.println("Ready");
  m = millis() + interval;
}
void loop()
{

  if (digitalRead(SW) != sw_default)
  {
    counter = 0;
    Serial.println("Reset");
    delay(400);
    return;
  }
  currentStateCLK = digitalRead(CLK);
  if (currentStateCLK != lastStateCLK)
  {
    if (digitalRead(DT) != currentStateCLK)
    {
      counter++;
    }
    else
    {
      counter--;
    }
    Serial.println(counter);
  }
  lastStateCLK = currentStateCLK;

  if (button1.pressed)
  {
    Serial.printf("Button has been pressed %u times\n", button1.numberKeyPresses);
    button1.pressed = false;
  }
  if (millis() > m)
  {
    m = millis() + interval;
    Serial.println("Proof of life");
  }
}
