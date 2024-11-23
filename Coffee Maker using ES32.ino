#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>

/* LCD Connection */
LiquidCrystal_I2C lcd(0x27, 16, 2); // Try 0x27 or 0x3F based on your module

/* Custom LCD Heart Display */
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

/* Next and Select Button */
const int Next = 13;
const int Select = 12;

/* Relay */
const int Water_pump = 33;      

/* Wheels */
const int Left_Forward = 14;
const int Left_Reverse = 27;
const int Right_Forward = 26;
const int Right_Reverse = 25;

/* Coffee Variety */
const char* CoffeeVarieties[] = {"Black Coffee", "Chocolate", "Caramel"};
int selectedIndex = 0;

/* Servo motors for dropping */
Servo Cup;
Servo Black_Coffee;
Servo Caramel_Coffee;
Servo Chocolate;

/* Position of Servo */
const int BlackCoffeePosition = 90;
const int CaramelCoffeePosition = 90;
const int ChocolatePosition = 90;
const int CupPosition = 90;
const int ServoClosed = 0;

/* WiFi credentials */
const char* ssid = "Kodic";
const char* password = "kodicpogi21";

/* Web server */
WebServer server(80);

/* Function declarations */
void DisplaySelection();
void OpeningScreen();
void serveCoffee(int index, int table);
void handleRoot();
void handleOrder();

/* Setup */
void setup() {
  /* Initialize serial communication */
  Serial.begin(115200);
  Serial.println("System Initialized");

  /* Initialize LCD */
  Serial.println("Initializing LCD...");
  lcd.init();
  lcd.createChar(0, heart);
  lcd.backlight();

  /* Buttons */
  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);

  /* Relay Input pins */
  pinMode(Water_pump, OUTPUT);
  pinMode(Left_Forward, OUTPUT);
  pinMode(Right_Forward, OUTPUT);

  /* Servo */
  Cup.attach(17);
  Black_Coffee.attach(16);
  Caramel_Coffee.attach(4);
  Chocolate.attach(2);

  OpeningScreen();

  /* Initialize WiFi */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  /* Initialize web server */
  server.on("/", handleRoot);
  server.on("/order", handleOrder);
  server.begin();
  Serial.println("Web server started");
}

/* Loop */
void loop() {
  /* Reset servo positions after a small delay */
  Cup.write(ServoClosed);
  Black_Coffee.write(ServoClosed);
  Chocolate.write(ServoClosed); 
  Caramel_Coffee.write(ServoClosed);
  digitalWrite(Left_Forward, HIGH);
  digitalWrite(Right_Forward, HIGH);
  digitalWrite(Water_pump, HIGH);

  /* Handle button inputs */
  if (digitalRead(Next) == LOW) { // Next button
    selectedIndex = (selectedIndex + 1) % 3; // Rotate through coffee options
    DisplaySelection();
    delay(200); // debounce delay
  }

  if (digitalRead(Select) == LOW) { // Select button
    serveCoffee(selectedIndex, 1); // Change the table number as needed
    delay(200); // debounce delay
  }

  /* Handle web server */
  server.handleClient();
}

/* Serve Coffee */
void serveCoffee(int index, int table) {
  /* Processing Display */
  lcd.clear();
  lcd.print("Processing");
  lcd.setCursor(0, 1);
  lcd.print("Please wait");
  for (int i = 0; i < 4; i++) { // Number of dots to display 
    delay(500); // Delay in milliseconds 
    lcd.setCursor(12 + i, 1); // Adjust the cursor position for each dot 
    lcd.print(".");
  }

  Serial.println("Processing coffee...");

  /* Cup Dropper */
  delay(800);
  Cup.write(180);
  delay(400);
  Cup.write(0);
  delay(2000);

  /* Serve selected coffee */
  switch (index) {
    case 0:
      Black_Coffee.write(BlackCoffeePosition);
      delay(500);
      Black_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Black Coffee");
      break;
    case 1:
      Chocolate.write(ChocolatePosition);
      delay(500);
      Chocolate.write(ServoClosed);
      Serial.println("Dispensed: Chocolate");
      break;
    case 2:
      Caramel_Coffee.write(CaramelCoffeePosition);
      delay(500);
      Caramel_Coffee.write(ServoClosed);
      Serial.println("Dispensed: Caramel Coffee");
      break;
  }
  delay(6000);
  digitalWrite(Water_pump, LOW);
  lcd.clear();
  lcd.print(" Your Coffee is");
  lcd.setCursor(0, 1);
  lcd.print("     Ready!");
  delay(6000);

  /* Heart */
  lcd.clear();
  lcd.print("     Thanks!");
  for (int i = 0; i < 5; i++) { // Number of blink cycles 
    lcd.setCursor(7, 1); 
    lcd.write(byte(0)); 
    lcd.setCursor(8, 1); 
    lcd.write(byte(0)); 
    lcd.setCursor(9, 1); 
    lcd.write(byte(0)); 
    delay(800); // Delay in milliseconds // Clear the hearts to create the blink effect 
    lcd.setCursor(7, 1); 
    lcd.print(" "); 
    lcd.setCursor(8, 1); 
    lcd.print(" "); 
    lcd.setCursor(9, 1); 
    lcd.print(" "); 
    delay(800); // Delay in milliseconds 
  }
  delay(1000);
  digitalWrite(Left_Forward, LOW);
  digitalWrite(Right_Forward, LOW);
  delay(20000);

  /* Return to Display selection */
  DisplaySelection();
}

