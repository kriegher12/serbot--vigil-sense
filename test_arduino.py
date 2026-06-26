#!/usr/bin/env python3
"""
Test script to read sensor data from Arduino
Run this to verify Arduino is sending data correctly
"""

import serial
import serial.tools.list_ports
import time
import json

def find_arduino():
    """Find Arduino port"""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if 'USB' in port.description.upper() or 'ARDUINO' in port.description.upper():
            print(f"Found Arduino at: {port.device}")
            return port.device
    
    # Try common ports
    for port_name in ['/dev/ttyACM0', '/dev/ttyUSB0']:
        try:
            test = serial.Serial(port_name, 9600, timeout=1)
            test.close()
            print(f"Found Arduino at: {port_name}")
            return port_name
        except:
            continue
    
    return None

def main():
    port_name = find_arduino()
    if not port_name:
        print("❌ Arduino not found!")
        return
    
    try:
        arduino = serial.Serial(port_name, 9600, timeout=2)
        time.sleep(2)  # Wait for Arduino to initialize
        
        print(f"\n✅ Connected to Arduino at {port_name}")
        print("Reading sensor data...\n")
        print("-" * 60)
        
        for i in range(10):
            if arduino.in_waiting > 0:
                line = arduino.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(f"[{i+1}] {line}")
                    
                    # Try to parse JSON
                    if line.startswith('{'):
                        try:
                            data = json.loads(line)
                            print(f"     Temperature: {data.get('temp', 'N/A')}°C")
                            print(f"     Humidity: {data.get('humidity', 'N/A')}%")
                            print(f"     Gas: {data.get('gas', 'N/A')}")
                            print(f"     Vibration: {data.get('vibration', 'N/A')}")
                            print(f"     Flame: {data.get('flame', 'N/A')}")
                            print(f"     Sound: {data.get('sound', 'N/A')}")
                        except:
                            pass
            time.sleep(0.5)
        
        print("-" * 60)
        print("\n✅ Arduino is sending sensor data correctly!")
        
    except Exception as e:
        print(f"❌ Error: {e}")
    finally:
        if 'arduino' in locals():
            arduino.close()

if __name__ == "__main__":
    main()

