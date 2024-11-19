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


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

/* LCD Connection */
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* Custom LCD Heart Display */
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

/* Next and Select Button */
const int Next = 13;
const int Select = 12;

/* Relay */
const int Water_pump = 33;      

/* Wheels */
const int Left_Motor = 14;
const int Right_Motor = 27;

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
const int SugarPosition = 90;
const int CupPosition = 90;
const int ServoClosed = 0;

/* WiFi credentials */
const char* ssid = "Kodic";
const char* password = "kodicpogi21";

/* Web server */
AsyncWebServer server(80);

// Function declarations
void DisplaySelection();
void OpeningScreen();
void serveCoffee(int index, int table);

void setup() {
  /* Display */
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.backlight();

  /* Buttons */
  pinMode(Next, INPUT_PULLUP);
  pinMode(Select, INPUT_PULLUP);

  /* Relay Input pins */
  pinMode(Water_pump, OUTPUT);
  pinMode(Left_Motor, OUTPUT);
  pinMode(Right_Motor, OUTPUT);

  /* Servo */
  Cup.attach(17);
  Black_Coffee.attach(16);
  Caramel_Coffee.attach(4);
  Chocolate.attach(2);
  
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("System Initialized");

  // Initialize Wi-Fi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  // Keep retrying until connected
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) { // Retry for 30 seconds
    delay(1000);
    Serial.print(".");
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi. Continuing without network.");
    return;
  }

  // Initialize web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head>";
    html += "<style>";
    html += "body { background-color: #D2B48C; font-family: Arial, sans-serif; color: #000000; margin: 0; height: 100%; display: flex; flex-direction: column; justify-content: center; align-items: center; text-align: center; }";
    html += "h1 { color: #000000; font-size: 5em; margin-bottom: 20px; }";
    html += ".highlight { color: #8B4513; }";
    html += "form { background-color: #FFFFFF; padding: 100px; border-radius: 8px; display: flex; flex-direction: column; align-items: center; justify-content: center; }";
    html += "label { font-size: 2.2em; margin-bottom: 10px; }";
    html += "select { font-size: 2.2em; padding: 10px; margin-bottom: 20px; width: 400px; }";
    html += "input[type='submit'] { font-size: 2.2em; color: #FFFFFF; background-color: #8B4513; padding: 10px 20px; margin-top: 10px; width: 400px; cursor: pointer; }";
    html += ".button { font-size: 2.2em; padding: 10px 20px; margin: 10px; cursor: pointer; width: 400px; }";
    html += "</style>";
    html += "</head><body>";
    html += "<h1>Smart<span class='highlight'>Brew</span></h1>";
    html += "<form action=\"/table\" method=\"get\">";
    html += "<label for=\"type\">Select Coffee:</label>";
    html += "<select name=\"type\">";
    html += "<option value=\"Black Coffee\">Black Coffee</option>";
    html += "<option value=\"Chocolate\">Chocolate</option>";
    html += "<option value=\"Caramel\">Caramel</option>";
    html += "</select>";
    html += "<input type=\"submit\" value=\"Next\">";
    html += "</form>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/table", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("type")) {
      String type = request->getParam("type")->value();
      String html = "<html><head>";
      html += "<style>";
      html += "body { background-color: #D2B48C; font-family: Arial, sans-serif; color: #000000; margin: 0; height: 100%; display: flex; flex-direction: column; justify-content: center; align-items: center; text-align: center; }";
      html += "h1 { color: #000000; font-size: 5em; margin-bottom: 20px; }";
      html += ".highlight { color: #8B4513; }";
      html += "h2 { font-size: 3.2em; margin-bottom: 20px; }";
      html += ".button { font-size: 2.2em; padding: 10px 20px; margin: 10px; cursor: pointer; width: 200px; }";
      html += "</style>";
      html += "</head><body>";
      html += "<h1>Smart<span class='highlight'>Brew</span></h1>";
      html += "<h2>Select Your Table</h2>";
      html += "<form action=\"/order\" method=\"get\">";
      html += "<input type=\"hidden\" name=\"type\" value=\"" + type + "\">";
      html += "<button class=\"button\" type=\"submit\" name=\"table\" value=\"1\">Table 1</button>";
      html += "<button class=\"button\" type=\"submit\" name=\"table\" value=\"2\">Table 2</button>";
      html += "<button class=\"button\" type=\"submit\" name=\"table\" value=\"3\">Table 3</button>";
      html += "<button class=\"button\" type=\"submit\" name=\"table\" value=\"4\">Table 4</button>";
      html += "</form>";
      html += "</body></html>";
      request->send(200, "text/html", html);
    } else {
      request->send(400, "text/plain", "Missing coffee type parameter");
    }
  });

  server.on("/order", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("type") && request->hasParam("table")) {
      String type = request->getParam("type")->value();
      int table = request->getParam("table")->value().toInt();
      if (type == "Black Coffee") {
        selectedIndex = 0;
      } else if (type == "Chocolate") {
        selectedIndex = 1;
      } else if (type == "Caramel") {
        selectedIndex = 2;
      } else {
        request->send(400, "text/plain", "Invalid coffee type");
        return;
      }
      serveCoffee(selectedIndex, table);
      String html = "<html><head>";
      html += "<meta http-equiv=\"refresh\" content=\"3; url=/\" />";
      html += "</head><body>";
      html += "<h1>Order received: " + type + " for Table " + String(table) + "</h1>";
      html += "<p>Redirecting to home...</p>";
      html += "</body></html>";
      request->send(200, "text/html", html);
    } else {
      request->send(400, "text/plain", "Missing coffee type or table parameter");
    }
  });

  server.begin();

  DisplaySelection();
  OpeningScreen();
}

void loop() {
  digitalWrite(Water_pump, LOW);
  delay(5000);
  
  if (digitalRead(Next) == LOW) {
    selectedIndex = (selectedIndex + 1) % 3; // Coffee selection loop
    DisplaySelection();
    Serial.print("Coffee selection changed: ");
    Serial.println(CoffeeVarieties[selectedIndex]);
    delay(300);
  }

  if (digitalRead(Select) == LOW) {
    Serial.print("Selected coffee: ");
    Serial.println(CoffeeVarieties[selectedIndex]);
    serveCoffee(selectedIndex, 0); // Default to table 0 for manual selections
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
  /* Loading Screen */
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

void serveCoffee(int index, int table){
  /* Processing Display */
  lcd.clear();
  lcd.print("  Processing");
  lcd.setCursor(0, 1);
  lcd.print(" Please wait");

  Serial.println("Processing coffee...");

  /* Dispensing */
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

  /* Heart */
  lcd.clear();
  lcd.print("   Thanks!");
  lcd.setCursor(7, 1);
  lcd.write(byte(0));
  lcd.setCursor(8, 1);
  lcd.write(byte(0));
  lcd.setCursor(9, 1);
  lcd.write(byte(0));
  delay(2000);

  /* Return to Display selection */
  DisplaySelection();

  delay(1000);
  digitalWrite(Left_Motor, LOW);
  digitalWrite(Right_Motor, LOW);
  delay(20000);
}
