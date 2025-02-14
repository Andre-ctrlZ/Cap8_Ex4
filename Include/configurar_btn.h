#ifndef CONFIG_BTN_H
#define CONFIG_BTN_H

#include "hardware/timer.h"

//definição de GPIO botões
#define button_0 5
#define button_1 6
#define LED1_PIN 11  
#define LED2_PIN 13  
#define LED3_PIN 12
#define SW_PIN 22  
bool leds_apagados = false;
bool sw_apagado = false;

// Variáveis globais 
static volatile uint32_t last_time = 0;// Armazena o tempo do último evento (em microssegundos)
static volatile uint32_t last_time_B = 0;
volatile bool BTN_A_PRESSIONADO = false;
volatile bool BTN_B_PRESSIONADO = false;
int a = 0;

// Configuração inicial ds botões
void setup_gpio_buttons() {
  //Botão A
  gpio_init(button_0);
  gpio_set_dir(button_0, GPIO_IN);
  gpio_pull_up(button_0);

  //Botão B
  gpio_init(button_1);
  gpio_set_dir(button_1, GPIO_IN);
  gpio_pull_up(button_1);
}

// Função de interrupção com debouncing do botão
void gpio_irq_handler(uint gpio, uint32_t events) {
  uint32_t current_time = to_us_since_boot(get_absolute_time());
  static uint32_t last_time = 0;

  // Debouncing: verifica se passou tempo suficiente desde o último evento
  if (current_time - last_time > 100000) {  // 100 ms de debounce
      last_time = current_time;

      if (gpio == SW_PIN) {
          // Lógica para o botão SW (controla LED1)
          sw_apagado = !sw_apagado;

          if (sw_apagado) {
              gpio_put(LED1_PIN, true);  // Acende o LED1
              printf("Botão SW pressionado! LED1 aceso.\n");
          } else {
              gpio_put(LED1_PIN, false);  // Apaga o LED1
              printf("Botão SW pressionado! LED1 apagado.\n");
          }
          
      } else if (gpio == button_0) {
          // Lógica para o botão 0 (controla LEDs PWM)
          leds_apagados = !leds_apagados;

          if (leds_apagados) {
              // Apaga os LEDs PWM
              pwm_set_gpio_level(LED2_PIN, 0);
              pwm_set_gpio_level(LED3_PIN, 0);
              printf("Botão 0 pressionado! LEDs apagados.\n");
          } else {
              // Acende os LEDs PWM
              pwm_set_gpio_level(LED2_PIN, 4095);
              pwm_set_gpio_level(LED3_PIN, 4095);
              printf("Botão 0 pressionado! LEDs acesos.\n");
          }
      }
  }
}


void gpio_set_irq_interrupt_btn(){
    // Configuração da interrupção com callback para botão A
    gpio_set_irq_enabled_with_callback(button_0, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Configuração da interrupção com callback para botão B
    gpio_set_irq_enabled_with_callback(button_1, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

#endif