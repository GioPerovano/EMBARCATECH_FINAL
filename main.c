#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pio_archive.h"

// Definição de pinos e configurações
#define SAIDA 7
#define FRAMES 25
#define BRILHO 1 
#define JOYSTICK_BUTTON 22
#define JOYSTICK_X 26
#define JOYSTICK_Y 27

// Variáveis globais
static volatile uint a = 0;
static volatile uint32_t last_time = 0;
static volatile uint32_t ultimo_tempo = 0;
PIO pio = pio0; 
uint sm = 0;
uint16_t valor_x, valor_y;
uint16_t valor_x_anterior = 0, valor_y_anterior = 0;
int posicao_centro = 2047;

// Função para calcular a cor RGB dos LEDs
uint32_t RGB(double r, double g, double b)
{
  unsigned char R, G, B;
  R = r * 255 * BRILHO;
  G = g * 255 * BRILHO;
  B = b * 255 * BRILHO;
  return (G << 24) | (R << 16) | (B << 8);
}

// Função que desenha padrões nos LEDs
void DESENHO (double frame[FRAMES][3], uint32_t valor_led, PIO pio, uint sm) {
    int ordem_indices[FRAMES] = {24, 23, 22, 21, 20, 15, 16, 17, 18, 19, 14, 13, 12, 11, 10, 5, 6, 7, 8, 9, 4, 3, 2, 1, 0};
    for (int i = 0; i < FRAMES; i++) {
        int idx = ordem_indices[i];
        double r = frame[idx][0];
        double g = frame[idx][1];
        double b = frame[idx][2];
        valor_led = RGB(r, g, b);  
        pio_sm_put_blocking(pio, sm, valor_led); 
    }
}

// Definição dos padrões de cores dos LEDs para diferentes ações
static double frames_digitos[6][FRAMES][3] = { // Desligados
{       {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},

// Direita
{       {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},

// Esquerda
{       {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 1.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},

// Freio
{       {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},

// Atenção
{       {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}},

// Acelerando 
{       {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0},
        {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0},
        {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0},
        {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0},
        {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}, {1.0, 1.0, 0.0}},};

// Funções para exibir padrões de luz correspondentes às ações
void freio(){
  DESENHO (frames_digitos[4], 0, pio, sm);
  sleep_ms(700);
  DESENHO (frames_digitos[0], 0, pio, sm);
  sleep_ms(500);
}

void direita(){
  DESENHO (frames_digitos[1], 0, pio, sm);
  sleep_ms(700);
  DESENHO (frames_digitos[0], 0, pio, sm);
  sleep_ms(500);
}

void esquerda(){
  DESENHO (frames_digitos[2], 0, pio, sm);
  sleep_ms(700);
  DESENHO (frames_digitos[0], 0, pio, sm);
  sleep_ms(500);
}

void atencao(){
  DESENHO (frames_digitos[3], 0, pio, sm);
  sleep_ms(700);
  DESENHO (frames_digitos[0], 0, pio, sm);
  sleep_ms(500);
}

void acelerando(){
  DESENHO (frames_digitos[5], 0, pio, sm);
  sleep_ms(700);
  DESENHO (frames_digitos[0], 0, pio, sm);
  sleep_ms(500);
}

// Verificação dos movimentos do joystick
void verificar_joystick() {
    if (valor_x > posicao_centro) {
        printf("ESQUERDA\n");
        esquerda();
    } else if (valor_x < posicao_centro) {
        printf("DIREITA\n");
        direita();
    }
    else if (valor_y > posicao_centro) {
        printf("ACELERANDO\n");
        acelerando();

    } else if (valor_y < posicao_centro) {
        printf("FREIO\n");
        freio();
    }

    if (gpio_get(JOYSTICK_BUTTON) == 0) {  // Verifica se o botão foi pressionado
        printf("PISCA-ALERTA!\n");
        atencao();
    }
}

// Configuração inicial dos pinos do joystick e do botão
void configurar_hardware() {
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);  // Habilita o pull-up para evitar ruídos
}

// Leitura dos valores do joystick
void ler_joystick(uint16_t *eixo_x, uint16_t *eixo_y) {
    adc_select_input(0);
    sleep_us(20);
    *eixo_x = adc_read();
    adc_select_input(1);
    sleep_us(20);
    *eixo_y = adc_read();
}

// Função principal do programa
int main() {
    stdio_init_all(); // Inicializa a comunicação serial
    pio = pio0;  // Seleciona a primeira unidade PIO
    uint offset = pio_add_program(pio, &pio_matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, SAIDA); // Configura a PIO
    configurar_hardware(); // Configura o joystick

    printf("****************** INICIANDO O PROGRAMA ******************\n\n");
    while (true) {
        ler_joystick(&valor_x, &valor_y); // Lê os valores do joystick
        verificar_joystick();  // Verifica a posição do joystick e executa a ação correspondente
        sleep_ms(50);
    }

    return 0;
}
