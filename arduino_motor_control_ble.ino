/*
 * VigilSense - Arduino UNO R4 WiFi Motor Control via Bluetooth (BLE)
 * Bluetooth-controlled car with L293D motor driver and 4 motors
 * Compatible with Bluefruit Connect app via Bluetooth Low Energy (BLE)
 * 
 * Hardware Setup:
 * - Arduino UNO R4 WiFi
 * - L293D Motor Driver (1 or 2 chips for 4 motors)
 * - 4 DC Motors
 * 
 * L293D Pin Configuration (for 4 motors):
 * Motor 1 (Left Front):  IN1=D2, IN2=D3, ENA=D5 (PWM)
 * Motor 2 (Right Front): IN3=D4, IN4=D7, ENB=D6 (PWM)
 * Motor 3 (Left Rear):   IN1=D8, IN2=D9, ENA=D10 (PWM)
 * Motor 4 (Right Rear):  IN3=D11, IN4=D12, ENB=D13 (PWM)
 * 
 * Note: If using single L293D, you can control 2 motors.
 * Adjust pins according to your wiring.
 */

#include <ArduinoBLE.h>

// Motor pin definitions
// Motor 1 (Left Front)
#define MOTOR1_IN1 2
#define MOTOR1_IN2 3
#define MOTOR1_ENA 5   // PWM pin

// Motor 2 (Right Front)
#define MOTOR2_IN3 4
#define MOTOR2_IN4 7
#define MOTOR2_ENB 6   // PWM pin

// Motor 3 (Left Rear)
#define MOTOR3_IN1 8
#define MOTOR3_IN2 9
#define MOTOR3_ENA 10  // PWM pin

// Motor 4 (Right Rear)
#define MOTOR4_IN3 11
#define MOTOR4_IN4 12
#define MOTOR4_ENB 13  // PWM pin

// Motor speed (0-255, PWM value)
int motorSpeed = 200;  // Default speed (adjust as needed, max 255)

// BLE Service and Characteristic
BLEService motorService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Custom service UUID

// Characteristic for receiving commands
BLEStringCharacteristic commandChar("19B10001-E8F2-537E-4F6C-D104768A1214", 
                                    BLERead | BLEWrite, 20);

// Characteristic for sending status
BLEStringCharacteristic statusChar("19B10002-E8F2-537E-4F6C-D104768A1214", 
                                   BLERead | BLENotify, 20);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("VigilSense Motor Control - Bluetooth");
  Serial.println("=====================================");
  
  // Configure motor pins as outputs
  pinMode(MOTOR1_IN1, OUTPUT);
  pinMode(MOTOR1_IN2, OUTPUT);
  pinMode(MOTOR1_ENA, OUTPUT);
  
  pinMode(MOTOR2_IN3, OUTPUT);
  pinMode(MOTOR2_IN4, OUTPUT);
  pinMode(MOTOR2_ENB, OUTPUT);
  
  pinMode(MOTOR3_IN1, OUTPUT);
  pinMode(MOTOR3_IN2, OUTPUT);
  pinMode(MOTOR3_ENA, OUTPUT);
  
  pinMode(MOTOR4_IN3, OUTPUT);
  pinMode(MOTOR4_IN4, OUTPUT);
  pinMode(MOTOR4_ENB, OUTPUT);
  
  // Stop all motors initially
  stopAllMotors();
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("❌ Starting BLE failed!");
    while (1);
  }
  
  // Set BLE device name
  BLE.setLocalName("VigilSense Car");
  BLE.setDeviceName("VigilSense Car");
  
  // Set advertised service
  BLE.setAdvertisedService(motorService);
  
  // Add characteristics to service
  motorService.addCharacteristic(commandChar);
  motorService.addCharacteristic(statusChar);
  
  // Add service
  BLE.addService(motorService);
  
  // Start advertising
  BLE.advertise();
  
  Serial.println("✅ BLE initialized!");
  Serial.println("Device name: VigilSense Car");
  Serial.println("Waiting for Bluetooth connection...");
  Serial.println("Use Bluefruit Connect app to connect!");
}

void loop() {
  // Listen for BLE central device to connect
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("✅ Connected to: ");
    Serial.println(central.address());
    
    // Update status
    statusChar.writeValue("Connected");
    
    // While connected
    while (central.connected()) {
      // Check if command was received
      if (commandChar.written()) {
        String command = commandChar.value();
        Serial.print("Received command: ");
        Serial.println(command);
        
        // Process command
        processCommand(command);
        
        // Update status
        String status = "Cmd: " + command + " | Speed: " + String(motorSpeed);
        statusChar.writeValue(status);
      }
      
      delay(10);
    }
    
    // Disconnected
    Serial.print("❌ Disconnected from: ");
    Serial.println(central.address());
    stopAllMotors();
    statusChar.writeValue("Disconnected");
  }
  
  delay(10);
}

