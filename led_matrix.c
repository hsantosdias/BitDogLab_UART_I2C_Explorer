#include "led_matrix.h"

static PIO np_pio;
static uint sm;
static npLED_t leds[LED_COUNT];

static int map_index(int row, int col) {
    return row * COLS + col;
}

static uint32_t rgb_to_grb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b;
}

void led_matrix_set_pixel(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < LED_COUNT) {
        leds[index] = (npLED_t){g, r, b};
    }
}

void led_matrix_init(void) {
    uint offset = pio_add_program(pio0, &ws2812b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2812b_program_init(np_pio, sm, offset, MATRIX_LED_PIN);
    led_matrix_clear();
}

void led_matrix_clear(void) {
    for (uint i = 0; i < LED_COUNT; i++) {
        led_matrix_set_pixel(i, 0, 0, 0);
    }
}

void led_matrix_write(void) {
    uint32_t save = save_and_disable_interrupts();
    for (uint i = 0; i < LED_COUNT; i++) {
        pio_sm_put_blocking(np_pio, sm, rgb_to_grb(leds[i].R, leds[i].G, leds[i].B));
    }
    busy_wait_us(300);
    restore_interrupts(save);
}

void led_matrix_display_number(int number) {
    led_matrix_clear();
    static const uint numbers[10][5][5] = {
        {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}}, // 0
        {{0, 1, 1, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0}}, // 1
        {{0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 0, 1, 0}, {0, 0, 1, 0, 0}}, // 2
        {{0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}}, // 3
        {{0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}}, // 4
        {{0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}}, // 5
        {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}}, // 6
        {{0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}}, // 7
        {{0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}}, // 8
        {{0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}}  // 9
    };

    for (uint i = 0; i < 5; i++) {
        for (uint j = 0; j < 5; j++) {
            if (numbers[number][i][j]) {
                led_matrix_set_pixel(map_index(i, j), 255, 255, 255);
            }
        }
    }
    led_matrix_write();
}
