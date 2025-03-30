#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "DCP.h"

void main_task(void *params) {

    struct DCP_Message_t* message = NULL;
    while(1){

        message = ReadMessage();

        if (message) {
            printf("read: %s", message->generic.payload);

            free(message);
            message = NULL;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
        printf("reading\n");
    }
}

int main( void )
{
    stdio_init_all();

    DCP_MODE mode = {.addr = 0x10, .flags.flags = FLAG_Instant, .isController = 0, .speed = SLOW};

    if (!DCPInit(2, mode)){
        printf("não foi possível iniciar o barramento\n");
        while(1);
    }

    xTaskCreate(main_task, "MainThread", 2*1024, NULL, configMAX_PRIORITIES-2, NULL);

    vTaskStartScheduler();

    return 0;
}
