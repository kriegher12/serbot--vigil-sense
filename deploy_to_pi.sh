#!/bin/bash
# VigilSense Dashboard - Deployment Script for Raspberry Pi
# Usage: ./deploy_to_pi.sh pi@<raspberry-pi-ip>

set -e

PI_USER="${1:-pi@raspberrypi.local}"
PI_DIR="~/vigil_sense_dashboard"

echo "🚀 Deploying VigilSense Dashboard to Raspberry Pi..."
echo "Target: $PI_USER"
echo ""

# Check if we can connect
echo "📡 Testing connection..."
ssh -o ConnectTimeout=5 $PI_USER "echo 'Connection successful!'" || {
    echo "❌ Cannot connect to $PI_USER"
    echo "Please ensure:"
    echo "  1. Raspberry Pi is powered on and on the same network"
    echo "  2. SSH is enabled (touch /Volumes/bootfs/ssh if needed)"
    echo "  3. Correct IP address or hostname"
    exit 1
}

echo "✅ Connection established"
echo ""

# Create remote directory
echo "📁 Creating remote directory..."
ssh $PI_USER "mkdir -p $PI_DIR"

# Copy files (excluding screenshots and git files)
echo "📦 Copying files..."
scp -r app.py requirements.txt templates/ static/ arduino_code.ino HARDWARE_SETUP.md $PI_USER:$PI_DIR/

# Install dependencies on Pi
echo "📥 Installing Python dependencies..."
ssh $PI_USER "cd $PI_DIR && pip3 install --break-system-packages -r requirements.txt 2>&1 | tail -10"

echo ""
echo "✅ Deployment complete!"
echo ""
echo "To run the dashboard on Raspberry Pi:"
echo "  ssh $PI_USER"
echo "  cd $PI_DIR"
echo "  python3 app.py"
echo ""
echo "Then access at: http://<raspberry-pi-ip>:8080"

