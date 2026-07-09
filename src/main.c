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
#include "clock.h"
#include "screen.h"
 
/* === Macros definitions ====================================================================== */
 
/** Período del bucle principal (ms). board_delay ya refresca el display internamente. */
#define PERIODO_MS 100
 
/** Cantidad de vueltas del bucle principal equivalentes a un segundo. */
#define TICKS_POR_SEGUNDO (1000 / PERIODO_MS)
 
/** Tiempo de pulsación larga para F1 / F2 (3 segundos). */
#define TICKS_PULSACION_LARGA (3 * TICKS_POR_SEGUNDO)
 
/** Tiempo de inactividad para descartar cambios en modo edición (30 segundos). */
#define TICKS_INACTIVIDAD (30 * TICKS_POR_SEGUNDO)
 
/** Medio período de parpadeo del punto en modo normal (1 Hz => toggle cada 500 ms). */
#define TICKS_MEDIO_PARPADEO (TICKS_POR_SEGUNDO / 2)
 
/**
 * Frecuencia para DisplayFlashDigits, expresada en cantidad de llamadas a DisplayRefresh.
 * board_delay llama a DisplayRefresh aproximadamente una vez por milisegundo, así que
 * ~1000 produce un parpadeo cercano a 1 Hz. Ajustar empíricamente en la placa real.
 */
#define FRECUENCIA_PARPADEO 1000
 
/** Minutos que se pospone la alarma al aceptar mientras suena. */
#define MINUTOS_POSPONER 5
 
#define DIGITO_HORA_DECENA  0
#define DIGITO_HORA_UNIDAD  1
#define DIGITO_MIN_DECENA   2
#define DIGITO_MIN_UNIDAD   3
 
/* === Private data type declarations ========================================================== */
 

 
/* === Private function declarations =========================================================== */
 
static void time_to_digits(const clock_time_t * time, uint8_t digits[4]);
static void mostrar(board_t board, uint8_t digits[4], uint8_t mascara_puntos);
 
/* === Public function implementation ========================================================== */
 
