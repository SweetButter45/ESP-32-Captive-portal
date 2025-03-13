# ESP-32-Captive-portal
Project Description
This project is a Wi-Fi captive portal setup for an ESP32. It creates a fake access point (AP) that masquerades as a router to prompt users to input a Wi-Fi password. Upon receiving the correct password, the ESP32 connects to the real Wi-Fi router, stores the valid password in EEPROM, and disconnects the fake AP. The system also allows users to change the SSID (network name) of the ESP32 access point and view or clear the saved Wi-Fi passwords.

# Installation
1. Install the Arduino IDE: Download and install the Arduino IDE.
2. Install ESP32 Support:
    * Go to File > Preferences and add the following URL to Additional Boards Manager URLs:
      `https://dl.espressif.com/dl/package_esp32_index.json`
    * Go to Tools > Board > Boards Manager, search for ESP32, and install it.
3. Download the Code:
* Clone or [download the ZIP](https://github.com/SweetButter45/ESP-32-Captive-portal/archive/refs/heads/main.zip) of this repository.
* Extract the ZIP file if downloaded.
4. Open the Code:
* Open `main file.ino` in Arduino IDE.
5. Select ESP32 Board:
* Go to Tools > Board, and select the correct ESP32 model (e.g., ESP32 Dev Module).
6. Install Libraries:
  * Go to Sketch > Include Library > Manage Libraries.
  * Search and install the following libraries:
  * Adafruit SSD1306
  * Adafruit GFX and other required libraries from the Library Manager.
7. Upload to ESP32:
  * Upload the code to your ESP32 board.

# The system uses:

* Adafruit SSD1306 OLED for display feedback.
* Web Server (ESP32 WebServer library) to serve HTML pages.
* DNS Server to catch all DNS requests while connected to the fake AP.

# Endpoints and Their Purpose
* / - Index Page:-
                    Displays the main page of the captive portal with a message telling the user that the router's firmware is out of date and prompting them to enter the Wi-Fi password.
                    Contains a form to enter the Wi-Fi password.
  
* /post - Submit Password:- 
                    This endpoint is triggered when the user submits a password via the form on the index page.
                    The entered password is checked against the real router’s Wi-Fi. If correct, the ESP32 connects to the router and stops the fake AP. The password is stored in EEPROM.
                    If incorrect, the page reloads with a message asking the user to try again.
  
* /passwords - Display Saved Passwords:- 
                    Displays a list of all saved Wi-Fi passwords that have been submitted and stored in EEPROM.
  
* /clear - Clear Saved Passwords:-
                    Clears all saved passwords from EEPROM and resets the password list.
                    After clearing, the user is redirected back to the index page.
  
* /ssid - Change SSID:-
                    Displays a page where the user can change the SSID (Wi-Fi network name) of the ESP32’s fake access point.
                    Once the SSID is changed, the ESP32 restarts, and the user is prompted to reconnect to the new SSID.
  
* /postSSID - Submit New SSID
                    This endpoint is triggered when the user submits the new SSID.
                    It updates the SSID in EEPROM and restarts the ESP32 to apply the changes.

# Hardware Setup
* ESP32: Acts as both the Wi-Fi access point (AP) and connects to the real router.
* OLED Display (SSD1306): Used for basic feedback (boot messages, status).
* LED: The built-in LED will blink 5 times after a password is successfully posted to provide visual feedback.

#Additional Features
*Captive Portal: The ESP32 acts as a captive portal, where users who connect to the fake AP are redirected to the portal page.
*Password Storage: Saved passwords are stored in the ESP32's EEPROM for later use.
*Wi-Fi Connectivity: After a correct password is provided, the ESP32 connects to the real Wi-Fi network and shuts down the fake AP.

