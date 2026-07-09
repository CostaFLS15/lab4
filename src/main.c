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
#include <string.h>

/* === Macros definitions ====================================================================== */

#define PERIODO_MS              100
#define TICKS_POR_SEGUNDO       (1000 / PERIODO_MS)
#define TICKS_MEDIO_PARPADEO    (TICKS_POR_SEGUNDO / 2)
#define TICKS_HOLD_3S           (3000 / PERIODO_MS)
#define TICKS_TIMEOUT_EDICION   (30000 / PERIODO_MS)
#define MINUTOS_SNOOZE          5


#define FRECUENCIA_PARPADEO_DIGITOS 1000

#define DIGITO_HORA_DECENA  0
#define DIGITO_HORA_UNIDAD  1
#define DIGITO_MIN_DECENA   2
#define DIGITO_MIN_UNIDAD   3

/* === Private data type declarations ========================================================== */

typedef enum {
    MODO_SIN_AJUSTAR,
    MODO_NORMAL,
    MODO_MINUTOS,
    MODO_HORAS,
    MODO_MINUTOS_ALARMA,
    MODO_HORAS_ALARMA,
} modo_t;

/* === Private function declarations =========================================================== */

static void time_to_digits(const clock_time_t * time, uint8_t digits[4]);
static void mostrar(board_t board, uint8_t digits[4], uint8_t mascara_puntos);
static void configurar_parpadeo_digitos(board_t board, modo_t modo);

/* === Public function implementation ========================================================== */

int main(void) {
    board_t board = board_create();
    clock_t reloj = clock_create(TICKS_POR_SEGUNDO);

    modo_t modo = MODO_SIN_AJUSTAR;
    modo_t modo_previo = MODO_NORMAL;
    
    clock_time_t buffer_edicion = {0};
    bool editando_alarma = false;

    bool habia_hora_valida_al_entrar = false;

    uint8_t digitos[4] = {0, 0, 0, 0};
    uint16_t contador_parpadeo = 0;
    bool parpadeo_encendido = false;

    uint16_t contador_edicion = 0;
    uint16_t contador_hold_f1 = 0;
    uint16_t contador_hold_f2 = 0;

    while (true) {
        clock_new_tick(reloj);
        if (modo != modo_previo) {
            configurar_parpadeo_digitos(board, modo);
            modo_previo = modo;
        }

        bool f1_activo = digital_input_get_state(board->f1);
        bool f2_activo = digital_input_get_state(board->f2);

        switch (modo) {

            /* ------------------------------------------------------------------------------- */
            case MODO_SIN_AJUSTAR: {
                uint8_t ceros[4] = {0, 0, 0, 0};

                contador_parpadeo++;
                if (contador_parpadeo >= TICKS_MEDIO_PARPADEO) {
                    contador_parpadeo = 0;
                    parpadeo_encendido = !parpadeo_encendido;
                }

                uint8_t mascara = parpadeo_encendido ? (1 << DIGITO_HORA_UNIDAD) : 0;
                mostrar(board, ceros, mascara);

                if (f1_activo) {
                    contador_hold_f1++;
                    if (contador_hold_f1 == TICKS_HOLD_3S) {
                        memset(&buffer_edicion, 0, sizeof(buffer_edicion));
                        editando_alarma = false;
                        habia_hora_valida_al_entrar = false;
                        modo = MODO_MINUTOS;
                        contador_edicion = 0;
                        contador_parpadeo = 0;
                        parpadeo_encendido = false;
                    }
                } else {
                    contador_hold_f1 = 0;
                }
                break;
            }

            /* ------------------------------------------------------------------------------- */
            case MODO_NORMAL: {
                if (clock_alarm_triggered(reloj)) {
                    /* alarma sonando: ACEPTAR pospone 5', CANCELAR silencia hasta mañana */
                    digital_output_toggle(board->buzzer);

                    if (digital_input_has_activated(board->aceptar)) {
                        clock_snooze_alarm(reloj, MINUTOS_SNOOZE);
                        digital_output_deactivate(board->buzzer);
                    } else if (digital_input_has_activated(board->cancelar)) {
                        
                        for (uint8_t i = 0; i < 6; i++) {
                            clock_snooze_alarm(reloj, 240);
                        }
                        digital_output_deactivate(board->buzzer);
                    }
                } else {
                    clock_time_t actual;
                    clock_get_time(reloj, &actual);
                    time_to_digits(&actual, digitos);

                    contador_parpadeo++;
                    if (contador_parpadeo >= TICKS_MEDIO_PARPADEO) {
                        contador_parpadeo = 0;
                        parpadeo_encendido = !parpadeo_encendido;
                    }

                    uint8_t mascara = 0;
                    if (parpadeo_encendido) mascara |= (1 << DIGITO_HORA_UNIDAD);
                    if (clock_is_alarm_enabled(reloj)) mascara |= (1 << DIGITO_MIN_UNIDAD);
                    mostrar(board, digitos, mascara);

                    if (digital_input_has_activated(board->aceptar)) {
                        clock_enable_alarm(reloj);
                    }
                    if (digital_input_has_activated(board->cancelar)) {
                        clock_disable_alarm(reloj);
                    }
                }

                if (!clock_alarm_triggered(reloj)) {
                    if (f1_activo) {
                        contador_hold_f1++;
                        if (contador_hold_f1 == TICKS_HOLD_3S) {
                            clock_get_time(reloj, &buffer_edicion);
                            editando_alarma = false;
                            habia_hora_valida_al_entrar = true;
                            modo = MODO_MINUTOS;
                            contador_edicion = 0;
                            contador_parpadeo = 0;
                            parpadeo_encendido = false;
                        }
                    } else {
                        contador_hold_f1 = 0;
                    }

                    if (f2_activo) {
                        contador_hold_f2++;
                        if (contador_hold_f2 == TICKS_HOLD_3S) {
                            clock_get_alarm_time(reloj, &buffer_edicion);
                            editando_alarma = true;
                            habia_hora_valida_al_entrar = true;
                            modo = MODO_MINUTOS_ALARMA;
                            contador_edicion = 0;
                            contador_parpadeo = 0;
                            parpadeo_encendido = false;
                        }
                    } else {
                        contador_hold_f2 = 0;
                    }
                }
                break;
            }

            /* ------------------------------------------------------------------------------- */
            case MODO_MINUTOS:
            case MODO_MINUTOS_ALARMA: {
                bool tecla_presionada = false;
                modo_t modo_actual = modo;

                if (digital_input_has_activated(board->f4)) {
                    clock_increment_minutes(&buffer_edicion);
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->f3)) {
                    clock_decrement_minutes(&buffer_edicion);
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->aceptar)) {
                    modo = (modo_actual == MODO_MINUTOS) ? MODO_HORAS : MODO_HORAS_ALARMA;
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->cancelar)) {
                    modo = habia_hora_valida_al_entrar ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    tecla_presionada = true;
                }

                if (tecla_presionada) {
                    contador_edicion = 0;
                } else {
                    contador_edicion++;
                    if (contador_edicion >= TICKS_TIMEOUT_EDICION) {
                        modo = habia_hora_valida_al_entrar ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    }
                }

                time_to_digits(&buffer_edicion, digitos);
                uint8_t mascara_puntos = editando_alarma ? 0x0F : 0;
                mostrar(board, digitos, mascara_puntos);
                break;
            }

            /* ------------------------------------------------------------------------------- */
            case MODO_HORAS:
            case MODO_HORAS_ALARMA: {
                bool tecla_presionada = false;

                if (digital_input_has_activated(board->f4)) {
                    clock_increment_hours(&buffer_edicion);
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->f3)) {
                    clock_decrement_hours(&buffer_edicion);
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->aceptar)) {
                    if (editando_alarma) {
                        clock_set_alarm_time(reloj, &buffer_edicion);
                    } else {
                        clock_set_time(reloj, &buffer_edicion);
                    }
                    modo = MODO_NORMAL;
                    tecla_presionada = true;
                }
                if (digital_input_has_activated(board->cancelar)) {
                    modo = habia_hora_valida_al_entrar ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    tecla_presionada = true;
                }

                if (tecla_presionada) {
                    contador_edicion = 0;
                } else {
                    contador_edicion++;
                    if (contador_edicion >= TICKS_TIMEOUT_EDICION) {
                        modo = habia_hora_valida_al_entrar ? MODO_NORMAL : MODO_SIN_AJUSTAR;
                    }
                }

                time_to_digits(&buffer_edicion, digitos);
                uint8_t mascara_puntos = editando_alarma ? 0x0F : 0;
                mostrar(board, digitos, mascara_puntos);
                break;
            }

            default:
                break;
        }

        board_delay(board, PERIODO_MS);
    }

    return 0;
}

