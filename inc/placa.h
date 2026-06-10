/*********************************************************************************************************************
Copyright (c) 2026, Costa Francisco Lucas Sebastian  <correo@ejemplo.com>

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

#ifndef PLACA_H_
#define PLACA_H_

/** @file PLACA.h
 ** @brief PLACA para la creación de archivos de de cabeceras en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */
#include "digital.h"
#include "screen.h"


/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
/**
 * @brief estructura de la PLACA
 * 
 */
typedef struct board_s{
    digital_output_t buzzer;
    digital_input_t f1;
    digital_input_t f2;
    digital_input_t f3;
    digital_input_t f4;
    digital_input_t cancelar;
    digital_input_t aceptar;
    display_t display;
} const* const board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief funcion para crear una estructura de tipo board_t con los pines y bits de gpio correspondientes a cada led y tecla
 * 
 * @return board_t da como resultado una estructura de tipo board_t con los pines y bits de gpio correspondientes a cada led y tecla
 */
board_t board_create();
void board_delay(board_t placa, uint16_t ms);
void board_refresh_display(void);
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* PLACA_H_ */