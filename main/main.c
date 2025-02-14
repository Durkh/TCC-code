#include <stdio.h>
#include "DCP.h"

#if defined(__ESP32__)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <string.h>

#include "DCP.h"

#define READ 1

void SanityCheck(unsigned const size, uint8_t const msg[size]){
    for (int i = 0; i < size; ++i)
        for (int j = 7; j >= 0; --j){
            printf("%d", ((msg[i] >> j) & 0x1) == 1 );
        }
    printf("\r\n");
}

void app_main(void){

    //unsigned char msg[] = "egidio neto da computacao";

    DCP_MODE mode;
#ifdef READ
    mode.addr = 1;
#else
    mode.addr = 2;
#endif
    mode.flags.flags = FLAG_Instant;
    mode.isController = true;
    mode.speed = SLOW;

    if (!DCPInit(1, mode)){
        ESP_LOGE("MAIN", "Error initializing bus");
        return;
    }

#ifdef READ
    struct DCP_Message_t* message = NULL;

    while(1){
        message = ReadMessage();

        if (message) {
            DCP_Data_t debug = {.message = message};
            SanityCheck(message->type, debug.data);
            free(message);
            message = NULL;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
#else 

    unsigned char msg[] = "egidio";

    while(1){
        DCP_Data_t message = {.data = malloc(sizeof(struct DCP_Message_Generic_t) + strlen((char*)msg)+1 + sizeof(enum DCP_Message_type_e))};

        message.message->type = strlen((char*)msg)+3;
        message.message->generic.addr = mode.addr;
        memcpy(message.message->generic.payload, msg, strlen((char*)msg)+1);
        SanityCheck(message.message->type, message.data);

        SendMessage(message);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
#endif
}

#elif defined(__RPICO__)

#endif
