/*********************************************************************************************************************
Copyright (c) 2026, Costa Francisco Lucas Sebastian

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

/** @file clock.c
 ** @brief clock para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "clock.h"
#include <stddef.h>
#include <string.h>
/* === Macros definitions ========================================================================================== */
 
/* === Private data type declarations ============================================================================== */
 
struct clock_s {
    clock_time_t current_time;
    clock_time_t alarm_time;
    bool valid;
    bool alarm_enabled;
    bool alarm_triggered_flag;
    uint16_t tick_count;
    uint16_t ticks_per_second;
};
 
/* === Private function declarations =============================================================================== */
 
static void bcd_increment(clock_time_t *time);
static void ajustar_campo(uint8_t *unidad, uint8_t *decena, uint8_t maximo, bool incrementar);
 
/* === Private variable definitions ================================================================================ */
 
/* === Public variable definitions ================================================================================= */
 
/* === Private function definitions ================================================================================ */
 
static void bcd_increment(clock_time_t *time) {
    if (time->bcd[5] == 2 && time->bcd[4] == 3 &&
        time->bcd[3] == 5 && time->bcd[2] == 9 &&
        time->bcd[1] == 5 && time->bcd[0] == 9) {
        memset(time, 0, sizeof(clock_time_t));
        return;
    }
    if (++time->bcd[0] > 9) {
        time->bcd[0] = 0;
        if (++time->bcd[1] > 5) {
            time->bcd[1] = 0;
            if (++time->bcd[2] > 9) {
                time->bcd[2] = 0;
                if (++time->bcd[3] > 5) {
                    time->bcd[3] = 0;
                    if (++time->bcd[4] > 9) {
                        time->bcd[4] = 0;
                        ++time->bcd[5];
                    }
                    if (time->bcd[5] == 2 && time->bcd[4] > 3) {
                        memset(time, 0, sizeof(clock_time_t));
                    }
                }
            }
        }
    }
}
 
/**
 * @brief ajusta en 1 (subiendo o bajando) un campo decena/unidad aislado, con vuelta circular.
 *
 * Se usa para minutos (maximo=59) y horas (maximo=23) durante la edición manual, sin tocar
 * el resto de los campos de la hora (a diferencia de bcd_increment, que hace acarreo completo
 * segundo -> minuto -> hora porque simula el paso real del tiempo).
 */
static void ajustar_campo(uint8_t *unidad, uint8_t *decena, uint8_t maximo, bool incrementar) {
    uint8_t valor = (uint8_t)((*decena) * 10 + (*unidad));
    if (incrementar) {
        valor = (uint8_t)((valor + 1) % (maximo + 1));
    } else {
        valor = (valor == 0) ? maximo : (uint8_t)(valor - 1);
    }
    *decena = valor / 10;
    *unidad = valor % 10;
}
 
/* === Public function implementation ============================================================================== */
 
clock_t clock_create(uint16_t ticks_per_second) {
    static struct clock_s self[1];
    memset(self, 0, sizeof(struct clock_s));
    self->ticks_per_second = ticks_per_second;
    return self;
}
 
bool clock_time_is_valid(clock_t clock) {
    if (!clock) return false;
    return clock->valid;
}
 
bool clock_get_time(clock_t self, clock_time_t *result) {
    if (!self || !result) return false;
    memcpy(result, &self->current_time, sizeof(clock_time_t));
    return self->valid;
}
 
bool clock_set_time(clock_t self, const clock_time_t *new_time) {
    if (!self || !new_time) return false;
    self->valid = true;
    memcpy(&self->current_time, new_time, sizeof(clock_time_t));
    return self->valid;
}
 
void clock_increment_minutes(clock_time_t *time) {
    if (!time) return;
    /* bcd[2] = unidad de minutos, bcd[3] = decena de minutos */
    ajustar_campo(&time->bcd[2], &time->bcd[3], 59, true);
}
 
void clock_decrement_minutes(clock_time_t *time) {
    if (!time) return;
    ajustar_campo(&time->bcd[2], &time->bcd[3], 59, false);
}
 
void clock_increment_hours(clock_time_t *time) {
    if (!time) return;
    /* bcd[4] = unidad de horas, bcd[5] = decena de horas */
    ajustar_campo(&time->bcd[4], &time->bcd[5], 23, true);
}
 
void clock_decrement_hours(clock_time_t *time) {
    if (!time) return;
    ajustar_campo(&time->bcd[4], &time->bcd[5], 23, false);
}
 
void clock_new_tick(clock_t clock) {
    if (!clock->valid) return;
    clock->tick_count++;
    if (clock->tick_count >= clock->ticks_per_second) {
        clock->tick_count = 0;
        bcd_increment(&clock->current_time);
 
        if (clock->alarm_enabled &&
            memcmp(&clock->current_time, &clock->alarm_time, sizeof(clock_time_t)) == 0) {
            clock->alarm_triggered_flag = true;
        }
    }
}
 
bool clock_set_alarm_time(clock_t clock, const clock_time_t *alarm_time) {
    if (!clock || !alarm_time) return false;
    memcpy(&clock->alarm_time, alarm_time, sizeof(clock_time_t));
    return true;
}
 
bool clock_get_alarm_time(clock_t clock, clock_time_t *alarm_time) {
    if (!clock || !alarm_time) return false;
    memcpy(alarm_time, &clock->alarm_time, sizeof(clock_time_t));
    return true;
}
 
void clock_enable_alarm(clock_t clock) {
    clock->alarm_enabled = true;
    clock->alarm_triggered_flag = false;
}
 
void clock_disable_alarm(clock_t clock) {
    clock->alarm_enabled = false;
    clock->alarm_triggered_flag = false;
}
 
bool clock_is_alarm_enabled(clock_t clock) {
    return clock->alarm_enabled;
}
 
bool clock_alarm_triggered(clock_t clock) {
    return clock->alarm_triggered_flag;
}
 
void clock_snooze_alarm(clock_t clock, uint8_t minutes) {
    clock->alarm_triggered_flag = false;
    for (uint8_t i = 0; i < minutes; i++) {
        if (++clock->alarm_time.bcd[2] > 9) {
            clock->alarm_time.bcd[2] = 0;
            if (++clock->alarm_time.bcd[3] > 5) {
                clock->alarm_time.bcd[3] = 0;
                if (++clock->alarm_time.bcd[4] > 9) {
                    clock->alarm_time.bcd[4] = 0;
                    if (++clock->alarm_time.bcd[5] > 2 ||
                        (clock->alarm_time.bcd[5] == 2 && clock->alarm_time.bcd[4] > 3)) {
                        memset(&clock->alarm_time, 0, sizeof(clock_time_t));
                    }
                }
            }
        }
    }
}/* === End of documentation ======================================================================================== */
