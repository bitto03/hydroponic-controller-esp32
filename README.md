# hydroponic-controller-esp32
# IoT-Based Smart Hydroponic System using ESP32

Developed by **Basirul Hasin**

This is an **IoT-powered hydroponic system** built with an ESP32 microcontroller. It automates and remotely controls **lighting**, **fan**, and **water pump** operations based on preset cycles, while also monitoring temperature and humidity. The system hosts its own **Wi-Fi dashboard** for real-time control and countdown tracking.


## Key Features

-  Live Temperature & Humidity Monitoring (via DHT11 sensor)
-  Web-based dashboard hosted on ESP32 (no internet required)
-  Manual control of 3 relays (Light, Fan, Water Pump)
-  Automated relay cycle management for smart plant care
-  Real-time countdown timers shown on dashboard
-  Works offline as Wi-Fi Access Point (no router needed)


##  Use Case

Ideal for **urban farming**, **automated plant growing**, and **DIY hydroponic kits**. This system ensures that plants receive:

- Sufficient **light**
- Proper **air circulation**
- Timely **water supply**

All without user intervention.


##  Hardware Components

| Component       | Quantity |
|----------------|----------|
| ESP32 Dev Board | 1        |
| DHT11 Sensor    | 1        |
| Relay Module (3-Channel) | 1        |
| 12V Water Pump  | 1        |
| 5V DC Fan       | 1        |
| Grow Light (LED) | 1       |
| Jumper Wires    | As needed |


##  Relay Cycle Timing

| Device | ON Duration | OFF Duration |
|--------|-------------|--------------|
| **Light** | 2 hours     | 30 minutes   |
| **Fan**   | 10 minutes  | 1.5 hours    |
| **Pump**  | 15 seconds  | 2 hours      |

*All cycles are handled using the `Ticker` library in non-blocking mode.*


##  How to Use
1. **Power up the ESP32** – it starts in AP (Access Point) mode:
   - SSID: `ESP32_Dashboard`
   - Password: `12345678`
2. **Connect from your phone or laptop.**
3. Open browser → Visit: `http://192.168.4.1`
4. Use the dashboard to:
   - View live temp/humidity
   - Turn devices on/off manually
   - Watch countdowns during auto cycles


## To-Do / Future Upgrades

- 📱 Mobile App Integration (Flutter or Android)
- 🌐 Remote control via Blynk / MQTT / Firebase
- 📊 Real-time sensor graphing (using charts.js)
- 💾 Store relay status in Preferences or EEPROM
- 🔋 Solar power integration


## 🙌 About  

This project is made with care by **[BHB Enterprise]** # Basirul Hasin.
empowering smart agriculture and eco-automation systems in Bangladesh and beyond.
