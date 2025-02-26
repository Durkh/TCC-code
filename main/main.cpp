#include <stdint.h>
#include <stdio.h>
#include "DCP.h"

#if defined(__ESP32__)

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <string.h>

#include "Arduino.h"
#include "Adafruit_AHTX0.h"
#include "ScioSense_ENS160.h"

#include "Wire.h"

//#define READ 1

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

    //unsigned char msg[] = "egidio neto da computacao";

    DCP_MODE mode;
#ifdef READ
    mode.addr = 1;
#else
    mode.addr = 2;
#endif
    mode.flags.flags = FLAG_Instant;
    mode.isController = true;
    mode.speed = DCP_MODE::SLOW;

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


    unsigned char msg[] = "egidio";

    while(1){

        if (aht.getStatus() != 0xFF) {
            aht.getEvent(&humidity1, &temp);// populate temp and humidity objects with fresh data

            ESP_LOGI("AHT", "temp: %.2f\thumidity: %.2f", temp.temperature, humidity1.relative_humidity);

            if (ens160.available()) {
                ens160.set_envdata(temp.temperature, humidity1.relative_humidity);

                ens160.measure(true);
                ens160.measureRaw(true);

                ESP_LOGI("ENS", "AQI: %i\tTVOC: %ippb\teCO2: %ippm\t", ens160.getAQI(), ens160.getTVOC(), ens160.geteCO2());
            }
        }


        DCP_Data_t message = {.data = (uint8_t*)malloc(sizeof(struct DCP_Message_Generic_t) + strlen((char*)msg)+1 + sizeof(uint8_t))};

        message.message->type = strlen((char*)msg)+3;
        message.message->generic.addr = mode.addr;
        memcpy(message.message->generic.payload, msg, strlen((char*)msg)+1);
        SanityCheck(message.message->type, message.data);

        SendMessage(message);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
#endif

    while(true);

}
#elif defined(__RPICO__)

#endif
