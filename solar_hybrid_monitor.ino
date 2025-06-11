#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions
#define CLK 2
#define DT 5
#define SW 4
#define LDR_PIN A0

int lastCLKState;
int currentCLKState;
int soc = 50;  // Start at 50%
bool dieselOn = false;

unsigned long lastPrintTime = 0;
unsigned long lastDischargeTime = 0;
const unsigned long dischargeInterval = 3000;  // Simulate discharge every 3s

// Custom I2C pins (for ESP8266)
#define SDA_PIN 0
#define SCL_PIN 14

void setup() {
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);

  Wire.begin(SDA_PIN, SCL_PIN);

  lastCLKState = digitalRead(CLK);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(1000);
}

void loop() {
  currentCLKState = digitalRead(CLK);

  // Detect rotary encoder rotation
  if (currentCLKState != lastCLKState && currentCLKState == LOW) {
    if (digitalRead(DT) != currentCLKState) {
      soc += 1;  // Turned clockwise
    } else {
      soc -= 1;  // Turned counter-clockwise
    }

    if (soc > 100) soc = 100;
    if (soc < 0) soc = 0;
  }

  lastCLKState = currentCLKState;

  // Reset with push button
  if (digitalRead(SW) == LOW) {
    soc = 50;
    delay(200);  // Debounce
  }

  // Simulate battery discharge
  if (millis() - lastDischargeTime >= dischargeInterval) {
    soc--;
    if (soc < 0) soc = 0;
    lastDischargeTime = millis();
  }

  // Diesel hysteresis logic
  if (soc < 25) {
    dieselOn = true;
  } else if (soc >= 40) {
    dieselOn = false;
  }

  // Display every 500ms
  if (millis() - lastPrintTime >= 500) {
    int lightVal = analogRead(LDR_PIN);
    float watts = (1023 - lightVal) / 10.23;

    Serial.print("Light Power: ");
    Serial.print(watts);
    Serial.print("W\t");

    Serial.print("Battery SoC: ");
    Serial.print(soc);
    Serial.print("%\t");

    Serial.print("Diesel: ");
    Serial.println(dieselOn ? "ON" : "OFF");

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Team 100");

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("Solar Power: ");
    display.print(watts, 1);
    display.println(" W");

    display.print("Battery SoC: ");
    display.print(soc);
    display.println(" %");

    display.print("Status: ");
    display.println(dieselOn ? "Diesel ON" : "Solar Only");

    display.display();
    lastPrintTime = millis();
  }
}