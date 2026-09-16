/* SPDX-License-Identifier: GPL-2.0-or-later */
#include QMK_KEYBOARD_H

// Tap Dance declarations
enum {
    TD_BSPC,
    TD_PRU,
};

// TG(4) can't be passed to ACTION_TAP_DANCE_DOUBLE (it only supports basic
// keycodes/mod-taps via register_code16), so the mouse-layer toggle is done
// manually here instead.
void td_pru_finished(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 2) {
        layer_invert(4);
    } else {
        register_code16(KC_ESC);
    }
}

void td_pru_reset(tap_dance_state_t *state, void *user_data) {
    if (state->count < 2) {
        unregister_code16(KC_ESC);
    }
}

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    // Tap once for backspace, twice for opt+backspace
    [TD_BSPC] = ACTION_TAP_DANCE_DOUBLE(KC_BSPC, LALT(KC_BSPC)),
    // Tap once for Escape, twice to toggle (lock) the mouse layer
    [TD_PRU] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_pru_finished, td_pru_reset),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT(
		LALT_T(KC_TAB), KC_Q, KC_W, KC_E, KC_R, KC_T,                         KC_Y, KC_U, KC_I, KC_O, KC_P, TD(TD_PRU),
		KC_LCTL, KC_A, KC_S, KC_D, LSFT_T(KC_F), KC_G,                        KC_H, RSFT_T(KC_J), KC_K, KC_L, KC_SCLN, KC_QUOT,
		KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,                                KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
		OSM(MOD_HYPR), MO(1), LGUI_T(KC_ENT),                                 KC_SPC, MO(2), HYPR_T(KC_BSPC)
	),
	[1] = LAYOUT(
		QK_BOOT, LSFT(KC_1), LSFT(KC_2), LSFT(KC_3), LSFT(KC_4), LSFT(KC_5),   LSFT(KC_6), LSFT(KC_7), LSFT(KC_8), LSFT(KC_9), LSFT(KC_0), KC_TRNS,
		KC_TRNS, KC_1, KC_2, KC_3, LSFT_T(KC_4), KC_5,                        KC_MINS, RSFT_T(KC_EQL), KC_GRV, KC_LBRC, KC_RBRC, KC_BSLS,
		KC_TRNS, KC_6, KC_7, KC_8, KC_9, KC_0,                                LSFT(KC_MINS), LSFT(KC_EQL), LSFT(KC_GRV), LSFT(KC_LBRC), LSFT(KC_RBRC), KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS,                                            OSM(MOD_RGUI), TD(TD_BSPC), LALT(KC_BSPC)
	),
	[2] = LAYOUT(
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_F2, KC_NO,                           LCTL(LSFT(LALT(LGUI(KC_Y)))), KC_MPRV, KC_MNXT, KC_MPLY, LGUI(LSFT(KC_T)), KC_TRNS,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_LSFT, KC_NO,                         KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, LGUI(KC_GRV), LSFT(KC_BSLS),
		KC_TRNS, KC_NO, LGUI(LSFT(KC_4)), KC_NO, KC_NO, KC_NO,                LGUI(KC_PPLS), LGUI(LSFT(KC_LBRC)), LGUI(LSFT(KC_RBRC)), LGUI(KC_PMNS), LGUI(KC_P0), KC_TRNS,
		OSM(MOD_LALT|MOD_LGUI), KC_TRNS, OSM(MOD_LGUI),                       KC_TRNS, KC_TRNS, KC_TRNS
	),
	[3] = LAYOUT(
		QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                           KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, KC_NO, KC_NO,                     LALT(KC_LEFT), LALT(KC_DOWN), LALT(KC_UP), LALT(KC_RGHT), KC_NO, KC_NO,
		KC_NO, RM_HUED, RM_SATD, RM_VALD, KC_NO, KC_NO,                       KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_TRNS, KC_TRNS,                                            KC_TRNS, KC_TRNS, KC_TRNS
	),
	[4] = LAYOUT(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                 KC_WWW_BACK, MS_WHLU, MS_WHLD, KC_WWW_FORWARD, KC_TRNS, TO(0),
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                 MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                 KC_TRNS, MS_LEFT, MS_DOWN, MS_RGHT, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, KC_TRNS,                                            MS_BTN1, MS_BTN1, MS_BTN2
	)
};

// Couleur RGB en fonction du layer actif. On ne synchronise pas layer_state
// vers la moitié droite (cf. historique dans config.h) : à la place on pilote
// le mode + la couleur rgb_matrix, qui sont déjà synchronisés vers l'esclave
// par le RPC intégré de QMK (RGB_MATRIX_SPLIT, activé via rgb_matrix.split_count
// dans keyboard.json). Ce canal existe indépendamment de nos changements et ne
// nécessite donc aucune transaction supplémentaire côté split_common.
static void rgb_matrix_update_layer_color(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 1:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_BLUE);
            break;
        case 2:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_ORANGE);
            break;
        case 3:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_PURPLE);
            break;
        case 4:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
            break;
        default:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_TYPING_HEATMAP);
            break;
    }
}

// Accès automatique au Layer 3 (Layer 1 + Layer 2)
layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, 1, 2, 3);
    rgb_matrix_update_layer_color(state);
    return state;
}

// Force la heatmap allumée au boot : RGB_MATRIX_DEFAULT_MODE ne s'applique
// qu'à une EEPROM vierge, donc un mode ou un état enable/disable déjà
// enregistré (ex. cycle_all, RGB éteint via RM_TOGG lors d'un test) ne
// serait sinon jamais remplacé par un flash ultérieur.
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_TYPING_HEATMAP);
}
