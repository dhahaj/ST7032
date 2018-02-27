#include <ST7032.h>

ST7032 lcd = ST7032(8,9,10,11,13);

void setup() {
  lcd.begin();
  lcd.display();
  lcd.print("Hello world!");  
  lcd.clear();
}

void loop() {

}