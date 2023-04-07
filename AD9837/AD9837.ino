#include <pico/time.h>
#include <hardware/pwm.h>
#include <SPI.h>

// AD9837 SPI configuration
const uint8_t SPI_CS = 5;
const uint8_t SPI_SCK = 18;
const uint8_t SPI_TX = 19;

// AD9837 registers
const uint16_t AD9837_RESET = 0x100;
const uint16_t AD9837_B28 = 0x2000;
const uint16_t AD9837_FREQ0 = 0x4000;
const uint16_t AD9837_PHASE0 = 0xC000;

// Clock Configuration
const uint8_t CLOCK_PIN = 0;
const uint32_t CLOCK_FREQUENCY = 4000000;
const uint32_t PWM_CHANNEL_FREQUENCY = 125000000;
const double CLOCK_WRAP = (PWM_CHANNEL_FREQUENCY / CLOCK_FREQUENCY);

// Frequency and Phase Configuration
uint32_t FREQUENCY = 50000; // Hz
uint32_t PHASE = 0; // Degrees

SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE2);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize Clock
  gpio_set_function(CLOCK_PIN, GPIO_FUNC_PWM); // Enable GPIO port to PWM

  uint slice_num = pwm_gpio_to_slice_num(CLOCK_PIN); // Get PWM channel number

  pwm_set_enabled(slice_num, true); // Turn on PWM on #slice_num channel

  pwm_set_wrap(slice_num, CLOCK_WRAP); // Set clock period

  pwm_set_chan_level(slice_num, PWM_CHAN_A, CLOCK_WRAP / 2); // Set 50% duty cycle

  // Initialize SPI
  pinMode(SPI_CS, OUTPUT);

  digitalWrite(SPI_CS, HIGH);
  
  SPI.begin();
  SPI.beginTransaction(spiSettings);

  // Configure AD9837
  uint32_t frequency_word = (uint32_t)((double)FREQUENCY * 268435456.0 / (double)CLOCK_FREQUENCY);
  uint16_t frequency_lsb = (frequency_word & 0x3FFF) | AD9837_FREQ0;
  uint16_t frequency_msb = ((frequency_word >> 14) & 0x3FFF) | AD9837_FREQ0;

  uint16_t phase_word = (uint16_t)((double)PHASE / 360.0 * 4096.0);
  uint16_t phase_reg = phase_word | AD9837_PHASE0;

  // Set frequency and phase of first AD9837
  digitalWrite(SPI_CS, LOW);
  SPI.transfer16(AD9837_RESET);
  digitalWrite(SPI_CS, HIGH);
  writeRegister(AD9837_B28, true); // Enable 28-bit frequency mode
  writeRegister(frequency_lsb, true); // Write frequency LSB
  writeRegister(frequency_msb, true); // Write frequency MSB
  writeRegister(phase_reg, true); // Write phase
  writeRegister(0, true); // Clear reset bit

  // Set frequency and phase of second AD9837
  writeRegister(AD9837_B28, false); // Enable 28-bit frequency mode
  writeRegister(frequency_lsb, false); // Write frequency LSB
  writeRegister(frequency_msb, false); // Write frequency MSB
  writeRegister(phase_reg, false); // Write phase
  writeRegister(0, false); // Clear reset bit
}

void loop() {
}

void writeRegister(uint16_t data, bool first_ad9837) {
  if (first_ad9837) {
    digitalWrite(SPI_CS, LOW);
    SPI.transfer16(data);
    digitalWrite(SPI_CS, HIGH);
  } else {
    digitalWrite(SPI_CS, LOW);
    SPI.transfer16(data);
    digitalWrite(SPI_CS, HIGH);
  }
}
