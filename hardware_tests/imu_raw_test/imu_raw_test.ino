/* MPU6050 Raw Data Reader
 * This program reads raw data from the MPU6050 module and supports different output formats.
 * Full documentation: https://github.com/ElectronicCats/mpu6050/wiki
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

    // Print configured offsets
    Serial.print("Offsets:\t");
    Serial.print(imu.getXAccelOffset()); Serial.print("\t");
    Serial.print(imu.getYAccelOffset()); Serial.print("\t");
    Serial.print(imu.getZAccelOffset()); Serial.print("\t");
    Serial.print(imu.getXGyroOffset()); Serial.print("\t");
    Serial.print(imu.getYGyroOffset()); Serial.print("\t");
    Serial.println(imu.getZGyroOffset());

    // Configure on-board LED for output
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    // Read raw accelerometer and gyroscope data
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Output data in the selected format
    #ifdef OUTPUT_READABLE_ACCELGYRO
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.println(gz);
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
        Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
        Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
        Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
        Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
    #endif
    Serial.print("\n");
    // Blink the LED to indicate activity
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);

    // Prevent looping too fast
    while(1);
}
