#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include "DHT.h"
#include <cstdio>

ThreadLvgl threadLvgl(30);
static lv_style_t style_btn_on; // Style boutton pour etat on
static lv_style_t style_btn_off; // Style boutton pour etat off
static lv_style_t style_btn_auto; // Style boutton pour mode auto
BufferedSerial pc(USBTX, USBRX, 9600); // Initialisation de la liaison serie

// Temperature
DHT sensor(D8, DHT22);  // Pin STM 32
float temp = 0.0f, humi = 0.0f;// Variable pour stocker la temperature et l'humidite
static lv_obj_t *meter_temp;
lv_meter_indicator_t *indic_temp; // Indicateur de temperature
void indicateur_temp(void);  // Gauge temperature

// Humidite
static lv_obj_t *meter_humi;
lv_meter_indicator_t *indic_humi; // Indicateur d'humidite
void indicateur_humi(void); // Gauge humidite

//Moteur
PwmOut moteur(D3); // PWM ventilation
lv_obj_t * btn_mot;
bool click_moteur = true; // Boolean pour detection d'un click
static lv_obj_t * slider_label_mot;
lv_obj_t * slider_mot_obj;
void slider_mot(void); // Slider puisance ventilation
void boutton_moteur(void);// Boutton pour activer ou desactiver la ventilation
static void slider_event_mot(lv_event_t * e);
static void boutton_event_moteur(lv_event_t * e);

//Resistance
PwmOut resistance(D6); //PWM du chauffage
lv_obj_t * btn_res;
bool click_resistance = true; // Boolean pour detection d'un click
static lv_obj_t * slider_label_res;
lv_obj_t * slider_res_obj;
void slider_res(void); // Slider puissance chuaffage
void boutton_resistance(void);// Boutton pour activer ou desactiver le chauffage 
static void slider_event_res(lv_event_t * e);
static void boutton_event_resistance(lv_event_t * e);

//Mode auto
lv_obj_t * btn_auto;
bool click_auto = true; // Boolean pour detection d'un cclick
static lv_obj_t * saisie_temp;
int val_user; 
void boutton_auto(void); // Boutton mode auto
static void boutton_event_auto(lv_event_t * e);
void zone_saisie_temp(void); // Zone de saisie température
static void decrement_event(lv_event_t * e);
static void increment_event(lv_event_t * e);

