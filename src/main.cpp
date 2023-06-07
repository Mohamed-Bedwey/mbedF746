#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);

// ======================= Capteur =================== //

DHT sensor(D8, DHT22);
float temp = 0.0f, humi = 0.0f;
// Temperature
void indicateur_temp(void);
void set_value_temp(lv_meter_indicator_t *indic, int32_t v);
static lv_obj_t *meter_temp;
lv_meter_indicator_t *indic_temp;
// Humidite
void indicateur_humi(void);
void set_value_humi(lv_meter_indicator_t *indic, int32_t v);
static lv_obj_t *meter_humi;
lv_meter_indicator_t *indic_humi;

PwmOut moteur(D3);
PwmOut resistance(D6);
BufferedSerial pc(USBTX, USBRX, 9600);

int main()
{
    int error = 0;

    moteur.period(1.0 / 25000.0);
    moteur.write(0.0);

    resistance.period(1.0 / 25000.0);
    resistance.write(0.0);

    threadLvgl.lock();
    indicateur_temp();
    indicateur_humi();
    threadLvgl.unlock();
    

    while (1)
    {
        // put your main code here, to run repeatedly:

        // =============== Lecture capteur de temperature ===========//

        error = sensor.readData();
        if (0 == error)
        {
            temp = sensor.ReadTemperature(CELCIUS);
            humi = sensor.ReadHumidity();
            printf("Temperature: %.2f\n", temp);
            printf("Humidite: %.2f\n", humi);
        }
        else
        {
            printf("Error: %d\n", error);
        }

        // ====================== Moteur ==================//
        char data;
        if (pc.readable())
        {
            pc.read(&data, 1);
            printf("Lecture : %c\n", data);
            if ((data >= '0') && (data <= '9'))
            {
                moteur.write((data - '0') * 0.1);
            }
        }

        // =================== Reistance ================//

        resistance.write(0.0);

        // ======================Interface ==================//
        threadLvgl.lock();
        set_value_temp(indic_temp, temp);
        set_value_humi(indic_humi, humi);
        threadLvgl.unlock();
        ThisThread::sleep_for(500);
    }
}

void indicateur_temp(void)
{
    meter_temp = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_temp,10,35);
    lv_obj_set_size(meter_temp, 160, 160);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_temp);
    lv_meter_set_scale_ticks(meter_temp, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_temp, scale, 8, 4, 15, lv_color_black(), 10);

    /*Add a blue arc to the start*/
    indic_temp = lv_meter_add_arc(meter_temp, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter_temp, indic_temp, 0);
    lv_meter_set_indicator_end_value(meter_temp, indic_temp, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic_temp = lv_meter_add_scale_lines(meter_temp, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter_temp, indic_temp, 0);
    lv_meter_set_indicator_end_value(meter_temp, indic_temp, 20);

    /*Add a red arc to the end*/
    indic_temp = lv_meter_add_arc(meter_temp, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter_temp, indic_temp, 80);
    lv_meter_set_indicator_end_value(meter_temp, indic_temp, 100);

    /*Make the tick lines red at the end of the scale*/
    indic_temp = lv_meter_add_scale_lines(meter_temp, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter_temp, indic_temp, 80);
    lv_meter_set_indicator_end_value(meter_temp, indic_temp, 100);

    /*Add a needle line indicator*/
    indic_temp = lv_meter_add_needle_line(meter_temp, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
}
void set_value_temp(lv_meter_indicator_t *indic, int32_t v)
{
    lv_meter_set_indicator_value(meter_temp, indic, v);

}


void indicateur_humi(void)
{
    meter_humi = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_humi,310,35);
    lv_obj_set_size(meter_humi, 160, 160);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_humi);
    lv_meter_set_scale_ticks(meter_humi, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_humi, scale, 8, 4, 15, lv_color_black(), 10);

    /*Add a blue arc to the start*/
    indic_humi = lv_meter_add_arc(meter_humi, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter_humi, indic_humi, 0);
    lv_meter_set_indicator_end_value(meter_humi, indic_humi, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic_humi = lv_meter_add_scale_lines(meter_humi, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter_humi, indic_humi, 0);
    lv_meter_set_indicator_end_value(meter_humi, indic_humi, 20);

    /*Add a red arc to the end*/
    indic_humi = lv_meter_add_arc(meter_humi, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter_humi, indic_humi, 80);
    lv_meter_set_indicator_end_value(meter_humi, indic_humi, 100);

    /*Make the tick lines red at the end of the scale*/
    indic_humi = lv_meter_add_scale_lines(meter_humi, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter_humi, indic_humi, 80);
    lv_meter_set_indicator_end_value(meter_humi, indic_humi, 100);

    /*Add a needle line indicator*/
    indic_humi = lv_meter_add_needle_line(meter_humi, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
}
void set_value_humi(lv_meter_indicator_t *indic, int32_t v)
{
    lv_meter_set_indicator_value(meter_humi, indic, v);
}