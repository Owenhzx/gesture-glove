# Gesture-to-Speech Translator Glove

A wearable glove prototype that converts hand gestures into audible speech.
Finger bend and palm orientation are read from two sensors; the resulting
gesture combination is mapped to a pre-recorded voice clip and played through
a speaker.

**C/C++ (Arduino) · Jun – Aug 2024**

## Features

- **Two sensing modalities fused** — a 350 Ω strain-gauge flex sensor read on
  the ADC for finger bend, and an MPU6050 IMU read over I2C for palm
  orientation from the gravity vector.
- **Experimentally calibrated thresholds**, set by logging raw sensor output
  before the detection logic was written.
- **Edge-triggered playback**, so holding a gesture plays its clip once rather
  than restarting it every loop.
- **Seven iterative sketches**, from single-module tests to an integrated build.

## Structure

```
glove_integrated/       Release build — sensing + audio playback
glove_sensors_only/     Both sensors with activation flags, no audio
                        (used to calibrate thresholds)
hardware_tests/
  flex_sensor_test/       Raw flex-sensor ADC readout
  imu_raw_test/           Raw MPU6050 readout
  audio_player_test/      DFPlayer Mini playback test
```

## Hardware

- Arduino board
- MPU6050 IMU — I2C, default address 0x68
- 350 Ω strain-gauge flex sensor — analog pin A0
- DFRobot DFPlayer Mini + speaker — SoftwareSerial on pins 10/11
- microSD card with numbered clips (`0001.mp3`, `0002.mp3`, …)

## Libraries

`I2Cdev`, `MPU6050` (ElectronicCats), `SoftwareSerial`, `DFRobotDFPlayerMini`
