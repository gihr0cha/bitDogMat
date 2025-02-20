#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "ssd1306.h"
#include "ws2818b.pio.h"

// Definição dos pinos de botões
#define JOYSTICK_Y 26
#define JOYSTICK_X 27
#define JOYSTICK_SW 22
#define BUTTON_A 5
#define BUTTON_B 6

// Definição do número de LEDs e pino
#define LED_COUNT 25
#define LED_PIN 7
#define LED_R 13
#define LED_G 11



// Definição dos pinos de comunicação I2C
#define I2C_SDA 14
#define I2C_SCL 15

struct repeating_timer timer; // Timer para atualização do display
// Definição de pixel rgb
struct pixel_t
{
  uint8_t G, R, B; // Três valores de 8-bits compõem um pixel
};
typedef struct pixel_t pixel_t; // Renomeia a struct para pixel_t
typedef pixel_t npLED_t;        // Renomeia a struct para npLED_t

// Declaração do buffer de pixels que formam a matriz
npLED_t leds[LED_COUNT];

// Variáveis para uso da máquina PIO
PIO np_pio;
uint sm;

int led_count = 0;                  // Contador de LEDs acesos
int point = -1;                     // Pontuação
int result = 0;                     // Resultado esperado
int num1, num2;                     // Números para operação
char operator;                      // Operação
uint8_t ssd[ssd1306_buffer_length]; // Buffer do display
int countdown = 60;

// Configuração do display
struct render_area frame_area = {
    .start_column = 0,
    .end_column = ssd1306_width - 1,
    .start_page = 0,
    .end_page = ssd1306_n_pages - 1};

// Mapeamento dos LEDs na matriz
const uint8_t led_index[LED_COUNT] = {
    24, 23, 22, 21, 20,
    15, 16, 17, 18, 19,
    14, 13, 12, 11, 10,
    5, 6, 7, 8, 9,
    4, 3, 2, 1, 0};

/**
 * Inicializa a máquina PIO para controle da matriz de LEDs
 */
void npInit(uint pin)
{
  uint offset = pio_add_program(pio0, &ws2818b_program);
  np_pio = pio0;
  sm = pio_claim_unused_sm(np_pio, false);
  if (sm < 0)
  {
    np_pio = pio1;
    sm = pio_claim_unused_sm(np_pio, true);
  }
  ws2818b_program_init(np_pio, sm, offset, pin, 800000.f);
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    leds[i].R = 0;
    leds[i].G = 0;
    leds[i].B = 0;
  }
}

/**
 * Atribui uma cor RGB a um LED
 */

void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b)
{
  leds[index].R = r;
  leds[index].G = g;
  leds[index].B = b;
}

/**
 * Limpa o buffer de pixels
 */
void npClear()
{
  for (uint i = 0; i < LED_COUNT; ++i)
    npSetLED(i, 0, 0, 0);
}

/**
 * Escreve os dados do buffer nos LEDs
 */
void npWrite()
{
  for (uint i = 0; i < LED_COUNT; ++i)
  {
    pio_sm_put_blocking(np_pio, sm, leds[i].G);
    pio_sm_put_blocking(np_pio, sm, leds[i].R);
    pio_sm_put_blocking(np_pio, sm, leds[i].B);
  }
  sleep_us(100); // Espera 100us, sinal de RESET do datasheet
}

/**
 * Atualiza o display com o contador de LEDs
 */
void startDisplay()
{

  char inicio_str[16];
  char regra_str[16];
  char botaoA_str[16];
  char botaoB_str[16];
  char joystck_str[16];

  memset(ssd, 0, ssd1306_buffer_length);

  sprintf(inicio_str, "Aperte A");
  sprintf(regra_str, "Para iniciar");
  sprintf(joystck_str, "Mova o joystick");
  sprintf(botaoA_str, "A para responder");
  sprintf(botaoB_str, "B para proxima");

  ssd1306_draw_string(ssd, 0, 8, inicio_str);
  ssd1306_draw_string(ssd, 0, 20, regra_str);
  ssd1306_draw_string(ssd, 0, 32, botaoA_str);
  ssd1306_draw_string(ssd, 0, 44, botaoB_str);

  render_on_display(ssd, &frame_area);
}



// Atualiza o display com o problema matemático, pontuaçao e operação matemática
void updateDisplay()
{
  char point_str[16];
  char mat_str[16];
  char timer_str[16];

  sprintf(mat_str, "%d %c %d", num1, operator, num2);
  sprintf(point_str, "Pontos: %d", point);
  sprintf(timer_str, "%d s", countdown);
  // Limpa o display
  memset(ssd, 0, ssd1306_buffer_length);

  // Desenha as strings no display
  ssd1306_draw_string(ssd, 0, 8, mat_str);
  ssd1306_draw_string(ssd, 0, 20, point_str);
  ssd1306_draw_string(ssd, 0, 32, timer_str);

  render_on_display(ssd, &frame_area);

  gpio_put(LED_G, 0);
  gpio_put(LED_R, 0);
}

