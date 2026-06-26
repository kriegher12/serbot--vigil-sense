#!/bin/bash
# Upload Bluetooth motor control code to Arduino UNO R4 WiFi using Arduino CLI

echo "🚗 VigilSense Motor Control - Bluetooth Upload"
echo "==============================================="
echo ""

# Check if Arduino CLI is installed
if ! command -v arduino-cli &> /dev/null; then
    echo "❌ Arduino CLI not found!"
    echo "Install it from: https://arduino.github.io/arduino-cli/"
    exit 1
fi

# Find Arduino port
echo "🔍 Searching for Arduino UNO R4 WiFi..."
PORT=$(arduino-cli board list | grep -i "uno.*r4.*wifi" | head -1 | awk '{print $1}')

if [ -z "$PORT" ]; then
    PORT=$(arduino-cli board list | grep -i "arduino" | head -1 | awk '{print $1}')
fi

if [ -z "$PORT" ]; then
    echo "❌ No Arduino board found!"
    echo "Please connect your Arduino UNO R4 WiFi and try again."
    exit 1
fi

echo "✅ Found Arduino at: $PORT"
echo ""

# Create sketch directory if it doesn't exist
SKETCH_DIR="arduino_motor_control_ble"
mkdir -p "$SKETCH_DIR"

# Copy .ino file to sketch directory
cp arduino_motor_control_ble.ino "$SKETCH_DIR/arduino_motor_control_ble.ino"

echo "📦 Compiling sketch..."
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi "$SKETCH_DIR" || {
    echo "❌ Compilation failed!"
    echo "Make sure you have installed the Arduino UNO R4 WiFi board support:"
    echo "  arduino-cli core install arduino:renesas_uno"
    echo ""
    echo "Also ensure ArduinoBLE library is installed:"
    echo "  arduino-cli lib install ArduinoBLE"
    exit 1
}

echo ""
echo "📤 Uploading to Arduino..."
arduino-cli upload -p "$PORT" --fqbn arduino:renesas_uno:unor4wifi "$SKETCH_DIR" || {
    echo "❌ Upload failed!"
    exit 1
}

echo ""
echo "✅ Upload successful!"
echo ""
echo "📱 Next steps:"
echo "1. Open Serial Monitor (9600 baud) to see BLE status"
echo "2. Download Bluefruit Connect app on your phone"
echo "3. Open app and connect to 'VigilSense Car'"
echo "4. Use Controller or UART tab to control the car"
echo ""

