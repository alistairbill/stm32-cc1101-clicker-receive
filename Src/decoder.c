#include "decoder.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define SYNC_HIGH_US 45
#define SYNC_LOW_US 1500
#define BIT_ONE_HIGH_US 150
#define BIT_ONE_LOW_US 45
#define BIT_ZERO_HIGH_US 45
#define BIT_ZERO_LOW_US 150

extern TIM_HandleTypeDef htim3;

typedef enum
{
  RX_SYNC,
  RX_PAYLOAD
} rx_mode;

rx_mode mode = RX_SYNC;
uint32_t last_pulse_width = 0;
size_t bits = 0;
uint32_t data = 0;
uint32_t times[1024];
size_t counter = 0;
bool ignore_edge = false;
bool initialized = false;

void Decoder_Init()
{
  HAL_TIM_Base_Start(&htim3);
  initialized = true;
}

void Data_Received(void)
{
  printf("%lu", data);
}

static int abs(int a) 
{
  return (a < 0) ? -a : a;
}

/* Is a == (b +/- b/4)? */
static bool approx_eq(uint32_t a, uint32_t b)
{
  int tol = (b < 4) ? 1 : (b / 4);
  return (abs(a - b) <= tol);
}

static void sync(uint32_t high_pulse_width, uint32_t low_pulse_width)
{
  if(approx_eq(high_pulse_width, SYNC_HIGH_US) && approx_eq(low_pulse_width, SYNC_LOW_US)) {
      // ignore this pulse but start pushing data on the next pulse
      mode = RX_PAYLOAD;
  }
}

static void payload(uint32_t high_pulse_width, uint32_t low_pulse_width)
{
  if (approx_eq(high_pulse_width, BIT_ONE_HIGH_US) && approx_eq(low_pulse_width, BIT_ONE_LOW_US)) {
    // one bit
    data |= (1UL << bits++);
  } else if (approx_eq(high_pulse_width, BIT_ZERO_HIGH_US) && approx_eq(low_pulse_width, BIT_ZERO_LOW_US)) {
    // zero bit
    data |= (0UL << bits++);
  } else {
    // fail! look for a sync bit again
    mode = RX_SYNC;
  }
  if (bits >= 24) {
    Data_Received();
    // reset the state machine
    bits = 0;
    data = 0;
    mode = RX_SYNC;
  }
}

void Decoder_Edge_Callback(void)
{
  if (!initialized) {
    return;
  }
  if (ignore_edge) {
    ignore_edge = false;
    return;
  }
  uint32_t pulse_width = __HAL_TIM_GetCounter(&htim3);
  if (pulse_width > 30) {
    __HAL_TIM_SetCounter(&htim3, 0);
  } else {
    // short pulse
    // ignore this and the next edge (and don't reset the timer)
    ignore_edge = true;
    return;
  }
  bool state = (GPIO_PIN_SET == HAL_GPIO_ReadPin(RF_Pin_GPIO_Port, RF_Pin_Pin));
  if (state)
  {
    // rising edge, the pulse width is the width of the previous low pulse
    switch (mode) {
    case RX_SYNC:
      (void)sync(last_pulse_width, pulse_width);
      break;
    case RX_PAYLOAD:
      (void)payload(last_pulse_width, pulse_width);
      break;
    }
  } else {
    // falling edge, the pulse width is the width of the previous high pulse
    // save this pulse width but don't do anything until we know the length
    // of the low pulse as well
    last_pulse_width = pulse_width;
  }
  times[counter++] = pulse_width;
  if (counter > 1023) counter = 0;
}