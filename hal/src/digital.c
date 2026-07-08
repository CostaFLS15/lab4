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

/** @file digital.c
 ** @brief digital para la creación de archivos de código fuente en lenguaje C
 **/

/* === Headers files inclusions ==================================================================================== */

#include "digital.h"
#include "chip.h"
#include <stdlib.h>

/* === Macros definitions ========================================================================================== */

/* === Private data type declarations ============================================================================== */
struct digital_output_s{
    uint32_t puerto;
    uint8_t terminal;
};
struct digital_input_s{
    uint32_t puerto;
    uint8_t terminal;
    bool invertida;
    bool last_state;

};
/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */

/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */

digital_output_t digital_output_create(uint32_t puerto, uint8_t terminal){
    digital_output_t self;
    self=malloc(sizeof(struct digital_output_s));
    if(self){
        self->puerto=puerto;
        self->terminal=terminal;
        digital_output_deactivate(self);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->puerto, self->terminal, true);
    }
    return self;
}

void digital_output_activate(digital_output_t self){
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,self->puerto, self->terminal, true);
}
void digital_output_deactivate(digital_output_t self){
    Chip_GPIO_SetPinState(LPC_GPIO_PORT,self->puerto, self->terminal, false);
}
void digital_output_toggle(digital_output_t self){
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, self->puerto, self->terminal);
}
digital_input_t digital_input_create(uint32_t puerto, uint8_t terminal, bool inverted){
    digital_input_t self;
    self=malloc(sizeof(struct digital_input_s));
    if(self){
        
        self->puerto=puerto;
        self->terminal=terminal;
        self->invertida=inverted;
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, self->puerto, self->terminal, false);
        self->last_state=digital_input_get_state(self);
    }
    return self;
}
bool digital_input_get_state(digital_input_t self){
    bool state= (Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, self->puerto, self->terminal)!=0);
    return self->invertida ? !state:state;
}
int digital_input_has_changed(digital_input_t self){
    int resultado=0;
    bool actual=digital_input_get_state(self);
    if(actual&&!self->last_state){
        resultado=ACTIVATE_EVENT;
    }else if(!actual&&self->last_state){
        resultado=DEACTIVATE_EVENT;
    }
    self->last_state=actual;
    return resultado;
}
bool digital_input_has_activated(digital_input_t self){
    return digital_input_has_changed(self)==ACTIVATE_EVENT;
}
bool digital_input_has_deactivated(digital_input_t self){
    return digital_input_has_changed(self)==DEACTIVATE_EVENT;
}
/* === End of documentation ======================================================================================== */
