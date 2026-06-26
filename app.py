"""
VigilSense - Smart Field Patrolling & Hazard Detection Bot
Flask Backend for Live Monitoring Dashboard
Real Hardware Integration: Arduino UNO R4 WiFi + Pi Camera v3
"""

from flask import Flask, render_template, Response, jsonify, request
from flask_cors import CORS
import cv2
import serial
import serial.tools.list_ports
import time
from datetime import datetime
import threading
import json
import re
import subprocess
import numpy as np
from ultralytics import YOLO
import requests

app = Flask(__name__)
CORS(app)

# Sensor data storage
sensor_data = {
    "temperature": 25.0,
    "humidity": 50.0,
    "gas": 0,
    "flame": 0,
    "sound": 0,
    "vibration": 0
}

# Detection logs
detection_logs = []

# Arduino serial connection
arduino = None
arduino_port = None

# Camera instance
camera = None
camera_process = None

# YOLO model for person detection
yolo_model = None
person_count = 0
previous_person_count = 0

# Telegram Bot Configuration
TELEGRAM_BOT_TOKEN = "8466970568:AAHbWgIEzGKsCZto38IcqmZ4wG4yAslcgNg"
TELEGRAM_CHAT_ID = "946411640"
TELEGRAM_ENABLED = True  # Enabled with existing credentials
last_alert_time = 0
alert_cooldown = 30  # Seconds between alerts (prevents spam)

def find_arduino_port():
    """Find Arduino UNO R4 WiFi port"""
    global arduino_port
    ports = serial.tools.list_ports.comports()
    for port in ports:
        # Look for Arduino or USB serial devices
        if 'USB' in port.description.upper() or 'ARDUINO' in port.description.upper() or 'CH340' in port.description.upper() or 'CP210' in port.description.upper():
            arduino_port = port.device
            print(f"Found potential Arduino at: {port.device} ({port.description})")
            return port.device
    # Fallback: try common ports
    for port_name in ['/dev/ttyACM0', '/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyACM1']:
        try:
            test_serial = serial.Serial(port_name, 9600, timeout=1)
            test_serial.close()
            arduino_port = port_name
            print(f"Found Arduino at: {port_name}")
            return port_name
        except:
            continue
    return None

def init_arduino():
    """Initialize Arduino serial connection"""
    global arduino, arduino_port
    port = find_arduino_port()
    if port:
        try:
            arduino = serial.Serial(port, 9600, timeout=2)
            time.sleep(2)  # Wait for Arduino to initialize
            print(f"✅ Arduino connected on {port}")
            return True
        except Exception as e:
            print(f"❌ Error connecting to Arduino: {e}")
            arduino = None
            return False
    else:
        print("⚠️  Arduino not found. Using mock data.")
        return False

def parse_arduino_data(line):
    """Parse sensor data from Arduino
    Expected format: JSON or comma-separated values
    Example: {"temp":25.5,"humidity":50,"gas":300,"flame":0,"sound":150,"vibration":0}
    Or: temp:25.5,humidity:50,gas:300,flame:0,sound:150,vibration:0
    """
    global sensor_data
    
    try:
        # Try JSON format first
        if line.strip().startswith('{'):
            data = json.loads(line.strip())
            sensor_data["temperature"] = float(data.get("temp", sensor_data["temperature"]))
            sensor_data["humidity"] = float(data.get("humidity", sensor_data["humidity"]))
            sensor_data["gas"] = int(data.get("gas", sensor_data["gas"]))
            sensor_data["flame"] = int(data.get("flame", sensor_data["flame"]))
            sensor_data["sound"] = int(data.get("sound", sensor_data["sound"]))
            sensor_data["vibration"] = int(data.get("vibration", sensor_data["vibration"]))
            return True
        # Try comma-separated format
        elif ':' in line or ',' in line:
            # Parse format like "temp:25.5,humidity:50,gas:300"
            pattern = r'(\w+)[:=](\d+\.?\d*)'
            matches = re.findall(pattern, line)
            for key, value in matches:
                key = key.lower()
                if key in ['temp', 'temperature']:
                    sensor_data["temperature"] = float(value)
                elif key == 'humidity':
                    sensor_data["humidity"] = float(value)
                elif key == 'gas':
                    sensor_data["gas"] = int(value)
                elif key == 'flame':
                    sensor_data["flame"] = int(value)
                elif key in ['sound', 'mic', 'microphone']:
                    sensor_data["sound"] = int(value)
                elif key == 'vibration':
                    sensor_data["vibration"] = int(value)
            return True
    except Exception as e:
        print(f"Error parsing Arduino data: {e}")
        return False
    
    return False

