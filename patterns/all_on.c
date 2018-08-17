/* Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * Modified by David Turner, 2018 <git@dwt27.co.uk>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.  2.
 *     Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.  3.
 *     Neither the name of the owner nor the names of its contributors may be
 *     used to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "../ws2811.h"

#define LED_COUNT 150


static void leds_clear(ws2811_led_t *leds) {
    for (int x = 0; x < LED_COUNT; x++) {
        leds[x] = 0;
    }
}

static void leds_update(ws2811_led_t *leds) {
    for(int i = 0; i < LED_COUNT; i++)
        leds[i] = 0x00ffffff;
}

static uint8_t running = 1;
static void ctrl_c_handler(int signum) {
    (void)(signum);
    running = 0;
}

static void setup_handlers(void) {
    struct sigaction sa = {
        .sa_handler = ctrl_c_handler,
    };

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int main() {
    ws2811_return_t ret = 0;

    setup_handlers();

    ws2811_t ledstring = {
        .freq = WS2811_TARGET_FREQ,
        .dmanum = 10,
        .channel = {
            [0] = {
                .gpionum = 18,
                .count = LED_COUNT,
                .invert = 0,
                .brightness = 255,
                .strip_type = WS2811_STRIP_GBR,
            },
            [1] = {
                .gpionum = 0,
                .count = 0,
                .invert = 0,
                .brightness = 0,
            },
        },
    };
    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS) {
        fprintf(stderr, "ws2811_init failed: %s\n",
                ws2811_get_return_t_str(ret));
        return ret;
    }

    while (running) {
        leds_update(ledstring.channel[0].leds);

        if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS) {
            fprintf(stderr, "ws2811_render failed: %s\n",
                    ws2811_get_return_t_str(ret));
            break;
        }

        usleep(1000000 / 1); // 1 frames /sec
    }

    // Clear before exiting
    leds_clear(ledstring.channel[0].leds);
    ws2811_render(&ledstring);
    ws2811_fini(&ledstring);

    return ret;
}
