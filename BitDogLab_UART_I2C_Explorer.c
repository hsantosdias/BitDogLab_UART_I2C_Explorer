#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/uart.h"
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
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde
#define BUTTON_PIN_A 5
#define BUTTON_PIN_B 6

// Variáveis globais
static volatile bool estado_led_verde = false;
static volatile bool estado_led_azul = false;
static ssd1306_t ssd;  // Definição global do display

// Prototipação
static void gpio_irq_handler(uint gpio, uint32_t events);
void init_uart(void);
void init_gpio(void);
void init_display(void);
void atualizar_display(const char *mensagem);
void processar_uart(void);

// Função de interrupção com debounce e detecção de botão
void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 200000) { // Debounce de 200ms
        last_time = current_time;

        if (gpio == BUTTON_PIN_A) {
            estado_led_verde = !estado_led_verde;
            gpio_put(LED_PIN_G, estado_led_verde);
            printf("Botão A pressionado: LED Verde %s\n", estado_led_verde ? "Ligado" : "Desligado");
            atualizar_display(estado_led_verde ? "LED Verde Ligado" : "LED Verde Desligado");
        } 
        else if (gpio == BUTTON_PIN_B) {
            estado_led_azul = !estado_led_azul;
            gpio_put(LED_PIN_B, estado_led_azul);
            printf("Botão B pressionado: LED Azul %s\n", estado_led_azul ? "Ligado" : "Desligado");
            atualizar_display(estado_led_azul ? "LED Azul Ligado" : "LED Azul Desligado");
        }
    }
}

// Inicializa UART
void init_uart(void) {
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(UART_ID, false, false);  // Desativa controle de fluxo
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_fifo_enabled(UART_ID, false);
    printf("UART Inicializada com sucesso\n");
}

// Inicializa GPIOs
void init_gpio(void) {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

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
void init_display(void) {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Atualiza o display com uma mensagem
void atualizar_display(const char *mensagem) {
    printf("Atualizando display: %s\n", mensagem);  // Debug print
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_draw_string(&ssd, mensagem, 10, 25); // Escreve a mensagem no display
    ssd1306_send_data(&ssd);
}

if (uart_is_readable(UART_ID)) {
// Processa entrada via UART
void processar_uart(void) {
    if (uart_is_readable(UART_ID)) {
        char recebido = uart_getc(UART_ID);
        printf("Recebido via UART: %c\n", recebido);

        // Se for uma letra, exibe no display
        if ((recebido >= 'A' && recebido <= 'Z') || (recebido >= 'a' && recebido <= 'z')) {
            char mensagem[20];
            snprintf(mensagem, sizeof(mensagem), "Letra: %c", recebido);
            atualizar_display(mensagem);
            printf("Letra recebida e enviada ao OLED: %c\n", recebido);
        } 
        // Se for um número, exibe na matriz de LEDs
        else if (recebido >= '0' && recebido <= '9') {
            int numero = recebido - '0';
            printf("Número recebido via UART: %d\n", numero);
            led_matrix_display_number(numero);
            char mensagem[20];
            snprintf(mensagem, sizeof(mensagem), "Número: %d", numero);
            atualizar_display(mensagem);
        }
    }
}

// Função Principal
int main() {
    init_uart();
    init_gpio();
    init_display();
    led_matrix_init();

    printf("Sistema iniciado. Digite letras ou números no terminal UART.\n");

    while (true) {
        processar_uart(); // Processa entrada da UART
        sleep_ms(50);
    }

    return 0;
}
