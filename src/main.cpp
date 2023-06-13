#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);

// Temperature
DHT sensor(D8, DHT22);
float temp = 0.0f, humi = 0.0f;
void indicateur_temp(void);
void set_value_temp(lv_meter_indicator_t *indic, int32_t v);
static lv_obj_t *meter_temp;
lv_meter_indicator_t *indic_temp;

// Humidite
void indicateur_humi(void);
void set_value_humi(lv_meter_indicator_t *indic, int32_t v);
static lv_obj_t *meter_humi;
lv_meter_indicator_t *indic_humi;

//Moteur
PwmOut moteur(D3);
void boutton_moteur(void);
static void event_boutton_moteur(lv_event_t * e);
void led_moteur(void);
lv_obj_t * led_mot;
bool click_moteur = true;

//Resistance
PwmOut resistance(D6);
void boutton_resistance(void);
static void event_boutton_resistance(lv_event_t * e);
void led_resistance(void);
lv_obj_t * led_res;
bool click_resistance = true;


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
    boutton_moteur();
    boutton_resistance();
    led_moteur();
    led_resistance();
    threadLvgl.unlock();
    

    while (1)
    {
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
    lv_obj_set_pos(meter_temp,10,10);
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
    lv_obj_set_pos(meter_humi,310,10);
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

void boutton_moteur(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_boutton_moteur, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn1,10,220);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Ventillation");
    lv_obj_center(label);
}
static void event_boutton_moteur(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {

        if(click_moteur == true)
        {
            moteur.write(0.9);
            threadLvgl.lock();
            lv_led_on(led_mot);
            threadLvgl.unlock();
            click_moteur = false;
        }
        else
        {
            moteur.write(0.0);
            threadLvgl.lock();
            lv_led_off(led_mot);
            threadLvgl.unlock();
            click_moteur = true;
        }

    }
}
void led_moteur(void)
{
    led_mot  = lv_led_create(lv_scr_act());
    lv_led_set_brightness(led_mot, LV_LED_BRIGHT_MAX);
    lv_obj_set_pos(led_mot,53,190);
    lv_led_set_color(led_mot, lv_palette_main(LV_PALETTE_GREEN));
    lv_led_off(led_mot);
}

void boutton_resistance(void)
{
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, event_boutton_resistance, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn1,150,220);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Chauffage");
    lv_obj_center(label);
}
static void event_boutton_resistance(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {

         if(click_resistance == true)
        {
            resistance.write(0.9);
            threadLvgl.lock();
            lv_led_on(led_res);
            threadLvgl.unlock();
            click_resistance = false;
        }
        else
        {
            resistance.write(0.0);
            threadLvgl.lock();
            lv_led_off(led_res);
            threadLvgl.unlock();
            click_resistance = true;
        }

    }
}
void led_resistance(void)
{
    led_res  = lv_led_create(lv_scr_act());
    lv_led_set_brightness(led_res, LV_LED_BRIGHT_MAX);
    lv_obj_set_pos(led_res,185,190);
    lv_led_set_color(led_res, lv_palette_main(LV_PALETTE_GREEN));
    lv_led_off(led_res);
}