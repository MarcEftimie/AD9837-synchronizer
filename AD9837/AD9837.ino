#include <pico/time.h>
#include <hardware/pwm.h>
#include <SPI.h>

// AD9837 SPI configuration
const uint8_t SPI_CS = 5;
const uint8_t SPI_SCK = 18;
const uint8_t SPI_TX = 19;

// AD9837 registers
const uint16_t AD9837_B28 = 0x2000;
const uint16_t AD9837_RESET = 0x100;
const uint16_t AD9837_FREQ0 = 0x4000;
const uint16_t AD9837_PHASE0 = 0xC000;

// Clock Configuration
const uint8_t CLOCK_PIN = 0;
const uint32_t CLOCK_FREQUENCY = 1000000;
const uint32_t PICO_CLOCK_FREQUENCY = 125000000;
const double CLOCK_WRAP = (PICO_CLOCK_FREQUENCY / CLOCK_FREQUENCY);
uint32_t copy_freq;
SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE2);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // Configure clock signal
  gpio_set_function(CLOCK_PIN, GPIO_FUNC_PWM);

  // Get PWM slice for the clock pin
  uint slice_num = pwm_gpio_to_slice_num(CLOCK_PIN);

  pwm_set_enabled(slice_num, true);

  pwm_set_wrap(slice_num, CLOCK_WRAP);

  pwm_set_chan_level(slice_num, PWM_CHAN_A, CLOCK_WRAP / 2);

  // Initialize SPI
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);
  SPI.begin();
  SPI.beginTransaction(spiSettings);

  // Configure AD9837
  uint32_t frequency = 100000; // 100 kHz output frequency
  uint32_t phase = 0; // 0-degree phase

  uint32_t freq_word = (uint32_t)((double)frequency * 268435456.0 / (double)CLOCK_FREQUENCY);
  uint16_t f_lsb = (freq_word & 0x3FFF) | AD9837_FREQ0;
  uint16_t f_msb = ((freq_word >> 14) & 0x3FFF) | AD9837_FREQ0;

  uint16_t p_word = (uint16_t)((double)phase / 360.0 * 4096.0);
  uint16_t p_reg = p_word | AD9837_PHASE0;
  writeRegister(AD9837_RESET); // Reset AD9837
  writeRegister(AD9837_B28); // Enable 28-bit frequency mode
  
  writeRegister(f_lsb); // Write frequency LSB
  writeRegister(f_msb); // Write frequency MSB
  writeRegister(p_reg); // Write phase
  writeRegister(0); // Clear reset bit
}

void loop() {
}

void writeRegister(uint16_t data) {
  digitalWrite(SPI_CS, LOW);
  SPI.transfer16(data);
  digitalWrite(SPI_CS, HIGH);
}
