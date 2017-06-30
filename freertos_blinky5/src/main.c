/* Copyright 2015, Pablo Ridolfi
 * Copyright 2017, Gustavo Muro.
 * All rights reserved.
 *
 * This file is part of lpc1769_template.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \addtogroup rtos_blink FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "board.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

#include "main.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
static SemaphoreHandle_t semaphore;

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();
}

static void task1(void * a)
{
   Board_LED_Set(0, false);

   while (1)
   {
      vTaskDelay(500 / portTICK_RATE_MS);
      xSemaphoreGive(semaphore);
		Board_LED_Toggle(0);
	}
}

static void task2(void * a)
{
   Board_LED_Set(3, false);

   xSemaphoreTake(semaphore, 0);

   while (1)
   {
      xSemaphoreTake(semaphore, portMAX_DELAY);
      vTaskDelay(100 / portTICK_RATE_MS);
      Board_LED_Toggle(3);
   }
}

/*==================[external functions definition]==========================*/

int main(void)
{
	initHardware();

	semaphore = xSemaphoreCreateBinary();

	xTaskCreate(task1, (const char *)"task1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, NULL);

	xTaskCreate(task2, (const char *)"task2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
