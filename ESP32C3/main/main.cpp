#include <stdint.h>
#include <stdio.h>
#include "DCP.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <string.h>

#include "Arduino.h"
#include "Adafruit_AHTX0.h"
#include "ScioSense_ENS160.h"

#include "Wire.h"

#define DCP_ADDR 0xF0

Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

void SanityCheck(unsigned const int size, uint8_t const msg[]){
    for (int i = 0; i < size; ++i)
        for (int j = 7; j >= 0; --j){
            printf("%d", ((msg[i] >> j) & 0x1) == 1 );
        }
    printf("\r\n");
}

extern "C" void app_main(void){

    initArduino();
    Serial.begin(115200);

    DCP_MODE mode;
    mode.addr = DCP_ADDR;
    mode.flags.flags = FLAG_Instant;
    mode.isController = true;
    //mode.speed = DCP_MODE::FAST2;
    mode.speed = DCP_MODE::SLOW;

    if (!DCPInit(1, mode)){
        ESP_LOGE("MAIN", "Error initializing bus");
        return;
    }

    ens160.setI2C(4, 3);
    Wire.setPins(4, 3);

    ens160.begin();
    if (ens160.available()) {
        ens160.setMode(ENS160_OPMODE_STD);
        ESP_LOGI("ENS", "successfully initialized");
    }else {
        ESP_LOGE("ENS", "error initializing");
    }

    if (!aht.begin()) {
        ESP_LOGE("AHT", "error initializing");

        for (int i = 0; i < 10; ++i){
                vTaskDelay(pdMS_TO_TICKS(10));
        }
    } else {
        ESP_LOGI("AHT", "successfully initialized");
    }

    sensors_event_t humidity1, temp;
    struct {float t; float h; unsigned v; unsigned a; unsigned c;} sensorData;
    gpio_set_direction(GPIO_NUM_10, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_10, 0);

    while(1){

        if (aht.getStatus() != 0xFF) {
            aht.getEvent(&humidity1, &temp);// populate temp and humidity objects with fresh data

            ESP_LOGI("AHT", "temp: %.2f\thumidity: %.2f", temp.temperature, humidity1.relative_humidity);
            sensorData.t = temp.temperature;
            sensorData.h = humidity1.relative_humidity;

            if (ens160.available()) {
                ens160.set_envdata(temp.temperature, humidity1.relative_humidity);
                ens160.measure(true);
                ens160.measureRaw(true);

                sensorData.a = ens160.getAQI();
                sensorData.v = ens160.getTVOC();
                sensorData.c = ens160.geteCO2();

                ESP_LOGI("ENS", "AQI: %i\tTVOC: %ippb\teCO2: %ippm\t", ens160.getAQI(), ens160.getTVOC(), ens160.geteCO2());
            }
        }

        DCP_Data_t message = {.data = (uint8_t*)malloc(sizeof(struct DCP_Message_Generic_t) + sizeof sensorData + sizeof(uint8_t))};

        message.message->type = sizeof sensorData + 2;
        message.message->generic.addr = mode.addr;

        memcpy(message.message->generic.payload, &sensorData, sizeof sensorData);
        SanityCheck(message.message->type, message.data);

        SendMessage(message);

	DCP_Message_t* received = ReadMessage();
	if (received){
	    assert(received->type == 0);
	    assert(received->L3.IDD == DCP_ADDR);
	    gpio_set_level(GPIO_NUM_10, received->L3.data[0]);

	    free(received);
	}
    }

    while(true);

}