def read_arduino_sensors():
    """Thread function to continuously read sensor data from Arduino"""
    global sensor_data, detection_logs, arduino
    
    while True:
        if arduino and arduino.is_open:
            try:
                if arduino.in_waiting > 0:
                    line = arduino.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        parse_arduino_data(line)
                        
                        # Check for hazards and add to logs
                        if (sensor_data["temperature"] > 50 or 
                            sensor_data["gas"] > 600 or 
                            sensor_data["flame"] == 1 or 
                            sensor_data["sound"] > 300 or 
                            sensor_data["vibration"] > 100):
                            
                            detection_type = []
                            if sensor_data["temperature"] > 50:
                                detection_type.append("High Temp")
                            if sensor_data["gas"] > 600:
                                detection_type.append("Gas Leak")
                            if sensor_data["flame"] == 1:
                                detection_type.append("Flame")
                            if sensor_data["sound"] > 300:
                                detection_type.append("Loud Sound")
                            if sensor_data["vibration"] > 100:
                                detection_type.append("Vibration")
                            
                            log_entry = {
                                "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                                "detection": ", ".join(detection_type),
                                "temp": sensor_data["temperature"],
                                "gas": sensor_data["gas"],
                                "flame": sensor_data["flame"],
                                "sound": sensor_data["sound"],
                                "vibration": sensor_data["vibration"]
                            }
                            
                            detection_logs.insert(0, log_entry)
                            if len(detection_logs) > 10:
                                detection_logs.pop()
                else:
                    time.sleep(0.1)
            except Exception as e:
                print(f"Error reading from Arduino: {e}")
                time.sleep(1)
        else:
            # Fallback to mock data if Arduino not connected
            time.sleep(2)
        time.sleep(0.1)  # Small delay to prevent CPU overload

def init_yolo():
    """Initialize YOLOv8 model for person detection"""
    global yolo_model
    try:
        # Load YOLOv8 model (will download if not available)
        # Using 'yolov8n.pt' for faster inference (nano model)
        yolo_model = YOLO('yolov8n.pt')
        print("✅ YOLOv8 model loaded for person detection")
    except Exception as e:
        print(f"⚠️  YOLO initialization error: {e}")
        print("   Continuing without YOLO detection...")
        yolo_model = None

def send_telegram_alert(message):
    """Send alert message to Telegram"""
    global TELEGRAM_BOT_TOKEN, TELEGRAM_CHAT_ID, TELEGRAM_ENABLED
    
    if not TELEGRAM_ENABLED or not TELEGRAM_BOT_TOKEN or not TELEGRAM_CHAT_ID:
        return False
    
    try:
        url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendMessage"
        payload = {
            "chat_id": TELEGRAM_CHAT_ID,
            "text": message,
            "parse_mode": "HTML"
        }
        response = requests.post(url, json=payload, timeout=5)
        if response.status_code == 200:
            return True
        else:
            print(f"⚠️  Telegram alert failed: {response.status_code}")
            return False
    except Exception as e:
        print(f"⚠️  Telegram alert error: {e}")
        return False

def detect_people(frame):
    """Detect people in frame using YOLOv8 and draw bounding boxes"""
    global person_count, yolo_model, previous_person_count, last_alert_time, alert_cooldown
    
    if yolo_model is None:
        return frame
    
    try:
        # Run YOLO inference (smaller imgsz for lower latency)
        results = yolo_model(frame, classes=[0], verbose=False, imgsz=480)  # class 0 = person
        
        person_count = 0
        
        # Draw bounding boxes and count people
        for result in results:
            boxes = result.boxes
            for box in boxes:
                # Get box coordinates
                x1, y1, x2, y2 = box.xyxy[0].cpu().numpy()
                confidence = box.conf[0].cpu().numpy()
                
                # Only draw if confidence > 0.5
                if confidence > 0.5:
                    person_count += 1
                    
                    # Draw bounding box
                    cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 2)
                    
                    # Draw label with confidence
                    label = f"Person {confidence:.2f}"
                    label_size = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.6, 2)[0]
                    cv2.rectangle(frame, (int(x1), int(y1) - label_size[1] - 10),
                                (int(x1) + label_size[0], int(y1)), (0, 255, 0), -1)
                    cv2.putText(frame, label, (int(x1), int(y1) - 5),
                              cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 0), 2)
        
        # Draw person count in top-left corner
        count_text = f"People: {person_count}"
        cv2.rectangle(frame, (10, 10), (200, 50), (0, 0, 0), -1)
        cv2.putText(frame, count_text, (15, 35),
                   cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0, 255, 0), 2)
        
        # Send Telegram alert when person count changes
        current_time = time.time()
        if person_count != previous_person_count:
            if person_count > 0 and (current_time - last_alert_time) >= alert_cooldown:
                # People detected - send alert
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                message = f"🚨 <b>Person Detected!</b>\n\n"
                message += f"👥 <b>Count:</b> {person_count} person(s)\n"
                message += f"🕐 <b>Time:</b> {timestamp}\n"
                message += f"📍 <b>Location:</b> VigilSense Monitoring Area\n\n"
                message += f"⚠️ Check live feed: http://10.120.83.221:8080"
                
                if send_telegram_alert(message):
                    print(f"✅ Telegram alert sent: {person_count} person(s) detected")
                    last_alert_time = current_time
                else:
                    print(f"⚠️  Failed to send Telegram alert")
            elif person_count == 0 and previous_person_count > 0:
                # All people left - send confirmation
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                message = f"✅ <b>Area Clear</b>\n\n"
                message += f"👥 <b>Count:</b> 0 persons\n"
                message += f"🕐 <b>Time:</b> {timestamp}\n"
                message += f"📍 <b>Location:</b> VigilSense Monitoring Area"
                
                if send_telegram_alert(message):
                    print(f"✅ Telegram alert sent: Area cleared")
                    last_alert_time = current_time
            
            previous_person_count = person_count
        
    except Exception as e:
        print(f"Error in YOLO detection: {e}")
    
    return frame