/* Display Selection */
void DisplaySelection() {
  lcd.clear();
  lcd.print("Select Coffee:");
  lcd.setCursor(0, 1);
  lcd.print(CoffeeVarieties[selectedIndex]);
}

/* Opening Screen */
void OpeningScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Brew Coffee");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  delay(2000); // Wait for 2 seconds
  DisplaySelection();
}

/* Handle root page */
void handleRoot() {
  String html = "<html><head><style>";
  html += "body {font-family: 'Roboto', sans-serif; text-align: center; background-color: #B17457; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center;}"; // Centering container
  html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: #D7CCC8; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5);}"; // Added max-width for better control
  html += ".title {font-size: 80px; margin-top: 20px;}";
  html += ".smart {color: #000000;}";
  html += ".brew {color: #8D493A;}"; // Corrected the color code syntax
  html += "p {color: #000000; margin: 40px 0; font-size: 40px;}";
  html += "select {font-size: 40px; padding: 5px; border-radius: 5px; width: 100%; max-width: 400px; margin: 0 auto; height: 90px;}"; // Adjusted height
  html += "option {font-size: 24px;}"; // Adjusted font size of options
  html += "button {font-size: 40px; margin-top: 20px; padding: 5px; border-radius: 5px; width: 100%; max-width: 400px; background-color: #4E342E; color: white; border: none; display: block; margin: 20px auto; height: 90px;}"; // Adjusted height
  html += "button:hover {background-color: #3E2925;}";
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

/* Handle coffee order */
/*void handleOrder() {
  String coffeeIndexStr = server.arg("coffee");
  String tableIndexStr = server.arg("table");
  int coffeeIndex = coffeeIndexStr.toInt();
  int tableIndex = tableIndexStr.toInt();

  if (coffeeIndex >= 0 && coffeeIndex < 3) {
    if (tableIndex == 0) {
      // Only display table selection after coffee is chosen
      String html = "<html><body>";
      html += "<p>Select your table:</p>";
      for (int i = 1; i <= 4; i++) {
        html += "<p><a href=\"/order?coffee=" + String(coffeeIndex) + "&table=" + String(i) + "\">Table " + String(i) + "</a></p>";
      }
      html += "</body></html>";
      server.send(200, "text/html", html);
    } else {
      serveCoffee(coffeeIndex, tableIndex);
      server.send(200, "text/html", "<html><body><h1>Your coffee is being prepared for Table " + String(tableIndex) + "!</h1></body></html>");
    }
  } else {
    server.send(400, "text/html", "<html><body><h1>Invalid selection!</h1></body></html>");
  }
}*/
void handleOrder() {
  String coffeeIndexStr = server.arg("coffee");
  String tableIndexStr = server.arg("table");
  int coffeeIndex = coffeeIndexStr.toInt();
  int tableIndex = tableIndexStr.toInt();

  if (coffeeIndex >= 0 && coffeeIndex < 3) {
    if (tableIndex == 0) {
      // Only display table selection after coffee is chosen
      String html = "<html><head><style>";
      html += "body {font-family: 'Roboto', sans-serif; text-align: center; background-color: #F8C794; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center;}";
      html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: #D7CCC8; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5);}";
      html += ".title {font-size: 50px; margin-top: 20px;}";
      html += ".smart {color: #000000;}";
      html += ".brew {color: #B17457;}";
      html += "p {color: #000000; margin: 20px 0; font-size: 24px;}";
      html += "a {color: #8D493A; text-decoration: none; font-weight: bold; padding: 10px 20px; background-color: #F8C794; border-radius: 5px; display: inline-block; margin: 10px;}";
      html += "a:hover {background-color: #D7CCC8;}";
      html += "</style></head><body>";
      html += "<div class='container'>";
      html += "<div class='title'>Select your table:</div>";
      for (int i = 1; i <= 4; i++) {
        html += "<p><a href=\"/order?coffee=" + String(coffeeIndex) + "&table=" + String(i) + "\">Table " + String(i) + "</a></p>";
      }
      html += "</div>";
      html += "</body></html>";
      server.send(200, "text/html", html);
    } else {
      String html = "<html><head><style>";
      html += "body {font-family: 'Roboto', sans-serif; text-align: center; background-color: #F8C794; color: #4E342E; height: 100vh; display: flex; justify-content: center; align-items: center;}";
      html += ".container {margin: auto; padding: 20px; width: 80%; max-width: 600px; background-color: #D7CCC8; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.5); text-align: center;}";
      html += "h1 {font-size: 50px; margin-top: 20px; color: #4E342E;}";
      html += ".home-button {font-size: 30px; margin-top: 20px; padding: 10px 20px; border-radius: 5px; background-color: #4E342E; color: white; border: none;}";
      html += ".home-button:hover {background-color: #3E2925;}";
      html += "</style></head><body>";
      html += "<div class='container'>";
      html += "<h1>Your coffee is being prepared for Table " + String(tableIndex) + "!</h1>";
      html += "<button class='home-button' onclick='goHome()'>Home</button>";
      html += "</div>";
      html += "<script>";
      html += "function goHome() { window.location.href = '/'; }";
      html += "</script></body></html>";
      serveCoffee(coffeeIndex, tableIndex);
      server.send(200, "text/html", html);
    }
  } else {
    server.send(400, "text/html", "<html><body><h1>Invalid selection!</h1></body></html>");
  }
}
