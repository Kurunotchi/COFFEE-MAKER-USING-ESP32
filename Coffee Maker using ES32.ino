#include <LiquidCrystal.h>
#include <ESP32Servo.h>

const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12; //change the GPIO according to your need
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int Next = 3;
const int Select = 4;
const int Water_pump = 5; //Relay input no.5
const int Heater = 6; //Relay input no.6

const char* CoffeeVarieties[] = {"Black Coffee", "Chocolate", "Caramel"};
int selectedIndex = 0;

Servo Cup;
Servo Black_Coffee;
Servo Caramel_Coffee;
Servo Chocolate;

const int BlackCoffeePosition = 90;
const int CaramelCoffeePosition = 90;
const int SugarPosition = 90;
const int CupPosition = 90;

const int ServoClosed = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);
  pinMode(Water_pump, OUTPUT);
  pinMode(Heater, OUPUT);
  displaySelection();
  
  Cup.attach(13);
  Black_Coffee.attach(2);
  Caramel_Coffee.attach(5);
  Chocolate.attach(6);
  
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Water_pump, LOW);
  delay(5000);
  digitalWrite(Heater, HIGH);
  
  if(digitalRead(Next) == LOW){
    selectedIndex = (selectedIndex + 1) % 3;
    displaySelection();
    delay(300);
  }

  if(digitalRead(Select) == LOW){
    serveCoffee(selectedIndex);
    delay(300);
  }
}

void displaySelection(){
  lcd.clear();
  lcd.print("Select ur Coffee: ");
  lcd.setCursor(0, 1);
  lcd.print(CoffeeVarieties[selectedIndex]);
}

void serveCoffee(int index){
  Cup.write(180);
  delay(400);
  Cup.write(0);
  delay(2000);

  switch (index){
    case 0:
      Black_Coffee.write(BlackCoffeePosition);
      delay(500);
      Black_Coffee.write(ServoClosed);
      break;

    case 1:
      Caramel_Coffee.write(CaramelCoffeePosition);
      delay(500);
      Caramel_Coffee.write(ServoClosed);
      break;

    case 2:
      Chocolate.write(180);
      delay(400);
      Chocolate.write(0);
      break;
  }
  digital.write(Water_pump, HIGH);
  delay(6000);
}