int main()
{
    int error = 0;

    moteur.period(1.0 / 25000.0); //Periode PWM moteur
    moteur.write(0.0); // Initialisation de la PWM a 0

    resistance.period(1.0 / 25000.0); //Periode PWM resistance
    resistance.write(0.0);// Initialisation de la PWM a 0

    // ============ Affichage des éléments sur l'interface ===============//
    threadLvgl.lock();
    lv_style_set_bg_color(&style_btn_off, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_color(&style_btn_on, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_color(&style_btn_auto, lv_palette_main(LV_PALETTE_ORANGE));
    indicateur_temp(); // Gauge de température
    indicateur_humi(); // Gauge d'humidite
    boutton_moteur(); // Bouton pour activer ou desactiver la ventilation          
    boutton_resistance(); // Bouton pour activer ou desactiver le chauffage 
    slider_mot(); // Slider pour la puissance de la ventilation
    slider_res(); // Slider pour la puissance du chuaffage
    boutton_auto(); // Bouttton pour activer le mode auto
    zone_saisie_temp(); // Zone de saisie pour la température de l'utilisateur
    threadLvgl.unlock();
    
    while (1)
    {
        // =============== Lecture du capteur ===========//

        error = sensor.readData();
        if (0 == error)
        {
            temp = sensor.ReadTemperature(CELCIUS); //Recuperation de la température
            humi = sensor.ReadHumidity(); // Recuperation de l'humidite
            // Affichage dans la liaison serie
            printf("Temperature: %.2f\n", temp);
            printf("Humidite: %.2f\n", humi);
        }
        else
        {
            printf("Error: %d\n", error);
        }

        threadLvgl.lock();
        // ================= Recuperation de la valeur des sliders et ecriture de la PWM ============//
        if(click_moteur == false)
        {
            moteur.write((int)lv_slider_get_value(slider_mot_obj)*0.01);
        }
        if(click_resistance == false)
        {
            resistance.write((int)lv_slider_get_value(slider_res_obj)*0.01);
        }
        // ================== Affichage des valeurs sur les gauges de l'interface ============//
        lv_meter_set_indicator_value(meter_temp, indic_temp, temp);
        lv_meter_set_indicator_value(meter_humi, indic_humi, humi);
        threadLvgl.unlock();

        // =============================== mode auto ===========================//

        if (click_auto == false)
        {
            threadLvgl.lock();
            val_user = (int)lv_spinbox_get_value(saisie_temp);
            threadLvgl.unlock();
            printf("Val user: %.2d\n", val_user);

            if (temp > val_user) // Si la tempearture est supérieure a la valeur de l'utilisateur donc on refroidie
            {
                moteur.write(1.0);
                resistance.write(0.0);
            }
            else //Si la tempearture est inférieure a la valeur de l'utilisateur donc on chauffe la serre
            {
                moteur.write(0.3);
                resistance.write(1.0);
            }
        }

        ThisThread::sleep_for(500);
    }
}

void indicateur_temp(void)
{
    // Creation de la gauge
    lv_obj_t * label;
    meter_temp = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_temp,5,10);
    lv_obj_set_size(meter_temp, 180, 180);

    // Choix de l'echelle des valeurs
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_temp);
    lv_meter_set_scale_ticks(meter_temp, scale, 51, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_temp, scale, 5, 4, 15, lv_color_black(), 10);

    // Indicateur de gauge
    indic_temp = lv_meter_add_needle_line(meter_temp, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    // Ajout du nom de la gauge
    label = lv_label_create(meter_temp);
    lv_label_set_text(label, "Temperature");
    lv_obj_set_pos(label,25,120);
}

void indicateur_humi(void)
{
    // Creation de la gauge
    lv_obj_t * label;
    meter_humi = lv_meter_create(lv_scr_act());
    lv_obj_set_pos(meter_humi,295,10);
    lv_obj_set_size(meter_humi, 180, 180);

    // Choix de l'echelle des valeurs
    lv_meter_scale_t *scale = lv_meter_add_scale(meter_humi);
    lv_meter_set_scale_ticks(meter_humi, scale, 51, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_humi, scale, 5, 4, 15, lv_color_black(), 10);

    // Indicateur de gauge
    indic_humi = lv_meter_add_needle_line(meter_humi, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    // Ajout du nom de la gauge
    label = lv_label_create(meter_humi);
    lv_label_set_text(label, "Humidite");
    lv_obj_set_pos(label,40,120);
}

void boutton_moteur(void)
{
    lv_obj_t * label;

    // Creation du boutton
    btn_mot = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_mot, boutton_event_moteur, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn_mot,190,80);
    lv_obj_set_size(btn_mot,100,30);
    lv_obj_add_style(btn_mot,&style_btn_off,0);

    // Label inscrit sur le boutton
    label = lv_label_create(btn_mot);
    lv_label_set_text(label, "Ventilation");
    lv_obj_center(label);
}
static void boutton_event_moteur(lv_event_t * e)
{
    // Detection d'un click
    lv_event_code_t code = lv_event_get_code(e);

    if (click_auto == true)
    {
        if(code == LV_EVENT_CLICKED) {

        if(click_moteur == true)
        {
            threadLvgl.lock();
            lv_obj_add_style(btn_mot,&style_btn_on,0); //Le style du boutton passe en position on
            threadLvgl.unlock();
            click_moteur = false;
        }
        else
        {
            moteur.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_mot,&style_btn_off,0); //Le style du boutton passe en position off
            threadLvgl.unlock();
            click_moteur = true;
        }

     }
    }

    
}
void slider_mot(void)
{
    //Creation du slider
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider,20,200);
    lv_obj_add_event_cb(slider, slider_event_mot, LV_EVENT_VALUE_CHANGED, NULL);

    // Creation du label pour indiquer le pourcentage
    slider_label_mot = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label_mot, "0%");
    lv_obj_align_to(slider_label_mot, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);

    //Nom du slider
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Ventilation");
    lv_obj_set_pos(label,120,215);

}
static void slider_event_mot(lv_event_t * e)
{
    // Recuperation de la valeur du slider a cahque changement de valeur
    slider_mot_obj = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider_mot_obj));
    lv_label_set_text(slider_label_mot, buf);
    lv_obj_align_to(slider_label_mot, slider_mot_obj, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0); 

}

