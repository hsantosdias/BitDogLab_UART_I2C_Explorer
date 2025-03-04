#include <stdio.h> // Inclui biblioteca padrão de E/S em C (printf, scanf, etc) 
#include <stdlib.h> // Inclui biblioteca padrão de funções em C (malloc, free, etc)
#include "pico/stdlib.h" // Inclui biblioteca de funções do Pico (gpio_init, sleep_ms, etc)
#include "hardware/i2c.h" // Inclui biblioteca de funções de I2C (i2c_init, i2c_write_blocking, etc)
#include "hardware/irq.h" // Inclui biblioteca de funções de interrupção (IRQ)
#include "hardware/uart.h" // Inclui biblioteca de funções de UART (uart_init, uart_getc, etc)

//bibliotecas adicionais fornecidas inicialmente pelo professor Wilson
#include "inc/ssd1306.h" // Inclui biblioteca de funções do display OLED SSD1306
#include "inc/font.h" // Inclui biblioteca de fontes para o display OLED SSD1306 numeros e letras maiusculas e minusculas

//bibliotecas adicional - para manipulação do display de matriz de leds
#include "led_matrix.h" // Inclui biblioteca de funções da matriz de LEDs 5x5

// Definições do display SSD1306 128x64 I2C OLED
// Configuração i2c para o display OLED
#define I2C_PORT i2c1 // Define a porta I2C utilizada - porta 1 da bitdoglab
#define I2C_SDA 14 // Define o pino SDA - GPIO 14
#define I2C_SCL 15 // Define o pino SCL - GPIO 15
#define ENDERECO 0x3C // Endereço do display OLED SSD1306

// Definições para UART (Comunicação Serial)
#define UART_ID uart0 // Define a porta UART utilizada - porta 0 da bitdoglab
#define BAUD_RATE 115200 // Define a taxa de transmissão de dados (baud rate) - 115200 bps
#define UART_TX_PIN 0 // Define o pino TX da UART - GPIO 0
#define UART_RX_PIN 1 // Define o pino RX da UART - GPIO 1

// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho 
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde
#define BUTTON_PIN_A 5 // Botão A
#define BUTTON_PIN_B 6 // Botão B

// Variáveis globais
static volatile bool estado_led_verde = false; // Estado do LED Verde (inicialmente desligado) 
static volatile bool estado_led_azul = false; // Estado do LED Azul (inicialmente desligado)
static ssd1306_t ssd;  // Definição global do display OLED SSD1306 128x64 I2C

// Prototipação de funções (assinaturas) - declaração de funções
static void gpio_irq_handler(uint gpio, uint32_t events); // Função de interrupção com debounce e detecção de botão
void init_uart(void); // Inicializa UART (Comunicação Serial) 
void init_gpio(void); // Inicializa GPIOs (LEDs e Botões) 
void init_display(void); // Inicializa Display OLED SSD1306 128x64 I2C 
void atualizar_display(const char *linha1, const char *linha2); // Atualiza o display com duas mensagens (duas linhas)
void processar_uart(void); // Processa entrada via UART (Comunicação Serial)
void desligar_matrix(void); // Desliga a matriz 5x5 e exibe mensagem

// Função de interrupção com debounce e detecção de botão
void gpio_irq_handler(uint gpio, uint32_t events) {
    static uint32_t last_time = 0; // Último tempo de pressionamento do botão (inicialmente 0)
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Tempo atual em microssegundos desde o boot

    if (current_time - last_time > 200000) { // Debounce de 200ms (200000us)
        last_time = current_time; // Atualiza o tempo de pressionamento do botão

        // Verifica qual botão foi pressionado e atualiza o estado do LED correspondente e exibe no display OLED SSD1306
        if (gpio == BUTTON_PIN_A) {
            estado_led_verde = !estado_led_verde; // Inverte o estado do LED Verde  (liga/desliga)
            gpio_put(LED_PIN_G, estado_led_verde); // Atualiza o estado do LED Verde 
            printf("Botão A pressionado: LED Verde %s\n", estado_led_verde ? "Ligado" : "Desligado"); // Exibe mensagem no terminal UART 
            atualizar_display(estado_led_verde ? "LED Verde ON" : "LED Verde off", ""); // Atualiza o display OLED SSD1306 (LED Verde ON/OFF)
        } 
        else if (gpio == BUTTON_PIN_B) {
            estado_led_azul = !estado_led_azul; // Inverte o estado do LED Azul (liga/desliga)
            gpio_put(LED_PIN_B, estado_led_azul); // Atualiza o estado do LED Azul
            printf("Botão B pressionado: LED Azul %s\n", estado_led_azul ? "Ligado" : "Desligado"); // Exibe mensagem no terminal UART
            atualizar_display(estado_led_azul ? "LED Azul ON" : "LED Azul off", ""); // Atualiza o display OLED SSD1306 (LED Azul ON/OFF)
        }
    }
}

