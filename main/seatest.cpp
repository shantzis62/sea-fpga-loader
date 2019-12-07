#include <bitset>
#include <cmath>
#include <stdio.h>
#include <vector>
#include <cstring>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"

const char *SEATEST_TAG = "SEATEST";

#define FPGA_CCLK_PIN GPIO_NUM_17
#define FPGA_CCLK_PIN_SEL GPIO_SEL_17
#define FPGA_DIN_PIN GPIO_NUM_27
#define FPGA_DIN_PIN_SEL GPIO_SEL_27
#define FPGA_PROGRAM_B_PIN GPIO_NUM_25
#define FPGA_PROGRAM_B_PIN_SEL GPIO_SEL_25
#define FPGA_INT_B_PIN GPIO_NUM_26
#define FPGA_INT_B_PIN_SEL GPIO_SEL_26
#define FPGA_DONE_PIN GPIO_NUM_34
#define FPGA_DONE_PIN_SEL GPIO_SEL_34


void WriteToFpga(const uint8_t* data, size_t size) {
  // Initialize pointer to configuration. See:
  // http://www.pldtool.com/pdf/fmt_xilinxbit.pdf

  // Skip Field 1.
  const uint8_t* ptr = data;
  ptr += 11;

  // Skip Field 2.
  ptr += 3;

  // Skip Field 3
  const uint16_t field_3_size = ptr[1] | (ptr[0] << 8);
  ptr += 2;
  ESP_LOGI(SEATEST_TAG, "Field 3: (%d) %s", field_3_size, ptr);
  ptr += field_3_size;

  // Skip Field 4 (first "Field 4" in PDF).
  ptr += 1;
  const uint16_t field_4_size = ptr[1] | (ptr[0] << 8);
  ptr += 2;
  ESP_LOGI(SEATEST_TAG, "Field 4: %s", ptr);
  ptr += field_4_size;

  // Skip Field 5 (second "Field 4" in PDF).
  ptr += 1;
  const uint16_t field_5_size = ptr[1] | (ptr[0] << 8);
  ptr += 2;
  ESP_LOGI(SEATEST_TAG, "Field 5: %s", ptr);
  ptr += field_5_size;

  // Skip Field 6 ("Field 5" in PDF).
  ptr += 1;
  const uint16_t field_6_size = ptr[1] | (ptr[0] << 8);
  ptr += 2;
  ESP_LOGI(SEATEST_TAG, "Field 6: (%d) %s", field_6_size, ptr);
  ptr += field_6_size;

  // Get Config out of Field 7 ("Field 6" in PDF).
  ptr += 1;
  const uint32_t config_size =
      ptr[3] | (ptr[2] << 8) | (ptr[1] << 16) | (ptr[0] << 24);
  ptr += 4;
  const uint8_t *config_ptr = ptr;
  ESP_LOGI(SEATEST_TAG, "FPGA config_size: %d", config_size);

  // Transfer configuration from memory to FPGA. Please see:
  // https://www.xilinx.com/support/documentation/user_guides/ug470_7Series_Config.pdf
  // "Serial Configuration Mode - Slave Serial Configuration" (P 38).
  // Initialize inputs.
  gpio_config_t input_io_conf;
  std::memset(&input_io_conf, 0, sizeof(input_io_conf));
  input_io_conf.mode =  GPIO_MODE_INPUT;
  input_io_conf.pin_bit_mask = FPGA_INT_B_PIN_SEL | FPGA_DONE_PIN_SEL;
  input_io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  input_io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&input_io_conf);

  // Initialize outputs.
  gpio_config_t output_io_conf;
  std::memset(&output_io_conf, 0, sizeof(output_io_conf));
  output_io_conf.mode = GPIO_MODE_OUTPUT;
  output_io_conf.pin_bit_mask =
      FPGA_CCLK_PIN_SEL | FPGA_PROGRAM_B_PIN_SEL | FPGA_DIN_PIN_SEL;
  output_io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  output_io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&output_io_conf);

  // FPGA configuration start sign
  gpio_set_level(FPGA_PROGRAM_B_PIN, 0);
  gpio_set_level(FPGA_CCLK_PIN, 0);
  gpio_set_level(FPGA_PROGRAM_B_PIN, 1);

  // Wait until fpga reports reset complete
  while (gpio_get_level(FPGA_INT_B_PIN)  == 0) {
  }

  for (int i = 0; i < config_size; i++) {
    const std::bitset<8> config_bits(config_ptr[i]);
    for (int j = 0; j < 8; j++) {
        gpio_set_level(FPGA_CCLK_PIN, 0);
        gpio_set_level(FPGA_DIN_PIN, config_bits[7 - j]);
        gpio_set_level(FPGA_CCLK_PIN, 1);
    }
  }
  gpio_set_level(FPGA_CCLK_PIN, 0);

  if (gpio_get_level(FPGA_DONE_PIN)) {
    ESP_LOGI(SEATEST_TAG, "FPGA Configuration: SUCCESS");
  } else {
    ESP_LOGI(SEATEST_TAG, "FPGA Configuration: FAILURE");
  }
}

void MainLoop(void *arg) {
  extern const uint8_t blinky_bit_start[] asm("_binary_Blinky_bit_start");
  extern const uint8_t blinky_bit_end[]   asm("_binary_Blinky_bit_end");
  WriteToFpga(blinky_bit_start, blinky_bit_end - blinky_bit_start);
  while (true) {
  }
}


extern "C" {
void app_main() {
  xTaskCreatePinnedToCore(MainLoop, "MainLoop", 2048, NULL, 5, NULL, 1);
}
}
