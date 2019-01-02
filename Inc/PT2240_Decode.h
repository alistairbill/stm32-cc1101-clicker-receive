#ifndef PT2240_DECODE_H_
#define PT2240_DECODE_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef enum {
    PT2240_INIT, PT2240_AGC_OK, PT2240_AGC_FAIL, PT2240_FAIL, PT2240_OK 
} PT2240_STATE;

typedef struct {
    int rawTimerData[32];
    uint8_t decoded[4];

    PT2240_STATE state;
    TIM_HandleTypeDef *timerHandle;

    uint32_t timerChannel;
    HAL_TIM_ActiveChannel timerChannelActive;

    uint16_t timingBitBoundary;
    uint16_t timingAgcBoundary;

    void (*DecodedCallback)(uint16_t, uint16_t);
    void (*ErrorCallback)();
    void (*RepeatCallback)();
} PT2240;

void PT2240_Init(PT2240 *handle);
void PT2240_DeInit(PT2240 *handle);
void PT2240_TIM_IC_CaptureCallback(PT2240 *handle);
void PT2240_Read(PT2240 *handle);

#endif