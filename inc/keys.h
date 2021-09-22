/*=============================================================================
 * Author: Fernando Prokopiuk <fernandoprokopiuk@gmail.com>
 * Date: 2021/09/21
 *===========================================================================*/

#ifndef KEYS_H_
#define KEYS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "sapi.h"

/* public macros ================================================================= */
#define KEYS_INVALID_TIME   -1
/* types ================================================================= */
typedef enum
{
    STATE_BUTTON_UP,
    STATE_BUTTON_DOWN,
    STATE_BUTTON_FALLING,
    STATE_BUTTON_RISING
} keys_ButtonState_t;

typedef struct
{
    gpioMap_t btn;			//config
    SemaphoreHandle_t sem_btn; //semaforo
} t_key_config;

typedef struct
{
    keys_ButtonState_t state;   //variables

    TickType_t time_down;		//timestamp of the last High to Low transition of the key
    TickType_t time_up;		    //timestamp of the last Low to High transition of the key
    TickType_t time_diff;	    //variables
} t_key_data;

/* methods ================================================================= */
void keys_Init( void );
TickType_t get_diff( uint32_t index );
void clear_diff( uint32_t index );

#endif /* PDM_ANTIRREBOTE_MEF_INC_DEBOUNCE_H_ */
