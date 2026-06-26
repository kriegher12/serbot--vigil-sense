/*
 * VigilSense - Arduino UNO R4 WiFi Motor Control (WiFi Version)
 * WiFi-controlled car with L293D motor driver and 4 motors
 * Compatible with web browser
 * 
 * NOTE: For Bluetooth control, use arduino_motor_control_ble.ino instead!
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

// WiFi library for Arduino UNO R4 WiFi
// The library name may vary - try WiFi.h or WiFiS3.h
// If WiFiS3.h doesn't work, change to: #include <WiFi.h>
#include <WiFi.h>

// WiFi credentials - CHANGE THESE!
const char* ssid = "Stark954";           // Your WiFi network name
const char* password = "123456789";       // Your WiFi password

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

// Create WiFi server on port 80
WiFiServer server(80);

// Status variables
String wifiStatus = "Disconnected";
IPAddress ipAddress;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  
  Serial.println("VigilSense Motor Control");
  Serial.println("========================");
  
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
  
  // Connect to WiFi
  connectToWiFi();
  
  // Start web server
  server.begin();
  Serial.println("Web server started!");
  Serial.print("Access the car at: http://");
  Serial.println(ipAddress);
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    connectToWiFi();
    delay(1000);
  }
  
  // Listen for incoming clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') {
          // End of line, check if line is blank
          if (currentLine.length() == 0) {
            // Send HTTP response
            sendWebPage(client);
            break;
          } else {
            // Process command
            processCommand(currentLine);
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    // Close connection
    client.stop();
    Serial.println("Client disconnected");
  }
  
  delay(10);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiStatus = "Connected";
    ipAddress = WiFi.localIP();
    Serial.println("");
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(ipAddress);
  } else {
    wifiStatus = "Failed";
    Serial.println("");
    Serial.println("WiFi connection failed!");
    Serial.println("Please check your credentials and try again.");
  }
}

void processCommand(String request) {
  // Process GET requests with commands
  if (request.startsWith("GET /")) {
    // Extract command from URL
    // Format: GET /command?param=value HTTP/1.1
    
    // Forward
    if (request.indexOf("forward") > 0 || request.indexOf("up") > 0) {
      moveForward();
      Serial.println("Command: Forward");
    }
    // Backward
    else if (request.indexOf("backward") > 0 || request.indexOf("down") > 0) {
      moveBackward();
      Serial.println("Command: Backward");
    }
    // Left
    else if (request.indexOf("left") > 0) {
      turnLeft();
      Serial.println("Command: Left");
    }
    // Right
    else if (request.indexOf("right") > 0) {
      turnRight();
      Serial.println("Command: Right");
    }
    // Stop
    else if (request.indexOf("stop") > 0) {
      stopAllMotors();
      Serial.println("Command: Stop");
    }
    // Speed up
    else if (request.indexOf("speedup") > 0) {
      motorSpeed = min(255, motorSpeed + 20);
      Serial.print("Speed increased to: ");
      Serial.println(motorSpeed);
    }
    // Speed down
    else if (request.indexOf("speeddown") > 0) {
      motorSpeed = max(100, motorSpeed - 20);
      Serial.print("Speed decreased to: ");
      Serial.println(motorSpeed);
    }
  }
}

void sendWebPage(WiFiClient client) {
  // Send HTTP headers
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  
  // Send HTML page
  client.println("<!DOCTYPE html>");
  client.println("<html><head>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>VigilSense Car Control</title>");
  client.println("<style>");
  client.println("body { font-family: Arial; background: #000; color: #0f0; text-align: center; }");
  client.println(".container { max-width: 400px; margin: 50px auto; }");
  client.println("button { width: 100px; height: 100px; font-size: 20px; margin: 5px; border: 2px solid #0f0; background: #000; color: #0f0; border-radius: 10px; }");
  client.println("button:active { background: #0f0; color: #000; }");
  client.println(".control { margin: 20px 0; }");
  client.println(".status { margin: 20px; padding: 10px; border: 1px solid #0f0; }");
  client.println("</style>");
  client.println("</head><body>");
  client.println("<div class='container'>");
  client.println("<h1>🚗 VigilSense Car Control</h1>");
  client.print("<div class='status'>WiFi: ");
  client.print(wifiStatus);
  client.print(" | IP: ");
  client.print(ipAddress);
  client.println("</div>");
  
  // Control buttons
  client.println("<div class='control'>");
  client.println("<button onclick='sendCmd(\"forward\")'>⬆️</button>");
  client.println("</div>");
  client.println("<div class='control'>");
  client.println("<button onclick='sendCmd(\"left\")'>⬅️</button>");
  client.println("<button onclick='sendCmd(\"stop\")'>⏹️</button>");
  client.println("<button onclick='sendCmd(\"right\")'>➡️</button>");
  client.println("</div>");
  client.println("<div class='control'>");
  client.println("<button onclick='sendCmd(\"backward\")'>⬇️</button>");
  client.println("</div>");
  client.println("<div class='control'>");
  client.println("<button onclick='sendCmd(\"speedup\")'>⚡+</button>");
  client.println("<button onclick='sendCmd(\"speeddown\")'>⚡-</button>");
  client.println("</div>");
  
  client.println("<script>");
  client.println("function sendCmd(cmd) {");
  client.print("  fetch('/");
  client.print("' + cmd).catch(e => console.log(e));");
  client.println("}");
  client.println("</script>");
  client.println("</body></html>");
  client.println();
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

