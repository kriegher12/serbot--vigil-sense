#!/bin/bash
# Start VigilSense Dashboard on Raspberry Pi

cd ~/vigil_sense_dashboard

echo "🚀 Starting VigilSense Dashboard..."
echo "📡 Checking Arduino connection..."
python3 -c "import serial.tools.list_ports; ports = [p for p in serial.tools.list_ports.comports() if 'USB' in p.description.upper() or 'ARDUINO' in p.description.upper()]; print(f'✅ Arduino found: {ports[0].device}' if ports else '⚠️  Arduino not found')" 2>/dev/null

echo "📷 Checking camera..."
python3 -c "import cv2; cap = cv2.VideoCapture(0); print('✅ Camera available' if cap.isOpened() else '⚠️  Camera not available'); cap.release()" 2>/dev/null

echo ""
echo "🌐 Starting Flask server..."
echo "Access at: http://$(hostname -I | awk '{print $1}'):8080"
echo ""

python3 app.py