// Inicializa UART (Comunicação Serial) 
void init_uart(void) {
    stdio_init_all(); // Inicializa a comunicação serial padrão (stdio) com a UART
    uart_init(UART_ID, BAUD_RATE); // Inicial a UART com a taxa de transmissão de dados (baud rate) definida
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Define o pino TX da UART como pino de função UART 
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); 
    uart_set_hw_flow(UART_ID, false, false);  // Desativa controle de fluxo de hardware (RTS/CTS)
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE); // Configura formato de dados da UART (8 bits de dados, 1 bit de parada, sem paridade)
    uart_set_fifo_enabled(UART_ID, false); // Desativa FIFO da UART (sem buffer)
    printf("UART Inicializada com sucesso\n"); // Exibe mensagem no terminal UART (Comunicação Serial)
} 

// Inicializa GPIOs (LEDs e Botões) 
// Inicializa os pinos dos LEDs e botões como saída ou entrada
// Configura os LEDs como saída e os botões como entrada com pull-up
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

    // Configura interrupção para os botões A e B com detecção de borda de descida (queda)
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// Inicializa Display OLED SSD1306 128x64 I2C 
void init_display(void) {
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa a porta I2C com velocidade de 400kHz (400 * 1000 bps)
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define o pino SDA como pino de função I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define o pino SCL como pino de função I2C
    gpio_pull_up(I2C_SDA); // Habilita pull-up no pino SDA
    gpio_pull_up(I2C_SCL); // Habilita pull-up no pino SCL

    // Inicializa o display OLED SSD1306 128x64 I2C
    // Configura o display com resolução de 128x64, sem rotação, endereço 0x3C e porta I2C
    // Preenche o display com cor preta (false)
    // Envia os dados para o display
    // Exibe mensagem no terminal UART
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Atualiza o display com duas mensagens (duas linhas) 
void atualizar_display(const char *linha1, const char *linha2) {
    printf("Atualizando display: %s | %s\n", linha1, linha2);
    ssd1306_fill(&ssd, false); // Limpa o display com cor preta
    ssd1306_draw_string(&ssd, linha1, 10, 10); // Primeira linha no topo do display
    ssd1306_draw_string(&ssd, linha2, 10, 30); // Segunda linha abaixo da primeira
    ssd1306_send_data(&ssd);
}

// Desliga a matriz 5x5 e exibe mensagem 
void desligar_matrix(void) {
    led_matrix_clear();  // Limpa todos os LEDs da matriz 5x5 
    led_matrix_write();  // Atualiza a matriz para garantir que os LEDs sejam apagados
    atualizar_display("Matrix 5x5 off", ""); // Mostra apenas a mensagem de desligamento da matriz
    printf("Matrix 5x5 desligada\n"); // Exibe mensagem no terminal UART
}

// Processa entrada via UART (Comunicação Serial) 
void processar_uart(void) {
    if (stdio_usb_connected()) {  // Certifica-se de que o USB está conectado para evitar erros de leitura 
        char recebido;
        if (scanf("%c", &recebido) == 1) {  // Lê caractere da entrada padrão (UART) 
            printf("Recebido via UART: %c\n", recebido); // Exibe caractere recebido no terminal UART

            // Se for uma letra, exibe no display e desliga a matriz de LEDs 
            // Exibe a letra no display OLED SSD1306 e desliga a matriz de LEDs
            // Exibe a letra no terminal UART
            if ((recebido >= 'A' && recebido <= 'Z') || (recebido >= 'a' && recebido <= 'z')) {
                char mensagem[20];
                snprintf(mensagem, sizeof(mensagem), "Letra: %c", recebido);
                desligar_matrix();  // Desliga a matriz
                atualizar_display("Matrix 5x5 off", mensagem); // Primeira linha "Matrix 5x5 off", segunda linha mostra a letra
                printf("Letra recebida e enviada ao OLED: %c\n", recebido);
            } 
            // Se for um número, exibe na matriz de LEDs e no display OLED SSD1306 
            else if (recebido >= '0' && recebido <= '9') {
                int numero = recebido - '0'; // Converte caractere numérico para inteiro (0-9)
                printf("Número recebido via UART: %d\n", numero);
                led_matrix_display_number(numero);  // Exibe o número na matriz de LEDs 5x5 
                char mensagem[20]; // Exibe o número no display OLED SSD1306 
                snprintf(mensagem, sizeof(mensagem), "Número: %d", numero);
                atualizar_display("", mensagem); // Apenas exibe o número, sem "Matrix 5x5 off"
            }
            // Se for qualquer outro caractere especial, também desliga a matriz
            // Exibe mensagem no display OLED SSD1306 e no terminal UART 
            else {
                desligar_matrix();
                atualizar_display("Matrix 5x5 off", ""); // Apenas exibe que a matriz foi desligada
            }
        }
    }
}

// Função Principal (main) 
int main() {
    init_uart(); // Inicializa UART (Comunicação Serial) 
    init_gpio(); // Inicializa GPIOs (LEDs e Botões)
    init_display(); // Inicializa Display OLED SSD1306 128x64 I2C 
    led_matrix_init(); // Inicializa a matriz de LEDs 5x5

    // Exibe mensagem no terminal UART
    printf("Sistema iniciado. Digite letras ou números no terminal UART.\n");

    while (true) {
        processar_uart(); // Processa entrada da UART (Comunicação Serial) 
        sleep_ms(50); // Aguarda 50ms para evitar sobrecarga do processador
    }

    return 0;
}
