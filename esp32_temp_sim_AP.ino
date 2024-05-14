#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <U8g2lib.h>

const char* ssid = "ESP_temp_sim"; // Change this to your desired SSID
const char* password = "12345678"; // Change this to your desired password

WebServer server(80);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // ESP32 Example

String html_page = "<!DOCTYPE html><html><head><title>ESP32 Web Interface</title><style>body {background-color: #222;color: #fff;font-family: Arial, sans-serif;}h1 {color: #ff7f00;}input[type=\"text\"], input[type=\"submit\"] {padding: 10px;margin: 5px;border: none;border-radius: 5px;background-color: #444;color: #fff;}input[type=\"submit\"] {background-color: #ff7f00;cursor: pointer;}</style></head><body><h1>Enter Temperature:</h1><form action=\"/update\" method=\"get\"><input type=\"text\" name=\"temperature\" placeholder=\"Enter temperature...\"><input type=\"submit\" value=\"Submit\"></form></body></html>";

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.sendBuffer();

  // Set up ESP32 as an access point
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  // Route for root / web page
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", html_page);
  });

  // Route to handle updating temperature
  server.on("/update", HTTP_GET, []() {
    String tempStr = server.arg("temperature");
    float temp = tempStr.toFloat(); // Convert string to float

    // Validate input
    if (tempStr.length() > 0 && !isnan(temp) && temp >= 0) {
      // Format temperature
      if (temp >= 100) {
        updateDisplay(tempStr, true); // Draw a bit more to the left
      } else {
        updateDisplay(tempStr, false); // Draw at the original position
      }
    } else {
      // Invalid input, display error message
      updateDisplay("Invalid input", false);
    }
    server.send(200, "text/html", html_page); // Send back the same HTML page
  });

  // Start server
  server.begin();
}

void loop() {
  server.handleClient();
}

void updateDisplay(String tempStr, bool drawLeft) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_helvB08_tr);
  u8g2.drawStr(3, 11, "Mode:");
  u8g2.drawStr(50, 24, "ORAL");
  u8g2.drawStr(3, 37, "Temperature:");
  u8g2.setFont(u8g2_font_profont29_tr);

  // Format temperature with one decimal place
  tempStr = String(tempStr.toFloat(), 1);

  if (drawLeft) {
    u8g2.drawStr(5, 60, tempStr.c_str());
  } else {
    u8g2.drawStr(15, 60, tempStr.c_str());
  }

  u8g2.drawStr(81, 60, " C");
  u8g2.drawEllipse(91, 44, 3, 3);
  u8g2.sendBuffer();
}
