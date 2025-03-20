#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include "Adafruit_TCS34725.h"

struct Order {
    int table;
    int coffeeType;
};

std::vector<Order> orderQueue; // Order queue

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_60MS, TCS34725_GAIN_1X);
bool coffeeDelivered = false;

int selectedCoffee = -1;
int selectedTable = -1;
bool orderReceived = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

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

//Ito ay para sa line following

#define IR_LEFT 34   
#define IR_RIGHT 35  

//Wheels

int Left_Reverse = 26;
int Left_Forward = 25;
int Right_Forward = 14;
int Right_Reverse = 27;

//Button

int Next = 13;
int Select = 12;
int Water_pump = 33; 

const char* CoffeeVarieties[] = {"Black Coffee", "Chocolate", "Caramel"};
int selectedIndex = 0;

Servo Cup;
Servo Black_Coffee;
Servo Caramel_Coffee;
Servo Chocolate;
Servo Waterpump_Angle;

const int BlackCoffeePosition = 90;
const int CaramelCoffeePosition = 90;
const int ChocolatePosition = 90;
const int WaterpumpAngle = 90;
const int CupPosition = 90;
const int ServoClosed = 0;

const char* ssid = "Kodic";
const char* password = "kodicpogi21";

WebServer server(80);

void DisplaySelection();
void moveToTable(int table);
 void moveToHomePosition();
void OpeningScreen();
void serveCoffee(int index, int table);
void handleRoot();
void handleOrder();
void handleStatus();

