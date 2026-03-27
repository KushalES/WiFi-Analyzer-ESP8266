#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

String html;

void scanNetworks() {
  int n = WiFi.scanNetworks();

  // HTML START
  html = "<html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<style>";
  html += "body { font-family: Arial; background:#111; color:#fff; text-align:center; }";
  html += "h2 { color:#00ffcc; }";
  html += "table { width:90%; margin:auto; border-collapse: collapse; }";
  html += "th, td { padding:10px; border-bottom:1px solid #444; }";
  html += "th { background:#222; }";
  html += "tr:hover { background:#333; }";
  html += "</style></head><body>";

  html += "<h2>📡 WiFi Analyzer</h2>";
  html += "<table><tr><th>SSID</th><th>Signal</th><th>Channel</th><th>Security</th></tr>";

  int channelCount[14] = {0};

  for (int i = 0; i < n; i++) {

    //  Security Detection
    String security;
    switch (WiFi.encryptionType(i)) {
      case ENC_TYPE_NONE: security = "OPEN"; break;
      case ENC_TYPE_WEP: security = "WEP"; break;
      case ENC_TYPE_TKIP: security = "WPA"; break;
      case ENC_TYPE_CCMP: security = "WPA2"; break;
      case ENC_TYPE_AUTO: security = "AUTO"; break;
      default: security = "UNKNOWN";
    }

    //  Signal Strength
    int rssi = WiFi.RSSI(i);
    String color;

    if (rssi > -50) color = "green";
    else if (rssi > -70) color = "orange";
    else color = "red";

    int width = map(rssi, -100, -30, 10, 100);

    //  Table Row
    html += "<tr><td>" + WiFi.SSID(i) + "</td>";

    html += "<td>";
    html += "<div style='width:100px; background:#333; border-radius:5px;'>";
    html += "<div style='width:" + String(width) + "%; background:" + color + "; padding:5px; border-radius:5px;'>";
    html += String(rssi);
    html += "</div></div>";
    html += "</td>";

    html += "<td>" + String(WiFi.channel(i)) + "</td>";
    html += "<td>" + security + "</td></tr>";

    channelCount[WiFi.channel(i)]++;
  }

  html += "</table>";

  // Channel Recommendation
  int bestChannel = 1;
  for (int i = 1; i <= 13; i++) {
    if (channelCount[i] < channelCount[bestChannel]) {
      bestChannel = i;
    }
  }

  html += "<h3>Recommended Channel: " + String(bestChannel) + "</h3>";

  // HTML END
  html += "</body></html>";
}

void handleRoot() {
  scanNetworks();
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting AP Mode...");

  // Create WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP("WiFi-Analyzer", "12345678", 6, false, 4);

  Serial.println("Access Point Started");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Start Web Server
  server.on("/", handleRoot);
  server.begin();

  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}