void WrongAnswer()
{
  point = 0;
  countdown = 0;
  
  char wrong_str[16];
  char reset_str[16];

  if (countdown == 0)
  {
      sprintf(wrong_str, "Tempo esgotado");
  }
  if (result != led_count)
  {
    sprintf(wrong_str, "Resposta errada");
  }
  else
  {
    sprintf(wrong_str, " ");
  }
  sprintf(reset_str, "B para reiniciar");

  memset(ssd, 0, ssd1306_buffer_length);
  ssd1306_draw_string(ssd, 0, 8, wrong_str);
  ssd1306_draw_string(ssd, 0, 20, reset_str);
  render_on_display(ssd, &frame_area);

  gpio_put(LED_G, 0);
  gpio_put(LED_R, 1);
}

bool countdown_callback(struct repeating_timer *t)
{
  if (countdown > 0)
  {
    countdown--;
    updateDisplay();
  }
  if (countdown == 0)
  {
    WrongAnswer();
  }
  return true; // Continue repeating
}
/**
 * Liga um LED ao mover o joystick */

void turnOnLED()
{
  if (led_count < LED_COUNT)
  {
    npSetLED(led_index[led_count], 0, 0, 50); // Acende o LED
    led_count++;
    npWrite();
  }
}

/**
 * Desliga um LED ao mover o joystick
 */
void turnOffLED()
{
  if (led_count > 0)
  {
    led_count--;
    npSetLED(led_index[led_count], 0, 0, 0); // Apaga o LED
    npWrite();
  }
}
// gera um problema matemático para o usuário resolver
void generateMathProblem()
{
  cancel_repeating_timer(&timer); 
  countdown = 60;
  add_repeating_timer_ms(-1000, countdown_callback, NULL, &timer);
   // Cancela o timer anterior
 
  do
  {
    num1 = rand() % 25;
    num2 = rand() % 25;
    operator=(rand() % 2) ? '+' : '-';
    result = (operator== '+') ? (num1 + num2) : (num1 - num2);
  } while (result < 0 || result >= 25);
}

// Exibe mensagem de erro no display quando a resposta está errada



// Lê a resposta do usuário e atualiza a pontuação
void readCount(int count)
{
  if (gpio_get(BUTTON_A) == 0) // Botão A pressionado
  {
    if (result == count)
    {
      gpio_put(LED_G, 1);
      gpio_put(LED_R, 0);
      point++;               // Incrementa a pontuação
      led_count = 0;         // Reseta o contador de LEDs
      npClear();             // Limpa os LEDs
      npWrite();             // Atualiza os LEDs
      led_count = 0;         // Reseta o contador de LEDs
      generateMathProblem(); 
      updateDisplay(); // Gera um novo problema matemático
    }
    else
    {
      WrongAnswer();
    }

    sleep_ms(200); // Debouncing
  }
}

// Gera um novo problema matemático ao pressionar o botão B
void nextMath()
{
  if (gpio_get(BUTTON_B) == 0)
  {
    npClear();
    npWrite();
    led_count = 0;
    generateMathProblem();
    updateDisplay();

    sleep_ms(200); // Debouncing
  }
}

// configuração do sistema
void setup()
{
  // Inicialização do ADC
  stdio_init_all();
  adc_init();

  // Inicialização dos pinos do joystick
  adc_gpio_init(JOYSTICK_X);
  adc_gpio_init(JOYSTICK_Y);

  gpio_init(LED_R);
  gpio_set_dir(LED_R, GPIO_OUT);
  gpio_put(LED_R, 0);

  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);
  gpio_put(LED_G, 0);



  // Inicialização da máquina PIO
  npInit(LED_PIN);
  npClear();
  npWrite();
  srand(time(NULL)); // Inicializa o gerador de números aleatórios

  // Inicialização do i2c
  i2c_init(i2c1, ssd1306_i2c_clock * 1000); // Inicializa o i2c com a velocidade de clock definida
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);



  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Inicialização do display
  ssd1306_init();
  calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer de renderização
  memset(ssd, 0, ssd1306_buffer_length);            // Zera o display
  render_on_display(ssd, &frame_area);
  startDisplay(); // Inicializa o display com o contador de LEDs

  // Configuração dos botões
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A); // Habilita o pull-up

  gpio_init(BUTTON_B);
  gpio_set_dir(BUTTON_B, GPIO_IN);
  gpio_pull_up(BUTTON_B);

  frame_area.start_column = 0;
  frame_area.end_column = ssd1306_width - 1;
  frame_area.start_page = 0;
  frame_area.end_page = ssd1306_n_pages - 1;
  // Inicializa o display com o contador de LEDs

  // Aguarda o botão A ser pressionado para iniciar o jogo
  while (gpio_get(BUTTON_A) != 0)
  {
    sleep_ms(100);
  }

  startDisplay();
}

int main()
{
  setup();

  while (true)
  {

    readCount(led_count); // Botão A pressionado para responder
    nextMath();           // Botão B pressionado

    adc_select_input(1); // Seleciona o pino do joystick
    uint16_t joystick_x = adc_read();

    if (joystick_x > 3000)
    { // Mover para direita
      turnOnLED();
      sleep_ms(200); // Debouncing
    }
    else if (joystick_x < 1000)
    { // Mover para esquerda
      turnOffLED();
      sleep_ms(200); // Debouncing
    }
  }
  return 0;
}