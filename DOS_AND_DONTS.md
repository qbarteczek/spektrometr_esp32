# 🛑 Dos & Don'ts (Zasady i Ostrzeżenia)

Important rules and troubleshooting tips for working with the ESP32/ESP8266 and sensors.  
*Ważne zasady bezpieczeństwa i wskazówki dotyczące pracy z mikrokontrolerami ESP i czujnikami.*

---

## ✅ DOs (Zalecane)

*   **Experiment with the code!** If anything goes wrong, you can discard changes and restore clean code from the Git repository.
    *   *Eksperymentuj z kodem! Jeśli coś popsujesz, zawsze możesz przywrócić czystą wersję z Git.*
*   **Use USB power** for flashing and development. It is the safest option.
    *   *Używaj zasilania USB do wgrywania kodu. To najbezpieczniejsza opcja.*
*   **Keep the Serial Monitor open** (baud rate 115200 or as defined). This is where the board logs debug logs.
    *   *Sprawdzaj monitor portu szeregowego. Tam płytka wypisuje ewentualne błędy i logi diagnostyczne.*
*   **Use Level Shifters** if you connect I2C/SPI devices designed for 5V signals to ESP (which expects 3.3V levels).
    *   *Używaj konwerterów poziomów logicznych, jeśli podłączasz moduły 5V pod piny sygnałowe (które tolerują tylko 3.3V).*

---

## ❌ DON'Ts (Niezalecane)

*   **Never connect 5V to 3.3V pins!** The microcontroller is not 5V tolerant on its GPIOs or power input pins. You will damage the chip.
    *   *Nigdy nie podłączaj 5V pod piny 3.3V lub GPIO! Uszkodzisz mikrokontroler.*
*   **Do not disconnect the USB cable during flashing.** While it rarely bricks the device, it might corrupt the flash memory.
    *   *Nie odłączaj kabla USB podczas wgrywania kodu. Może to uszkodzić zawartość pamięci flash.*
*   **Do not overload GPIO pins.** Microcontroller pins are rated for very low currents (max 12-20mA). Never drive high-current loads like motors, solenoids, or heaters directly from the pins; use transistors, MOSFETs, or relays instead.
    *   *Nie przeciążaj pinów GPIO. Piny dają radę małym LED-om, ale dla silników, przekaźników czy grzałek musisz użyć tranzystorów lub modułów pośredniczących.*
