# ESP-32-Captive-portal
Project Description
This project is a Wi-Fi captive portal setup for an ESP32. It creates a fake access point (AP) that masquerades as a router to prompt users to input a Wi-Fi password. Upon receiving the correct password, the ESP32 connects to the real Wi-Fi router, stores the valid password in EEPROM, and disconnects the fake AP. The system also allows users to change the SSID (network name) of the ESP32 access point and view or clear the saved Wi-Fi passwords.

The system uses:

* Adafruit SSD1306 OLED for display feedback.
* Web Server (ESP32 WebServer library) to serve HTML pages.
* DNS Server to catch all DNS requests while connected to the fake AP.