void processCommand(String command) {
  command.toLowerCase();
  command.trim();
  
  // Forward
  if (command == "forward" || command == "up" || command == "f" || command == "w") {
    moveForward();
    Serial.println("→ Forward");
  }
  // Backward
  else if (command == "backward" || command == "down" || command == "b" || command == "s") {
    moveBackward();
    Serial.println("→ Backward");
  }
  // Left
  else if (command == "left" || command == "l" || command == "a") {
    turnLeft();
    Serial.println("→ Left");
  }
  // Right
  else if (command == "right" || command == "r" || command == "d") {
    turnRight();
    Serial.println("→ Right");
  }
  // Stop
  else if (command == "stop" || command == "s" || command == "x") {
    stopAllMotors();
    Serial.println("→ Stop");
  }
  // Speed up
  else if (command == "speedup" || command == "faster" || command == "+") {
    motorSpeed = min(255, motorSpeed + 20);
    Serial.print("→ Speed increased to: ");
    Serial.println(motorSpeed);
    stopAllMotors(); // Stop to apply new speed
  }
  // Speed down
  else if (command == "speeddown" || command == "slower" || command == "-") {
    motorSpeed = max(100, motorSpeed - 20);
    Serial.print("→ Speed decreased to: ");
    Serial.println(motorSpeed);
    stopAllMotors(); // Stop to apply new speed
  }
  // Set speed (format: "speed:200")
  else if (command.startsWith("speed:")) {
    int newSpeed = command.substring(6).toInt();
    motorSpeed = constrain(newSpeed, 100, 255);
    Serial.print("→ Speed set to: ");
    Serial.println(motorSpeed);
    stopAllMotors();
  }
  else {
    Serial.print("→ Unknown command: ");
    Serial.println(command);
  }
}

// Motor control functions
void moveForward() {
  // Left motors forward
  digitalWrite(MOTOR1_IN1, HIGH);
  digitalWrite(MOTOR1_IN2, LOW);
  analogWrite(MOTOR1_ENA, motorSpeed);
  
  digitalWrite(MOTOR3_IN1, HIGH);
  digitalWrite(MOTOR3_IN2, LOW);
  analogWrite(MOTOR3_ENA, motorSpeed);
  
  // Right motors forward
  digitalWrite(MOTOR2_IN3, HIGH);
  digitalWrite(MOTOR2_IN4, LOW);
  analogWrite(MOTOR2_ENB, motorSpeed);
  
  digitalWrite(MOTOR4_IN3, HIGH);
  digitalWrite(MOTOR4_IN4, LOW);
  analogWrite(MOTOR4_ENB, motorSpeed);
}

void moveBackward() {
  // Left motors backward
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, HIGH);
  analogWrite(MOTOR1_ENA, motorSpeed);
  
  digitalWrite(MOTOR3_IN1, LOW);
  digitalWrite(MOTOR3_IN2, HIGH);
  analogWrite(MOTOR3_ENA, motorSpeed);
  
  // Right motors backward
  digitalWrite(MOTOR2_IN3, LOW);
  digitalWrite(MOTOR2_IN4, HIGH);
  analogWrite(MOTOR2_ENB, motorSpeed);
  
  digitalWrite(MOTOR4_IN3, LOW);
  digitalWrite(MOTOR4_IN4, HIGH);
  analogWrite(MOTOR4_ENB, motorSpeed);
}

void turnLeft() {
  // Left motors backward
  digitalWrite(MOTOR1_IN1, LOW);
  digitalWrite(MOTOR1_IN2, HIGH);
  analogWrite(MOTOR1_ENA, motorSpeed);
  
  digitalWrite(MOTOR3_IN1, LOW);
  digitalWrite(MOTOR3_IN2, HIGH);
  analogWrite(MOTOR3_ENA, motorSpeed);
  
  // Right motors forward
  digitalWrite(MOTOR2_IN3, HIGH);
  digitalWrite(MOTOR2_IN4, LOW);
  analogWrite(MOTOR2_ENB, motorSpeed);
  
  digitalWrite(MOTOR4_IN3, HIGH);
  digitalWrite(MOTOR4_IN4, LOW);
  analogWrite(MOTOR4_ENB, motorSpeed);
}

void turnRight() {
  // Left motors forward
  digitalWrite(MOTOR1_IN1, HIGH);
  digitalWrite(MOTOR1_IN2, LOW);
  analogWrite(MOTOR1_ENA, motorSpeed);
  
  digitalWrite(MOTOR3_IN1, HIGH);
  digitalWrite(MOTOR3_IN2, LOW);
  analogWrite(MOTOR3_ENA, motorSpeed);
  
  // Right motors backward
  digitalWrite(MOTOR2_IN3, LOW);
  digitalWrite(MOTOR2_IN4, HIGH);
  analogWrite(MOTOR2_ENB, motorSpeed);
  
  digitalWrite(MOTOR4_IN3, LOW);
  digitalWrite(MOTOR4_IN4, HIGH);
  analogWrite(MOTOR4_ENB, motorSpeed);
}

void stopAllMotors() {
  // Stop all motors
  analogWrite(MOTOR1_ENA, 0);
  analogWrite(MOTOR2_ENB, 0);
  analogWrite(MOTOR3_ENA, 0);
  analogWrite(MOTOR4_ENB, 0);
}

