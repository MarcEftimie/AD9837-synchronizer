const uint8_t SQUARE_WAVE_PIN = 1;
const uint32_t SQUARE_WAVE_FREQUENCY = 100000;

void setup() {
  // Configure square wave pin
  pinMode(SQUARE_WAVE_PIN, OUTPUT);
  analogWriteFreq(SQUARE_WAVE_FREQUENCY);
  analogWrite(SQUARE_WAVE_PIN, 128); // 50% duty cycle (128 / 255)
}

void loop() {
  // No need to do anything in the loop; the square wave is generated automatically
}