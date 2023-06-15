#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);
static lv_style_t style_btn_on;
static lv_style_t style_btn_off;
BufferedSerial pc(USBTX, USBRX, 9600);

// Temperature
DHT sensor(D8, DHT22);
float temp = 0.0f, humi = 0.0f;
static lv_obj_t *meter_temp;
lv_meter_indicator_t *indic_temp;
void indicateur_temp(void);

// Humidite
static lv_obj_t *meter_humi;
lv_meter_indicator_t *indic_humi;
void indicateur_humi(void);

//Moteur
PwmOut moteur(D3);
lv_obj_t * btn_mot;
bool click_moteur = true;
static lv_obj_t * slider_label_mot;
void slider_mot(void);
void boutton_moteur(void);
static void slider_event_mot(lv_event_t * e);
static void boutton_even_moteur(lv_event_t * e);

//Resistance
PwmOut resistance(D6);
lv_obj_t * btn_res;
bool click_resistance = true;
static lv_obj_t * slider_label_res;
void slider_res(void);
void boutton_resistance(void);
static void slider_event_res(lv_event_t * e);
static void boutton_even_resistance(lv_event_t * e);

int main()
{
    int error = 0;

    moteur.period(1.0 / 25000.0);
    moteur.write(0.0);

    resistance.period(1.0 / 25000.0);
    resistance.write(0.0);

    threadLvgl.lock();
    lv_style_set_bg_color(&style_btn_off, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_color(&style_btn_on, lv_palette_main(LV_PALETTE_GREEN));
    indicateur_temp();
    indicateur_humi();
    boutton_moteur();
    boutton_resistance();
    slider_mot();
    slider_res();
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

        // ======================Interface ==================//
        threadLvgl.lock();
        lv_meter_set_indicator_value(meter_temp, indic_temp, temp);
        lv_meter_set_indicator_value(meter_humi, indic_humi, humi);
        threadLvgl.unlock();
        ThisThread::sleep_for(500);
    }
}

void indicateur_temp(void)
{
    lv_obj_t * label;
    meter_temp = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_temp,5,10);
    lv_obj_set_size(meter_temp, 180, 180);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_temp);
    lv_meter_set_scale_ticks(meter_temp, scale, 51, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_temp, scale, 5, 4, 15, lv_color_black(), 10);

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

    label = lv_label_create(meter_temp);
    lv_label_set_text(label, "Temperature");
    lv_obj_set_pos(label,25,120);
}

void indicateur_humi(void)
{
    lv_obj_t * label;
    meter_humi = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_humi,295,10);
    lv_obj_set_size(meter_humi, 180, 180);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_humi);
    lv_meter_set_scale_ticks(meter_humi, scale, 51, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_humi, scale, 5, 4, 15, lv_color_black(), 10);

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

    label = lv_label_create(meter_humi);
    lv_label_set_text(label, "Humidite");
    lv_obj_set_pos(label,40,120);
}

void boutton_moteur(void)
{
    lv_obj_t * label;

    btn_mot = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_mot, boutton_even_moteur, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn_mot,190,80);
    lv_obj_set_size(btn_mot,100,30);
    lv_obj_add_style(btn_mot,&style_btn_off,0);

    label = lv_label_create(btn_mot);
    lv_label_set_text(label, "Ventillation");
    lv_obj_center(label);
}
static void boutton_even_moteur(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {

        if(click_moteur == true)
        {
            threadLvgl.lock();
            lv_obj_add_style(btn_mot,&style_btn_on,0);
            threadLvgl.unlock();
            click_moteur = false;
        }
        else
        {
            moteur.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_mot,&style_btn_off,0);
            threadLvgl.unlock();
            click_moteur = true;
        }

    }
}
void slider_mot(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider,20,200);
    lv_obj_add_event_cb(slider, slider_event_mot, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a label below the slider*/
    slider_label_mot = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label_mot, "0%");

    lv_obj_align_to(slider_label_mot, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
}
static void slider_event_mot(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label_mot, buf);
    lv_obj_align_to(slider_label_mot, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0); 
    if(click_moteur == false)
    {
        moteur.write((int)lv_slider_get_value(slider)*0.01);
    }
}

void boutton_resistance(void)
{
    lv_obj_t * label;

    btn_res = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_res, boutton_even_resistance, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn_res,190,120);
    lv_obj_set_size(btn_res,100,30);
    lv_obj_add_style(btn_res,&style_btn_off,0);

    label = lv_label_create(btn_res);
    lv_label_set_text(label, "Chauffage");
    lv_obj_center(label);
}
static void boutton_even_resistance(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {

         if(click_resistance == true)
        {
            threadLvgl.lock();
            lv_obj_add_style(btn_res,&style_btn_on,0);
            threadLvgl.unlock();
            click_resistance = false;
        }
        else
        {
            resistance.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_res,&style_btn_off,0);
            threadLvgl.unlock();
            click_resistance = true;
        }

    }
}
void slider_res(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider,20,240);
    lv_obj_add_event_cb(slider, slider_event_res, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a label below the slider*/
    slider_label_res = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label_res, "0%");

    lv_obj_align_to(slider_label_res, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
}
static void slider_event_res(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label_res, buf);
    lv_obj_align_to(slider_label_res, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0); 
    if(click_resistance == false)
    {
        resistance.write((int)lv_slider_get_value(slider)*0.01);
    }
}