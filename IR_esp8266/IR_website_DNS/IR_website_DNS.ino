#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// Wi-Fi credentials
const char* ssid = "RSHome";
const char* password = "0Per@ti0n@2022";

// IR LED pin
const uint16_t irLedPin = 4; // D5 on NodeMCU, corresponds to GPIO14
IRsend irsend(irLedPin);

// Create an instance of the server
ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><body><h1>Control IR LED</h1>";
  html += "<button onclick=\"sendIR()\">Send IR Signal</button>";
  html += "<script>function sendIR() { fetch('/sendIR'); }</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSendIR() {
  irsend.sendNEC(0xCF8976, 32); // Replace with your IR code
  server.send(200, "text/plain", "IR signal sent");
}

void setup() {
  Serial.begin(115200);
  irsend.begin();

  // Set static IP address
//  if (!WiFi.config(local_IP, gateway, subnet)) {
  //  Serial.println("STA Failed to configure");
 // }

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
    if (MDNS.begin("mynodemcu")) { // Replace with your desired hostname
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