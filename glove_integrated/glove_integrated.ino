/*
 * Gesture-to-Speech Translator Glove — Integrated Build
 *
 * Fuses a flex sensor (finger bend, 10-bit ADC on A0) with an MPU6050 IMU
 * (palm orientation from the gravity vector, read over I2C). The resulting
 * gesture combination is mapped to a pre-recorded clip played through a
 * DFRobot DFPlayer Mini MP3 module.
 */
#include "I2Cdev.h"
#include "MPU6050.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// DFPlayer Mini setup
SoftwareSerial playerSerial(10, 11); // RX, TX
DFRobotDFPlayerMini audioPlayer;

// Define MPU6050 instance with default I2C address (0x68)
MPU6050 imu;

// Enable human-readable serial output of the raw IMU values
#define OUTPUT_READABLE_ACCELGYRO

// Detection thresholds (calibrated experimentally)
const int FLEX_PIN = A0;
const int FLEX_THRESHOLD = 700;          // of 1023 ADC counts (~68% of full scale)
const int ORIENTATION_THRESHOLD = 15000; // LSB on the Y axis (~0.9 g at +/-2 g)

// Raw IMU readings
int16_t ax, ay, az; // Accelerometer
int16_t gx, gy, gz; // Gyroscope

// Sensor activation flags
int flexActive = 0;
int palmRaised = 0;

int lastGesture = 0; // Last gesture played, so a held gesture is not retriggered
bool ledState = false; // On-board LED state, toggled once per loop

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
    
    // Initialize MPU6050
    imu.initialize();
    if (!imu.testConnection()) {
        Serial.println("MPU6050 connection failed");
        while (true);
    } else {
        Serial.println("MPU6050 connection successful");
    }

    // Configure MPU offsets (adjust as needed)
    imu.setXAccelOffset(0);
    imu.setYAccelOffset(0);
    imu.setZAccelOffset(0);
    imu.setXGyroOffset(0);
    imu.setYGyroOffset(0);
    imu.setZGyroOffset(0);

    // Configure on-board LED
    pinMode(LED_BUILTIN, OUTPUT);

    // Initialize DFPlayer Mini
    playerSerial.begin(9600);
    if (!audioPlayer.begin(playerSerial)) {
        Serial.println("Unable to initialize DFPlayer Mini!");
        while (true);
    }
    Serial.println("DFPlayer Mini online.");
    audioPlayer.volume(30); // Set volume (0-30)
}

void loop() {
    // Flex sensor logic
    int val = analogRead(FLEX_PIN);
    Serial.println(val, DEC);
    if (val >= FLEX_THRESHOLD) {
        Serial.println("Flex sensor activated");
        flexActive = 1;
    } else {
        Serial.println("Flex sensor idle");
        flexActive = 0;
    }

    // MPU6050 logic
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    #ifdef OUTPUT_READABLE_ACCELGYRO
        Serial.print(ay); Serial.print("\n");
    #endif

    if (ay > ORIENTATION_THRESHOLD) {
        Serial.println("MPU activated");
        palmRaised = 1;
    } else {
        Serial.println("MPU idle");
        palmRaised = 0;
    }

    // Map the sensor flags to a gesture ID (0 = no gesture recognised)
    int gesture = 0;
    if (flexActive == 1 && palmRaised == 0) {
        gesture = 1;
    } else if (flexActive == 0 && palmRaised == 1) {
        gesture = 2;
    } else if (flexActive == 1 && palmRaised == 1) {
        gesture = 3;
    }

    // Trigger playback only when the gesture changes, so that holding a
    // gesture plays its clip once instead of restarting it every loop.
    if (gesture != 0 && gesture != lastGesture) {
        audioPlayer.play(gesture); // Plays 000<gesture>.mp3
        Serial.print("Playing clip ");
        Serial.println(gesture);
    }
    lastGesture = gesture;

    // Blink LED
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);

    // Prevent looping too fast
    delay(1000);
}
