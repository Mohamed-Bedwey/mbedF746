#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);

DHT sensor(D8, DHT22);

int main() {

    threadLvgl.lock();
   // lv_demo_widgets();
    threadLvgl.unlock();
    
    int error=0;
    float temp = 0.0f, humi = 0.0f;

    while (1) {
        // put your main code here, to run repeatedly:

        ThisThread::sleep_for(2500);

        error = sensor.readData();
        if (0 == error) {
            temp   = sensor.ReadTemperature(CELCIUS);
            humi   = sensor.ReadHumidity();
            printf("Temperature: %.2f\n",temp);
            printf("Humidite: %.2f\n",humi);
        } else {
            printf("Error: %d\n", error);
        }

    }
}