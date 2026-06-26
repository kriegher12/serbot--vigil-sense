# Bluetooth Motor Control Setup Guide

## Hardware Connection

Same as WiFi version - see `MOTOR_CONTROL_SETUP.md` for pin connections.

## Software Setup

### 1. Install Arduino IDE
Download from: https://www.arduino.cc/en/software

### 2. Install Arduino UNO R4 WiFi Board Support
1. Open Arduino IDE
2. Go to **Tools** → **Board** → **Boards Manager**
3. Search for "Arduino UNO R4"
4. Install "Arduino UNO R4 Boards" by Arduino

### 3. Upload Bluetooth Code
1. Open `arduino_motor_control_ble.ino` in Arduino IDE
2. Connect Arduino UNO R4 WiFi via USB
3. Select **Tools** → **Board** → **Arduino UNO R4 WiFi**
4. Select **Tools** → **Port** → Your Arduino port
5. Click **Upload**

### 4. Verify Connection
1. Open **Serial Monitor** (9600 baud)
2. You should see: "✅ BLE initialized!"
3. Device name: "VigilSense Car"

## Using Bluefruit Connect App

### Step 1: Download Bluefruit Connect App
- **iOS**: [App Store](https://apps.apple.com/app/adafruit-bluefruit-le-connect/id830125974)
- **Android**: [Google Play](https://play.google.com/store/apps/details?id=com.adafruit.bluefruitconnect)

### Step 2: Connect to Arduino
1. Open Bluefruit Connect app
2. Tap **Connect** button (top right)
3. Look for "VigilSense Car" in the device list
4. Tap to connect
5. Wait for connection confirmation

### Step 3: Control the Car

#### Option A: Controller Tab
1. Tap the **Controller** tab (gamepad icon)
2. Use the on-screen joystick or buttons
3. Map controls to commands:
   - Up/Forward → Send "forward"
   - Down/Backward → Send "backward"
   - Left → Send "left"
   - Right → Send "right"
   - Center → Send "stop"

#### Option B: UART Tab (Text Commands)
1. Tap the **UART** tab (terminal icon)
2. Type commands and press send:
   - `forward` or `f` or `w` - Move forward
   - `backward` or `b` or `s` - Move backward
   - `left` or `l` or `a` - Turn left
   - `right` or `r` or `d` - Turn right
   - `stop` or `x` - Stop all motors
   - `speedup` or `+` - Increase speed
   - `speeddown` or `-` - Decrease speed
   - `speed:200` - Set speed (100-255)

#### Option C: Color Picker Tab
1. Tap **Color Picker** tab
2. Configure colors to send commands:
   - Red → "stop"
   - Green → "forward"
   - Blue → "backward"
   - Yellow → "left"
   - Purple → "right"

### Step 4: Configure Custom Controls
1. In Controller tab, tap **Settings** (gear icon)
2. Configure button mappings:
   - Map buttons to text commands
   - Set joystick dead zone
   - Adjust sensitivity

## Command Reference

| Command | Aliases | Description |
|---------|---------|-------------|
| `forward` | `f`, `w`, `up` | Move car forward |
| `backward` | `b`, `s`, `down` | Move car backward |
| `left` | `l`, `a` | Turn left |
| `right` | `r`, `d` | Turn right |
| `stop` | `x`, `s` | Stop all motors |
| `speedup` | `+`, `faster` | Increase speed by 20 |
| `speeddown` | `-`, `slower` | Decrease speed by 20 |
| `speed:200` | - | Set speed to 200 (100-255) |

## Troubleshooting

### App Not Finding Arduino
- Ensure Arduino is powered on and code is uploaded
- Check Serial Monitor for "BLE initialized" message
- Make sure Bluetooth is enabled on your phone
- Try restarting the app
- Restart Arduino (unplug and replug USB)

### Connection Drops
- Keep phone within 10 meters (30 feet)
- Avoid interference from other devices
- Ensure Arduino has stable power supply
- Check Serial Monitor for disconnection messages

### Motors Not Responding
- Verify all pin connections (see MOTOR_CONTROL_SETUP.md)
- Check motor power supply (VS on L293D)
- Test motors individually
- Check Serial Monitor for received commands

### Commands Not Working
- Verify you're sending commands in UART tab
- Check Serial Monitor to see if commands are received
- Ensure correct command spelling (case insensitive)
- Try sending "stop" first, then other commands

### Speed Issues
- Default speed is 200 (adjust in code)
- Use `speedup`/`speeddown` commands
- Or send `speed:150` for specific value
- Ensure motors can handle the speed

## Advanced Configuration

### Custom Button Mapping
In Bluefruit Connect Controller tab:
1. Tap Settings → Button Configuration
2. Create custom buttons
3. Map to commands: "forward", "backward", etc.

### Joystick Mode
1. Enable joystick in Controller tab
2. Configure dead zone (recommended: 10-20%)
3. Map joystick directions to commands
4. Use smooth control with analog input

### Automation
Send commands via UART tab:
- Create sequences: `forward`, wait, `stop`
- Use delays between commands
- Combine with sensor feedback

## Safety Notes
- Always stop motors when not in use
- Test at low speeds first
- Keep hands clear of moving parts
- Use proper power supply for motors
- Monitor battery voltage if battery-powered

## Next Steps
- Add sensor feedback (battery level, distance, etc.)
- Implement autonomous features
- Add LED indicators for status
- Create custom control interface

