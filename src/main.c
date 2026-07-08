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

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */
/* === Public variable definitions ============================================================= */
/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */



/* === Public function implementation ========================================================== */

int main(void) {
    uint8_t entrada[4]={0,0,0,0};
    uint16_t frecuencia =0;
    board_t board = board_create();
    DisplayWriteBCD(board->display, entrada, sizeof(entrada));
    while (true) {
        if(digital_input_has_activated(board->aceptar)){
            if(frecuencia==0){
                frecuencia=100;
            }else if(frecuencia==100){
                frecuencia=250;
            }else{
                frecuencia=0;
            }
            DisplayFlashDigits(board->display,0,3,frecuencia);
        }
        if(digital_input_has_activated(board->cancelar)){
            DisplayToggleDots(board->display,0,3);
        }
        if(digital_input_has_activated(board->f1)){
            entrada[3]=(entrada[3]+1)%10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
        if(digital_input_has_activated(board->f2)){
            entrada[2]=(entrada[2]+1)%10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
        if(digital_input_has_activated(board->f3)){
            entrada[1]=(entrada[1]+1)%10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
        if(digital_input_has_activated(board->f4)){
            entrada[0]=(entrada[0]+1)%10;
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
        }
        board_delay(board,10);
        
    }

    return 0;
}
/* === End of documentation ==================================================================== */
    

/** @} End of module definition for doxygen */

