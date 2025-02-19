# BitDogMat

BitDogMat é um projeto que visa auxiliar no aprendizado de matemática por meio de um jogo interativo que utiliza o BitDogLab. O jogo permite a interação do usuário através de joystick e botões, e registra a pontuação.

## Introdução

Este projeto propõe a criação de um jogo interativo para aprendizado de matemática, utilizando o microcontrolador Raspberry Pi Pico, uma matriz de LEDs, um display OLED e um joystick. O jogo gera problemas matemáticos aleatórios que o usuário deve resolver, recebendo feedback visual e registrando a pontuação.

## Objetivos

- Tornar o aprendizado de matemática mais envolvente e interativo.
- Utilizar tecnologia para facilitar a compreensão e resolução de problemas matemáticos.
- Promover a prática de operações matemáticas de forma lúdica e divertida.

## Hardware Utilizado

- Raspberry Pi Pico
- Joystick
- Botões (2x)
- Matriz de LEDs (25 LEDs)
- Display OLED (SSD1306)

## Pinos e Circuito

### Joystick:

- X: GPIO 27
- Y: GPIO 26

### Botões:

- A: GPIO 5
- B: GPIO 6

### Matriz de LEDs:

- LED_PIN: GPIO 7

### Display OLED:

- SDA: GPIO 14
- SCL: GPIO 15

## Funcionamento do Jogo

### Interação com o Joystick e Botões:

- O usuário navega e interage com o jogo utilizando o joystick e os botões.
- O botão A é utilizado para confirmar a resposta do problema matemático.
- O botão B é utilizado para gerar um novo problema matemático ou reiniciar o jogo em caso de resposta incorreta.

### Geração de Problemas Matemáticos:

- O sistema gera problemas matemáticos aleatórios que o usuário possa resolver.

### Display OLED:

- O display exibe os problemas matemáticos e a pontuação do usuário. 

### Controle de Pontuação:

- O sistema registra e atualiza a pontuação com base nas respostas corretas ou incorretas dos problemas matemáticos.

## Configuração do Software

### Blocos Funcionais e Funções

#### Main Loop (main):

- Gerencia a interação do usuário com o joystick e botões.
- Atualiza os LEDs e o display OLED.

#### Inicialização (setup):

- Configura o sistema, incluindo ADC, joystick, PIO, LEDs, display OLED e botões.

#### Controle da Matriz de LEDs:

- Inicializa a máquina PIO e controla os LEDs da matriz.
- Funções: `npInit`, `npSetLED`, `npClear`, `npWrite`.

#### Display OLED (SSD1306):

- Controla o display OLED.
- Funções: `startDisplay`, `updateDisplay`.

#### Interação do Usuário:

- Lê as entradas do joystick e botões para controlar o jogo.
- Funções: `turnOnLED`, `turnOffLED`, `generateMathProblem`, `readCount`, `nextMath`, `WrongAnswer`.

## Inicialização do Sistema

### ADC:

- `stdio_init_all()`: Inicializa a entrada e saída padrão.
- `adc_init()`: Inicializa o ADC.
- `adc_gpio_init(JOYSTICK_X)`: Inicializa o pino do eixo X do joystick.
- `adc_gpio_init(JOYSTICK_Y)`: Inicializa o pino do eixo Y do joystick.

### PIO:

- `npInit(LED_PIN)`: Configura a máquina PIO para controlar a matriz de LEDs.
- `npClear()`: Limpa o buffer de pixels.
- `npWrite()`: Atualiza os LEDs para desligados.

### I2C:

- `i2c_init(i2c1, ssd1306_i2c_clock * 1000)`: Inicializa a comunicação I2C.
- `gpio_set_function(I2C_SDA, GPIO_FUNC_I2C)`: Configura o pino SDA.
- `gpio_set_function(I2C_SCL, GPIO_FUNC_I2C)`: Configura o pino SCL.
- `ssd1306_init()`: Inicializa o display OLED.

### Botões:

- `gpio_init(BUTTON_A)`: Inicializa o pino do Botão A.
- `gpio_set_dir(BUTTON_A, GPIO_IN)`: Configura o Botão A como entrada.
- `gpio_pull_up(BUTTON_A)`: Habilita o pull-up no Botão A.
- `gpio_init(BUTTON_B)`: Inicializa o pino do Botão B.
- `gpio_set_dir(BUTTON_B, GPIO_IN)`: Configura o Botão B como entrada.
- `gpio_pull_up(BUTTON_B)`: Habilita o pull-up no Botão B.

### Interação:

- Utilize o joystick para mover os LEDs e resolver os problemas matemáticos.
- Use o botão A para confirmar a resposta e o botão B para gerar um novo problema ou reiniciar o jogo.