/*Missing Components*/

//Color Sensors
//Ultrasonic Sensors
//IR Sensors
//Heater
//RTC or Real Time Clock

/*Code that need to add*/

//Table Serving
//Object Detection
//Line Following 
//Temperature 
//Timing
//IP Address for WiFi Connectivity 


#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

/*LCD Connection*/
LiquidCrystal_I2C lcd(0x27, 16, 2);

/*Custom LCD Heart Display*/
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

/*Next and Select Button*/
const int Next = 13;
const int Select = 12;

/*Relay*/
const int Water_pump = 33;      

/*Wheels*/
const int Left_Motor = 14;
const int Right_Motor = 27;

/*Coffee Variety*/
const char* CoffeeVarieties[] = {"Black Coffee", "Chocolate", "Caramel"};
int selectedIndex = 0;

/*Servo motor for dropping*/
Servo Cup;
Servo Black_Coffee;
Servo Caramel_Coffee;
Servo Chocolate;

/*Position of Servo*/
const int BlackCoffeePosition = 90;
const int CaramelCoffeePosition = 90;
const int SugarPosition = 90;
const int CupPosition = 90;
const int ServoClosed = 0;

// Function declarations
void DisplaySelection();
void OpeningScreen();
void serveCoffee(int index);
void testComponents();

void setup() {
  /*Display*/
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.backlight();

  /*Buttons*/
  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);

  /*Relay Input pins*/
  pinMode(Water_pump, OUTPUT);
  pinMode(Left_Motor, OUTPUT);
  pinMode(Right_Motor, OUTPUT);

  /*Servo*/
  Cup.attach(17);
  Black_Coffee.attach(16);
  Caramel_Coffee.attach(4);
  Chocolate.attach(2);
  
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("System Initialized");

  // Test all inputs and outputs (except buttons)
  testComponents();

  DisplaySelection();
  OpeningScreen();
}

void loop() {
  digitalWrite(Water_pump, LOW);
  delay(5000);
  
  if(digitalRead(Next) == LOW){
    selectedIndex = (selectedIndex + 1) % 3; //Coffee selection loop
    DisplaySelection();
    Serial.print("Coffee selection changed: ");
    Serial.println(CoffeeVarieties[selectedIndex]);
    delay(300);
  }

  if(digitalRead(Select) == LOW){
    Serial.print("Selected coffee: ");
    Serial.println(CoffeeVarieties[selectedIndex]);
    serveCoffee(selectedIndex);
    delay(300);
  }
  Cup.write(ServoClosed);
  Black_Coffee.write(ServoClosed);
  Chocolate.write(ServoClosed);
  Caramel_Coffee.write(ServoClosed);
  digitalWrite(Left_Motor, HIGH);
  digitalWrite(Right_Motor, HIGH);
  digitalWrite(Water_pump, HIGH);
}

void OpeningScreen(){
  /*Loading Screen*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    BOSS");
  lcd.setCursor(0, 1);
  lcd.print("   KAPE TAYO");
  delay(3000);
  lcd.clear();
}

void DisplaySelection(){
  lcd.clear();
  lcd.print("Select ur Coffee: ");
  lcd.setCursor(0, 1);
  lcd.print(CoffeeVarieties[selectedIndex]);
  Serial.print("Displayed selection: ");
  Serial.println(CoffeeVarieties[selectedIndex]);
}

void serveCoffee(int index){
  /*Processing Display*/
  lcd.clear();
  lcd.print("  Processing");
  lcd.setCursor(0, 1);
  lcd.print(" Please wait");

  Serial.println("Processing coffee...");

  /*Dispensing*/
  Cup.write(180);
  delay(400);
  Cup.write(0);
  delay(2000);

  switch (index){
    case 0:
      Black_Coffee.write(BlackCoffeePosition);
      delay(500);
      Black_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Black Coffee");
      break;

    case 1:
      Chocolate.write(180);
      delay(500);
      Chocolate.write(0);
      Serial.println("Dispensed: Chocolate");
      break;

    case 2:
      Caramel_Coffee.write(CaramelCoffeePosition);
      delay(500);
      Caramel_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Caramel Coffee");
      break;
  }
  digitalWrite(Water_pump, HIGH);
  lcd.clear();
  lcd.print("Your Coffee is");
  lcd.setCursor(0, 1);
  lcd.print("    Ready!");
  delay(6000);

  /*Heart*/
  lcd.clear();
  lcd.print("   Thanks!");
  lcd.setCursor(7, 1);
  lcd.write(byte(0));
  lcd.setCursor(8, 1);
  lcd.write(byte(0));
  lcd.setCursor(9, 1);
  lcd.write(byte(0));
  delay(2000);

  /*Return to Display selection*/
  DisplaySelection();

  delay(1000);
  digitalWrite(Left_Motor, LOW);
  digitalWrite(Right_Motor, LOW);
  delay(20000);
}

void testComponents() {
  Serial.println("Testing components...");

  // Test LCD
  lcd.clear();
  lcd.print("Testing LCD...");
  delay(2000);
  Serial.println("LCD test complete.");

  // Test Relay
  Serial.println("Testing Water Pump...");
  digitalWrite(Water_pump, HIGH);
  delay(1000);
  digitalWrite(Water_pump, LOW);
  Serial.println("Water Pump test complete.");

  // Test Motors
  Serial.println("Testing Motors...");
  digitalWrite(Left_Motor, HIGH);
  digitalWrite(Right_Motor, HIGH);
  delay(1000);
  digitalWrite(Left_Motor, LOW);
  digitalWrite(Right_Motor, LOW);
  Serial.println("Motors test complete.");

  // Test Servos
  Serial.println("Testing Servos...");
  Cup.write(180);
  delay(500);
  Cup.write(0);
  delay(500);
  Black_Coffee.write(180);
  delay(500);
  Black_Coffee.write(0);
  delay(500);
  Caramel_Coffee.write(180);
  delay(500);
  Caramel_Coffee.write(0);
  delay(500);
  Chocolate.write(180);
  delay(500);
  Chocolate.write(0);
  delay(500);
  Serial.println("Servos test complete.");

  Serial.println("All components tested successfully.");
}

