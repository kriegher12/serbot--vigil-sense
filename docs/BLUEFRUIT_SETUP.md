# Bluefruit Connect App Setup

## Overview
The Adafruit Bluefruit Connect app can control your Arduino UNO R4 WiFi car via **Bluetooth Low Energy (BLE)**. 

**⚠️ IMPORTANT:** For Bluetooth control, use `arduino_motor_control_ble.ino` (not the WiFi version)!

## Option 1: Using Bluefruit Connect App (WiFi Mode)

### Step 1: Download the App
- **iOS**: [App Store - Bluefruit Connect](https://apps.apple.com/app/adafruit-bluefruit-le-connect/id830125974)
- **Android**: [Google Play - Bluefruit Connect](https://play.google.com/store/apps/details?id=com.adafruit.bluefruitconnect)

### Step 2: Connect Arduino to WiFi
1. Upload `arduino_motor_control.ino` to your Arduino
2. Open Serial Monitor (9600 baud)
3. Wait for WiFi connection and note the IP address

### Step 3: Configure Bluefruit App
1. Open Bluefruit Connect app
2. Tap the **WiFi** icon (or go to Settings → Connection Type → WiFi)
3. Enter the Arduino's IP address (e.g., `192.168.1.100`)
4. Tap **Connect**

### Step 4: Control the Car
- Use the **Controller** tab in the app
- Or use the **Color Picker** to send commands
- Customize controls in the app settings

## Option 2: Web Browser Control (Easier)

If Bluefruit app doesn't work with WiFi, use a web browser:

1. Connect your phone/computer to the same WiFi network
2. Open browser: `http://[ARDUINO_IP_ADDRESS]`
3. Use the on-screen control buttons

## Option 3: Custom Control Interface

You can also create a custom mobile-friendly interface or use:
- **Blynk App**: https://blynk.io/
- **MIT App Inventor**: Create your own app
- **Web-based joystick**: Use HTML5 joystick libraries

## Troubleshooting

### Bluefruit App Not Connecting
- Ensure Arduino and phone are on the same WiFi network
- Check IP address in Serial Monitor
- Try using a web browser first to verify connection
- Bluefruit app may primarily support BLE, not WiFi

### Alternative: Use Web Interface
The code includes a built-in web interface that works on any device with a browser. This is the most reliable method for WiFi control.

## Direct URL Commands

You can also send commands via URL (useful for automation):
```
http://[IP]/forward
http://[IP]/backward
http://[IP]/left
http://[IP]/right
http://[IP]/stop
http://[IP]/speedup
http://[IP]/speeddown
```

## Notes
- Bluefruit Connect app is primarily designed for Bluetooth Low Energy (BLE)
- For WiFi control, a web browser or custom app is more reliable
- The built-in web interface works on all devices (phones, tablets, computers)

