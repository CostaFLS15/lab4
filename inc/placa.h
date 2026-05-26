/*********************************************************************************************************************
Copyright (c) Año, Nombre y Apellido del autor <correo@ejemplo.com>

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
/* === Header for C++ compatibility ================================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =================================================================================== */

/* === Public data type declarations =============================================================================== */
typedef struct board_s{
    digital_output_t led_rojo;
    digital_output_t led_verde;
    digital_output_t led_amarillo;
    digital_output_t led_rgb_rojo;
    digital_output_t led_rgb_verde;
    digital_output_t led_rgb_azul;
    digital_input_t tecla_prender;
    digital_input_t tecla_apagar;
    digital_input_t tecla_cambiar;
    digital_input_t tecla_probar;
} const* const board_t;

/* === Public variable declarations ================================================================================ */

/* === Public function declarations ================================================================================ */
/**
 * @brief funcion para crear una estructura de tipo board_t con los pines y bits de gpio correspondientes a cada led y tecla
 * 
 * @return board_t da como resultado una estructura de tipo board_t con los pines y bits de gpio correspondientes a cada led y tecla
 */
board_t board_create();
/* === End of conditional blocks =================================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* PLACA_H_ */