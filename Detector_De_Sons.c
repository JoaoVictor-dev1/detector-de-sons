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
const uint LED_RED_PIN = 13;  
const uint LED_GREEN_PIN = 11; 
const uint LED_BLUE_PIN = 12; 
const uint BUZZER_PIN = 10; 

// Configurações do ADC para detecção de som
const float SOUND_OFFSET = 1.65;
const float SILENCE_THRESHOLD = 0.3;  // Limite para o silêncio
const float ATTENTION_THRESHOLD = 1.0;  // Limite para a atenção
const float ALARM_THRESHOLD = 1.6;  // Limite para o alarme
const float ADC_REF = 3.3;        
const int ADC_RES = 4095;          

// Estado do sistema
bool system_active = false;
int sound_count = 0;  // Contador de sons altos
bool alarm_active = false;

// Função para atualizar as cores dos LEDs
void set_led_color(bool R, bool G, bool B) {
    gpio_put(LED_RED_PIN, R);   // Configura o estado do LED vermelho
    gpio_put(LED_GREEN_PIN, G); // Configura o estado do LED verde
    gpio_put(LED_BLUE_PIN, B);  // Configura o estado do LED azul
}

// Configuração dos LEDs de estado
void configure_leds() {
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    gpio_init(BUZZER_PIN); 
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);

    // Inicializa os LEDs como apagados
    set_led_color(0, 0, 0);
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
            set_led_color(0, 1, 0);  // Luz verde ao ativar
            ssd1306_draw_string(ssd, 0, 16, "Sistema ativado    ");
            render_on_display(ssd, &frame_area);
            sleep_ms(200);
        }

        // botão B
        if (gpio_get(BUTTON_B_PIN) == 0) {
            system_active = false;
            set_led_color(0, 0, 0);  // Desliga todas as luzes

            // Exibe o número de sons altos detectados
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "Sons altos: %d", sound_count);
            ssd1306_draw_string(ssd, 0, 16, "Sistema desativado ");
            ssd1306_draw_string(ssd, 0, 32, buffer); 
            render_on_display(ssd, &frame_area);
            sleep_ms(200);
        }

        if (system_active) {
            // Leitura do microfone
            uint16_t raw_adc = adc_read();
            float voltage = (raw_adc * ADC_REF) / ADC_RES;
            float sound_level = fabs(voltage - SOUND_OFFSET);

            if (sound_level < SILENCE_THRESHOLD) {
                // Silêncio - Luz verde
                set_led_color(0, 1, 0);  // Verde
                ssd1306_draw_string(ssd, 0, 32, "                ");  // Limpa a mensagem de atenção
                render_on_display(ssd, &frame_area);
            } else if (sound_level >= SILENCE_THRESHOLD && sound_level < ALARM_THRESHOLD) {
                // Atenção - Luz amarela
                set_led_color(1, 1, 0);  // Amarelo
                ssd1306_draw_string(ssd, 0, 32, "Atencao          ");
                render_on_display(ssd, &frame_area);
            } else if (sound_level >= ALARM_THRESHOLD) {
                // Alarme - Luz vermelha
                set_led_color(1, 0, 0);  // Vermelho
                ssd1306_draw_string(ssd, 0, 32, "Alarme!          ");
                render_on_display(ssd, &frame_area);

                // Incrementa o contador de sons altos
                sound_count++;

                // Emite o alarme
                sound_alarm(5000);
                play_melody();
            }
        } else {
            set_led_color(0, 0, 0);  // Desliga todas as luzes
        }

        sleep_ms(500);
    }

    return 0;
}