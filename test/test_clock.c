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
#include "unity.h"
/* === Macros definitions ========================================================================================== */
#define CLOCK_TICKS_PER_SECOND 5
#define TEST_ASSERT_TIME(hours_tens, hours_units, minutes_tens, minutes_units, seconds_tens, seconds_units) \
    TEST_ASSERT_TRUE_MESSAGE(clock_get_time(clock, &current_time), "Clock has invalid time"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_units, current_time.bcd[0], "Diferencia en unidad de segundos"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(seconds_tens,  current_time.bcd[1], "Diferencia en decena de segundos"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_units, current_time.bcd[2], "Diferencia en unidad de minutos"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(minutes_tens,  current_time.bcd[3], "Diferencia en decena de minutos"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_units,   current_time.bcd[4], "Diferencia en unidad de horas"); \
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(hours_tens,    current_time.bcd[5], "Diferencia en decena de horas");

/* === Private data type declarations ============================================================================== */

/* === Private function declarations =============================================================================== */

/* === Private variable definitions ================================================================================ */

/* === Public variable definitions ================================================================================= */
static void simulate_seconds(clock_t clock, uint8_t seconds) {
    for (uint32_t i = 0; i < CLOCK_TICKS_PER_SECOND*seconds; i++) {
        clock_new_tick(clock);
    }
}

static void simulate_minutes(clock_t clock, uint8_t minutes) {
    for (uint32_t i = 0; i < CLOCK_TICKS_PER_SECOND*60*minutes; i++) {
        clock_new_tick(clock);
    }
}
/* === Private function definitions ================================================================================ */

/* === Public function implementation ============================================================================== */
clock_t clock; // Declaración del reloj a nivel global
clock_time_t current_time = {
        .bcd = {0,0,0,0,0,0}, 
    };
    void setUp(void){
    // Inicializa el reloj antes de cada prueba
    clock = clock_create(CLOCK_TICKS_PER_SECOND);
    }

void test_set_up_with_invalid_time(void){
    //le asigno un valor inicial para no tener cualquier cosa paracuando se inicialice
    TEST_ASSERT_FALSE(clock_time_is_valid(clock));
    //clock_get_time(clock, &current_time);
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, sizeof(clock_time_t));
    
}
//al ajustar la hora el reloj queda en hora y en valida
void test_set_up_and_adjust_with_valid_time(void){
    static const clock_time_t new_time = {
        .time={
            .seconds={4,5},
            .minutes={0,0},
            .hours={1,4},
        }
    };
    
    TEST_ASSERT_TRUE(clock_set_time(clock, &new_time));
    TEST_ASSERT_TRUE(clock_get_time(clock, &current_time));
    TEST_ASSERT_TIME(4, 1, 0, 0, 5, 4);


}
    
void test_clock_advance_one_second(void){
    
    clock_set_time(clock, &(clock_time_t){0});//establezco el tiempo actual a 0
    simulate_seconds(clock, 1); //simulo el avance del reloj en 1 segundo
    TEST_ASSERT_TIME(0,0,0,0,0,1);

}
    
void test_clock_advance_ten_second(void){
    
    clock_set_time(clock, &(clock_time_t){0});//establezco el tiempo actual a 0
    simulate_seconds(clock, 10); //simulo el avance del reloj en 10 segundos
    TEST_ASSERT_TIME(0, 0, 0, 0, 1, 0);

}

void test_time_rollover_to_next_minute(void) {
    //esta funcion lo que hace es simular el avance del reloj en 60 segundos
    clock_set_time(clock, &(clock_time_t){0});//establezco el tiempo actual a 0
    simulate_seconds(clock, 60); //simulo el avance del reloj en 60 segundos
    TEST_ASSERT_TIME(0, 0, 0, 1, 0, 0);
}
void test_time_rollover_to_next_hour(void) {
    //esta funcion lo que hace es simular el avance del reloj en 3600 segundos
    clock_set_time(clock, &(clock_time_t){0});//establezco el tiempo actual a 0
    simulate_minutes(clock, 60); //simulo el avance del reloj en 3600 segundos
    TEST_ASSERT_TIME(0, 1, 0, 0, 0, 0);
}
void test_alarm_triggers_when_time_matches(void){
    clock_time_t alarm_time = {.bcd = {0, 1, 0, 0, 0, 0}}; // 00:00:10
    clock_set_time(clock, &(clock_time_t){0});
    clock_set_alarm_time(clock, &alarm_time);
    clock_enable_alarm(clock);
    simulate_seconds(clock, 10);
    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));
}

