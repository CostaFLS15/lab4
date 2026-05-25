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

#ifndef DIGITAL_H_
#define DIGITAL_H_

/** @file digital.h
 ** @brief digital para la creación de archivos de de cabeceras en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */
#include <stdint.h>
#include <stdbool.h>
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */
#define ACTIVATE_EVENT 1
#define DEACTIVATE_EVENT -1
/* === Public data type declarations =============================================================================== */

typedef struct digital_output_s * digital_output_t;
typedef struct digital_input_s * digital_input_t;
/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/** @brief Crea una salida digital
 * @param puerto puerto del microcontrolador al que se conecta la salida digital
 * @param terminal terminal del puerto al que se conecta la salida digital
 * @return digital_output_t puntero a la salida digital creada
 */
digital_output_t digital_output_create(uint32_t puerto, uint8_t terminal);

/** @brief Activa una salida digital
 * @param salida puntero a la salida digital
 */
void digital_output_activate(digital_output_t salida);

/** @brief Desactiva una salida digital
 * @param salida puntero a la salida digital
 */
void digital_output_deactivate(digital_output_t salida);

/** @brief Alterna el estado de una salida digital
 * @param salida puntero a la salida digital
 */
void digital_output_toggle(digital_output_t salida);

/** @brief Crea una entrada digital
 * @param puerto puerto del microcontrolador al que se conecta la entrada digital
 * @param terminal terminal del puerto al que se conecta la entrada digital
 * @return digital_input_t puntero a la entrada digital creada
 */
digital_input_t digital_input_create(uint32_t puerto, uint8_t terminal, bool inverted);

/**
 * @brief lee el estado logico actual de la entrada
 * 
 * @param input 
 * @return true 
 * @return false 
 */
bool digital_input_get_state(digital_input_t input);

int digital_input_has_changed(digital_input_t self);

bool digital_input_has_activated(digital_input_t self);

bool digital_input_has_deactivated(digital_input_t self);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_H_ */
