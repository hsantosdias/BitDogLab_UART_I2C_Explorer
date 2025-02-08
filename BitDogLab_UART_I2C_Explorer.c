#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "led_matrix.h"

// Definições do display SSD1306 128x64 I2C OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definições para UART
#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho
#define LED_PIN_B 12     // LED Azul
#define LED_PIN_G 11     // LED Verde
#define BUTTON_PIN_A 5
#define BUTTON_PIN_B 6

// Variáveis globais
static volatile bool led_green_state = false;
static volatile bool led_blue_state = false;

// Prototipagem
static void gpio_irq_handler(uint gpio, uint32_t events);
void init_uart(void);
void init_gpio(void);
void init_display(ssd1306_t *ssd);
void process_serial_input(ssd1306_t *ssd, char input);

// Função de interrupção para os botões
void gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN_A) {
        led_green_state = !led_green_state;
        gpio_put(LED_PIN_G, led_green_state);
        printf("LED Verde %s\n", led_green_state ? "Ligado" : "Desligado");
    } else if (gpio == BUTTON_PIN_B) {
        led_blue_state = !led_blue_state;
        gpio_put(LED_PIN_B, led_blue_state);
        printf("LED Azul %s\n", led_blue_state ? "Ligado" : "Desligado");
    }
}

// Inicializa UART
void init_uart(void) {
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

// Inicializa GPIOs
void init_gpio(void) {
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);

    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);

    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// Inicializa Display OLED SSD1306
void init_display(ssd1306_t *ssd) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "Caractere:", 8, 0);
    ssd1306_send_data(ssd);
}

// Processa entrada serial e exibe no display
void process_serial_input(ssd1306_t *ssd, char input) {
    char text[2] = {input, '\0'};
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "Caractere:", 8, 0);
    ssd1306_draw_string(ssd, text, 8, 10);
    ssd1306_send_data(ssd);
    printf("Caractere recebido: %c\n", input);

    if (input >= '0' && input <= '9') {
        int numero = input - '0';
        led_matrix_display_number(numero);
    }
}

// Função Principal
int main() {
    init_uart();
    init_gpio();
    
    ssd1306_t ssd;
    init_display(&ssd);
    led_matrix_init();
    
    printf("Sistema iniciado. Digite caracteres para exibi-los no display.\n");
    
    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            process_serial_input(&ssd, c);
        }
        sleep_ms(100);
    }
    
    return 0;
}
