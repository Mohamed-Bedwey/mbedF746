#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);

DHT sensor(D8, DHT22);
PwmOut moteur (D3);
PwmOut resistance (D6);
BufferedSerial pc(USBTX, USBRX, 9600);

int main() {

    threadLvgl.lock();
   // lv_demo_widgets();
    threadLvgl.unlock();
    
    int error=0;
    float temp = 0.0f, humi = 0.0f;

    moteur.period(1.0/25000.0);
    moteur.write(0.0);

    resistance.period(1.0/25000.0);
    resistance.write(0.0);
    
    

    while (1) {
        // put your main code here, to run repeatedly:

        // ================================================ Capteur de temperature ====================================//

        error = sensor.readData();
        if (0 == error) {
            temp   = sensor.ReadTemperature(CELCIUS);
            humi   = sensor.ReadHumidity();
            printf("Temperature: %.2f\n",temp);
            printf("Humidite: %.2f\n",humi);
        } else {
            printf("Error: %d\n", error);
        }

        // ============================================== Moteur ======================================================//
        char data;
        if (pc.readable()) {
            pc.read(&data, 1);
            printf("Lecture : %c\n", data);
            if ((data>='0')&&(data<='9')) {
                moteur.write((data-'0')*0.1);
            }
        }

        // ============================================== Reistance ======================================================//

        resistance.write(0.0);

    

        ThisThread::sleep_for(1000);

    }
}