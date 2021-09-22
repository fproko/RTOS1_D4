/*=============================================================================
 * Author: Fernando Prokopiuk <fernandoprokopiuk@gmail.com>
 * Date: 2021/09/21
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sapi.h"

#include "FreeRTOSConfig.h"
#include "keys.h"
/*==================[definiciones y macros]==================================*/
#define LED_RATE_MS 1000
#define LED_RATE pdMS_TO_TICKS(LED_RATE_MS)

#define LED_DUTY_MS 250
#define LED_DUTY pdMS_TO_TICKS(LED_DUTY_MS)

#define GPIO_COUNT   sizeof(gpio_t)/sizeof(gpio_t[0])
#define LED_COUNT    sizeof(leds_green)/sizeof(leds_green[0])

#define WELCOME_MSG  "Ejercicio D_4.\r\n"
#define USED_UART UART_USB
#define UART_RATE 115200
#define MALLOC_ERROR "Malloc Failed Hook!\n"
#define MSG_ERROR_SEM "Error al crear los semaforos.\r\n"
#define LED_ERROR LEDR

/*==================[definiciones de datos internos]=========================*/
//gpioMap_t leds_t[] = {LEDR,LED1,LEDG,LED3};
const gpioMap_t leds_red[] = {LEDR,LED1};
const gpioMap_t leds_green[] = {LEDG,LED3};
const gpioMap_t gpio_t[] = {GPIO7,GPIO5,GPIO3,GPIO1};

/*==================[definiciones de datos externos]=========================*/
DEBUG_PRINT_ENABLE;

extern t_key_config keys_config[];

/*==================[declaraciones de funciones internas]====================*/
void gpio_init( void );

/*==================[declaraciones de funciones externas]====================*/
TickType_t get_diff();
void clear_diff();

// Prototipo de funcion de la tarea
void tarea_led( void* taskParmPtr );
void tarea_tecla( void* taskParmPtr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
    // ---------- CONFIGURACIONES ------------------------------
    boardConfig();									// Inicializar y configurar la plataforma

    gpio_init();

    debugPrintConfigUart( USED_UART, UART_RATE );		// UART for debug messages
    printf( WELCOME_MSG );

    BaseType_t res;
    uint32_t i;

    // Crear tarea en freeRTOS
    for ( i = 0 ; i < LED_COUNT ; i++ )
    {
        res = xTaskCreate(
                  tarea_led,                     // Funcion de la tarea a ejecutar
                  ( const char * )"tarea_led",   // Nombre de la tarea como String amigable para el usuario
                  configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
		  (void *)i,                          // Parametros de tarea
                  tskIDLE_PRIORITY+1,         // Prioridad de la tarea
                  0                           // Puntero a la tarea creada en el sistema
            );
        // Gestion de errores
        configASSERT( res == pdPASS );
    }

    // Inicializo driver de teclas
    keys_Init();

    // Iniciar scheduler
    vTaskStartScheduler();					// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    // ---------- REPETIR POR SIEMPRE --------------------------
    configASSERT( 0 );

    // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
    // directamenteno sobre un microcontroladore y no es llamado por ningun
    // Sistema Operativo, como en el caso de un programa para PC.
    return TRUE;
}

/*==================[definiciones de funciones internas]=====================*/
void gpio_init( void )
{
    uint32_t i;

    gpioInit ( GPIO0, GPIO_OUTPUT );

    for( i = 0 ; i < GPIO_COUNT; i++ )
    {
        gpioInit ( gpio_t[i], GPIO_OUTPUT );
    }
}
/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea
void tarea_led( void* taskParmPtr )
{
    uint32_t index = ( uint32_t ) taskParmPtr;

    // ---------- CONFIGURACIONES ------------------------------
    TickType_t xPeriodicity = LED_RATE; // Tarea periodica cada 1000 ms
    TickType_t xLastWakeTime = xTaskGetTickCount();
    TickType_t dif;
    gpioMap_t led;
    // ---------- REPETIR POR SIEMPRE --------------------------
    while (TRUE)
    {
        if (xSemaphoreTake(keys_config[index].sem_btn, 0) == pdTRUE)
        {
            dif = get_diff(index);
            if (dif > LED_RATE)
            {
                dif = LED_RATE;
            }
            led = leds_green[index];
        }
        else
        {
            dif = LED_DUTY;
            led = leds_red[index];
        }

        gpioWrite(led, ON);
        gpioWrite(gpio_t[index], ON);
        vTaskDelay(dif);
        gpioWrite(led, OFF);
        gpioWrite(gpio_t[index], OFF);

        vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
    }
}

/* hook que se ejecuta si al necesitar un objeto dinamico, no hay memoria disponible */
void vApplicationMallocFailedHook()
{
    printf( MALLOC_ERROR );
    configASSERT( 0 );
}
/*==================[fin del archivo]========================================*/