def init_camera():
    """Initialize Pi Camera v3 using rpicam-vid"""
    global camera, camera_process
    
    # Kill any existing rpicam processes (from previous runs)
    try:
        subprocess.run(['pkill', '-9', 'rpicam-vid'], stderr=subprocess.DEVNULL, timeout=2)
        time.sleep(0.5)
    except:
        pass
    
    # Kill any existing camera_process
    if camera_process:
        try:
            camera_process.terminate()
            camera_process.wait(timeout=2)
        except:
            try:
                camera_process.kill()
            except:
                pass
    
    try:
        # Use rpicam-vid for Pi Camera v3
        # rpicam-vid outputs MJPEG stream which we'll read
        camera_process = subprocess.Popen(
            [
                'rpicam-vid',
                '--width', '640',
                '--height', '480',
                '--framerate', '24',
                '--codec', 'mjpeg',
                '--inline',
                '--timeout', '0',
                '--output', '-',
                '--nopreview'
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            bufsize=0  # Unbuffered
        )
        
        # Wait a moment to see if process starts successfully
        time.sleep(1)
        if camera_process.poll() is None:
            camera = True  # Mark as initialized
            print("✅ Pi Camera v3 initialized with rpicam-vid")
        else:
            try:
                stderr_output = camera_process.stderr.read().decode('utf-8', errors='ignore')
                print(f"⚠️  rpicam-vid failed: {stderr_output[:300]}")
            except:
                pass
            camera = None
            try:
                camera_process.terminate()
            except:
                pass
            camera_process = None
    except Exception as e:
        print(f"Camera initialization error: {e}")
        camera = None
        camera_process = None

def generate_frames():
    """Generate camera frames for MJPEG stream using rpicam-vid"""
    global camera_process
    
    # Buffer for accumulating JPEG data
    jpeg_buffer = bytearray()
    
    while True:
        if camera and camera_process and camera_process.poll() is None:
            try:
                # Read data from rpicam-vid
                chunk = camera_process.stdout.read(8192)
                if not chunk:
                    # Restart camera process if it died
                    init_camera()
                    time.sleep(1)
                    continue
                
                jpeg_buffer.extend(chunk)
                
                # Look for JPEG markers
                while True:
                    # Find start marker
                    start_idx = jpeg_buffer.find(b'\xff\xd8')
                    if start_idx == -1:
                        # No start marker, keep only last 1000 bytes to avoid memory issues
                        if len(jpeg_buffer) > 1000:
                            jpeg_buffer = jpeg_buffer[-1000:]
                        break
                    
                    # Find end marker after start
                    end_idx = jpeg_buffer.find(b'\xff\xd9', start_idx + 2)
                    if end_idx == -1:
                        # No end marker yet, need more data
                        break
                    
                    # Extract complete JPEG
                    jpeg_data = bytes(jpeg_buffer[start_idx:end_idx + 2])
                    jpeg_buffer = jpeg_buffer[end_idx + 2:]
                    
                    # Decode JPEG to numpy array for YOLO processing
                    try:
                        nparr = np.frombuffer(jpeg_data, np.uint8)
                        frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
                        
                        if frame is not None:
                            # Run YOLO person detection
                            frame = detect_people(frame)
                            
                            # Re-encode to JPEG (lower quality to reduce bandwidth/latency)
                            ret, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 70])
                            if ret:
                                jpeg_data = buffer.tobytes()
                    except Exception as e:
                        # If processing fails, use original frame
                        pass
                    
                    # Yield the frame
                    yield (b'--frame\r\n'
                           b'Content-Type: image/jpeg\r\n\r\n' + jpeg_data + b'\r\n')
                    
            except Exception as e:
                print(f"Error reading camera frame: {e}")
                time.sleep(0.1)
                # Try to restart camera
                if camera_process and camera_process.poll() is not None:
                    init_camera()
                    time.sleep(1)
        else:
            # Generate mock frame if camera not available
            frame = generate_mock_frame()
            ret, buffer = cv2.imencode('.jpg', frame, [cv2.IMWRITE_JPEG_QUALITY, 70])
            if ret:
                frame_bytes = buffer.tobytes()
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')
            time.sleep(0.033)  # ~30 FPS

