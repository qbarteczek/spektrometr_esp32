# spektrometr_esp32 🚀

Bilingual documentation for the **spektrometr_esp32** project.  
*Dwujęzyczna dokumentacja dla projektu spektrometr_esp32.*

---

## 📁 Project Overview (O Projekcie)
- **Microcontroller:** ESP32
- **IDE Support:** Arduino IDE & VS Code PlatformIO

---

## 🔌 Hardware Wiring (Połączenia Sprzętowe)
For connection mappings, refer to:  
*Rozkład pinów znajdziesz w:*  
👉 **[WIRING.md](file://WIRING.md)**

---

## 🚀 How to Compile & Flash (Instrukcja Uruchomienia)

### Method 1: VS Code PlatformIO (Recommended)
1. Open this directory in VS Code with PlatformIO extension installed.
2. Select your environment configuration.
3. Click **Build & Upload**.

### Method 2: Arduino IDE
1. Open the `.ino` file in the sketch folder.
2. Add support for ESP32 in Arduino Boards Manager.
3. Install the libraries listed below.
4. Upload to your board!

### Required Libraries (Biblioteki):
- `adafruit/Adafruit Unified Sensor`
- `adafruit/Adafruit AS7341`
- `bodmer/TFT_eSPI`

---
## 🛑 Safety Rules (Zasady Bezpieczeństwa)
Before connecting power, read:  
*Przed podłączeniem zasilania przeczytaj:*  
👉 **[DOS_AND_DONTS.md](file://DOS_AND_DONTS.md)**