int main(void) {
    board_t board = board_create();
    clock_t reloj = clock_create(TICKS_POR_SEGUNDO);
 
    modo_t modo = MODO_SIN_AJUSTAR;
 
    clock_time_t edicion_hora;
    clock_time_t edicion_alarma;
    uint8_t digitos[4] = {0, 0, 0, 0};
 
    uint16_t contador_f1 = 0;
    uint16_t contador_f2 = 0;
    uint16_t contador_inactividad = 0;
    uint16_t contador_parpadeo = 0;
    bool punto_segundo_encendido = false;
 
    while (true) {
        clock_new_tick(reloj);
 
        /* Lectura de entradas: una sola vez por vuelta para no perder flancos */
        bool f1_presionado = digital_input_has_state(board->f1);
        bool f2_presionado = digital_input_has_state(board->f2);
        bool f3_activada = digital_input_has_activated(board->f3);
        bool f4_activada = digital_input_has_activated(board->f4);
        bool aceptar_activada = digital_input_has_activated(board->aceptar);
        bool cancelar_activada = digital_input_has_activated(board->cancelar);
 
        /* -------- Alarma sonando: tiene prioridad sobre cualquier otro modo -------- */
        if (clock_alarm_triggered(reloj)) {
            digital_output_toggle(board->buzzer);
 
            if (aceptar_activada) {
                clock_snooze_alarm(reloj, MINUTOS_POSPONER);
                digital_output_deactivate(board->buzzer);
            } else if (cancelar_activada) {
                /* Silenciar hasta el otro día, sin modificar la hora programada */
                clock_snooze_alarm(reloj, 0);
                digital_output_deactivate(board->buzzer);
            }
 
            board_delay(board, PERIODO_MS);
            continue;
        }
        digital_output_deactivate(board->buzzer);
 
        switch (modo) {
 
            case MODO_SIN_AJUSTAR: {
                clock_time_t actual;
                clock_get_time(reloj, &actual);
                time_to_digits(&actual, digitos);
                mostrar(board, digitos, 0);
                DisplayFlashDigits(board->display, 0, 3, FRECUENCIA_PARPADEO);
 
                if (clock_time_is_valid(reloj)) {
                    modo = MODO_NORMAL;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    break;
                }
 
                if (f1_presionado) {
                    contador_f1++;
                    if (contador_f1 == TICKS_PULSACION_LARGA) {
                        clock_get_time(reloj, &edicion_hora);
                        edicion_hora.time.seconds[0] = 0;
                        edicion_hora.time.seconds[1] = 0;
                        modo = MODO_MINUTOS;
                        contador_inactividad = 0;
                        DisplayFlashDigits(board->display, 0, 3, 0);
                    }
                } else {
                    contador_f1 = 0;
                }
                break;
            }
 
            case MODO_NORMAL: {
                clock_time_t actual;
                clock_get_time(reloj, &actual);
                time_to_digits(&actual, digitos);
 
                contador_parpadeo++;
                if (contador_parpadeo >= TICKS_MEDIO_PARPADEO) {
                    contador_parpadeo = 0;
                    punto_segundo_encendido = !punto_segundo_encendido;
                }
 
                uint8_t mascara = 0;
                if (punto_segundo_encendido) mascara |= (1 << DIGITO_HORA_UNIDAD);
                if (clock_is_alarm_enabled(reloj)) mascara |= (1 << DIGITO_HORA_DECENA);
                mostrar(board, digitos, mascara);
 
                if (aceptar_activada) clock_enable_alarm(reloj);
                if (cancelar_activada) clock_disable_alarm(reloj);
 
                if (f1_presionado) {
                    contador_f1++;
                    if (contador_f1 == TICKS_PULSACION_LARGA) {
                        clock_get_time(reloj, &edicion_hora);
                        edicion_hora.time.seconds[0] = 0;
                        edicion_hora.time.seconds[1] = 0;
                        modo = MODO_MINUTOS;
                        contador_inactividad = 0;
                        DisplayFlashDigits(board->display, DIGITO_MIN_DECENA, DIGITO_MIN_UNIDAD, FRECUENCIA_PARPADEO);
                    }
                } else {
                    contador_f1 = 0;
                }
 
                if (f2_presionado) {
                    contador_f2++;
                    if (contador_f2 == TICKS_PULSACION_LARGA) {
                        clock_get_alarm_time(reloj, &edicion_alarma);
                        edicion_alarma.time.seconds[0] = 0;
                        edicion_alarma.time.seconds[1] = 0;
                        modo = MODO_MINUTOS_ALARMA;
                        contador_inactividad = 0;
                        DisplayFlashDigits(board->display, DIGITO_MIN_DECENA, DIGITO_MIN_UNIDAD, FRECUENCIA_PARPADEO);
                    }
                } else {
                    contador_f2 = 0;
                }
                break;
            }
 
            case MODO_MINUTOS: {
                bool actividad = f3_activada || f4_activada;
 
                if (f4_activada) clock_increment_minutes(&edicion_hora);
                if (f3_activada) clock_decrement_minutes(&edicion_hora);
                time_to_digits(&edicion_hora, digitos);
                mostrar(board, digitos, 0);
 
                if (aceptar_activada) {
                    modo = MODO_HORAS;
                    DisplayFlashDigits(board->display, DIGITO_HORA_DECENA, DIGITO_HORA_UNIDAD,
                                        FRECUENCIA_PARPADEO);
                    actividad = true;
                } else if (cancelar_activada || contador_inactividad >= TICKS_INACTIVIDAD) {
                    modo = clock_time_is_valid(reloj) ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                }
 
                contador_inactividad = actividad ? 0 : (uint16_t)(contador_inactividad + 1);
                break;
            }
 
            case MODO_HORAS: {
                bool actividad = f3_activada || f4_activada;
 
                if (f4_activada) clock_increment_hours(&edicion_hora);
                if (f3_activada) clock_decrement_hours(&edicion_hora);
                time_to_digits(&edicion_hora, digitos);
                mostrar(board, digitos, 0);
 
                if (aceptar_activada) {
                    clock_set_time(reloj, &edicion_hora);
                    modo = MODO_NORMAL;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                } else if (cancelar_activada || contador_inactividad >= TICKS_INACTIVIDAD) {
                    modo = clock_time_is_valid(reloj) ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                }
 
                contador_inactividad = actividad ? 0 : (uint16_t)(contador_inactividad + 1);
                break;
            }
 
            case MODO_MINUTOS_ALARMA: {
                bool actividad = f3_activada || f4_activada;
 
                if (f4_activada) clock_increment_minutes(&edicion_alarma);
                if (f3_activada) clock_decrement_minutes(&edicion_alarma);
                time_to_digits(&edicion_alarma, digitos);
                mostrar(board, digitos, 0x0F); /* todos los puntos encendidos: modo alarma */
 
                if (aceptar_activada) {
                    modo = MODO_HORAS_ALARMA;
                    DisplayFlashDigits(board->display, DIGITO_HORA_DECENA, DIGITO_HORA_UNIDAD,
                                        FRECUENCIA_PARPADEO);
                    actividad = true;
                } else if (cancelar_activada || contador_inactividad >= TICKS_INACTIVIDAD) {
                    modo = MODO_NORMAL;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                }
 
                contador_inactividad = actividad ? 0 : (uint16_t)(contador_inactividad + 1);
                break;
            }
 
            case MODO_HORAS_ALARMA: {
                bool actividad = f3_activada || f4_activada;
 
                if (f4_activada) clock_increment_hours(&edicion_alarma);
                if (f3_activada) clock_decrement_hours(&edicion_alarma);
                time_to_digits(&edicion_alarma, digitos);
                mostrar(board, digitos, 0x0F);
 
                if (aceptar_activada) {
                    clock_set_alarm_time(reloj, &edicion_alarma);
                    modo = MODO_NORMAL;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                } else if (cancelar_activada || contador_inactividad >= TICKS_INACTIVIDAD) {
                    modo = MODO_NORMAL;
                    DisplayFlashDigits(board->display, 0, 3, 0);
                    actividad = true;
                }
 
                contador_inactividad = actividad ? 0 : (uint16_t)(contador_inactividad + 1);
                break;
            }
 
            default:
                modo = MODO_SIN_AJUSTAR;
                break;
        }
 
        board_delay(board, PERIODO_MS);
    }
 
    return 0;
}
 