void boutton_resistance(void)
{
    lv_obj_t * label;

    // Creation du boutton
    btn_res = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_res, boutton_event_resistance, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn_res,190,120);
    lv_obj_set_size(btn_res,100,30);
    lv_obj_add_style(btn_res,&style_btn_off,0);

    // Label inscrit sur le boutton
    label = lv_label_create(btn_res);
    lv_label_set_text(label, "Chauffage");
    lv_obj_center(label);
}
static void boutton_event_resistance(lv_event_t * e)
{
    // Detection d'un click
    lv_event_code_t code = lv_event_get_code(e);

    if (click_auto == true)
    {
        if(code == LV_EVENT_CLICKED) {

        if(click_resistance == true)
        {
            threadLvgl.lock();
            lv_obj_add_style(btn_res,&style_btn_on,0); //Le style du boutton passe en position on
            threadLvgl.unlock();
            click_resistance = false;
        }
        else
        {
            resistance.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_res,&style_btn_off,0); //Le style du boutton passe en position off
            threadLvgl.unlock();
            click_resistance = true;
        }

        }
    }

    
}
void slider_res(void)
{
    //Creation du slider
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_set_pos(slider,20,235);
    lv_obj_add_event_cb(slider, slider_event_res, LV_EVENT_VALUE_CHANGED, NULL);

    // Creation du label pour indiquer le pourcentage
    slider_label_res = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label_res, "0%");
    lv_obj_align_to(slider_label_res, slider, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);

    //Nom du slider
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Chauffage");
    lv_obj_set_pos(label,120,250);

}
static void slider_event_res(lv_event_t * e)
{
    // Recuperation de la valeur du slider a cahque changement de valeur
    slider_res_obj = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider_res_obj));
    lv_label_set_text(slider_label_res, buf);
    lv_obj_align_to(slider_label_res, slider_res_obj, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0); 
}

void boutton_auto(void)
{
    lv_obj_t * label;

    // Creation du boutton
    btn_auto = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn_auto, boutton_event_auto, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn_auto,190,40);
    lv_obj_set_size(btn_auto,100,30);
    lv_obj_add_style(btn_auto,&style_btn_off,0);

    // Label inscrit sur le boutton
    label = lv_label_create(btn_auto);
    lv_label_set_text(label, "Auto");
    lv_obj_center(label);
}
static void boutton_event_auto(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {

        if(click_auto == true) //Si le mode auto est activé tout les boutton passe en off et desactivation de la ventilation et du chauffage
        {
            moteur.write(0.0);
            resistance.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_auto,&style_btn_auto,0);
            lv_obj_add_style(btn_mot,&style_btn_off,0);
            lv_obj_add_style(btn_res,&style_btn_off,0);
            threadLvgl.unlock();
            click_auto = false;
            click_moteur = true;
            click_resistance = true;
        }
        else
        {
            moteur.write(0.0);
            resistance.write(0.0);
            threadLvgl.lock();
            lv_obj_add_style(btn_auto,&style_btn_off,0);
            lv_obj_add_style(btn_mot,&style_btn_off,0);
            lv_obj_add_style(btn_res,&style_btn_off,0);
            threadLvgl.unlock();
            click_auto = true;
        }

    }

}
static void increment_event(lv_event_t * e)
{
    // Incrementation de la valeur dans la zone de saise de temperature
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(saisie_temp);
    }
}
static void decrement_event(lv_event_t * e)
{
    // Decrementation de la valeur dans la zone de saise de temperature
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(saisie_temp);
    }
}
void zone_saisie_temp(void)
{
    // Creation de la zone de texte
    saisie_temp = lv_spinbox_create(lv_scr_act());
    lv_spinbox_set_range(saisie_temp, 0, 80);
    lv_spinbox_set_digit_format(saisie_temp, 2, 0);
    lv_obj_set_width(saisie_temp, 40);
    lv_obj_set_pos(saisie_temp,380,205);

    lv_coord_t h = lv_obj_get_height(saisie_temp);

    // Creation du bouton plus
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, saisie_temp, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, increment_event, LV_EVENT_ALL,  NULL);

    // Creation du bouton moin
    btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, h, h);
    lv_obj_align_to(btn, saisie_temp, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, decrement_event, LV_EVENT_ALL, NULL);
}