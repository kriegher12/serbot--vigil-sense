#!/bin/bash
# VigilSense - Start All Services
# This script starts the dashboard, camera, and all services on Raspberry Pi

echo "🚀 Starting VigilSense Dashboard..."
echo "===================================="
echo ""

# Try to find Pi
PI_HOST="raspberrypi.local"
if ! ping -c 1 $PI_HOST &>/dev/null; then
    echo "⚠️  Cannot reach $PI_HOST"
    echo "Trying alternative IPs..."
    for ip in 192.168.1.8 192.168.1.100 10.120.83.221; do
        if ping -c 1 $ip &>/dev/null; then
            PI_HOST=$ip
            echo "✅ Found Pi at $PI_HOST"
            break
        fi
    done
fi

echo "🔗 Connecting to: $PI_HOST"
echo ""

# Clean up old processes
echo "🧹 Cleaning up old processes..."
ssh mrstark@$PI_HOST "cd ~/vigil_sense_dashboard && pkill -f 'python.*app.py' 2>/dev/null; pkill -9 rpicam-vid 2>/dev/null; sleep 2" 2>/dev/null

# Start dashboard
echo "📡 Starting Flask dashboard..."
ssh mrstark@$PI_HOST "cd ~/vigil_sense_dashboard && nohup python3 app.py > /tmp/vigil_dashboard.log 2>&1 &" 2>/dev/null

# Wait for startup
echo "⏳ Waiting for services to start..."
sleep 8

# Check status
echo ""
echo "📊 Checking status..."
ssh mrstark@$PI_HOST "ps aux | grep -E 'app.py|rpicam' | grep -v grep && echo '' && netstat -tlnp 2>/dev/null | grep 8080" 2>/dev/null

# Get IP address
PI_IP=$(ssh mrstark@$PI_HOST "hostname -I | awk '{print \$1}'" 2>/dev/null)

echo ""
echo "✅ VigilSense Dashboard Started!"
echo "===================================="
echo ""
echo "🌐 Access Dashboard: http://${PI_IP:-$PI_HOST}:8080"
echo ""
echo "📊 Services:"
echo "  - Flask Dashboard: Running"
echo "  - Pi Camera v3: Streaming"
echo "  - YOLOv8 Detection: Active"
echo "  - Sensor Monitoring: Active"
echo ""
echo "📝 View logs: ssh mrstark@$PI_HOST 'tail -f /tmp/vigil_dashboard.log'"
echo ""



