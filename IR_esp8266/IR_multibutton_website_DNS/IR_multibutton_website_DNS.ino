#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wi-Fi credentials
const char* ssid = "RSHome";
const char* password = "0Per@ti0n@2022";

// Static IP configuration

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
  String html = "<html><body><h1>Control IR LED</h1>";
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

  irsend.sendNEC(irCode, 32); // Replace with your IR code and protocol
  server.send(200, "text/plain", "IR signal sent for code " + code);
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  
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

    // Start mDNS
    if (MDNS.begin("mynodemcu")) {
      Serial.println("mDNS responder started");
    } else {
      Serial.println("Error setting up mDNS responder!");
    }
  } else {
    Serial.println("Failed to connect to Wi-Fi");
  }

  // Start the server
  server.on("/", handleRoot);
  server.on("/sendIR", handleSendIR);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  MDNS.update();
}
