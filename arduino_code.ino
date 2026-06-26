/*
 * VigilSense - Arduino UNO R4 WiFi Sensor Code
 * Sends sensor data to Raspberry Pi via Serial USB
 * 
 * Pin Configuration:
 * - DHT11: Digital D4 (Temperature & Humidity)
 * - Gas Sensor (MQ): Analog A0
 * - Vibration Sensor: Analog A3
 * - Flame Sensor: Digital D7
 * - Microphone: Digital D9
 * 
 * Install DHT library:
 * Sketch -> Include Library -> Manage Libraries -> Search "DHT sensor library" by Adafruit
 */

#include <DHT.h>

// Pin definitions
#define DHT_PIN 4        // DHT11 on Digital D4
#define GAS_PIN A0       // Gas sensor (MQ) on Analog A0
#define VIBRATION_PIN A3 // Vibration sensor on Analog A3
#define FLAME_PIN 7      // Flame sensor on Digital D7
#define MIC_PIN 9        // Microphone on Digital D9

// DHT11 setup
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Variables for sensor readings
float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;
int vibrationValue = 0;
int flameValue = 0;
int micValue = 0;

// Timing
unsigned long lastRead = 0;
const unsigned long readInterval = 200; // Read sensors every 200ms

void setup() {
  // Initialize serial communication at 9600 baud
  Serial.begin(9600);
  
  // Wait for serial port to connect (for USB serial)
  while (!Serial) {
    delay(10);
  }
  
  // Initialize DHT11 sensor
  dht.begin();
  
  // Configure pin modes
  pinMode(FLAME_PIN, INPUT);
  pinMode(MIC_PIN, INPUT);
  // Analog pins don't need pinMode, but we can set them for clarity
  pinMode(GAS_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  
  // Wait for sensors to stabilize
  delay(2000);
  
  // Send ready message
  Serial.println("VigilSense Arduino Ready");
  Serial.println("{\"status\":\"ready\"}");
}

void loop() {
  // Read sensors at regular intervals
  if (millis() - lastRead >= readInterval) {
    lastRead = millis();
    
    // Read DHT11 (temperature and humidity)
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    
    // Check if DHT11 read failed
    if (isnan(temperature) || isnan(humidity)) {
      temperature = 0.0;
      humidity = 0.0;
    }
    
    // Read analog sensors (0-1023 for 10-bit ADC on UNO)
    gasValue = analogRead(GAS_PIN);
    vibrationValue = analogRead(VIBRATION_PIN);
    
    // Read digital sensors (0 or 1)
    // Flame sensor logic inverted: 0 = flame detected, 1 = no flame
    flameValue = !digitalRead(FLAME_PIN);  // Inverted logic
    micValue = digitalRead(MIC_PIN);
    
    // Create and send JSON data
    Serial.print("{");
    Serial.print("\"temp\":");
    Serial.print(temperature, 1);
    Serial.print(",\"humidity\":");
    Serial.print(humidity, 1);
    Serial.print(",\"gas\":");
    Serial.print(gasValue);
    Serial.print(",\"vibration\":");
    Serial.print(vibrationValue);
    Serial.print(",\"flame\":");
    Serial.print(flameValue);
    Serial.print(",\"sound\":");
    Serial.print(micValue);
    Serial.println("}");
  }
  
  // Small delay to prevent CPU overload
  delay(10);
}

