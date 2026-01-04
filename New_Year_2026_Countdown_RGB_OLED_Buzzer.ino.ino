#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define SWITCH_PIN 7
#define BUZZER_PIN 8

#define RED_PIN   9
#define GREEN_PIN 10
#define BLUE_PIN  11

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== Common Anode RGB =====
void setRGB(int r, int g, int b) {
  analogWrite(RED_PIN, 255 - r);
  analogWrite(GREEN_PIN, 255 - g);
  analogWrite(BLUE_PIN, 255 - b);
}

// ===== Variables =====
bool counting = false;
bool celebrating = false;

unsigned long lastMillis = 0;
unsigned long blinkMillis = 0;

int countdown = 10;
int colorIndex = 0;

// Celebration RGB colors
int colors[6][3] = {
  {255,0,0}, {0,255,0}, {0,0,255},
  {255,255,0}, {0,255,255}, {255,0,255}
};

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);

  setRGB(0,0,0);
  drawStartScreen();
}

void loop() {
  unsigned long currentMillis = millis();

  // ===== Start Countdown =====
  if (digitalRead(SWITCH_PIN) == LOW && !counting && !celebrating) {
    while (digitalRead(SWITCH_PIN) == LOW);
    delay(200);

    countdown = 10;
    counting = true;
    lastMillis = currentMillis;
    display.clearDisplay();
  }

  // ===== Countdown Logic =====
  if (counting && countdown >= 0 && currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;

    // ---- RGB based on countdown ----
    switch (countdown) {
      case 10: setRGB(255,0,0); break;
      case 9:  setRGB(0,255,0); break;
      case 8:  setRGB(0,0,255); break;
      case 7:  setRGB(255,255,0); break;
      case 6:  setRGB(0,255,255); break;
      case 5:  setRGB(255,0,255); break;
      case 4:  setRGB(255,120,0); break;
      case 3:  setRGB(128,0,255); break;
      case 2:  setRGB(255,20,147); break;
      case 1:  setRGB(0,180,255); break;
      case 0:  setRGB(255,255,255); break;
    }

    // ---- Display number ----
    display.clearDisplay();
    display.setTextSize(5);
    display.setCursor(45, 15);
    display.println(countdown);
    display.display();

    tone(BUZZER_PIN, 1000, 120);

    countdown--;

    // ===== When countdown finishes =====
    if (countdown < 0) {
      counting = false;
      celebrating = true;

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(12, 8);
      display.println("NEW YEAR");
      display.setTextSize(3);
      display.setCursor(28, 35);
      display.println("2026");
      display.display();

      tone(BUZZER_PIN, 1500, 600);
      blinkMillis = currentMillis;
      colorIndex = 0;
    }
  }

  // ===== Celebration RGB Blink =====
  if (celebrating && currentMillis - blinkMillis >= 150) {
    setRGB(colors[colorIndex][0],
           colors[colorIndex][1],
           colors[colorIndex][2]);

    colorIndex++;
    if (colorIndex >= 6) colorIndex = 0;
    blinkMillis = currentMillis;
  }

  // ===== Reset =====
  if (celebrating && digitalRead(SWITCH_PIN) == LOW) {
    while (digitalRead(SWITCH_PIN) == LOW);
    delay(200);

    celebrating = false;
    setRGB(0,0,0);
    drawStartScreen();
  }
}

void drawStartScreen() {
  display.clearDisplay();

  display.drawCircle(64, 20, 15, WHITE);
  display.fillCircle(56, 15, 2, WHITE);
  display.fillCircle(72, 15, 2, WHITE);
  display.drawLine(56, 25, 72, 25, WHITE);

  display.setTextSize(1);
  display.setCursor(35, 50);
  display.println("PRESS SWITCH");
  display.display();
}