void setup() { 
  Cup.write(ServoClosed);
  Black_Coffee.write(ServoClosed);
  Chocolate.write(ServoClosed); 
  Caramel_Coffee.write(ServoClosed);
  Waterpump_Angle.write(WaterpumpAngle);
  digitalWrite(Water_pump, HIGH);

  Serial.begin(115200);

    if (tcs.begin()) {
    Serial.println("TCS34725 found");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // Halt if sensor is not found
  }

  Serial.println("System Initialized");

  Serial.println("Initializing LCD...");
  lcd.init();
  lcd.createChar(0, heart);
  lcd.backlight();

  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);

  // IR Sensors
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);


  pinMode(Water_pump, OUTPUT);
  pinMode( Left_Forward, OUTPUT);
  pinMode( Right_Forward, OUTPUT);
  pinMode( Left_Reverse, OUTPUT);
  pinMode( Right_Reverse, OUTPUT);
  
  //servo pins
  Cup.attach(17);
  Black_Coffee.attach(16);
  Caramel_Coffee.attach(4);
  Chocolate.attach(2);
  Waterpump_Angle.attach(15);

  OpeningScreen();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  server.on("/", handleRoot);
  server.on("/order", handleOrder);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Web server started");
}
void loop() {
  Cup.write(ServoClosed);
  Black_Coffee.write(ServoClosed);
  Chocolate.write(ServoClosed); 
  Caramel_Coffee.write(ServoClosed);
  Waterpump_Angle.write(WaterpumpAngle);
  digitalWrite(Left_Forward, LOW);
  digitalWrite(Right_Forward, LOW);
  digitalWrite(Water_pump, HIGH);

  if (digitalRead(Next) == LOW) { 
    selectedIndex = (selectedIndex + 1) % 3; 
    DisplaySelection();
    delay(200); 
  }

  if (digitalRead(Select) == LOW) {
    serveCoffee(selectedIndex, 1); 
    delay(200); 
  }
  server.handleClient();
  if (orderReceived) {
    orderReceived = false;
    serveCoffee(selectedCoffee, selectedTable);
    moveToTable(selectedTable);
  }
    delay(5000);
    processOrders();
}
void serveCoffee(int index, int table) {
  lcd.clear();
  lcd.print("Processing");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");
  for (int i = 0; i < 4; i++) {  
    delay(500); 
    lcd.setCursor(12 + i, 1); 
    lcd.print(".");
  }

  Serial.println("Processing coffee...");
  delay(800);
  Cup.write(180);
  delay(400);
  Serial.println("Cup dropper");
  Cup.write(0);
  delay(2000);
  switch (index) {
    case 0:
      Black_Coffee.write(BlackCoffeePosition);
      delay(2000);
      Black_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Black Coffee");
      delay(2000);
      Serial.println("Waterpump angle");
      Waterpump_Angle.write(135);
      delay(1000);
      Serial.println("Waterpump on");
      delay(100);
      digitalWrite(Water_pump, LOW); 
      Serial.println("Waterpump delay");
      for (int i = 10; i > 0; i--) {
      Serial.print("Countdown: ");
      Serial.println(i);
      delay(1000); 
      }
      Serial.println("Waterpump off");
      delay(100);
      digitalWrite(Water_pump, HIGH);
      delay(1000);
      Waterpump_Angle.write(WaterpumpAngle);
      delay(100);
      break;
    case 1:
      Chocolate.write(ChocolatePosition);
      delay(2000);
      Chocolate.write(ServoClosed);
      Serial.println("Dispensed: Chocolate");
      delay(2000);
      Serial.println("Waterpump on");
      delay(100);
      digitalWrite(Water_pump, LOW);
      Serial.println("Waterpump delay");
      for (int i = 10; i > 0; i--) {
      Serial.print("Countdown: ");
      Serial.println(i);
      delay(1000); 
      }
      Serial.println("Waterpump off");
      delay(100);
      digitalWrite(Water_pump, HIGH);
      break;
    case 2:
      Caramel_Coffee.write(CaramelCoffeePosition);
      delay(2000);
      Caramel_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Caramel Coffee");
      delay(2000);
      Serial.println("Waterpump angle");
      Waterpump_Angle.write(45);
      delay(1000);
      Serial.println("Waterpump on");
      delay(100);
      digitalWrite(Water_pump, LOW);
      Serial.println("Waterpump delay");
      for (int i = 10; i > 0; i--) {
      Serial.print("Countdown: ");
      Serial.println(i);
      delay(1000);
      }
      Serial.println("Waterpump off");
      delay(100);
      digitalWrite(Water_pump, HIGH);
      delay(1000);
      Waterpump_Angle.write(WaterpumpAngle);
      delay(100);
      break;
  }
  lcd.clear();
  lcd.print(" Your Coffee is");
  lcd.setCursor(0, 1);
  lcd.print("     Ready!");
  delay(6000);
  lcd.clear();
  lcd.print("     Thanks!");
  for (int i = 0; i < 25; i++) { 
    lcd.setCursor(7, 1); 
    lcd.write(byte(0)); 
    lcd.setCursor(8, 1); 
    lcd.write(byte(0)); 
    lcd.setCursor(9, 1); 
    lcd.write(byte(0)); 
    delay(800); 
    lcd.setCursor(7, 1); 
    lcd.print(" "); 
    lcd.setCursor(8, 1); 
    lcd.print(" "); 
    lcd.setCursor(9, 1); 
    lcd.print(" "); 
    delay(800);  
  }
  DisplaySelection();
}
void DisplaySelection() {
  lcd.clear();
  lcd.print("Select Coffee:");
  lcd.setCursor(0, 1);
  lcd.print(CoffeeVarieties[selectedIndex]);
}
void moveToTable(int table) {
  Serial.print("Moving to Table ");
  Serial.println(table);
  
  uint16_t r, g, b, c;
  float blueRatio, redRatio, yellowRatio, greenIntensity, pinkIntensity;
  
  while (true) {
    int leftSensor = digitalRead(IR_LEFT);
    int rightSensor = digitalRead(IR_RIGHT);

    tcs.getRawData(&r, &g, &b, &c);

    blueRatio = (float)b / (r + g + b);
    redRatio = (float)r / (r + g + b);
    yellowRatio = (float)(r + g) / (r + g + b);
    greenIntensity = (float)g / c;
    pinkIntensity = (float)(r - g + b) / c; 
    
    if (table == 1 && blueRatio > 0.33) { // Table 1 is blue

    Serial.print("Red: "); Serial.print(r);
    Serial.print(" Green: "); Serial.print(g);
    Serial.print(" Blue: "); Serial.print(b);
    Serial.print(" Clear (Intensity): "); Serial.println(c);
    
    Serial.println("Blue color detected!");
    Serial.println("STOP");
    coffeeDelivered = true;
    digitalWrite(Left_Forward, LOW);
    digitalWrite(Right_Reverse, LOW);
    delay(10000); // kuha ng kape

    Serial.println("Blue color detected! Table 1 reached.");
    break;
    }
    if (table == 2 && yellowRatio > 0.85) { 
    
    Serial.print("Red: "); Serial.print(r);
    Serial.print(" Green: "); Serial.print(g);
    Serial.print(" Blue: "); Serial.print(b);
    Serial.print(" Clear (Intensity): "); Serial.println(c);
    
    Serial.println("Yellow color detected!");
    Serial.println("STOP");
    coffeeDelivered = true;
    digitalWrite(Left_Forward, LOW);
    digitalWrite(Right_Reverse, LOW);
    delay(10000); // kuha ng kape
    Serial.println("Yellow color detected! Table 2 reached.");
    break;
    }
    if (table == 3 && greenIntensity > 0.45) {

    Serial.print("Red: "); Serial.print(r);
    Serial.print(" Green: "); Serial.print(g);
    Serial.print(" Blue: "); Serial.print(b);
    Serial.print(" Clear (Intensity): "); Serial.println(c);
    
    Serial.println("Green color detected!");
    Serial.println("STOP");
    coffeeDelivered = true;
    digitalWrite(Left_Forward, LOW);
    digitalWrite(Right_Reverse, LOW);
    delay(10000); // kuha ng kape

    Serial.println("Green color detected! Table 3 reached.");
    break;
    }
    if (table == 4 && pinkIntensity > 0.68) { 

    Serial.print("Red: "); Serial.print(r);
    Serial.print(" Green: "); Serial.print(g);
    Serial.print(" Blue: "); Serial.print(b);
    Serial.print(" Clear (Intensity): "); Serial.println(c);
    
    Serial.println("Pink color detected!");
    Serial.println("STOP");
    coffeeDelivered = true;
    digitalWrite(Left_Forward, LOW);
    digitalWrite(Right_Reverse, LOW);
    delay(10000); // kuha ng kape

    Serial.println("Green color detected! Table 3 reached.");
    break;
    }
     // Line following
    else if (leftSensor == LOW && rightSensor == LOW) { 
      Serial.println("Forward");
      delay(100);
      digitalWrite(Left_Forward, HIGH);
      digitalWrite(Right_Reverse, HIGH);
    } 
    else if (leftSensor == HIGH && rightSensor == LOW) { 
      Serial.println("Turn right");
      delay(100);
      digitalWrite(Left_Forward, HIGH);
      digitalWrite(Right_Reverse, LOW);
    } 
    else if (leftSensor == LOW && rightSensor == HIGH) { 
      Serial.println("Turn left");
      delay(100);
      digitalWrite(Left_Forward, LOW);
      digitalWrite(Right_Reverse, HIGH);
    } 
    else { 
      // Both sensors detect white → Stop
      digitalWrite(Left_Forward, LOW);    
      digitalWrite(Right_Reverse, LOW);
    }
    
    delay(100); 
  }
}
  void moveToHomePosition() {
    Serial.println("Returning to home position (Red)");
    uint16_t r, g, b, c;
    float orangeIntensity;
    // Move back to original position
    while (true) {
    int leftSensor = digitalRead(IR_LEFT);
    int rightSensor = digitalRead(IR_RIGHT);
    
    tcs.getRawData(&r, &g, &b, &c);
    orangeIntensity = (float)(r + g - b) / c;

    // Check if red (original position) is detected
    if (orangeIntensity > 0.75) { 
      Serial.println("Orange color detected! Returned to original position.");
      
      // Stop movement
      digitalWrite(Left_Forward, LOW);
      digitalWrite(Right_Reverse, LOW);
      break;
    }

    // Follow the line back
    if (leftSensor == LOW && rightSensor == LOW) { 
      Serial.println("Moving back - Forward");
      digitalWrite(Left_Forward, HIGH);
      digitalWrite(Right_Reverse, HIGH);
    } 
    else if (leftSensor == HIGH && rightSensor == LOW) { 
      Serial.println("Moving back - Turn right");
      digitalWrite(Left_Forward, HIGH);
      digitalWrite(Right_Reverse, LOW);
    } 
    else if (leftSensor == LOW && rightSensor == HIGH) { 
      Serial.println("Moving back - Turn left");
      digitalWrite(Left_Forward, LOW);
      digitalWrite(Right_Reverse, HIGH);
    } 
    else {
      Serial.println("Moving back - Stop");
      digitalWrite(Left_Forward, LOW);    
      digitalWrite(Right_Reverse, LOW);
    }
    
    delay(100); 
  }
  Serial.println("Original position");
}