void test_alarm_disable_prevents_trigger(void){
    clock_time_t alarm_time = {.bcd = {0, 1, 0, 0, 0, 0}}; // 00:00:10
    clock_set_time(clock, &(clock_time_t){0});// Establezco el tiempo actual a 0
    clock_set_alarm_time(clock, &alarm_time);// Establezco la hora de la alarma
    clock_disable_alarm(clock);// Deshabilito la alarma
    simulate_seconds(clock, 10);// Simulo el avance del reloj en 10 segundos
    TEST_ASSERT_FALSE(clock_alarm_triggered(clock));// Verifico que la alarma no se haya activado
}

void test_snooze_alarm_delays_alarm(void){
    //esta funcion lo que hace es que cuando suene la alarma la pospone 1 minuto
    clock_time_t alarm_time = {.bcd = {0, 1, 0, 0, 0, 0}}; // 00:00:10
    clock_set_time(clock, &(clock_time_t){0});
    clock_set_alarm_time(clock, &alarm_time);
    clock_enable_alarm(clock);
    clock_snooze_alarm(clock, 1); // pospone 1 minuto
    simulate_seconds(clock, 10);
    TEST_ASSERT_FALSE(clock_alarm_triggered(clock));
    simulate_seconds(clock, 60);
    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));
}

void test_time_rollover_to_zero(void) {
    clock_time_t max_time = { .bcd = {9, 5, 9, 5, 3, 2} }; // 23:59:59
    clock_set_time(clock, &max_time);
    simulate_seconds(clock, 1); // +1s -> 00:00:00
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 0);
}

void test_snooze_adds_minutes_correctly(void) {
    clock_time_t alarm = { .bcd = {0, 0, 0, 5, 0, 0} }; // 00:50:00
    clock_set_time(clock, &alarm);
    clock_set_alarm_time(clock, &alarm);
    clock_enable_alarm(clock);
    simulate_seconds(clock, 0);
    clock_snooze_alarm(clock, 15); // +15 min -> 01:05:00
    clock_time_t expected = { .bcd = {0, 0, 5, 0, 1, 0} }; // 01:05:00
    clock_get_alarm_time(clock, &current_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.bcd, current_time.bcd, 6);
}

void test_snooze_wraps_hour_on_60_minutes(void) {
    //esta funcion es para probar que si se le suma 60 minutos a la alarma,
    //la hora de la alarma se actualiza correctamente
    clock_time_t alarm = { .bcd = {0, 0, 5, 5, 0, 0} };// seteo a 00:55:00
    clock_set_time(clock, &alarm);
    clock_set_alarm_time(clock, &alarm);
    clock_enable_alarm(clock);
    clock_snooze_alarm(clock, 60);
    //esta linea lo que hace es sumar 60 minutos a la alarma
    clock_time_t expected = { .bcd = {0, 0, 5, 5, 1, 0} };
    clock_get_alarm_time(clock, &current_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.bcd, current_time.bcd, 6);
}

void test_set_alarm_and_trigger(void) {
    clock_time_t alarm = { .bcd = {0, 0, 0, 0, 0, 1} };
    clock_set_time(clock, &(clock_time_t){ .bcd = {9, 5, 9, 5, 9, 0} });
    clock_set_alarm_time(clock, &alarm);
    clock_enable_alarm(clock);
    simulate_seconds(clock, 1);
    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));
}

void test_alarm_not_triggered_if_disabled(void) {
    clock_time_t alarm = { .bcd = {0, 0, 0, 0, 0, 1} };
    clock_set_time(clock, &(clock_time_t){ .bcd = {9, 5, 9, 5, 9, 0} });
    clock_set_alarm_time(clock, &alarm);
    clock_disable_alarm(clock);
    simulate_seconds(clock, 1);
    TEST_ASSERT_FALSE(clock_alarm_triggered(clock));
}

