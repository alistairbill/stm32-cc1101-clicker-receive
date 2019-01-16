#include "decoder.h"
#include "cc1101.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHANGES 67

extern TIM_HandleTypeDef htim3;

typedef struct {
    uint8_t high;
    uint8_t low;
} pulse_t;

typedef struct {
    uint8_t bitlength;
    pulse_t sync;
    pulse_t zero;
    pulse_t one;
} protocol_t;

static const unsigned int separation_limit = 3000;
static const protocol_t protocols[] = {
    { 24, { 1, 31 }, {1, 3}, {3, 1} } // PT2240
};
static const unsigned int protocols_len = sizeof(protocols) / sizeof(protocols[0]);

static const int receive_tolerance = 60;
static bool initialized = false;
static bool ignore_pulse = false;

static volatile unsigned int change_count = 0;
static volatile unsigned int repeat_count = 0;
static volatile unsigned int timings[MAX_CHANGES];

void Decoder_Init(void)
{
    CC1101_Init();
    HAL_TIM_Base_Start(&htim3);
    initialized = true;
}

static inline unsigned int diff(int a, int b)
{
    return abs(a - b);
}

static void send_message(unsigned int value, int p)
{
    switch(p) {
        case 0:
            // PT2240: 20 address bits followed by 4 data bits
            printf("address: %u, cmd: %u", value >> 4, value & 0xf);
            break;
    }
}

static bool receive_protocol(const int p, unsigned int changes)
{
    protocol_t protocol;
    memcpy(&protocol, &protocols[p], sizeof(protocol_t));
    unsigned int code = 0;
    const int delay = timings[0] / protocol.sync.low;
    const int delay_tolerance = delay * receive_tolerance / 100;
    for (unsigned int i = 1; i < changes - 1; i += 2) {
        code <<= 1;
        if (diff(timings[i], delay * protocol.zero.high) < delay_tolerance &&
            diff(timings[i + 1], delay * protocol.zero.low) < delay_tolerance) {
            // zero
        } else if (diff(timings[i], delay * protocol.one.high) < delay_tolerance &&
                   diff(timings[i + 1], delay * protocol.one.low) < delay_tolerance) {
            // one
            code |= 1;
        } else {
            return false;
        }
    }

    if ((changes - 1) / 2 == protocol.bitlength) {
        send_message(code, p);
        return true;
    }

    return false;
}

void Decoder_Edge_Callback(void)
{
    if (!initialized) {
        return;
    }

    if (ignore_pulse) {
        ignore_pulse = false;
        return;
    }

    uint32_t pulse_width = __HAL_TIM_GetCounter(&htim3);
    if (pulse_width >= 100) {
        __HAL_TIM_SetCounter(&htim3, 0);
    } else {
        // short pulse
        // ignore this and the next edge (and don't reset the timer)
        ignore_pulse = true;
        return;
    }

    if (pulse_width > separation_limit) {
        if (diff(pulse_width, timings[0]) < 400) {
            repeat_count++;
            if (repeat_count == 2) {
                for (int i = 0; i < protocols_len; i++){
                    if (receive_protocol(i, change_count)) {
                        break;
                    }
                }
                repeat_count = 0;
            }
        }
        change_count = 0;
    }
    if (change_count >= MAX_CHANGES) {
        change_count = 0;
        repeat_count = 0;
    }
    timings[change_count++] = pulse_width;
}