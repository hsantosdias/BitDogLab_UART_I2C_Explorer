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
#define endereco 0x3C
//Definições para uart 
#define UART_ID uart0    // Seleciona a UART0
#define BAUD_RATE 115200 // Define a taxa de transmissão
#define UART_TX_PIN 0    // Pino GPIO usado para TX
#define UART_RX_PIN 1    // Pino GPIO usado para RX
#define LED_PIN_R 13       // Pino GPIO usado para o LED Vermelho
#define LED_PIN_B 12       // Pino GPIO usado para o LED Azul
#define LED_PIN_G 11       // Pino GPIO usado para o LED Verde
#define BUTTON_PIN_A 5    // Pino GPIO usado para o botão A
#define BUTTON_PIN_B 6    // Pino GPIO usado para o botão B

// Variáveis globais
static volatile uint16_t counter = 0;   // Variável inteira para contagem
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile bool flag_var = 0;      // Variável de flag para debouncing

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 50 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        counter++;                // Incrementa o contador
        flag_var = 1;
    }
}



int main()
{
    // Inicializa a biblioteca padrão
    stdio_init_all();
/*     Inicializa a UART com o padrão: ( 115200, 8N1)
*    115200: A taxa de transmissão de dados.
*    8: O número de bits de dados por caractere (neste caso, 8 bits).
*    N: Não há bit de paridade (No parity).
*    1: O número de bits de parada (neste caso, 1 bit).
*    Low bit first: A ordem dos bits é do bit menos significativo para o mais significativo.
*/
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

/*
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
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
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