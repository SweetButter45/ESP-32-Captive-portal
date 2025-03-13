# ESP-32-Captive-portal
Project Description
This project is a Wi-Fi captive portal setup for an ESP32. It creates a fake access point (AP) that masquerades as a router to prompt users to input a Wi-Fi password. Upon receiving the correct password, the ESP32 connects to the real Wi-Fi router, stores the valid password in EEPROM, and disconnects the fake AP. The system also allows users to change the SSID (network name) of the ESP32 access point and view or clear the saved Wi-Fi passwords.

The system uses:

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
