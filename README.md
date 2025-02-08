# BitDogLab_UART_I2C_Explorer

Projeto para explorar a comunicação **UART** e **I2C** no RP2040 usando a placa  **BitDogLab** .

O projeto implementa:

* Controle de LEDs RGB e matriz de LEDs WS2812.
* Exibição de caracteres no display **SSD1306** via  **I2C** .
* Uso de botões físicos com **interrupções** e **debounce** para interação.
* Entrada de dados via **UART** com interpretação de caracteres.
* **Desligamento automático da matriz** de LEDs após entrada de letras.

## Funcionalidades Implementadas

* **Comunicação UART:** Entrada de caracteres via  **Serial Monitor** .
* **Exibição no Display OLED:** Caracteres digitados são exibidos no  **SSD1306** .
* **Controle de LEDs RGB:** LEDs Verde e Azul são alternados com os botões físicos.
* **Matriz de LEDs WS2812:** Exibe números de 0 a 9 quando digitados.
* **Desligamento automático da matriz:** A matriz  **desliga ao receber uma letra** .
* **Tratamento de botões:** Uso de **interrupções (IRQ) e debounce** para evitar falsos acionamentos.
* **Comandos via UART:**
  * **Números (0-9):** São exibidos na  **matriz de LEDs WS2812** .
  * **Letras (A-Z, a-z):** São exibidas no **OLED** e desligam a matriz.

## Bibliotecas Utilizadas

### Pico SDK

O **Pico SDK** fornece suporte para a programação no RP2040, incluindo  **GPIO, UART, I2C e PIO** , permitindo a comunicação com periféricos.

### hardware/i2c.h

Utilizada para comunicação via **I2C** com o display  **SSD1306** , permitindo a troca de dados entre o microcontrolador e dispositivos I2C.

### hardware/irq.h

Gerencia  **interrupções de hardware** , permitindo detectar eventos como o acionamento de botões sem bloquear a execução principal do código.

### hardware/uart.h

Implementa a  **comunicação UART** , permitindo a entrada e saída de dados entre o RP2040 e um terminal serial.

### led_matrix.h

Biblioteca personalizada para controle da  **matriz de LEDs WS2812** , permitindo a exibição de números na matriz.

### ssd1306.h

Biblioteca para o  **display OLED SSD1306** , permitindo exibir caracteres e gráficos básicos via  **I2C** .

### font.h

Define fontes de caracteres usadas no  **display SSD1306** , incluindo suporte para  **letras minúsculas e maiúsculas** .

## Estrutura do Projeto

```plaintext
BitDogLab_UART_I2C_Explorer  # Nome do programa principal
├── inc/                     # Diretório de cabeçalhos
│   ├── font.h               # Fontes de caracteres para o display
│   ├── ssd1306.h            # Biblioteca do display OLED
├── led_matrix.h             # Cabeçalho da matriz de LED
├── led_matrix.c             # Implementação da matriz de LED
├── pio_config.h             # Configuração do PIO para WS2812
├── ws2812b.pio.h            # Código PIO para LEDs WS2812
├── BitDogLab_UART_I2C_Explorer.c  # Código-fonte principal
├── pico_sdk_import.cmake    # Configuração do SDK
├── CMakeLists.txt           # Configuração do projeto
├── wokwi.toml               # Configuração para simulação no Wokwi
├── diagram.json             # Fluxograma do projeto
├── README.md                # Introdução e documentação do projeto
```

## Dificuldades Encontradas

Durante o desenvolvimento, alguns desafios surgiram e foram superados:

1. **Comunicação UART** : Inicialmente, os caracteres recebidos via UART não eram processados corretamente. A solução foi utilizar `stdio_usb_connected()` e `scanf()` para garantir a leitura correta.
2. **Matriz 5x5 WS2812** : Ajustar o mapeamento correto dos LEDs foi desafiador, exigindo testes e cálculos para garantir a correta exibição de números.
3. **Interrupções nos botões** : O debounce foi refinado para evitar acionamentos múltiplos ao pressionar os botões físicos.
4. **Display SSD1306** : Algumas mensagens não apareciam corretamente, e foi necessário ajustar o tamanho do buffer e as funções de escrita no display.

## Comandos Especiais via UART

O sistema reconhece os seguintes comandos enviados via  **Serial Monitor** :

* **Números (0-9):** Exibidos na  **matriz de LEDs WS2812** .
* **Letras (A-Z ou a-z):** Exibidas no **display OLED** e desligam a matriz de LEDs.

## Configuração

1. Clone o repositório:
   ```bash
   git clone https://github.com/hsantosdias/BitDogLab_UART_I2C_Explorer.git
   ```
2. Compile e envie para o RP2040 utilizando o VS Code + Pico SDK.

## Fluxograma

O fluxograma abaixo representa o funcionamento geral do código principal.

**Fluxograma do projeto**

[![Fluxograma Simplificado](imgs/fluxograma-simplificado.png "Fluxograma Simplificado")](https://github.com/hsantosdias/BitDogLab_UART_I2C_Explorer/blob/main/imgs/fluxograma-simplificado.png?raw=true "Fluxograma Simplificado")

Este fluxograma ilustra as principais etapas do código, incluindo inicializações e o loop principal de execução.

**Fluxograma completo do projeto**

[![Flugrama de interações entre as funções](imgs/fluxograma-completo.png "Flugrama de interações entre as funções")](https://github.com/hsantosdias/BitDogLab_UART_I2C_Explorer/blob/main/imgs/fluxograma-completo.png?raw=true "Flugrama de interações entre as funções")


### Descrição Simplificada do Fluxograma

1. **Inicialização do Sistema**
   * O código inicializa  **UART** ,  **GPIOs** , **Display OLED** e  **Matriz de LED WS2812** .
   * Após a inicialização, entra no  **loop principal** .
2. **Entrada via UART**
   * Se houver entrada de um caractere pelo  **Serial Monitor** , o sistema o processa.
   * Se for uma  **letra** , ela é exibida no **display OLED** e  **desliga a matriz de LED** .
   * Se for um  **número (0-9)** , ele é exibido na  **matriz de LED** .
3. **Interação com Botões**
   * Se um botão for pressionado:
     * **Botão A** : Alterna o estado do **LED Verde** e exibe a mudança no  **OLED** .
     * **Botão B** : Alterna o estado do **LED Azul** e exibe a mudança no  **OLED** .
   * Se nenhum botão for pressionado, o sistema continua aguardando.
4. **Loop Contínuo**
   * Após processar uma entrada via **UART** ou um  **botão** , o sistema aguarda **50ms** antes de continuar o loop.

## Considerações Finais

O projeto integra **diferentes interfaces de comunicação** no  **RP2040** , permitindo explorar **UART, I2C e GPIO** de forma prática. Além disso, a implementação de **interrupções e debounce** aprimora a confiabilidade do sistema.

Este projeto é uma boa referência para quem deseja aprender a combinar **hardware e software** em microcontroladores.