/* === Private function implementation ========================================================= */
 
/**
 * @brief Convierte una hora del reloj a un arreglo de 4 dígitos para mostrar (HH:MM).
 *
 * clock_time_t guarda, para cada campo, la unidad en el índice 0 y la decena en el índice 1
 * (ver bcd_increment en clock.c). El arreglo de salida queda en orden de despliegue:
 * [decena_hora, unidad_hora, decena_minuto, unidad_minuto].
 */
static void time_to_digits(const clock_time_t * time, uint8_t digits[4]) {
    digits[DIGITO_HORA_DECENA] = time->time.hours[1];
    digits[DIGITO_HORA_UNIDAD] = time->time.hours[0];
    digits[DIGITO_MIN_DECENA] = time->time.minutes[1];
    digits[DIGITO_MIN_UNIDAD] = time->time.minutes[0];
}
 
/**
 * @brief Escribe los 4 dígitos en el display y prende los puntos indicados en la máscara.
 *
 * DisplayWriteBCD borra toda la memoria de video (incluidos los puntos), por eso los puntos
 * se vuelven a aplicar en cada llamada con DisplayToggleDots (equivale a "prender" porque
 * siempre se parte de 0).
 *
 * @param mascara_puntos bit i en 1 => punto del dígito i encendido.
 */
static void mostrar(board_t board, uint8_t digits[4], uint8_t mascara_puntos) {
    DisplayWriteBCD(board->display, digits, 4);
    for (uint8_t i = 0; i < 4; i++) {
        if (mascara_puntos & (1 << i)) {
            DisplayToggleDots(board->display, i, i);
        }
    }
}
/* === End of documentation ==================================================================== */
    

/** @} End of module definition for doxygen */

