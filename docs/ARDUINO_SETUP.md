# Arduino Setup Instructions for VigilSense

## Pin Connections

| Sensor | Pin | Type | Connection |
|--------|-----|------|------------|
| **DHT11** | D4 | Digital | Data pin to D4, VCC to 5V, GND to GND |
| **Gas Sensor (MQ)** | A0 | Analog | Output to A0, VCC to 5V, GND to GND |
| **Vibration Sensor** | A3 | Analog | Output to A3, VCC to 5V, GND to GND |
| **Flame Sensor** | D7 | Digital | Output to D7, VCC to 5V, GND to GND |
| **Microphone** | D9 | Digital | Output to D9, VCC to 5V, GND to GND |

## Upload Code to Arduino

### Step 1: Install Arduino IDE
- Download from: https://www.arduino.cc/en/software
- Or use Arduino Web Editor: https://create.arduino.cc/editor

### Step 2: Install DHT Library
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for: `DHT sensor library`
4. Install **"DHT sensor library" by Adafruit** (version 1.4.4 or newer)
5. Also install **"Adafruit Unified Sensor"** (required dependency)

### Step 3: Select Board
1. Go to **Tools → Board → Arduino UNO R4 Boards → Arduino UNO R4 WiFi**
2. If UNO R4 WiFi not available:
   - Go to **Tools → Board → Boards Manager**
   - Search "Arduino UNO R4"
   - Install "Arduino UNO R4 Boards"

### Step 4: Select Port
1. Connect Arduino to computer via USB-C
2. Go to **Tools → Port**
3. Select the port (usually `/dev/ttyACM0` on Linux or `COMx` on Windows)

### Step 5: Upload Code
1. Open `arduino_code.ino` in Arduino IDE
2. Click **Upload** button (→ arrow icon)
3. Wait for "Done uploading" message

### Step 6: Verify Serial Output
1. Open **Serial Monitor** (Tools → Serial Monitor)
2. Set baud rate to **9600**
3. You should see:
   ```
   VigilSense Arduino Ready
   {"status":"ready"}
   {"temp":25.5,"humidity":50.0,"gas":300,"vibration":0,"flame":0,"sound":0}
   ```

## Expected Output Format

The Arduino sends JSON data every 200ms:
```json
{
  "temp": 25.5,
  "humidity": 50.0,
  "gas": 300,
  "vibration": 0,
  "flame": 0,
  "sound": 0
}
```

## Sensor Value Ranges

- **Temperature**: -40°C to 80°C (DHT11 range)
- **Humidity**: 0% to 100%
- **Gas**: 0-1023 (analog reading, higher = more gas detected)
- **Vibration**: 0-1023 (analog reading, higher = more vibration)
- **Flame**: 0 or 1 (0 = no flame, 1 = flame detected)
- **Sound**: 0 or 1 (0 = no sound, 1 = sound detected)

## Troubleshooting

### No Serial Output
- Check USB connection
- Verify correct port selected
- Check Serial Monitor baud rate is 9600
- Try different USB cable

### DHT11 Not Working
- Check wiring (VCC, GND, Data)
- Verify 4.7kΩ pull-up resistor on data pin
- Wait 2 seconds after power-on for sensor to stabilize

### Wrong Sensor Values
- Check pin connections match code
- Verify sensor power (5V)
- Test each sensor individually

### Arduino Not Detected by Raspberry Pi
- Check USB connection
- Verify Arduino is powered (LED should be on)
- Check permissions: `sudo usermod -a -G dialout $USER`
- List ports: `ls -l /dev/ttyACM*`

## Testing Individual Sensors

### Test Serial Output
```bash
# On Raspberry Pi
cat /dev/ttyACM0
```

### Test Python Reading
```python
import serial
port = serial.Serial('/dev/ttyACM0', 9600, timeout=2)
while True:
    line = port.readline().decode('utf-8').strip()
    if line:
        print(line)
```

## Next Steps

Once Arduino code is uploaded:
1. Connect Arduino to Raspberry Pi via USB-C
2. Verify connection: `ls -l /dev/ttyACM0`
3. Start VigilSense dashboard
4. Check dashboard for real sensor values!

