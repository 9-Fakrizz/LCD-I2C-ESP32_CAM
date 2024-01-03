#include <LiquidCrystal_I2C.h> 
#include <Wire.h> 

#define COLUMNS 16
#define ROWS    2

LiquidCrystal_I2C lcd(0x27, COLUMNS, ROWS);
unsigned long timer0;
bool count = true;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  Serial.println("LCD Character Backpack I2C Test.");

  if (!Wire.begin()) {
    Serial.println("Wire_error");
    while(1);
  }

  lcd.begin(16, 2);  
  lcd.clear();           
  lcd.backlight();  
}

void loop() {
  lcd.setCursor(2, 1);
  lcd.print("Proba is ");
  lcd.print(int(0.54 * 100));
  lcd.print("%");

  if (count == true) {
    count = false;
    timer0 = millis();  
  }

  if (millis() - timer0 >= 3000 && count == false) {
    count = true;
    Serial.println("3000pppppppp");
  }
}
