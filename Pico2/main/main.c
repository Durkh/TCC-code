#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"

#include "FreeRTOS.h"
#include "task.h"

#include "DCP.h"

#define DCP_ADDR 0x10

struct SensorData {float t; float h; unsigned v; unsigned a; unsigned c;};

static struct DCP_Message_t buttonData = {
    .type = 0,
    .L3 = (struct DCP_Message_L3_t){
        .IDS = DCP_ADDR,
        .IDD = 0xF0,
        .COD = 0x1,
        .data = {0xFF},
        .PAD = 0x0
    }
};

void main_task(void *params) {

    struct SensorData sensorData = {0};
    struct DCP_Message_t* message = NULL;
    while(1){

        message = ReadMessage();

        if (message) {

	    memcpy(&sensorData, &message->generic.payload, sizeof sensorData);
	    printf("t:%.2f\th:%.2f\tv:%i\ta:%i\tc:%i\n",
			    sensorData.t,
			    sensorData.h,
			    sensorData.v,
			    sensorData.a,
			    sensorData.c);

	    gpio_put(14, sensorData.t > 35);
	    gpio_put(15, sensorData.c > 650);

            free(message);
            message = NULL;
        }

        vTaskDelay(pdMS_TO_TICKS(5));

        if(buttonData.L3.data[0] != 0xFF){
            DCP_Data_t buttonMessage = (DCP_Data_t){.data = malloc(sizeof (struct DCP_Message_t))};
            memcpy((void*)buttonMessage.message, &buttonData, sizeof buttonData);

            SendMessage(buttonMessage);
            buttonData.L3.data[0] = 0xFF;
        }

    }
}

void ButtonCallback(void){

    const uint32_t event = gpio_get_irq_event_mask(10);
    if (event & GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL){
        gpio_acknowledge_irq(10, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL);

        if(buttonData.L3.data[0] != 0xFF) return;

        if(event == GPIO_IRQ_EDGE_RISE){
            gpio_put(13,1);
            buttonData.L3.data[0] = 0x1;
        }else if (event == GPIO_IRQ_EDGE_FALL){
            gpio_put(13,0);
            buttonData.L3.data[0] = 0x0;
        }
    }
}

int main( void )
{
    stdio_init_all();

    //LEDs 
    gpio_init_mask(1U<<15 | 1U<<14 | 1U<<13 | 1U<<12 | 1U<<11 | 1U<<28);
    gpio_set_dir_out_masked(1U<<15 | 1U<<14 | 1U<<13 | 1U<<12 | 1U<<11 | 1U<<28);

    //button
    gpio_init(10);
    gpio_set_dir(10, GPIO_IN);
    gpio_set_irq_enabled(10, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_add_raw_irq_handler(10, &ButtonCallback);

    DCP_MODE mode = {.addr = DCP_ADDR, .flags.flags = FLAG_Instant, .isController = 1, .speed = SLOW};

    if (!DCPInit(2, mode)){
        printf("não foi possível iniciar o barramento\n");
	gpio_put(11, 1);
        while(1);
    }

    xTaskCreate(main_task, "MainThread", 2*1024, NULL, configMAX_PRIORITIES-2, NULL);

    vTaskStartScheduler();

    return 0;
}
