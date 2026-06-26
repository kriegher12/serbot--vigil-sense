# VigilSense Motor Control Setup Guide

## Hardware Connection

### L293D Motor Driver Pin Configuration

For **4 Motors** using **2 L293D chips** (or 1 L293D for 2 motors):

#### Motor 1 (Left Front)
- **IN1** → Arduino Digital Pin **2**
- **IN2** → Arduino Digital Pin **3**
- **ENA** → Arduino Digital Pin **5** (PWM)

#### Motor 2 (Right Front)
- **IN3** → Arduino Digital Pin **4**
- **IN4** → Arduino Digital Pin **7**
- **ENB** → Arduino Digital Pin **6** (PWM)

#### Motor 3 (Left Rear)
- **IN1** → Arduino Digital Pin **8**
- **IN2** → Arduino Digital Pin **9**
- **ENA** → Arduino Digital Pin **10** (PWM)

#### Motor 4 (Right Rear)
- **IN3** → Arduino Digital Pin **11**
- **IN4** → Arduino Digital Pin **12**
- **ENB** → Arduino Digital Pin **13** (PWM)

### Power Connections
- **L293D VCC** → Arduino 5V (for logic)
- **L293D VS** → External 5-12V (for motors - separate power supply recommended)
- **L293D GND** → Arduino GND
- **Motor GND** → Common GND with Arduino

### Notes
- For high-power motors, use a separate power supply for VS (motor power)
- Connect a 100µF capacitor between VS and GND for stability
- Ensure all GND connections are common

## Software Setup

### 1. Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### 2. Install Arduino UNO R4 WiFi Board Support
1. Open Arduino IDE
2. Go to **Tools** → **Board** → **Boards Manager**
3. Search for "Arduino UNO R4"
4. Install "Arduino UNO R4 Boards" by Arduino

### 3. Configure WiFi Credentials
Edit `arduino_motor_control.ino`:
```cpp
const char* ssid = "Stark954";           // Your WiFi network name
const char* password = "123456789";       // Your WiFi password
```

### 4. Upload Code
1. Connect Arduino UNO R4 WiFi via USB
2. Select **Tools** → **Board** → **Arduino UNO R4 WiFi**
3. Select **Tools** → **Port** → Your Arduino port
4. Click **Upload**

### 5. Get IP Address
1. Open **Tools** → **Serial Monitor** (9600 baud)
2. Wait for WiFi connection
3. Note the IP address displayed (e.g., `192.168.1.100`)

## Controlling the Car

### Method 1: Web Browser
1. Open a web browser on your phone/computer
2. Navigate to: `http://[IP_ADDRESS]`
   - Example: `http://192.168.1.100`
3. Use the on-screen buttons to control the car:
   - ⬆️ Forward
   - ⬇️ Backward
   - ⬅️ Turn Left
   - ➡️ Turn Right
   - ⏹️ Stop
   - ⚡+ Increase Speed
   - ⚡- Decrease Speed

### Method 2: Bluefruit Connect App
1. Download **Adafruit Bluefruit Connect** app on your phone
2. The app supports WiFi connection
3. Enter the Arduino's IP address
4. Use the app's control interface

### Method 3: Direct URL Commands
You can also send commands directly via URL:
- `http://[IP]/forward` - Move forward
- `http://[IP]/backward` - Move backward
- `http://[IP]/left` - Turn left
- `http://[IP]/right` - Turn right
- `http://[IP]/stop` - Stop all motors
- `http://[IP]/speedup` - Increase speed
- `http://[IP]/speeddown` - Decrease speed

## Troubleshooting

### WiFi Connection Issues
- Double-check SSID and password
- Ensure Arduino UNO R4 WiFi is in range
- Check Serial Monitor for connection status

### Motors Not Moving
- Verify all pin connections
- Check motor power supply (VS on L293D)
- Ensure motors are properly connected to L293D outputs
- Test individual motors by modifying code

### Motors Running in Wrong Direction
- Swap IN1/IN2 or IN3/IN4 connections
- Or modify the code to swap HIGH/LOW states

### Speed Issues
- Adjust `motorSpeed` variable (0-255)
- Use external power supply for VS if motors are weak
- Check PWM pin connections

## Safety Notes
- Always stop motors when not in use
- Test at low speeds first
- Ensure proper power supply rating for motors
- Keep hands clear of moving parts during testing

