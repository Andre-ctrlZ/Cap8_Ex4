#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#include "hardware/i2c.h"
#include "ssd1306.h"
// #include "font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

int stage_retangulo = 0;  //controla a exibicao do retangulo 0: Retângulo grande, 1: Retângulo menor, 2: Retângulo ainda menor
int x_pos, y_pos;  //variáveis para os valores do joystick para as coordenadas do display (128x64 pixels)

bool cor = true;    // Define a cor do display (preto/branco)

uint16_t vrx_value;  
uint16_t vry_value;

ssd1306_t ssd;    // Inicializa a estrutura do display

void display_init(){
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); //Inicializa o display
    ssd1306_config(&ssd); //Configura o display
    ssd1306_send_data(&ssd); //Envia os dados para o display

    //O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void print_display(uint16_t vrx_value, uint16_t vry_value){
    ssd1306_fill(&ssd, !cor); // Limpa o display
    
    //valores iniciais da área mapeada para que o quadrado 8x8 permaneça dentro do espaço 128x64
    int x_min = 0, x_max = 120;
    int y_min = 0, y_max = 56;

    //Define limites do retângulo conforme cada estágio
    if (stage_retangulo == 0) { 
        ssd1306_rect(&ssd, 0, 0, 128, 64, !cor, !cor); //Sem retângulo
    } else if (stage_retangulo == 1) {
        ssd1306_rect(&ssd, 0, 0, 128, 64, cor, !cor); //Retângulo grande
    } else if (stage_retangulo == 2) {
        ssd1306_rect(&ssd, 10, 10, 108, 54, cor, !cor); // Retângulo médio
        x_min = 10, x_max = 110;
        y_min = 10, y_max = 56;
    }else if (stage_retangulo == 3) {
        ssd1306_rect(&ssd, 20, 20, 88, 44, cor, !cor); // Retângulo pequeno
        x_min = 20, x_max = 100;
        y_min = 20, y_max = 56;
    }

    // Mapeia os valores do joystick para as coordenadas do display com a limitação do retângulo
    x_pos = (vrx_value * (x_max - x_min)) / 4095 + x_min;   // Ajusta X para o display
    y_pos = y_max - (vry_value * (y_max - y_min)) / 4095;  // Ajusta Y para o display invertendo para corresponder ao display

    // Garantir que o quadrado 8x8 não ultrapasse os limites
    if (x_pos < x_min) x_pos = x_min;
    if (x_pos > x_max - 8) x_pos = x_max - 8;
    if (y_pos < y_min) y_pos = y_min;
    if (y_pos > y_max - 8) y_pos = y_max - 8;

    // Desenha o quadrado 8x8 na posição calculada
    ssd1306_rect(&ssd, x_pos, y_pos, 8, 8, cor, cor);

    ssd1306_send_data(&ssd); // Atualiza o display
}

#endif