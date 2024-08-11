#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AS7341.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <SD.h>

// Pin definitions
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  17
#define SD_CS    4
#define BUTTON_PIN 13

// Threshold for significant data change
#define THRESHOLD 50

// TFT display and AS7341 sensor objects
TFT_eSPI tft = TFT_eSPI();       // TFT display object
Adafruit_AS7341 as7341 = Adafruit_AS7341();  // AS7341 sensor object

// Variables for tracking data and state
uint16_t previousValues[6];  // Array to store previous spectral values (6 channels)
bool saveData = false;        // Flag to trigger data saving

void setup() {
  Serial.begin(115200);

  // Initialize TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Initialize AS7341 sensor
  if (!as7341.begin()) {
    Serial.println("Cannot find AS7341 sensor!");
    while (1);
  }

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card not detected!");
    while (1);
  }

  // Set button pin as input with internal pull-up resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Clear previous values array
  memset(previousValues, 0, sizeof(previousValues));
}

void loop() {
  // Read data from the AS7341 sensor
  as7341.startReading();
  delay(100);  // Wait for data

  bool dataChanged = false;

  // Check if data has changed significantly
  for (int i = 0; i < 6; i++) {
    uint16_t intensity = as7341.readChannel((as7341_adc_channel_t)(AS7341_ADC_CHANNEL_0 + i));
    if (abs(intensity - previousValues[i]) > THRESHOLD) {
      dataChanged = true;
      previousValues[i] = intensity;
    }
  }

  // Update TFT display only if data has changed
  if (dataChanged) {
    displaySpectrum();
  }

  // Check button press to save data
  if (digitalRead(BUTTON_PIN) == LOW) {
    saveData = true;
    delay(300);  // Debounce delay
  }

  // Save screenshot if requested
  if (saveData) {
    saveScreenshot();
    saveData = false;
  }

  // Enter light sleep mode after 10 seconds of inactivity
  static unsigned long lastActivityTime = millis();
  if (dataChanged) {
    lastActivityTime = millis();
  } else if (millis() - lastActivityTime > 10000) {  // 10 seconds of inactivity
    esp_light_sleep_start();  // Enter light sleep mode
    lastActivityTime = millis();  // Update activity time after waking up
  }

  delay(500);  // Delay before the next loop
}

void displaySpectrum() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.println("Spectrum:");

  // Display the spectral data as colored bars
  for (int i = 0; i < 6; i++) {
    uint16_t intensity = previousValues[i];
    uint16_t color = tft.color565(255 - i * 40, i * 40, 255 - i * 40);
    tft.fillRect(20 + (i * 20), 240 - intensity / 10, 10, intensity / 10, color);
  }
}

void saveScreenshot() {
  const uint16_t w = tft.width(), h = tft.height();
  uint16_t buffer[w];  // Static array to hold pixel data

  // Open file for writing
  File file = SD.open("/screenshot.bmp", FILE_WRITE);
  if (!file) {
    Serial.println("Cannot open file for writing!");
    return;
  }

  // Save each line of the screen to the file
  for (uint16_t y = 0; y < h; y++) {
    tft.readRect(0, y, w, 1, buffer);
    file.write((uint8_t*)buffer, sizeof(buffer));
  }

  file.close();
  Serial.println("Screenshot saved.");
}