/* === Private function implementation ========================================================= */

static void time_to_digits(const clock_time_t * time, uint8_t digits[4]) {
    digits[DIGITO_HORA_DECENA] = time->time.hours[1];
    digits[DIGITO_HORA_UNIDAD] = time->time.hours[0];
    digits[DIGITO_MIN_DECENA] = time->time.minutes[1];
    digits[DIGITO_MIN_UNIDAD] = time->time.minutes[0];
}

static void mostrar(board_t board, uint8_t digits[4], uint8_t mascara_puntos) {
    DisplayWriteBCD(board->display, digits, 4);
    for (uint8_t i = 0; i < 4; i++) {
        if (mascara_puntos & (1 << i)) {
            DisplayToggleDots(board->display, i, i);
        }
    }
}

static void configurar_parpadeo_digitos(board_t board, modo_t modo) {
    switch (modo) {
        case MODO_SIN_AJUSTAR:
            DisplayFlashDigits(board->display, 0, 3, FRECUENCIA_PARPADEO_DIGITOS);
            break;
        case MODO_NORMAL:
            DisplayFlashDigits(board->display, 0, 3, 0);
            break;
        case MODO_MINUTOS:
        case MODO_MINUTOS_ALARMA:
            DisplayFlashDigits(board->display, DIGITO_MIN_DECENA, DIGITO_MIN_UNIDAD, FRECUENCIA_PARPADEO_DIGITOS);
            break;
        case MODO_HORAS:
        case MODO_HORAS_ALARMA:
            DisplayFlashDigits(board->display, DIGITO_HORA_DECENA, DIGITO_HORA_UNIDAD, FRECUENCIA_PARPADEO_DIGITOS);
            break;
        default:
            break;
    }
}

/* === End of documentation ==================================================================== */
/** @} End of module definition for doxygen */

