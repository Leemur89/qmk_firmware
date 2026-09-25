// Heatmap persistante : même rendu que TYPING_HEATMAP de QMK, mais la
// "chaleur" vit dans un tampon propre au keymap au lieu de
// g_rgb_frame_buffer, que QMK remet à zéro à chaque (ré)activation de l'effet.
// Changer de layer (couleur unie) ou afficher une forme de mod puis revenir au
// layer 0 retrouve donc la heatmap telle qu'elle a évolué entre-temps.
//
// L'accumulation et le refroidissement ne dépendent pas du mode affiché :
// persistent_heatmap_task() est appelée depuis housekeeping_task_user() sur
// les deux moitiés (comme la heatmap QMK, chaque moitié calcule la chaleur de
// ses propres touches à partir de sa matrice), et continue de compter les
// appuis et de refroidir pendant qu'un autre layer est affiché.

#include "matrix.h"
#include "timer.h"

#ifndef RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP
#    define RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP 32
#endif
#ifndef RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS
#    define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 25
#endif
#ifndef RGB_MATRIX_TYPING_HEATMAP_SPREAD
#    define RGB_MATRIX_TYPING_HEATMAP_SPREAD 40
#endif
#ifndef RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT
#    define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 16
#endif

static uint8_t      persistent_heat[MATRIX_ROWS][MATRIX_COLS];
static matrix_row_t persistent_heat_prev[MATRIX_ROWS];
static uint16_t     persistent_heat_timer;

static void persistent_heatmap_add(uint8_t row, uint8_t col) {
    if (g_led_config.matrix_co[row][col] == NO_LED) {
        return;
    }
    led_point_t hit = g_led_config.point[g_led_config.matrix_co[row][col]];
    for (uint8_t i_row = 0; i_row < MATRIX_ROWS; i_row++) {
        for (uint8_t i_col = 0; i_col < MATRIX_COLS; i_col++) {
            if (g_led_config.matrix_co[i_row][i_col] == NO_LED) {
                continue;
            }
            if (i_row == row && i_col == col) {
                persistent_heat[row][col] = qadd8(persistent_heat[row][col], RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP);
                continue;
            }
            led_point_t p        = g_led_config.point[g_led_config.matrix_co[i_row][i_col]];
            int32_t     dx       = (int32_t)hit.x - p.x;
            int32_t     dy       = (int32_t)hit.y - p.y;
            uint8_t     distance = sqrt16(dx * dx + dy * dy);
            if (distance <= RGB_MATRIX_TYPING_HEATMAP_SPREAD) {
                uint8_t amount = qsub8(RGB_MATRIX_TYPING_HEATMAP_SPREAD, distance);
                if (amount > RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT) {
                    amount = RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT;
                }
                persistent_heat[i_row][i_col] = qadd8(persistent_heat[i_row][i_col], amount);
            }
        }
    }
}

void persistent_heatmap_task(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t current = matrix_get_row(row);
        matrix_row_t pressed = current & ~persistent_heat_prev[row];
        persistent_heat_prev[row] = current;
        for (uint8_t col = 0; pressed && col < MATRIX_COLS; col++) {
            if (pressed & ((matrix_row_t)1 << col)) {
                persistent_heatmap_add(row, col);
            }
        }
    }

    if (timer_elapsed(persistent_heat_timer) >= RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS) {
        persistent_heat_timer = timer_read();
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                persistent_heat[row][col] = qsub8(persistent_heat[row][col], 1);
            }
        }
    }
}

bool PERSISTENT_HEATMAP(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led[LED_HITS_TO_REMEMBER];
            uint8_t led_count = rgb_matrix_map_row_column_to_led(row, col, led);
            uint8_t val       = persistent_heat[row][col];
            for (uint8_t i = 0; i < led_count; i++) {
                if (led[i] < led_min || led[i] >= led_max || !HAS_ANY_FLAGS(g_led_config.flags[led[i]], params->flags)) {
                    continue;
                }
                hsv_t hsv = {170 - qsub8(val, 85), rgb_matrix_config.hsv.s, scale8((qadd8(170, val) - 170) * 3, rgb_matrix_config.hsv.v)};
                rgb_t rgb = rgb_matrix_hsv_to_rgb(hsv);
                rgb_matrix_set_color(led[i], rgb.r, rgb.g, rgb.b);
            }
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}
