#include "decoder.h"
#include "cc1101.h"
#include "main.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHANGES 68

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

static unsigned int codes[4];
static unsigned int code_count = 0;
static const int receive_tolerance = 60;
static bool initialized = false;
static bool ignore_pulse = false;

static volatile unsigned int change_count = 0;
static volatile unsigned int repeat_count = 0;
static volatile unsigned int timings[MAX_CHANGES];
static volatile unsigned int rolling_buffer = 0;
static volatile unsigned int pulse_width = 0;


static void setup_interrupt_handler_pre(void);
static void rolling_interrupt_handler_post(void);
static void noop(void);

void (*interrupt_handler_pre)(void) = &setup_interrupt_handler_pre;
void (*interrupt_handler_post)(void) = &noop;

void Decoder_Init(void)
{
    CC1101_Init();
    HAL_TIM_Base_Start(&htim3);
    initialized = true;
}

static inline unsigned int diff(const int a, const int b)
{
    return abs(a - b);
}

static void send_message(const unsigned int value, const int p)
{
    if (code_count < 4) {
        codes[code_count++] = value;
        if (code_count == 4) {
            interrupt_handler_pre = &noop;
            interrupt_handler_post = &rolling_interrupt_handler_post;
        }
    }
    switch(p) {
        case 0:
            // PT2240: 20 address bits followed by 4 data bits
            printf("address: %u, cmd: %u", value >> 4, value & 0xf);
            break;
    }
}

static bool setup_receive_protocol(const int p, const unsigned int changes)
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

static bool decode_bit_rolling(const int p)
{
    protocol_t protocol;
    memcpy(&protocol, &protocols[p], sizeof(protocol_t));
    if (diff(timings[change_count] / protocol.one.high, timings[change_count + 1] / protocol.one.low) < 250) {
        rolling_buffer <<= 1;
        rolling_buffer |= 1;
        return true;
    } else if (diff(timings[change_count] / protocol.zero.high, timings[change_count + 1] / protocol.zero.low) < 250) {
        rolling_buffer <<= 1;
        return true;
    }
    return false;
}

static void noop(void)
{
    return;
}

static void rolling_interrupt_handler_post(void)
{
    if (change_count % 2 == 1) {
        int i = 0;
        for (i = 0; i < protocols_len; i++) {
            if (decode_bit_rolling(i)) {
                break;
            }
        }
        if ((rolling_buffer & 0xffffff) == codes[0] || (rolling_buffer & 0xffffff) == codes[1]) {
            send_message(rolling_buffer & 0xffffff, i);
            rolling_buffer = 0;
        }
    }
}

static void setup_interrupt_handler_pre(void)
{
    if (pulse_width > separation_limit) {
        if (diff(pulse_width, timings[0]) < 400) {
            repeat_count++;
            if (repeat_count == 2) {
                for (int i = 0; i < protocols_len; i++){
                    if (setup_receive_protocol(i, change_count)) {
                        break;
                    }
                }
                repeat_count = 0;
            }
        }
        change_count = 0;
    }
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

    pulse_width = __HAL_TIM_GetCounter(&htim3);
    if (pulse_width >= 400) {
        __HAL_TIM_SetCounter(&htim3, 0);
    } else {
        // short pulse
        // ignore this and the next edge (and don't reset the timer)
        ignore_pulse = true;
        return;
    }
    (*interrupt_handler_pre)();

    if (change_count >= MAX_CHANGES) {
        change_count = 0;
        repeat_count = 0;
    }
    timings[change_count++] = pulse_width;
    (*interrupt_handler_post)();
}
