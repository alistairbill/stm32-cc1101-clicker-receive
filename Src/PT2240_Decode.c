#include "PT2240_Decode.h"

void PT2240_TIM_IC_CaptureCallback(PT2240 *handle)
{
    if (handle->state == PT2240_INIT) {
        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

        if (handle->rawTimerData[1] < handle->timingAgcBoundary) {
            handle->state = PT2240_OK;
            handle->RepeatCallback();
        } else {
            handle->state = PT2240_AGC_OK;
            HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                (uint32_t*) handle->rawTimerData, 32);
        }
    } else if (handle->state == PT2240_AGC_OK) {
        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

        for (int i = 0; i < 32; i++) {
            if (handle->rawTimerData[i] > handle->timingBitBoundary) {
                // 1
                handle->decoded[i / 8] |= 1 << (i % 8);
            } else {
                // 0
                handle->decoded[i / 8] |= ~(1 << (i % 8));
            }
        }
        handle->state = PT2240_OK;
        handle->DecodedCallback(handle->decoded[0], handle->decoded[1]);

    }
}

void PT2240_Read(PT2240 *handle)
{
    handle->state = PT2240_INIT;
    HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
        (uint32_t*)handle->rawTimerData, 2);
}