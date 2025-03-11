#include "RP2040.h"
#include "can2040.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdint.h>

static struct can2040 cbus;

static void can2040_cb(struct can2040 *cd, uint32_t notify,
                       struct can2040_msg *msg) {
  printf("callback hit\n");
}

static void PIOx_IRQHandler(void) { can2040_pio_irq_handler(&cbus); }

void canbus_setup(void) {
  uint32_t pio_num = 0;
  uint32_t sys_clock = 125000000, bitrate = 500000;
  uint32_t gpio_rx = 19, gpio_tx = 18;

  // Setup canbus
  can2040_setup(&cbus, pio_num);
  can2040_callback_config(&cbus, can2040_cb);

  // Enable irqs
  irq_set_exclusive_handler(PIO0_IRQ_0_IRQn, PIOx_IRQHandler);
  NVIC_SetPriority(PIO0_IRQ_0_IRQn, 1);
  NVIC_EnableIRQ(PIO0_IRQ_0_IRQn);

  // Start canbus
  can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

void main(void) {
  stdio_init_all();
  setup_default_uart();

  canbus_setup();
  printf("entering idle\n");

  struct can2040_msg msg;
  msg.id = 0x033;
  msg.dlc = 8;
  msg.data[0] = 1;
  msg.data[1] = 1;
  msg.data[2] = 1;
  msg.data[3] = 1;
  msg.data[4] = 1;
  msg.data[5] = 1;
  msg.data[6] = 1;
  msg.data[7] = 1;

  printf("retval %d\n", can2040_transmit(&cbus, &msg));

  while (1) {
  }
}
