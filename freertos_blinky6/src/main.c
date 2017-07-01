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

typedef enum
{
   MSG_ENCENDER_LED = 0,
   MSG_APAGAR_LED,
   MSG_CAMBIAR_INTENSIDAD,
}msgCmd_enum;

typedef struct
{
   int32_t delay;
}camposEncApa_type;

typedef struct
{
   int32_t intensidad;
}camposCamInt_type;

typedef struct
{
   msgCmd_enum cmd;
   union
   {
      camposEncApa_type camposEncApa;
      camposCamInt_type camposCamInt;
   }datos;
}msgData_type;

#define MSG_QUEUE_LENGTH   5

/*==================[internal data declaration]==============================*/
static QueueHandle_t xQueue;

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
   msgData_type msg;
   int32_t cont = 0;
   int32_t intensidad = 0;

   while (1)
   {
      vTaskDelay(500 / portTICK_RATE_MS);

      Board_LED_Toggle(0);

      if (msg.cmd == MSG_ENCENDER_LED)
         msg.cmd = MSG_APAGAR_LED;
      else
         msg.cmd = MSG_ENCENDER_LED;

      msg.datos.camposEncApa.delay = 250;

      xQueueSend(xQueue, &msg, portMAX_DELAY);

      cont++;
      if (cont > 1000)
      {
         cont = 0;
         intensidad += 10;

         msg.cmd = MSG_CAMBIAR_INTENSIDAD;
         msg.datos.camposCamInt = intensidad;
         xQueueSend(xQueue, &msg, portMAX_DELAY);
      }

	}
}

static void task2(void * a)
{
   msgData_type msg;

   while (1)
   {
      xQueueReceive(xQueue, &msg, portMAX_DELAY);

      switch (msg.cmd)
      {
         case MSG_ENCENDER_LED:
            vTaskDelay(msg.datos.camposEncApa.delay / portTICK_RATE_MS);
            Board_LED_Set(3, true);
            break;

         case MSG_APAGAR_LED:
            vTaskDelay(msg.datos.camposEncApa.delay / portTICK_RATE_MS);
            Board_LED_Set(3, false);
            break;

         case MSG_CAMBIAR_INTENSIDAD:
            break;
      }
   }
}

/*==================[external functions definition]==========================*/

int main(void)
{
	initHardware();

	xQueue = xQueueCreate(MSG_QUEUE_LENGTH, sizeof(msgData_type));

	xTaskCreate(task1, (const char *)"task1", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, NULL);

	xTaskCreate(task2, (const char *)"task2", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+1, NULL);

	vTaskStartScheduler();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
