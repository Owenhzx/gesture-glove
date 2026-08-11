/*
 * Glove Sensors Only — Gesture-to-Speech Translator Glove
 * Intermediate build: reads the flex sensor and the MPU6050 IMU
 * together and prints activation flags, without audio output.
 * Used to calibrate both detection thresholds before integrating
 * the DFPlayer Mini audio module.
 */
#include "I2Cdev.h"
#include "MPU6050.h"

// Define MPU6050 instance with default I2C address (0x68)
MPU6050 imu;

// Select output format: human-readable or binary (uncomment one)
#define OUTPUT_READABLE_ACCELGYRO
// #define OUTPUT_BINARY_ACCELGYRO

// Variables for storing accelerometer and gyroscope data
int16_t ax, ay, az; // Accelerometer data
int16_t gx, gy, gz; // Gyroscope data
bool ledState;    // LED state
int palmRaised;

// Detection thresholds (calibrated experimentally)
const int FLEX_PIN = A0;
const int FLEX_THRESHOLD = 700;          // of 1023 ADC counts (~68% of full scale)
const int ORIENTATION_THRESHOLD = 15000; // LSB on the Y axis (~0.9 g at +/-2 g)

void setup() {
    // Initialize I2C communication
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // Initialize Serial communication
    Serial.begin(9600);
    Serial.println("Initializing MPU...");

    // Initialize MPU6050 and check connection
    imu.initialize();
    if (!imu.testConnection()) {
        Serial.println("MPU6050 connection failed");
        while (true); // Stop if connection fails
    } else {
        Serial.println("MPU6050 connection successful");
    }

    // Set accelerometer and gyroscope offsets (use MPU6050_Zero for calibration)
    imu.setXAccelOffset(0);
    imu.setYAccelOffset(0);
    imu.setZAccelOffset(0);
    imu.setXGyroOffset(0);
    imu.setYGyroOffset(0);
    imu.setZGyroOffset(0);

    // Configure on-board LED for output
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Flex sensor logic
  int val, flexActive;
  val = analogRead(FLEX_PIN);
  Serial.println(val, DEC);
  if (val >= FLEX_THRESHOLD) {
    Serial.println("Flex sensor activated");
    flexActive = 1;
  }
  else {
    Serial.println("Flex sensor idle");
    flexActive = 0;
  }

  // MPU6050 logic
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Output data in the selected format
  #ifdef OUTPUT_READABLE_ACCELGYRO
      Serial.print(ay); Serial.print("\n");
  #endif

  #ifdef OUTPUT_BINARY_ACCELGYRO
      Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
  #endif

  if (ay > ORIENTATION_THRESHOLD) {
    Serial.println("MPU activated");
    palmRaised = 1;
  }
  else {
    Serial.println("MPU idle");
    palmRaised = 0;
  }
  Serial.print("\n");

  // Blink the LED to indicate activity
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState);

  // Prevent looping too fast
  delay(2000);
}
