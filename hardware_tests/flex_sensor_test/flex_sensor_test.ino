/*
 * Flex Sensor Test — Gesture-to-Speech Translator Glove
 * Reads the 350-ohm strain-gauge flex sensor on analog pin A0
 * and prints the raw 10-bit ADC value for threshold calibration.
 */
const int FLEX_PIN = A0;  // Flex sensor on analog pin A0

void setup()
{
  Serial.begin(9600);  // Open serial port at 9600 bps
}

void loop()
{
  int val = analogRead(FLEX_PIN);
  Serial.println(val, DEC);  // Print the raw 10-bit reading (0-1023)
  delay(100);
}
