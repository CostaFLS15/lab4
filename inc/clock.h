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

#ifndef CLOCK_H_
#define CLOCK_H_
 
/** @file clock.h
 ** @brief archivo h de clock
 **/
 
/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
/* === Header for C++ compatibility ================================================================================ */
 
#ifdef __cplusplus
extern "C" {
#endif
 
/* === Public macros definitions =================================================================================== */
 
/* === Public data type declarations =============================================================================== */
 
typedef union{
    struct{
        uint8_t seconds[2];
        uint8_t minutes[2];
        uint8_t hours[2];
    }time;
    uint8_t bcd[6];
}clock_time_t;
 
 
typedef struct clock_s *clock_t;
 
/* === Public function declarations ================================================================================ */
 
/**
 * @brief funcion que crea el reloj.
 * @param ticks_per_second cantidad de llamadas a clock_new_tick equivalentes a un segundo real.
 * @return un puntero al reloj creado.
 */
clock_t clock_create(uint16_t ticks_per_second);
 
/**
 * @brief obtiene la hora actual del reloj.
 *
 * @param clock
 * @param result
 * @return true
 * @return false
 */
bool clock_get_time(clock_t clock, clock_time_t *result);
 
/**
 * @brief función que verifica si el tiempo del reloj es válido.
 *
 * @param clock
 * @return true
 * @return false
 */
bool clock_time_is_valid(clock_t clock);
 
/**
 * @brief función que ajusta el tiempo del reloj.
 *
 * @param clock
 * @param new_time
 * @return true
 * @return false
 */
bool clock_set_time(clock_t clock, const clock_time_t *new_time);
 
/**
 * @brief función que simula el avance del reloj en segundos.
 *
 * @param clock
 */
void clock_new_tick(clock_t clock);
 
/* === Ajuste manual de campos aislados (para el modo edición) ==================================================== */
 
/**
 * @brief incrementa únicamente el campo de minutos en 1, sin afectar horas ni segundos.
 * Da la vuelta de 59 a 00.
 *
 * @param time hora a modificar (típicamente un buffer de edición, no el reloj en marcha).
 */
void clock_increment_minutes(clock_time_t *time);
 
/**
 * @brief decrementa únicamente el campo de minutos en 1, sin afectar horas ni segundos.
 * Da la vuelta de 00 a 59.
 *
 * @param time hora a modificar.
 */
void clock_decrement_minutes(clock_time_t *time);
 
/**
 * @brief incrementa únicamente el campo de horas en 1 (formato 24hs), sin afectar minutos ni segundos.
 * Da la vuelta de 23 a 00.
 *
 * @param time hora a modificar.
 */
void clock_increment_hours(clock_time_t *time);
 
/**
 * @brief decrementa únicamente el campo de horas en 1 (formato 24hs), sin afectar minutos ni segundos.
 * Da la vuelta de 00 a 23.
 *
 * @param time hora a modificar.
 */
void clock_decrement_hours(clock_time_t *time);
 
/* === Alarma ======================================================================================================= */
 
/**
 * @brief función que establece la hora de la alarma.
 *
 * @param clock
 * @param alarm_time
 * @return true
 * @return false
 */
bool clock_set_alarm_time(clock_t clock, const clock_time_t *alarm_time);
 
/**
 * @brief función que obtiene la hora de la alarma.
 *
 * @param clock
 * @param alarm_time
 * @return true
 * @return false
 */
bool clock_get_alarm_time(clock_t clock, clock_time_t *alarm_time);
 
/**
 * @brief función que habilita la alarma.
 *
 * @param clock
 */
void clock_enable_alarm(clock_t clock);
 
/**
 * @brief función que deshabilita la alarma.
 *
 * @param clock
 */
void clock_disable_alarm(clock_t clock);
 
/**
 * @brief función que indica si la alarma está habilitada.
 *
 * @param clock
 * @return true
 * @return false
 */
bool clock_is_alarm_enabled(clock_t clock);
 
/**
 * @brief función que indica si la alarma fue activada.
 *
 * @param clock
 * @return true
 * @return false
 */
bool clock_alarm_triggered(clock_t clock);
 
/**
 * @brief función que pospone la alarma una cantidad de minutos.
 *
 * @param clock
 * @param minutes
 */
void clock_snooze_alarm(clock_t clock, uint8_t minutes);
 
/* === End of conditional blocks =================================================================================== */
 
#ifdef __cplusplus
}
#endif
 
#endif /* CLOCK_H_ */