#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

// Configurações de pinos
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
const uint MIC_PIN = 28;    
const uint BUTTON_A_PIN = 5;  
const uint BUTTON_B_PIN = 6;  
const uint LED_RED_PIN = 10;  
const uint LED_GREEN_PIN = 11;
const uint LED_BLUE_PIN = 12;
const uint BUZZER_PIN = 13; 

// Configurações do ADC para detecção de som
const float SOUND_OFFSET = 1.65;
const float SOUND_THRESHOLD = 1.0;
const float ADC_REF = 3.3;        
const int ADC_RES = 4095;          

// Estado do sistema
bool system_active = false;
int sound_count = 0;
bool alarm_active = false;

// Configuração dos LEDs de estado
void configure_leds() {
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, 0);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, 0);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_BLUE_PIN, 1);

    gpio_init(BUZZER_PIN); 
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

// Estado dos LEDs
void update_led_status(bool active, bool sound_detected) {
    if (!active) {
        gpio_put(LED_RED_PIN, 0);  
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 1);
        gpio_put(BUZZER_PIN, 0); 
    } else if (sound_detected) {
        gpio_put(LED_RED_PIN, 1);  
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_BLUE_PIN, 0);  
        gpio_put(BUZZER_PIN, 1); 
    } else {
        gpio_put(LED_RED_PIN, 0);  
        gpio_put(LED_GREEN_PIN, 1);
        gpio_put(LED_BLUE_PIN, 0);  
        gpio_put(BUZZER_PIN, 0); 
    }
}

// Função para emitir som no buzzer
void sound_alarm(uint duration_ms) {
    uint32_t end_time = to_ms_since_boot(get_absolute_time()) + duration_ms;

    while (to_ms_since_boot(get_absolute_time()) < end_time) {
        gpio_put(BUZZER_PIN, 1); 
        sleep_ms(500); 
        gpio_put(BUZZER_PIN, 0); 
        sleep_ms(500); 
    }
}


// Função para tocar uma melodia
void play_melody() {
    int melody[] = {440, 494, 523, 587, 659, 698, 784, 880}; 
    int duration = 50; 

    for (int i = 0; i < 8; i++) {
        gpio_put(BUZZER_PIN, 1); 
        sleep_us(1000000 / melody[i]); 
        gpio_put(BUZZER_PIN, 0); 
        sleep_ms(duration); 
    }
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(2);

    configure_leds();

    // Inicialização do display OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // Limpar
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    // Mensagem inicial
    ssd1306_draw_string(ssd, 0, 0, "Detector de Sons");
    ssd1306_draw_string(ssd, 0, 16, "Aguardando ativacao...");
    render_on_display(ssd, &frame_area);

    while (true) {
        // botão A
        if (gpio_get(BUTTON_A_PIN) == 0) {
            system_active = true;
            update_led_status(true, false);
            ssd1306_draw_string(ssd, 0, 16, "Sistema ativado    ");
            render_on_display(ssd, &frame_area);
            sleep_ms(200);
        }

        // botão B
        if (gpio_get(BUTTON_B_PIN) == 0) {
            system_active = false;
            update_led_status(false, false);
            ssd1306_draw_string(ssd, 0, 16, "Sistema desativado ");
            render_on_display(ssd, &frame_area);
            sleep_ms(200);
        }

        if (system_active) {
            // Leitura do microfone
            uint16_t raw_adc = adc_read();
            float voltage = (raw_adc * ADC_REF) / ADC_RES;
            float sound_level = fabs(voltage - SOUND_OFFSET);

            if (sound_level > SOUND_THRESHOLD) {
                sound_count++;
                update_led_status(true, true);
                ssd1306_draw_string(ssd, 0, 32, "Som detectado!");
               
                // Exibe o número de sons detectados
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "Total: %d", sound_count);
                ssd1306_draw_string(ssd, 0, 48, buffer);
                render_on_display(ssd, &frame_area);

                // Emite o alarme
                sound_alarm(5000);
                play_melody();
            } else {
                update_led_status(true, false);
            }
        } else {
            update_led_status(false, false);
        }

        sleep_ms(100);
    }

    return 0;
}