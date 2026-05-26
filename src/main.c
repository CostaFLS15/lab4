/************************************************************************************************
Copyright (c) 2022-2023, Laboratorio de Microprocesadores
Facultad de Ciencias Exactas y Tecnología, Universidad Nacional de Tucumán
https://www.microprocesadores.unt.edu.ar/

Copyright (c) 2022-2023, Esteban Volentini <evolentini@herrera.unt.edu.ar>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

/** \brief EDU-CIAA-NXP board sample application
 **
 ** \addtogroup samples Samples
 ** \brief Samples applications with MUJU Framwork
 ** @{ */

/* === Headers files inclusions =============================================================== */

#ifndef EDU_CIAA_NXP
#error "This program can only be compiled for the EDU-CIAA-NXP board"
#endif

#include "board.h"
#include "placa.h"
#include "digital.h"
#include <stdio.h>

/* === Macros definitions ====================================================================== */



/* === Private data type declarations ========================================================== */

/**
 * @brief Enumeration with color sequence of RGB led
 */
typedef enum rgb_color_e {
    LED_RED_ON = 0,
    LED_RED_OFF,
    LED_GREEN_ON,
    LED_GREEN_OFF,
    LED_BLUE_ON,
    LED_BLUE_OFF,
} rgb_color_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/**
 * @brief Function to flash RGB led in sequence
 */
static void FlashLed(board_t placa);

/**
 * @brief Function to switch on and off a led with two keys
 */
static void SwitchLed(board_t placa);

/**
 * @brief Function to switch on and off a led with a single key
 */
static void ToggleLed(board_t placa);

/**
 * @brief Function to turn on a led while a key is pressed
 */
static void TestLed(board_t placa);

/**
 * @brief Function to generate a delay of approximately 100 ms
 */
static void Delay(void);

/* === Public variable definitions ============================================================= */
/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */



static void FlashLed(board_t placa) {
    static int divisor = 0;
    static rgb_color_t state = LED_BLUE_OFF;
    

    divisor++;
    if (divisor == 5) {
        divisor = 0;
        state = (state + 1) % (LED_BLUE_OFF + 1);

        switch (state) {
        case LED_RED_ON:
            digital_output_activate(placa->led_rgb_rojo);
            break;
        case LED_GREEN_ON:
            digital_output_activate(placa->led_rgb_verde);
            break;
        case LED_BLUE_ON:
            digital_output_activate(placa->led_rgb_azul);
            break;
        default:
            digital_output_deactivate(placa->led_rgb_rojo);
            digital_output_deactivate(placa->led_rgb_verde);
            digital_output_deactivate(placa->led_rgb_azul);
            break;
        }
    }
}

static void SwitchLed(board_t placa) {
    if (digital_input_get_state(placa->tecla_prender)) {
        digital_output_activate(placa->led_amarillo);
    }
    if (digital_input_get_state(placa->tecla_apagar)) {
        digital_output_deactivate(placa->led_amarillo);
    }
}

static void ToggleLed(board_t placa) {
    if (digital_input_has_activated(placa->tecla_cambiar)) {
        digital_output_toggle(placa->led_rojo);
    }
}

static void TestLed(board_t placa) {
    if (digital_input_get_state(placa->tecla_probar)) {
        digital_output_activate(placa->led_verde);
    } else {
        digital_output_deactivate(placa->led_verde);   
    }
}

static void Delay(void) {
    for (int index = 0; index < 100; index++) {
        for (int delay = 0; delay < 25000; delay++) {
            __asm("NOP");
        }
    }
}

/* === Public function implementation ========================================================== */

int main(void) {
    board_t placa = board_create();
    while (true) {
        FlashLed(placa);
        SwitchLed(placa);
        ToggleLed(placa);
        TestLed(placa);

        Delay();
    }

    return 0;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
