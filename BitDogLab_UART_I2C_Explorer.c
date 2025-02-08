#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/irq.h"

// Definições do display SSD1306 128x64 I2C OLED 
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definições para UART 
#define UART_ID uart0    // Seleciona a UART0
#define BAUD_RATE 115200 // Taxa de transmissão
#define UART_TX_PIN 0    // Pino TX
#define UART_RX_PIN 1    // Pino RX

// Definições para LEDs e botões
#define LED_PIN_R 13     // LED Vermelho
#define LED_PIN_B 12     // LED Azul
#define LED_PIN_G 11     // LED Verde
#define BUTTON_PIN_A 5   // Botão A
#define BUTTON_PIN_B 6   // Botão B

// Variáveis globais
static volatile uint16_t counter = 0;   // Contador de eventos do botão
static volatile uint32_t last_time = 0; // Tempo do último evento para debounce
static volatile bool flag_var = false;  // Flag de atualização do contador

// Prototipação das funções
static void gpio_irq_handler(uint gpio, uint32_t events);
void init_uart(void);
void init_gpio(void);
void init_display(ssd1306_t *ssd);

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica debounce (50ms)
    if (current_time - last_time > 50000) {
        last_time = current_time;
        counter++;
        flag_var = true;  // Define a flag para atualização
    }
}

// Inicialização da UART
void init_uart(void) {
    stdio_init_all();
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

// Inicialização de GPIOs (LEDs e botões)
void init_gpio(void) {
    // Configura LEDs
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    // Configura Botões
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    gpio_init(BUTTON_PIN_B);
    gpio_set_dir(BUTTON_PIN_B, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_B);

    // Configuração da interrupção para o botão A
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

// Inicialização do display SSD1306
void init_display(ssd1306_t *ssd) {
    // Configura I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    ssd1306_init(ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}






/*


//codigo para teste de UART e GPIO com interrupção de botão e envio de mensagem pela UART 
int main()
{
    // Inicializa a biblioteca padrão
    stdio_init_all();
//     Inicializa a UART com o padrão: ( 115200, 8N1)
//    115200: A taxa de transmissão de dados.
//    8: O número de bits de dados por caractere (neste caso, 8 bits).
//    N: Não há bit de paridade (No parity).
//    1: O número de bits de parada (neste caso, 1 bit).
//    Low bit first: A ordem dos bits é do bit menos significativo para o mais significativo.

    uart_init(UART_ID, BAUD_RATE);

    // Configura os pinos GPIO para a UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Configura o pino do LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    // Configura o pino do botão com resistor pull-up
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (1)
    {
        // Verifica se a flag está ativa
        if (flag_var)
        {
            // Cria a mensagem com o valor atual do contador
            char message[50];
            sprintf(message, "Contagem: %d\n", counter);

            // Envia a mensagem pela UART
            uart_puts(UART_ID, message);
            //            printf("Mensagem enviada: %s", message); // Exibe a mensagem no serial monitor

            // Pisca o LED
            gpio_put(LED_PIN_R, 1);
            sleep_ms(50);
            gpio_put(LED_PIN_R, 0);
            sleep_ms(50);
            flag_var = 0; // Limpa a flag de interrupção
        }

        // Pequeno atraso para evitar sobrecarga no loop principal
        sleep_ms(10);
    }

    return 0;
}

*/

/*
//Codigo para teste do display OLED

int main()
{
  stdio_init_all(); // Inicializa a UART para debug
  
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Define a função do pino GPIO para UART TX
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Define a função do pino GPIO para UART RX
  uart_init(UART_ID, BAUD_RATE); // Inicializa a UART

    // Configura o pino do LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);


    // Configura o pino do botão com resistor pull-up
    gpio_init(BUTTON_PIN_A);
    gpio_set_dir(BUTTON_PIN_A, GPIO_IN);
    gpio_pull_up(BUTTON_PIN_A);


  // Configuração da interrupção com callback
  gpio_set_irq_enabled_with_callback(BUTTON_PIN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  // I2C Initialisation. Using it at 400Khz.
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  bool cor = true;
  while (true)
  {
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Cepedi   TIC37", 8, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
    ssd1306_draw_string(&ssd, "PROF WILTON", 15, 48); // Desenha uma string      
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(1000);
  }
}

*/