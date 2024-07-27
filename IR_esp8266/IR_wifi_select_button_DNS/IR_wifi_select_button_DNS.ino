#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Access Point credentials
const char* apSSID = "ESP8266_AP"; // SSID for the AP
const char* apPassword = "password123"; // Password for the AP (minimum 8 characters)

// IR LED pin
const uint16_t irLedPin = 4; // D5 on NodeMCU, corresponds to GPIO14
IRsend irsend(irLedPin);

// Create an instance of the server
ESP8266WebServer server(80);

// Define your IR codes
const uint32_t irCode1 = 0xCF8976; // Example IR code for button 1
const uint32_t irCode2 = 0xAFAF10; // Example IR code for button 2
const uint32_t irCode3 = 0xB1B2C3; // Example IR code for button 3

void handleRoot() {
  // Start Wi-Fi scan
  int n = WiFi.scanNetworks();
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body><h1>Control IR LED & Wi-Fi Setup</h1>";
  
  // Add form to select SSID and input password
  html += "<form action='/connect' method='POST'>";
  html += "SSID: <select name='ssid'>";
  
  // Populate dropdown with scanned networks
  for (int i = 0; i < n; ++i) {
    html += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>";
  }
  
  html += "</select><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Connect to Wi-Fi'>";
  html += "</form><br>";
  
  // Add IR control buttons
  html += "<button onclick=\"sendIR('1')\">Send IR Signal 1</button><br>";
  html += "<button onclick=\"sendIR('2')\">Send IR Signal 2</button><br>";
  html += "<button onclick=\"sendIR('3')\">Send IR Signal 3</button><br>";
  html += "<script>function sendIR(code) { fetch('/sendIR?code=' + code); }</script>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleSendIR() {
  String code = server.arg("code");
  uint32_t irCode;

  if (code == "1") {
    irCode = irCode1;
  } else if (code == "2") {
    irCode = irCode2;
  } else if (code == "3") {
    irCode = irCode3;
  } else {
    server.send(400, "text/plain", "Invalid code");
    return;
  }

  irsend.sendNEC(irCode, 32);
  server.send(200, "text/plain", "IR signal sent for code " + code);
}

void handleConnect() {
  String newSSID = server.arg("ssid");
  String newPassword = server.arg("password");

  if (newSSID.length() > 0) {
    Serial.println("Connecting to new SSID: " + newSSID);
    WiFi.begin(newSSID.c_str(), newPassword.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      server.send(200, "text/html", "<html><body><h1>Connected to Wi-Fi</h1><p>IP address: " + WiFi.localIP().toString() + "</p></body></html>");
      if (MDNS.begin("mynodemcu")) {
      Serial.println("mDNS responder started");
      } else {
      Serial.println("Error setting up mDNS responder!");
      }
    } else {
      Serial.println("Failed to connect");
      server.send(200, "text/html", "<html><body><h1>Failed to connect</h1><p>Please try again.</p></body></html>");
    }
  } else {
    server.send(400, "text/plain", "Invalid SSID or password");
    }
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

  // Set up Access Point
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/sendIR", handleSendIR);
  server.on("/connect", HTTP_POST, handleConnect);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}