/* Opening Screen */
void OpeningScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Brew Coffee");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(2000); 
  DisplaySelection();
}
void processOrders() {
    if (orderQueue.empty()) {
        return;
    }
    std::sort(orderQueue.begin(), orderQueue.end(), [](const Order &a, const Order &b) {
        return a.table < b.table;
    });
    Order firstOrder = orderQueue.front();
    orderQueue.erase(orderQueue.begin());

    serveCoffee(firstOrder.coffeeType, firstOrder.table);
    moveToTable(firstOrder.table);

    if (!orderQueue.empty() && orderQueue.front().table == 2) {
        Order secondOrder = orderQueue.front();
        orderQueue.erase(orderQueue.begin());

        serveCoffee(secondOrder.coffeeType, secondOrder.table);
        moveToTable(secondOrder.table);
    }
        moveToHomePosition();
}
// Handle root page 
void handleRoot() {
  String html = "<html><head><style>";
  html += "body {font-family: 'Roboto', sans-serif; text-align: center; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center; ";
  html += "background-image: url('https://images.unsplash.com/photo-1500989145603-8e7ef71d639e?q=80&w=2076&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D'); "; 
  html += "background-size: cover; background-position: center; background-repeat: no-repeat;}"; 
  html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: rgba(215, 204, 200, 0.8); border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5);}"; 
  html += ".title {font-size: 80px; margin-top: 20px;}";
  html += ".smart {color: #000000;}";
  html += ".brew {color: #8D493A;}"; 
  html += "p {color: #000000; margin: 40px 0; font-size: 40px;}";
  html += "select {font-size: 40px; padding: 5px; border-radius: 5px; width: 100%; max-width: 400px; margin: 0 auto; height: 90px;}"; 
  html += "option {font-size: 24px;}";
  html += "button {font-size: 40px; margin-top: 20px; padding: 5px; border-radius: 5px; width: 100%; max-width: 400px; background-color: #4E342E; color: white; border: none; display: block; margin: 20px auto; height: 90px; ";
  html += "transition: background-color 0.3s ease, transform 0.2s ease, box-shadow 0.3s ease;}"; 
  html += "button:hover {background-color: #3E2925; transform: scale(1.05); box-shadow: 0 0 15px rgba(0,0,0,0.6);}"; 
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<div class='title'><span class='smart'>Smart</span><span class='brew'>Brew</span></div>";
  html += "<p>Select your drink:</p>";
  html += "<select id='coffeeSelection'>";
  for (int i = 0; i < 3; i++) {
    html += "<option value=\"" + String(i) + "\">" + String(CoffeeVarieties[i]) + "</option>";
  }
  html += "</select>";
  html += "<br>";
  html += "<button onclick='orderCoffee()'>Order</button>";
  html += "</div>";
  html += "<script>";
  html += "function orderCoffee() {";
  html += "  var selection = document.getElementById('coffeeSelection').value;";
  html += "  window.location.href = '/order?coffee=' + selection;";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}
