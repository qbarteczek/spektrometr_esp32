1. Schemat Połączeń
Komponenty:
ESP32: mikrokontroler
AMS AS7341: czujnik spektralny (I2C)
Wyświetlacz TFT ILI9341: SPI
Karta SD: SPI (można współdzielić interfejs SPI z TFT)
Przycisk: do zapisu danych lub zmiany trybu
Włącznik: do zasilania urządzenia
Połączenia:
AMS AS7341 (I2C)

SDA (Czujnik) -> GPIO21 (ESP32)
SCL (Czujnik) -> GPIO22 (ESP32)
VCC -> 3.3V (ESP32)
GND -> GND (ESP32)
Wyświetlacz TFT ILI9341 (SPI)

SCK (Wyświetlacz) -> GPIO18 (ESP32)
MOSI (Wyświetlacz) -> GPIO23 (ESP32)
MISO (Wyświetlacz) -> GPIO19 (ESP32) (jeśli używane)
CS (Wyświetlacz) -> GPIO5 (ESP32)
DC (Wyświetlacz) -> GPIO16 (ESP32)
RESET (Wyświetlacz) -> GPIO17 (ESP32)
VCC -> 3.3V (ESP32)
GND -> GND (ESP32)
Karta SD (SPI)

SCK (SD) -> GPIO18 (ESP32) (współdzielone z TFT)
MOSI (SD) -> GPIO23 (ESP32) (współdzielone z TFT)
MISO (SD) -> GPIO19 (ESP32) (współdzielone z TFT)
CS (SD) -> GPIO4 (ESP32)
VCC -> 3.3V (ESP32)
GND -> GND (ESP32)
Przycisk

Jedna nóżka -> GPIO13 (ESP32)
Druga nóżka -> GND (ESP32)
Włącznik

Przełącznik zasilania dla całego układu.
