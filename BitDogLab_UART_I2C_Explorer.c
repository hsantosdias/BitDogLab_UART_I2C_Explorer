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
static volatile uint8_t numero_atual = 0;  // Número exibido na matriz de LED
static volatile uint32_t last_time = 0;    // Tempo do último evento para debounce

// Prototipação
static void gpio_irq_handler(uint gpio, uint32_t events);
void init_uart(void);
void init_gpio(void);
void init_display(ssd1306_t *ssd);

// Função de interrupção com debounce e detecção de botão
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 200000) { // Debounce de 200ms
        last_time = current_time;

        if (gpio == BUTTON_PIN_A) {
            numero_atual = (numero_atual + 1) % 10; // Incrementa
        } else if (gpio == BUTTON_PIN_B) {
            numero_atual = (numero_atual + 9) % 10; // Decrementa (circular)
        }

        printf("Número exibido: %d\n", numero_atual);
        led_matrix_display_number(numero_atual);
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
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

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
    ssd1306_send_data(ssd);
}

// Função Principal
int main() {
    init_uart();
    init_gpio();

    ssd1306_t ssd;
    init_display(&ssd);
    led_matrix_init();

    printf("Sistema iniciado. Pressione os botões para alterar os números.\n");

    // Exibir número inicial
    led_matrix_display_number(numero_atual);

    bool cor = true;
    while (true) {
        cor = !cor;
        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "Cepedi   TIC37", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "PROF WILTON", 15, 48); // Desenha uma string      
        ssd1306_send_data(&ssd); // Atualiza o display
    
        sleep_ms(1000);

        sleep_ms(50);
    }

    return 0;
}
