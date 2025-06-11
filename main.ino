// ESP32 Smart Dashboard with Relay Cycles and Live Countdown by BHB Enterprise
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Ticker.h>

// DHT11
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Relay pins
#define RELAY_LIGHT 16
#define RELAY_FAN 17
#define RELAY_PUMP 18

// Relay states
bool relayState[3] = {false, false, false};

// Timer data
Ticker lightCycle, fanCycle, pumpCycle;
Ticker manualTimers[3];
unsigned long manualEndTime[3] = {0, 0, 0};
unsigned long autoNextChange[3] = {0, 0, 0};

// WiFi AP
const char* ssid = "ESP32_Dashboard";
const char* password = "12345678";

// Web server
WebServer server(80);

// Relay control
void setRelay(int relay, bool state) {
  relayState[relay] = state;
  digitalWrite(RELAY_LIGHT + relay, state ? LOW : HIGH);
  if (!state) manualEndTime[relay] = 0;
}

// Auto-cycles
void lightOff();
void lightOn() {
  setRelay(0, true);
  lightCycle.once(7200, lightOff);
  autoNextChange[0] = millis() + 7200 * 1000;
}
void lightOff() {
  setRelay(0, false);
  lightCycle.once(1800, lightOn);
  autoNextChange[0] = millis() + 1800 * 1000;
}

void fanOn();
void fanOff() {
  setRelay(1, false);
  fanCycle.once(5400, fanOn);
  autoNextChange[1] = millis() + 5400 * 1000;
}
void fanOn() {
  setRelay(1, true);
  fanCycle.once(600, fanOff);
  autoNextChange[1] = millis() + 600 * 1000;
}

void pumpOn();
void pumpOff() {
  setRelay(2, false);
  pumpCycle.once(7200, pumpOn);
  autoNextChange[2] = millis() + 7200 * 1000;
}
void pumpOn() {
  setRelay(2, true);
  pumpCycle.once(15, pumpOff);
  autoNextChange[2] = millis() + 15 * 1000;
}

// Manual off
void manualOff(int relay) {
  setRelay(relay, false);
  manualEndTime[relay] = 0;
}

// HTML page
String htmlPage() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  unsigned long now = millis();

  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Dashboard</title><style>";
  html += "body{font-family:sans-serif;text-align:center;background:#1a0000;color:white;}";
  html += "h1{background:#800000;color:white;padding:20px;font-size:28px;}";
  html += ".card{background:#330000;padding:20px;margin:20px;border-radius:15px;box-shadow:0 0 10px rgba(255,255,255,0.1);}";
  html += "button,input{padding:12px 18px;margin:5px;border:none;border-radius:5px;font-size:18px;}";
  html += "form{margin:10px;}";
  html += "h2,h3,p{font-size:20px;}";
  html += "</style></head><body>";
  html += "<h1>🔧 Dev by BHB Enterprise</h1>";

  html += "<div class='card'><h2>Environment</h2>";
  html += "<p> Temp: " + String(t) + " °C</p>";
  html += "<p> Humidity: " + String(h) + " %</p></div>";

  const char* names[] = {"Light", "Fan", "Pump"};

  for (int i = 0; i < 3; i++) {
    html += "<div class='card'>";
    html += "<h3>" + String(names[i]) + " - Status: " + (relayState[i] ? "ON" : "OFF") + "</h3>";
    html += "<form action='/toggle' method='get'>";
    html += "<input type='hidden' name='relay' value='" + String(i) + "'>";
    html += "<button type='submit'>" + String(relayState[i] ? "Turn OFF" : "Turn ON") + "</button></form>";

    html += "<form action='/timer' method='get'>";
    html += "<input type='hidden' name='relay' value='" + String(i) + "'>";
    html += "<input type='number' name='time' placeholder='Time in sec'>";
    html += "<button type='submit'>Start Timer</button></form>";

    if (manualEndTime[i] > 0) {
      long remaining = (manualEndTime[i] - millis()) / 1000;
      if (remaining < 0) remaining = 0;
      html += "<p>⏳ Manual Remaining: <span id='timer" + String(i) + "'>" + String(remaining) + "</span> sec</p>";
    }

    if (autoNextChange[i] > 0) {
      long autoRemaining = (autoNextChange[i] - millis()) / 1000;
      if (autoRemaining < 0) autoRemaining = 0;
      html += "<p> Auto Cycle: Next change in <span id='autotimer" + String(i) + "'>" + String(autoRemaining) + "</span> sec</p>";
    }

    html += "</div>";
  }

  html += "<script>setInterval(() => {";
  html += "for(let i=0;i<3;i++){ "
          "let el=document.getElementById('timer'+i); if(el){ let t=parseInt(el.innerText); if(t>0){ el.innerText=t-1; }}";
  html += "let ae=document.getElementById('autotimer'+i); if(ae){ let a=parseInt(ae.innerText); if(a>0){ ae.innerText=a-1; }}";
  html += "} }, 1000);</script>";

  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleToggle() {
  int index = server.arg("relay").toInt();
  relayState[index] = !relayState[index];
  setRelay(index, relayState[index]);
  manualTimers[index].detach();
  manualEndTime[index] = 0;
  server.send(200, "text/html", htmlPage());
}

void handleTimer() {
  int index = server.arg("relay").toInt();
  int time = server.arg("time").toInt();

  if (time > 0) {
    setRelay(index, true);
    manualTimers[index].detach();
    manualTimers[index].once(time, manualOff, index);
    manualEndTime[index] = millis() + (time * 1000);
  }

  server.send(200, "text/html", htmlPage());
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_PUMP, OUTPUT);
  setRelay(0, false);
  setRelay(1, false);
  setRelay(2, false);

  WiFi.softAP(ssid, password);
  Serial.println("AP started. Connect to: " + String(ssid));

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/timer", handleTimer);
  server.begin();

  lightOn();
  fanOff();
  pumpOff();
}

void loop() {
  server.handleClient();
}
