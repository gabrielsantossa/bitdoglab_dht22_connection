#include <pico/stdlib.h>
#include <stdio.h>
#include <dht.h>
#include <hardware/i2c.h>
#include <src/pico-ssd1306/ssd1306.h>
#include <string.h>

// Configurações do DHT11
static const dht_model_t DHT_MODEL = DHT11;
static const uint DHT_DATA_PIN = 16;

// Pinos para o display I2C
#define I2C_SDA_PIN 14
#define I2C_SLC_PIN 15

// Inicializa o I2C para o display (utilizando i2c1 nos pinos 14 (SDA) e 15 (SLC))
void init_i2c_display(void) {
    i2c_init(i2c1, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SLC_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SLC_PIN);
}

int main() {
    stdio_init_all();

    // Inicializa o sensor DHT11
    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DHT_DATA_PIN, true);

    // Inicializa o I2C e o display OLED
    init_i2c_display();
    ssd1306_t disp;
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    
    // Buffers para armazenar as strings que serão exibidas
    char line1[32];
    char line2[32];

    while(true) {
        // Leitura do DHT11
        dht_start_measurement(&dht);
        float humidity, temperature_c;
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);

        // Limpa o display
        ssd1306_clear(&disp);

        // Formata as linhas com os dados dos sensores
        if(result == DHT_RESULT_OK) {
            sprintf(line1, "Temperatura: %.1f C", temperature_c);
            sprintf(line2, "Umidade: %.1f%% %", humidity);
        } else {
            sprintf(line1, "Erro DHT11");
            strcpy(line2, "");
        }

        // Atualiza o display OLED
        ssd1306_draw_string(&disp, 0, 0, 1, line1);
        ssd1306_draw_string(&disp, 0, 10, 1, line2);
        ssd1306_show(&disp);

        // Impressão dos dados no terminal para debug
        printf("--------------------------------\n");
        if(result == DHT_RESULT_OK) {
            printf("Temperatura: %.1f C \nUmidade: %.1f%%\n",
                   temperature_c,
                   humidity);
        } else {
            puts("Erro na leitura do DHT11");
        }
        sleep_ms(2000);
    }
    return 0;
}