void handleOrder() {
    String coffeeIndexStr = server.arg("coffee");
    String tableIndexStr = server.arg("table");
    int coffeeIndex = coffeeIndexStr.toInt();
    int tableIndex = tableIndexStr.toInt();

    if (coffeeIndex >= 0 && coffeeIndex < 3) {
        if (tableIndex == 0) {
            // Show table selection UI (unchanged)
            String html = "<html><head><style>";
            html += "body {font-family: 'Roboto', sans-serif; text-align: center; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center; ";
            html += "background-image: url('https://images.unsplash.com/photo-1495474472287-4d71bcdd2085'); ";
            html += "background-size: cover; background-position: center; background-repeat: no-repeat;}"; 
            html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: rgba(215, 204, 200, 0.8); border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5);}"; 
            html += ".title {font-size: 50px; margin-top: 20px;}"; 
            html += ".smart {color: #000000;}"; 
            html += ".brew {color: #B17457;}"; 
            html += "p {color: #000000; margin: 20px 0; font-size: 24px;}"; 
            html += "a.table-link {color: #8D493A; text-decoration: none; font-weight: bold; padding: 10px 20px; background-color: #F8C794; border-radius: 5px; display: inline-block; margin: 10px; transition: background-color 0.3s ease, color 0.3s ease;}"; 
            html += "a.table-link:hover {background-color: #D7CCC8; color: #4E342E; transform: scale(1.05); box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);}"; 
            html += "</style></head><body>";
            html += "<div class='container'>";
            html += "<div class='title'>Select your table:</div>";
            for (int i = 1; i <= 4; i++) {
                html += "<p><a class='table-link' href=\"/order?coffee=" + String(coffeeIndex) + "&table=" + String(i) + "\" onclick=\"showLoading()\">Table " + String(i) + "</a></p>";
            }
            html += "</div>";
            html += "<div id='loading' style='display:none;position:fixed;top:0;left:0;width:100%;height:100%;background-color:rgba(255,255,255,0.8);text-align:center;'>";
            html += "<div style='position:relative;top:50%;transform:translateY(-50%);'><h2>Loading...</h2></div>";
            html += "</div>";
            html += "<script>";
            html += "function showLoading() { document.getElementById('loading').style.display = 'block'; }";
            html += "</script>";
            html += "</body></html>";
            server.send(200, "text/html", html);
        } else {
            // Add order to queue
            orderQueue.push_back({tableIndex, coffeeIndex});

            // Confirmation UI
            String html = "<html><head><style>";
            html += "body {font-family: 'Roboto', sans-serif; text-align: center; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center; ";
            html += "background-image: url('https://images.unsplash.com/photo-1495474472287-4d71bcdd2085'); ";
            html += "background-size: cover; background-position: center; background-repeat: no-repeat;}"; 
            html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: rgba(215, 204, 200, 0.8); border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5);}"; 
            html += ".title {font-size: 50px; margin-top: 20px;}"; 
            html += "p {color: #000000; margin: 20px 0; font-size: 24px;}"; 
            html += "button {font-size: 24px; padding: 10px 20px; border-radius: 5px; background-color: #4E342E; color: white; border: none; cursor: pointer; transition: background-color 0.3s ease, transform 0.2s ease;}"; 
            html += "button:hover {background-color: #3E2925; transform: scale(1.05);}"; 
            html += "</style></head><body>";
            html += "<div class='container'>";
            html += "<div class='title'>Order Confirmed</div>";
            html += "<p>Your coffee is on the way to Table " + String(tableIndex) + "!</p>";
            html += "<button onclick='goHome()'>Home</button>";
            html += "</div>";
            html += "<script>";
            html += "function goHome() { window.location.href = '/'; }";
            html += "</script>";
            html += "</body></html>";

            server.send(200, "text/html", html);
        }
    } else {
        server.send(400, "text/plain", "Invalid coffee selection");
    }
}
void handleStatus() {
  if (coffeeDelivered) {
    server.send(200, "text/plain", "delivered");
    coffeeDelivered = false; // Reset after reporting
  } else {
    server.send(200, "text/plain", "pending");
  }
}
