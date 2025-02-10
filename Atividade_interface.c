#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "matrix.pio.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define BUTTON_A 5          //Pino em que se localiza o botão A
#define BUTTON_B 6          //Pino em que se localiza o botão B
#define PIN_MATRIX 7        //Pino em que se localiza a matriz de leds
#define LED_G 11          //Pino em que se localiza o led Verde
#define LED_B 12          //Pino em que se localiza o led Azul
#define LED_R 13          //Pino em que se localiza o led Vermelho


#define TAM_MATRIX 25

bool led_Status = false;
static volatile uint numero_inserido = 0;
static volatile uint32_t last_time = 0;
PIO pio = pio0;
int sm;


const uint32_t desenho_numeros[10][25] =  {
    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0
    },//0
    {0, 1, 1, 1, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0
     },//1
    {0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0
    },//2
    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0
    },//3
    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 0, 1, 0
    },//4
    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0
    },//5
    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0
    },//6
    {0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 0, 0, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0
    },//7
    {0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0
    },//8
    {0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     0, 1, 1, 1, 0,
     0, 1, 0, 1, 0,
     0, 1, 1, 1, 0
    } //9
};


uint32_t color_rgb(double r, double g, double b);
void number_display();
bool pisca_Led(uint gpio);
void gpio_irq_handler(uint gpio, uint32_t events);
void inicializaPinos();
void mensagem_Display(char *mensagem, ssd1306_t ssd);
int main() {
    inicializaPinos();

    ssd1306_t ssd; // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    uint offset = pio_add_program(pio, &matrix_program);
    sm = pio_claim_unused_sm(pio, true);
    matrix_program_init(pio, sm, offset, PIN_MATRIX);

    while (1) {

        if (stdio_usb_connected()){
            char caracter[2];
            if(scanf("%c", &caracter[0]) != EOF){
                numero_inserido = caracter[0] - 48;
                printf("Número: %d\n", numero_inserido);
                if (numero_inserido>=0 && numero_inserido < 10){
                    number_display();
                    char buffer[30];
                    sprintf(buffer, "Foi inserido o número: %d", numero_inserido);
                    mensagem_Display(buffer, ssd);
                    sleep_ms(100);
                }
                caracter[1] = '\0';
                mensagem_Display(caracter, ssd);
            }
        }
        sleep_ms(100);
    }
    return 0;
}

void mensagem_Display(char *mensagem, ssd1306_t ssd){
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, true); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, false, true); // Desenha um retângulo
    ssd1306_draw_string(&ssd, mensagem, 8, 10); // Desenha uma string 
    ssd1306_send_data(&ssd); // Atualiza o display
}

//Definir uma cor em hexidécimal
uint32_t color_rgb(double r, double g, double b) {
    return (((uint32_t)(g * 255) << 24) | ((uint32_t)(r * 255) << 16) | ((uint32_t)(b * 255) << 8));
}

// Função para imprimir número no display de leds
void number_display() {
    uint32_t cor_Led;
    for (int i = 0; i < TAM_MATRIX; i++) {
        cor_Led = desenho_numeros[numero_inserido][i] ? color_rgb(0.2, 0.0, 0.2) : color_rgb(0.0, 0.0, 0.0);
        pio_sm_put_blocking(pio, sm, cor_Led);
    }
}

bool pisca_Led(uint gpio){
    int situacao = gpio_get(gpio);
    gpio_put(gpio, !situacao);
    !situacao ? printf("Ligou led: "): printf("Desligou led: "); 
}

//Função para execução de ação quando algum botão definido for executado
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if ((current_time - last_time) > 200){
        if (gpio_get(gpio) == 0) {
            if (gpio == BUTTON_A) {
                pisca_Led(LED_G);
                printf("Verde\n");
            } else if (gpio == BUTTON_B) {
                pisca_Led(LED_B);
                printf("Azul\n");
            }
            number_display();
            last_time = current_time;
        }
    }
}

/**
 * Inicialização de todos os pinos a serem utilizados na placa
 **/
void inicializaPinos(){
    stdio_init_all();

    // Inicialização e Configuração dos pinos destinados ao led
    gpio_init(LED_R);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_init(LED_B);
    gpio_set_dir(LED_B, GPIO_OUT);
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT);

    // Inicialização e Definição dos botões A e B como pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
}