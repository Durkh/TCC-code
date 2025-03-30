#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "DCP.h"

void main_task(void *params) {

    struct DCP_Message_t* message = NULL;
    while(1){

        message = ReadMessage();

        if (message) {
            gpio_put(14, 1);
            printf("read: %s\n", message->generic.payload);

            free(message);
            message = NULL;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_xor_mask(1u << 15);
        printf("reading\n");
    }
}

int main( void )
{
    stdio_init_all();

    gpio_init_mask(1U<<15 | 1U<<14 | 1U<<13 | 1U<<12 | 1U<<11);
    gpio_set_dir_out_masked(1U<<15 | 1U<<14 | 1U<<13 | 1U<<12 | 1U<<11);

    DCP_MODE mode = {.addr = 0x10, .flags.flags = FLAG_Instant, .isController = 0, .speed = SLOW};

    if (!DCPInit(2, mode)){
        printf("não foi possível iniciar o barramento\n");
        while(1);
    }

    xTaskCreate(main_task, "MainThread", 2*1024, NULL, configMAX_PRIORITIES-2, NULL);

    vTaskStartScheduler();

    return 0;
}
