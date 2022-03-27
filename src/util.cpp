#include <util.h>

void pulsePin(uint8_t pin){
  digitalWrite(pin, HIGH);
  delay(50);
  digitalWrite(pin, LOW);
}