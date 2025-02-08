
# BitDogLab_UART_I2C_Explorer

Projeto para explorar a comunicação UART e I2C com o RP2040 na placa BitDogLab.

Inclui controle de LEDs RGB, matriz WS2812, botões com interrupções e debounce, além da exibição de caracteres no display SSD1306.

## Funcionalidades

* Comunicação UART com entrada de caracteres via Serial Monitor.
* Exibição de caracteres no display SSD1306 via I2C.
* Controle de LEDs com botões físicos e interrupções.
* Suporte a LEDs WS2812 para exibição de números.

## Bibliotecas Utilizadas

### **1. Pico SDK**

O **Pico SDK** fornece a base para a programação no RP2040, incluindo suporte para  **GPIO, UART, I2C e PIO** . É essencial para comunicação com periféricos e controle de hardware da placa.

### **2. hardware/i2c.h**

Utilizada para comunicação via **I2C** com o display SSD1306. Permite a troca de dados entre o microcontrolador e dispositivos I2C externos, como sensores e displays.

### **3. hardware/irq.h**

Gerencia  **interrupções de hardware** , usadas para detectar eventos como o pressionamento de botões e realizar ações imediatas sem bloquear o código principal.

### **4. led_matrix.h**

Biblioteca personalizada para controle da  **matriz de LEDs WS2812** . Permite a exibição de números na matriz com controle individual de cada LED RGB.

### **5. ssd1306.h**

Biblioteca para manipular o **display OLED SSD1306** via  **I2C** . Permite exibir caracteres e gráficos básicos no display.

### **6. font.h**

Define fontes de caracteres usadas no display SSD1306.

## Estrutura do Projeto

```plaintext
BitDogLab_UART_I2C_Explorer  # Nome do programa principal
├── inc\                     # Código base do Professor Wilton Lacerda
│   ├── font.h
│   ├── ssd1306.c
│   ├── ssd1306.h
├── main.c                    # Código-fonte principal
├── led_matrix.h               # Cabeçalho da matriz de LED
├── led_matrix.c               # Implementação da matriz de LED
├── pio_config.h               # Configuração do PIO para WS2812
├── ws2812b.pio.h              # Código PIO para LEDs WS2812
├── pico_sdk_import.cmake      # Configuração do SDK
├── CMakeLists.txt             # Configuração do projeto
├── wokwi.toml                 # Configuração para simulação no Wokwi
├── diagram.json               # Fluxograma do projeto
├── README.md                  # Introdução e documentação do projeto
```

## Configuração

1. Clone o repositório:
   ```bash
   git clone https://github.com/hsantosdias/BitDogLab_UART_I2C_Explorer.git
   ```
2. Compile e envie para o RP2040 utilizando o VS Code + Pico SDK.

## Fluxograma

O fluxograma abaixo representa o funcionamento do código principal:

**Fluxograma do projeto**

Este fluxograma mostra a estrutura básica do código principal, incluindo inicializações e o loop principal de execução.
