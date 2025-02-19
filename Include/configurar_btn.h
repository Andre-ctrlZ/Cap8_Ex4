#ifndef CONFIG_BTN_H
#define CONFIG_BTN_H

#include "hardware/timer.h"

//definição de GPIO botões
#define BTN_A 5
#define button_1 6
#define LED1_PIN 11  
#define LED2_PIN 13  
#define LED3_PIN 12
#define SW_PIN 22  
bool leds_apagados = false;
bool sw_apagado = false;

// Variáveis globais para controle dos LEDs
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
bool estado_pwm_leds = true; // Estado dos LEDs PWM

// Configuração inicial ds botões
void setup_gpio_buttons() {
  //Botão A
   // Configuração do botão 0 (BTN_A)
   gpio_init(BTN_A);
   gpio_set_dir(BTN_A, GPIO_IN);
   gpio_pull_up(BTN_A);
}

// Função de interrupção com debouncing do botão
void gpio_irq_handler(uint gpio, uint32_t events) {
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) {  // 200 ms de debouncing
        last_time = current_time;  // Atualiza o tempo do último evento

        if (gpio == SW_PIN) {
            // Lógica para o botão SW (controla LED1)
            static bool sw_apagado = false;
            sw_apagado = !sw_apagado;
            gpio_put(LED1_PIN, sw_apagado);  // Acende ou apaga o LED1
            printf("Botão SW pressionado! LED1 %s.\n", sw_apagado ? "aceso" : "apagado");
        } else if (gpio == BTN_A) {
            // Lógica para o botão 0 (controla LEDs PWM)
            if (!leds_apagados) {
                // Apaga os LEDs PWM (LED2 e LED3)
                pwm_set_gpio_level(LED2_PIN, 0);  // LED2 apagado
                pwm_set_gpio_level(LED3_PIN, 0);  // LED3 apagado
                leds_apagados = true;  // Marca que os LEDs foram apagados
                printf("Botão 0 pressionado! LEDs PWM apagados.\n");
            } else {
                // Acende os LEDs PWM (LED2 e LED3) com a intensidade máxima
                pwm_set_gpio_level(LED2_PIN, 4095);  // LED2 aceso
                pwm_set_gpio_level(LED3_PIN, 4095);  // LED3 aceso
                leds_apagados = false;  // Marca que os LEDs foram acesos
                printf("Botão 0 pressionado! LEDs PWM acesos.\n");
            }
        }
    }
}


void gpio_set_irq_interrupt_btn(){
    // Configuração da interrupção com callback para botão A
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Configuração da interrupção com callback para botão B
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

#endif