/*********************************************************************************************************************
Copyright (c) 2026, Costa, Francisco Lucas Sebastian <correo@ejemplo.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*********************************************************************************************************************/

/** @file screen.c
 ** @brief display para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "screen.h"
#include <string.h>


/* === Macros definitions ========================================================================================== */
#ifndef DISPLAY_MAX_DIGITS
#define DISPLAY_MAX_DIGITS 8
#endif
/* === Private data type declarations ============================================================================== */
struct display_s{
    uint8_t digits;
    uint8_t active_digit;
    uint8_t flashing_from;
    uint8_t flashing_to;
    uint8_t display_memory[DISPLAY_MAX_DIGITS];
    uint16_t flashing_frequency;
    uint16_t flashing_count;
    struct display_driver_s driver[1];
};
/* === Private function declarations =============================================================================== */
static display_t display_allocate(void);


/* === Private variable definitions ================================================================================ */
static const uint8_t IMAGES[10] = {
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F,             // 0
    SEGMENT_B | SEGMENT_C,                                                             // 1
    SEGMENT_A | SEGMENT_B | SEGMENT_D | SEGMENT_E | SEGMENT_G,                         // 2
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_G,                         // 3
    SEGMENT_B | SEGMENT_C | SEGMENT_F | SEGMENT_G,                                     // 4
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G,                         // 5
    SEGMENT_A | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G,             // 6
    SEGMENT_A | SEGMENT_B | SEGMENT_C,                                                 // 7
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_E | SEGMENT_F | SEGMENT_G, // 8
    SEGMENT_A | SEGMENT_B | SEGMENT_C | SEGMENT_D | SEGMENT_F | SEGMENT_G              // 9
};

/* === Public variable definitions ================================================================================= */

/* === Private function implementation ================================================================================ */
static display_t DisplayAllocate(void) {
    static struct display_s instances[1];
    static bool used = false;
    if (used) return NULL;
    used = true;
    return &instances[0];
}
/* === Public function implementation ============================================================================== */
display_t DisplayCreate(uint8_t digits, display_driver_t driver) {
    display_t display = display_allocate();
    if (display) {
        display->digits = digits;
        display->active_digit = digits-1;
        display->flashing_count = 0;
        display->flashing_from = 0;
        display->flashing_to = 0;
        display->flashing_frequency = 0;
        memcpy(display->driver, driver, sizeof(display->driver));
        memcpy(display->display_memory, 0, sizeof(display->driver));
        display->driver->update_segments(0x00);
    }

    return display;
}
void DisplayWriteBCD(display_t display, uint8_t * number, uint8_t size) {
    memset(display->display_memory, 0, sizeof(display->display_memory));
    for (int index = 0; index < size; index++) {
        if (index >= display->digits) break;
        display->display_memory[index] = IMAGES[number[index] % 10];
    }
}
void DisplayRefresh(display_t display) {
    uint8_t segments;
    bool flashing_active = false;
 
    /* Apagar todo antes de cambiar de dígito (evita ghosting) */
    display->driver->update_segments(0x00);
    display->driver->update_digits(0x00);
 
    /* Avanzar al siguiente dígito */
    display->active_digit = (display->active_digit + 1) % display->digits;
 
    /* Determinar si el dígito actual está en el rango de parpadeo */
    if (display->flashing_frequency > 0) {
        display->flashing_count++;
        if (display->flashing_count >= display->flashing_frequency) {
            display->flashing_count = 0;
        }
        flashing_active = (display->flashing_count < display->flashing_frequency / 2);
    }
 
    /* Si el dígito está parpadeando y en fase apagada, no mostrar */
    if (flashing_active &&
        display->active_digit >= display->flashing_from &&
        display->active_digit <= display->flashing_to) {
        return;
    }
 
    segments = display->display_memory[display->active_digit];
    display->driver->update_digits(1 << display->active_digit);
    display->driver->update_segments(segments);
}
void DisplayFlashDigits(display_t display, uint8_t from, uint8_t to, uint16_t frequency) {
    display->flashing_count     = 0;
    display->flashing_from      = from;
    display->flashing_to        = to;
    display->flashing_frequency = frequency;
}
void DisplayToggleDots(display_t display, uint8_t from, uint8_t to) {
    for (int index = from; index <= to; index++) {
        display->display_memory[index] ^= SEGMENT_P;
    }
}
/* === End of documentation ======================================================================================== */