def generate_mock_frame():
    """Generate a mock frame when camera is not available"""
    import numpy as np
    frame = np.zeros((720, 1280, 3), dtype=np.uint8)
    cv2.putText(frame, "Pi Camera v3 - Not Available", (300, 350), 
                cv2.FONT_HERSHEY_SIMPLEX, 1.2, (0, 255, 0), 2)
    cv2.putText(frame, "Please check camera connection", (250, 400), 
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
    return frame

@app.route('/')
def index():
    """Render the main dashboard page"""
    return render_template('index.html')

@app.route('/logs')
def logs():
    """Render the detection logs page"""
    return render_template('logs.html')

@app.route('/stream.mjpg')
def stream():
    """MJPEG camera stream endpoint"""
    return Response(generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/api/sensors')
def get_sensors():
    """API endpoint to get current sensor data"""
    return jsonify(sensor_data)

@app.route('/api/logs')
def get_logs():
    """API endpoint to get detection logs"""
    return jsonify(detection_logs)

@app.route('/api/status')
def get_status():
    """API endpoint to get system status"""
    is_hazard = (
        sensor_data["temperature"] > 50 or
        sensor_data["gas"] > 600 or
        sensor_data["flame"] == 1 or
        sensor_data["sound"] > 300 or
        sensor_data["vibration"] > 100
    )
    
    return jsonify({
        "status": "hazard" if is_hazard else "normal",
        "message": "⚠️ Hazard Detected" if is_hazard else "✅ All Systems Normal"
    })

@app.route('/api/arduino/status')
def arduino_status():
    """Check Arduino connection status"""
    return jsonify({
        "connected": arduino is not None and arduino.is_open if arduino else False,
        "port": arduino_port
    })

@app.route('/api/people')
def get_people_count():
    """Get current person count from YOLO detection"""
    return jsonify({
        "count": person_count,
        "yolo_enabled": yolo_model is not None,
        "telegram_enabled": TELEGRAM_ENABLED
    })

@app.route('/api/telegram/status')
def telegram_status():
    """Get Telegram alert configuration"""
    return jsonify({
        "enabled": TELEGRAM_ENABLED,
        "cooldown": alert_cooldown
    })

@app.route('/api/telegram/toggle', methods=['POST'])
def telegram_toggle():
    """Enable/disable Telegram alerts from the UI"""
    global TELEGRAM_ENABLED
    try:
        data = request.get_json(silent=True) or {}
        if 'enabled' not in data:
            return jsonify({"status": "error", "message": "Missing 'enabled' boolean"}), 400
        TELEGRAM_ENABLED = bool(data['enabled'])
        return jsonify({"status": "success", "enabled": TELEGRAM_ENABLED})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400

@app.route('/api/telegram/test')
def test_telegram():
    """Test Telegram alert"""
    if send_telegram_alert("🧪 <b>Test Alert</b>\n\nThis is a test message from VigilSense Dashboard."):
        return jsonify({"status": "success", "message": "Test alert sent!"})
    else:
        return jsonify({"status": "error", "message": "Telegram not configured or failed to send"}), 400

if __name__ == '__main__':
    # Initialize hardware
    print("🔧 Initializing VigilSense Hardware...")
    
    # Initialize YOLO model
    print("🤖 Loading YOLOv8 model for person detection...")
    init_yolo()
    
    # Initialize camera
    init_camera()
    
    # Initialize Arduino
    init_arduino()
    
    # Start sensor data reading thread
    sensor_thread = threading.Thread(target=read_arduino_sensors, daemon=True)
    sensor_thread.start()
    
    # Run Flask app
    print("🚀 Starting VigilSense Dashboard...")
    print(f"📡 Arduino: {'✅ Connected' if arduino and arduino.is_open else '❌ Not Connected'}")
    print(f"📷 Camera: {'✅ Connected' if camera and camera_process and camera_process.poll() is None else '❌ Not Connected'}")
    print("🌐 Access at: http://localhost:8080")
    print("📊 Or network: http://<raspberry-pi-ip>:8080")
    
    app.run(host='0.0.0.0', port=8080, debug=True, threaded=True)
