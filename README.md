# BitDogLab_UART_I2C_Explorer

Projeto para explorar a comunicação UART e I2C com o RP2040 na placa BitDogLab.
Inclui controle de LEDs RGB, matriz WS2812, botões com interrupções e debounce, além da exibição de caracteres no display SSD1306.

## Funcionalidades

- Comunicação UART com entrada de caracteres via Serial Monitor.
- Exibição de caracteres no display SSD1306 via I2C.
- Controle de LEDs com botões físicos e interrupções.
- Suporte a LEDs WS2812 para exibição de números.

## Estrutura do Projeto

```plaintext
BitDogLab_UART_I2C_Explorer #Nome do programa principal
├── inc\                    #Codigo base Professor Wilton Lacerda
    ├── font.h
    ├── ssd1306.c
    ├── ssd1306.h
 ├── main.c        # Código-fonte principal
 ├── README.md     # Introdução e documentação do projeto
    

```

#### Configuração

1. Clone o repositório:
2. Compile e envie para o RP2040 utilizando o VS Code + Pico SDK.

```bash

git clone https://github.com/hsantosdias/BitDogLab_UART_I2C_Explorer.git
```