void test_alarm_enable_status(void){
    TEST_ASSERT_FALSE(clock_is_alarm_enabled(clock));

    clock_enable_alarm(clock);

    TEST_ASSERT_TRUE(clock_is_alarm_enabled(clock));

    clock_disable_alarm(clock);

    TEST_ASSERT_FALSE(clock_is_alarm_enabled(clock));
}
void test_alarm_remains_triggered_after_activation(void)
{
    clock_time_t alarm = {.bcd={1,0,0,0,0,0}};

    clock_set_time(clock,&(clock_time_t){0});
    clock_set_alarm_time(clock,&alarm);
    clock_enable_alarm(clock);

    simulate_seconds(clock,10);

    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));

    simulate_seconds(clock,30);

    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));
}
void test_disable_alarm_clears_trigger_flag(void)
{
    clock_time_t alarm = {.bcd={1,0,0,0,0,0}};

    clock_set_time(clock,&(clock_time_t){0});
    clock_set_alarm_time(clock,&alarm);

    clock_enable_alarm(clock);

    simulate_seconds(clock,10);

    TEST_ASSERT_TRUE(clock_alarm_triggered(clock));

    clock_disable_alarm(clock);

    TEST_ASSERT_FALSE(clock_alarm_triggered(clock));
}
void test_get_alarm_before_setting(void){
    clock_time_t alarm = {0};

    TEST_ASSERT_TRUE(clock_get_alarm_time(clock, &alarm));

    TEST_ASSERT_EACH_EQUAL_UINT8(0, alarm.bcd, 6);
}
void test_adjusting_time_disarms_alarm_trigger(void) {
    //con esta funcion se comprueba que al ajustar la hora del reloj,
    //la alarma no se active inmediatamente
    clock_time_t alarm = { .bcd = {0, 0, 0, 0, 0, 1} };//creo la hora de la alarma en 10:00:00
    clock_set_time(clock, &(clock_time_t){ .bcd = {9, 5, 9, 5, 9, 0} });// seteo la hora del reloj en 9:59:59
    clock_set_alarm_time(clock, &alarm);
    clock_enable_alarm(clock);
    clock_set_time(clock, &(clock_time_t){ .bcd = {1, 0, 0, 0, 0, 1} });// seteo la hora del reloj en 10:00:01
    simulate_seconds(clock, 1);
    TEST_ASSERT_FALSE(clock_alarm_triggered(clock));
}

void test_multiple_snoozes_accumulate_correctly(void) {
    clock_time_t alarm = { .bcd = {0, 0, 0, 0, 1, 0} }; // 01:00:00
    clock_set_alarm_time(clock, &alarm);
    clock_snooze_alarm(clock, 10); // 01:10
    clock_snooze_alarm(clock, 5);  // 01:15
    clock_time_t expected = { .bcd = {0, 0, 5, 1, 1, 0} }; // 01:15:00
    clock_get_alarm_time(clock, &current_time);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.bcd, current_time.bcd, 6);
}

void test_get_alarm_returns_set_value(void) {
    clock_time_t alarm = { .bcd = {2, 1, 0, 2, 3, 0} }; // 03:20:12
    clock_set_alarm_time(clock, &alarm);
    clock_time_t received = {0};
    clock_get_alarm_time(clock, &received);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(alarm.bcd, received.bcd, 6);
}

void test_setting_valid_bcd_time(void) {
    // Esta función prueba que se pueda establecer una hora válida en formato BCD
    clock_time_t new_time = { .bcd = {9, 5, 9, 5, 3, 2} }; // 23:59:59
    TEST_ASSERT_TRUE(clock_set_time(clock, &new_time));
    TEST_ASSERT_TRUE(clock_get_time(clock, &current_time));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(new_time.bcd, current_time.bcd, 6);
}

void test_clock_does_not_advance_if_invalid(void) {
    simulate_seconds(clock, 10); // Intentamos avanzar sin haber seteado hora
    TEST_ASSERT_FALSE(clock_get_time(clock, &current_time)); // aún no es válida
    TEST_ASSERT_EACH_EQUAL_UINT8(0, current_time.bcd, sizeof(clock_time_t)); // sigue en 00:00:00
}

void test_clock_configures_ticks_per_second(void) {
    // Solo podemos verificar esto indirectamente haciendo avanzar el reloj
    clock_set_time(clock, &(clock_time_t){0});
    simulate_seconds(clock, 1); // con CLOCK_TICKS_PER_SECOND == 5
    TEST_ASSERT_TIME(0, 0, 0, 0, 0, 1);
}

void test_null_pointer_protection(void) {
    TEST_ASSERT_FALSE(clock_set_time(clock, NULL));
    TEST_ASSERT_FALSE(clock_get_time(clock, NULL));
    TEST_ASSERT_FALSE(clock_set_alarm_time(clock, NULL));
    TEST_ASSERT_FALSE(clock_get_alarm_time(clock, NULL));
}
/* === End of documentation ======================================================================================== */
