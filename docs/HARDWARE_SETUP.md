# VigilSense Hardware Setup Guide

## Hardware Connections

### Arduino UNO R4 WiFi Pin Connections

| Sensor | Pin | Type | Notes |
|--------|-----|------|-------|
| DHT11 | D4 | Digital | Temperature & Humidity |
| Gas Sensor (MQ) | A0 | Analog | Gas level detection |
| Vibration Sensor | A3 | Analog | Vibration detection |
| Flame Sensor | D7 | Digital | Flame detection (0/1) |
| Microphone | D9 | Digital | Sound detection |

### Connections Diagram

```
Arduino UNO R4 WiFi
├── D4  → DHT11 Data
├── A0  → Gas Sensor (MQ) Output
├── A3  → Vibration Sensor Output
├── D7  → Flame Sensor Digital Output
├── D9  → Microphone Digital Output
└── USB → Raspberry Pi (for Serial Communication)
```

## Arduino Setup

1. **Install Arduino IDE** or use online editor
2. **Install DHT Library:**
   - Open Arduino IDE
   - Go to Sketch → Include Library → Manage Libraries
   - Search for "DHT sensor library" by Adafruit
   - Install "DHT sensor library" and "Adafruit Unified Sensor"

3. **Upload Code:**
   - Open `arduino_code.ino` in Arduino IDE
   - Select board: Tools → Board → Arduino UNO
   - Select port: Tools → Port → (your Arduino port)
   - Click Upload

4. **Verify Serial Output:**
   - Open Serial Monitor (Tools → Serial Monitor)
   - Set baud rate to 9600
   - You should see JSON data like:
     ```json
     {"temp":25.5,"humidity":50.0,"gas":300,"vibration":0,"flame":0,"sound":0}
     ```

## Raspberry Pi Setup

### 1. Install Dependencies

```bash
pip3 install --user -r requirements.txt
```

### 2. Enable Camera (Pi Camera v3)

```bash
sudo raspi-config
# Navigate to: Interface Options → Camera → Enable
# Reboot: sudo reboot
```

### 3. Check Arduino Connection

```bash
# List serial devices
ls -l /dev/ttyACM* /dev/ttyUSB*

# Test serial connection
python3 -c "import serial; print(serial.tools.list_ports.comports())"
```

### 4. Run Dashboard

```bash
cd vigil_sense_dashboard
python3 app.py
```

## Troubleshooting

### Arduino Not Detected

1. **Check USB connection:**
   ```bash
   lsusb | grep -i arduino
   ```

2. **Check permissions:**
   ```bash
   sudo usermod -a -G dialout $USER
   # Log out and log back in
   ```

3. **Check serial port:**
   ```bash
   dmesg | grep -i tty
   ```

### Camera Not Working

1. **Check camera connection:**
   ```bash
   libcamera-hello --list-cameras
   ```

2. **Test camera:**
   ```bash
   libcamera-vid -t 0 --width 1280 --height 720
   ```

3. **Check OpenCV can access camera:**
   ```python
   import cv2
   cap = cv2.VideoCapture(0)
   print(cap.isOpened())
   ```

### Sensor Data Not Updating

1. **Check Arduino Serial Monitor** - verify data is being sent
2. **Check serial port in Python:**
   ```python
   import serial.tools.list_ports
   print([p.device for p in serial.tools.list_ports.comports()])
   ```
3. **Check app.py logs** for connection errors

## Sensor Calibration

### Gas Sensor (MQ)
- Normal: 0-400
- Warning: 400-600
- Alert: >600

### Vibration Sensor
- Normal: 0-50
- Warning: 50-100
- Alert: >100

### Microphone
- Normal: 0-200
- Warning: 200-300
- Alert: >300

### Flame Sensor
- 0 = No flame detected
- 1 = Flame detected (ALERT)

### Temperature (DHT11)
- Normal: 20-40°C
- Warning: 40-50°C
- Alert: >50°C

## Testing

1. **Test each sensor individually:**
   - Unplug other sensors
   - Monitor serial output
   - Verify readings change

2. **Test dashboard:**
   - Open http://raspberry-pi-ip:8080
   - Check sensor cards update
   - Verify camera feed displays

3. **Test hazard detection:**
   - Trigger each sensor to exceed threshold
   - Verify alerts appear in dashboard
   - Check detection logs update

