#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

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

const int Next = 13;
const int Select = 12;

const int Water_pump = 33; 
//const int SupplyArduino = 32;

/* IR Sensors */
//const int IR_Left = 5; 
//const int IR_Right = 18; 

/* Wheels */
const int Left_Reverse = 26;
const int Left_Forward = 25;
const int Right_Forward = 14;
const int Right_Reverse = 27;

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

IPAddress local_IP(192, 168, 23, 222);  
IPAddress gateway(192, 168, 23, 1);     
IPAddress subnet(255, 255, 255, 0);     
IPAddress primaryDNS(8, 8, 8, 8);       
IPAddress secondaryDNS(8, 8, 4, 4); 

WebServer server(80);

void DisplaySelection();
void moveToTable(int table);
void OpeningScreen();
void serveCoffee(int index, int table);
void handleRoot();
void handleOrder();

void setup() { 
  Cup.write(ServoClosed);
  Black_Coffee.write(ServoClosed);
  Chocolate.write(ServoClosed); 
  Caramel_Coffee.write(ServoClosed);
  Waterpump_Angle.write(WaterpumpAngle);
  digitalWrite(Left_Forward, LOW);
  digitalWrite(Right_Forward, LOW);
  digitalWrite(Water_pump, HIGH);
  //digitalWrite(SupplyArduino, HIGH);

  Serial.begin(115200);

    WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
    //WiFi.begin(ssid, password);

  Serial.println("System Initialized");
  
  Serial.println("Initializing LCD...");
  lcd.init();
  lcd.createChar(0, heart);
  lcd.backlight();
  
  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);
  
  //pinMode(IR_Left, INPUT);
  //pinMode(IR_Right, INPUT);

  /* Relay Input pins */
  pinMode(Water_pump, OUTPUT);
  //pinMode(SupplyArduino, OUTPUT);
  pinMode(Left_Forward, OUTPUT);
  pinMode(Right_Forward, OUTPUT);
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
  Cup.write(0);
  delay(2000);
  switch (index) {
    case 0:
      Black_Coffee.write(BlackCoffeePosition);
      delay(2000);
      Black_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Black Coffee");
      delay(2000);
      Waterpump_Angle.write(135);
      delay(1000);
      digitalWrite(Water_pump, LOW);
      delay(8000);
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
      digitalWrite(Water_pump, LOW);
      delay(8000);
      digitalWrite(Water_pump, HIGH);
      break;
    case 2:
      Caramel_Coffee.write(CaramelCoffeePosition);
      delay(2000);
      Caramel_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Caramel Coffee");
      delay(2000);
      Waterpump_Angle.write(45);
      delay(1000);
      digitalWrite(Water_pump, LOW);
      delay(8000);
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

  if (table == 1) {
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Forward, HIGH);
    delay(5000); 
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Forward, LOW); 
    delay(1000); 
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Forward, HIGH);
    delay(5000);
  } else if (table == 2) {
    digitalWrite(Left_Forward, LOW);
    digitalWrite(Right_Forward, LOW);
    delay(1000); 
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Forward, HIGH);
    delay(5000);
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Reverse, HIGH); 
    delay(1000);
    digitalWrite(Left_Forward, HIGH);
    digitalWrite(Right_Forward, HIGH);
    delay(5000);
  }
  digitalWrite(Left_Forward, LOW);
  digitalWrite(Right_Forward, LOW);
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
      moveToTable(tableIndex);
      serveCoffee(coffeeIndex, tableIndex);
      html += "</script></body></html>";
      server.send(200, "text/html", html);
    } else {
      String html = "<html><head><style>";
      html += "body {font-family: 'Roboto', sans-serif; text-align: center; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center; ";
      html += "background-image: url('https://images.unsplash.com/photo-1495474472287-4d71bcdd2085'); "; 
      html += "background-size: cover; background-position: center; background-repeat: no-repeat;}";
      html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: rgba(215, 204, 200, 0.8); border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5); text-align: center;}";
      html += "h1 {font-size: 50px; margin-top: 20px; color: #4E342E;}";
      html += ".home-button {font-size: 30px; margin-top: 20px; padding: 10px 20px; border-radius: 5px; background-color: #4E342E; color: white; border: none; transition: background-color 0.3s ease, transform 0.3s ease;}";
      html += ".home-button:hover {background-color: #3E2925; transform: scale(1.05); box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);}";
      html += "</style></head><body>";
      html += "<div class='container'>";
      html += "<h1>Your coffee is being prepared for Table " + String(tableIndex) + "!</h1>";
      html += "<button class='home-button' onclick='goHome()'>Home</button>";
      html += "</div>";
      html += "<script>";
      html += "function goHome() { window.location.href = '/'; }";
      html += "</script></body></html>";
      //serveCoffee(coffeeIndex, tableIndex);
      server.send(200, "text/html", html);
    }
  } else {
    server.send(400, "text/html", "<html><body><h1>Invalid selection!</h1></body></html>");
  }
}
