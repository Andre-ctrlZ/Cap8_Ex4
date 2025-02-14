#include <stdio.h>            
#include "pico/stdlib.h"      
#include "hardware/adc.h"     
#include "hardware/pwm.h"
#include "Include/ssd1306.h"
#include "Include/configurar_btn.h"     

#define VRX_PIN 26  
#define VRY_PIN 27   
#define SW_PIN 22    
#define LED1_PIN 11  
#define LED2_PIN 13  
#define LED3_PIN 12  
// bool leds_apagados = false;  // Controle para saber se os LEDs estão apagados


uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    
    pwm_set_enabled(slice_num, true);  
    return slice_num;  
}

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRX_PIN); 
    adc_gpio_init(VRY_PIN); 
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN); 
    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, false); 
    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, false); 
    gpio_init(LED3_PIN);
    gpio_set_dir(LED3_PIN, GPIO_OUT);
    gpio_put(LED3_PIN, false);
    setup_gpio_buttons();
    gpio_set_irq_interrupt_btn();
    

    uint pwm_wrap = 4096;  

    // Inicializando PWM para os dois LEDs
    uint pwm_slice_led2 = pwm_init_gpio(LED2_PIN, pwm_wrap);  // PWM para LED2 (pino 13)
    uint pwm_slice_led3 = pwm_init_gpio(LED3_PIN, pwm_wrap);  // PWM para LED1 (pino 11)
    
    uint32_t last_print_time = 0;

    void ledx() {
        if (!leds_apagados) {  // Só ajusta o PWM se os LEDs não estiverem apagados
            adc_select_input(0);  
            uint16_t vrx_value = adc_read(); 
            pwm_set_gpio_level(LED2_PIN, vrx_value);  // Ajusta o PWM para o LED2
    
            float duty_cycle = (vrx_value / 4095.0) * 100;  
    
            uint32_t current_time = to_ms_since_boot(get_absolute_time());  
            if (current_time - last_print_time >= 1000) {  
                printf("VRX: %u\n", vrx_value); 
                printf("Duty Cycle LED2: %.2f%%\n", duty_cycle); 
                last_print_time = current_time;  
            }
    
            sleep_ms(100);  
        }
    }

    void ledy() {
        if (!leds_apagados) {  // Só ajusta o PWM se os LEDs não estiverem apagados
            adc_select_input(1);  
            uint16_t vry_value = adc_read();  // Lê o valor do ADC  
            pwm_set_gpio_level(LED3_PIN, vry_value);  // Ajusta o PWM para o LED1
    
            float duty_cycle = (vry_value / 4095.0) * 100;  
    
            uint32_t current_time = to_ms_since_boot(get_absolute_time());  
            if (current_time - last_print_time >= 1000) {  
                printf("VRY: %u\n", vry_value); 
                printf("Duty Cycle LED1: %.2f%%\n", duty_cycle); 
                last_print_time = current_time;  
            }
    
            sleep_ms(100);  // Atraso para evitar leituras rápidas demais
        }
    }

    while (true) {
         
        ledx();  // Função para controle do LED2 (VRX)
        ledy();  // Função para controle do LED1 (VRY)
        sleep_ms(100);  // Atraso para evitar leituras rápidas demais
       
    }

    return 0;  
}
