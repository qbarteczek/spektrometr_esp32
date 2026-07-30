#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AS7341.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <SD.h>

// Definicje pinów
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  17
#define SD_CS    4
#define BUTTON_PIN 13

// Próg dla znaczącej zmiany danych
#define THRESHOLD 50

// Obiekty wyświetlacza TFT i czujnika AS7341
TFT_eSPI tft = TFT_eSPI();       // Wyświetlacz TFT
Adafruit_AS7341 as7341 = Adafruit_AS7341();  // Czujnik AS7341

// Zmienne do śledzenia danych i stanu
uint16_t previousValues[6];  // Tablica przechowująca poprzednie wartości widma (6 kanałów)
bool saveData = false;       // Flaga wyzwalająca zapis danych

void setup() {
  Serial.begin(115200);

  // Inicjalizacja wyświetlacza TFT
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Inicjalizacja czujnika AS7341
  if (!as7341.begin()) {
    Serial.println("Nie mozna znalezc czujnika AS7341!");
    while (1);
  }

  // Inicjalizacja karty SD
  if (!SD.begin(SD_CS)) {
    Serial.println("Nie wykryto karty SD!");
    while (1);
  }

  // Ustawienie pinu przycisku jako wejście z wewnętrznym rezystorem podciągającym (pull-up)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Czyszczenie tablicy poprzednich wartości
  memset(previousValues, 0, sizeof(previousValues));
}

void loop() {
  // Odczyt danych z czujnika AS7341
  as7341.startReading();
  delay(100);  // Oczekiwanie na dane

  bool dataChanged = false;

  // Sprawdzenie, czy dane uległy znacznej zmianie
  for (int i = 0; i < 6; i++) {
    uint16_t intensity = as7341.readChannel((as7341_adc_channel_t)(AS7341_ADC_CHANNEL_0 + i));
    if (abs(intensity - previousValues[i]) > THRESHOLD) {
      dataChanged = true;
      previousValues[i] = intensity;
    }
  }

  // Aktualizacja wyświetlacza TFT tylko w przypadku zmiany danych
  if (dataChanged) {
    displaySpectrum();
  }

  // Sprawdzenie naciśnięcia przycisku w celu zapisu danych
  if (digitalRead(BUTTON_PIN) == LOW) {
    saveData = true;
    delay(300);  // Opóźnienie zapobiegające drganiom styków (debounce)
  }

  // Zapisanie zrzutu ekranu, jeśli zażądano
  if (saveData) {
    saveScreenshot();
    saveData = false;
  }

  // Przejście w tryb uśpienia (light sleep) po 10 sekundach bezczynności
  static unsigned long lastActivityTime = millis();
  if (dataChanged) {
    lastActivityTime = millis();
  } else if (millis() - lastActivityTime > 10000) {  // 10 sekund bezczynności
    // Konfiguracja wybudzania przyciskiem (LOW na BUTTON_PIN)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, 0);
    esp_light_sleep_start();  // Przejście w tryb uśpienia
    lastActivityTime = millis();  // Aktualizacja czasu po wybudzeniu
  }

  delay(500);  // Opóźnienie przed kolejną pętlą
}

void displaySpectrum() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.println("Widmo:");

  // Wyświetlanie danych spektralnych w postaci kolorowych pasków
  for (int i = 0; i < 6; i++) {
    uint16_t intensity = previousValues[i];
    uint16_t color = tft.color565(255 - i * 40, i * 40, 255 - i * 40);
    tft.fillRect(20 + (i * 20), 240 - intensity / 10, 10, intensity / 10, color);
  }
}

void saveScreenshot() {
  const uint16_t w = tft.width(), h = tft.height();
  uint16_t* buffer = (uint16_t*) malloc(w * sizeof(uint16_t));
  if (!buffer) {
    Serial.println("Brak RAM na zrzut ekranu!");
    return;
  }

  File file = SD.open("/screenshot.bmp", FILE_WRITE);
  if (!file) {
    Serial.println("Nie mozna otworzyc pliku do zapisu!");
    free(buffer);
    return;
  }

  // 1. Zapis nagłówka BMP (24-bitowy, formatowanie DIB)
  uint32_t rowSize = (w * 3 + 3) & ~3; // Wyrównanie do 4 bajtów
  uint32_t fileSize = 54 + rowSize * h;

  unsigned char bmpFileHeader[14] = {
    'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0
  };
  bmpFileHeader[2] = (unsigned char)(fileSize);
  bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
  bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
  bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

  unsigned char bmpInfoHeader[40] = {
    40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0
  };
  bmpInfoHeader[4] = (unsigned char)(w);
  bmpInfoHeader[5] = (unsigned char)(w >> 8);
  bmpInfoHeader[6] = (unsigned char)(w >> 16);
  bmpInfoHeader[7] = (unsigned char)(w >> 24);
  bmpInfoHeader[8] = (unsigned char)(h);
  bmpInfoHeader[9] = (unsigned char)(h >> 8);
  bmpInfoHeader[10] = (unsigned char)(h >> 16);
  bmpInfoHeader[11] = (unsigned char)(h >> 24);

  file.write(bmpFileHeader, 14);
  file.write(bmpInfoHeader, 40);

  // 2. Odczyt pikseli (od dołu do góry ze względu na naturę plików BMP)
  uint8_t* rowBuffer = (uint8_t*) malloc(rowSize);
  for (int y = h - 1; y >= 0; y--) {
    tft.readRect(0, y, w, 1, buffer);
    int ptr = 0;
    for (int x = 0; x < w; x++) {
      uint16_t color = buffer[x];
      // Dekodowanie z RGB565 na BGR888 (wymóg BMP)
      uint8_t r = (color & 0xF800) >> 8;
      uint8_t g = (color & 0x07E0) >> 3;
      uint8_t b = (color & 0x001F) << 3;
      rowBuffer[ptr++] = b;
      rowBuffer[ptr++] = g;
      rowBuffer[ptr++] = r;
    }
    file.write(rowBuffer, rowSize);
  }

  file.close();
  free(buffer);
  free(rowBuffer);
  Serial.println("Zrzut ekranu zapisany jako prawidlowy plik BMP.");
}
