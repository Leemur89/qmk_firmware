/* SPDX-License-Identifier: GPL-2.0-or-later */
#include QMK_KEYBOARD_H

// Tap Dance declarations
enum {
    TD_BSPC,
};

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    // Tap once for backspace, twice for opt+backspace
    [TD_BSPC] = ACTION_TAP_DANCE_DOUBLE(KC_BSPC, LALT(KC_BSPC)),
};

// Home row mods via combo instead of tap-hold: holding the layer-1/layer-2
// thumb key (TLM/TRM) together with a home row letter registers a modifier
// for as long as both stay held, so there is no tap/hold timing to fight.
// Mirrored finger-for-finger across both halves (pinky->index: Alt, Ctrl,
// Cmd, Shift) so either hand can reach any of the 4 mods.
//
// Shift+Cmd and Shift+Opt are added as 3-key combos on the Shift finger (F/J)
// plus the Cmd/Alt finger, needed for e.g. Shift+Cmd+Arrow / Shift+Opt+Arrow:
// arrows only live on the opposite hand's layer 2 (held via TRM), so a
// same-hand Cmd/Alt combo can't be held at the same time as that arrow-layer
// hold - stacking both mods into one combo on the Shift hand is the only way
// to reach them. QMK resolves the overlap with the single-mod combos
// automatically (the longest fully-pressed combo wins), and a combo's output
// keycode can itself carry a mod (e.g. LSFT(KC_LGUI)), which registers both
// mod bits for as long as the combo is held - no extra code needed for that.
enum combos {
    COMBO_ALT_L,
    COMBO_CTL_L,
    COMBO_CMD_L,
    COMBO_SFT_L,
    COMBO_SFT_CMD_L,
    COMBO_SFT_ALT_L,
    COMBO_ALT_R,
    COMBO_CTL_R,
    COMBO_CMD_R,
    COMBO_SFT_R,
    COMBO_SFT_CMD_R,
    COMBO_SFT_ALT_R,
    COMBO_ENTER,
};

const uint16_t PROGMEM combo_alt_l[]     = {MO(1), KC_A, COMBO_END};
const uint16_t PROGMEM combo_ctl_l[]     = {MO(1), KC_S, COMBO_END};
const uint16_t PROGMEM combo_cmd_l[]     = {MO(1), KC_D, COMBO_END};
const uint16_t PROGMEM combo_sft_l[]     = {MO(1), KC_F, COMBO_END};
const uint16_t PROGMEM combo_sft_cmd_l[] = {MO(1), KC_F, KC_D, COMBO_END};
const uint16_t PROGMEM combo_sft_alt_l[] = {MO(1), KC_F, KC_A, COMBO_END};
const uint16_t PROGMEM combo_alt_r[]     = {LT(2, KC_SPC), KC_SCLN, COMBO_END};
const uint16_t PROGMEM combo_ctl_r[]     = {LT(2, KC_SPC), KC_L, COMBO_END};
const uint16_t PROGMEM combo_cmd_r[]     = {LT(2, KC_SPC), KC_K, COMBO_END};
const uint16_t PROGMEM combo_sft_r[]     = {LT(2, KC_SPC), KC_J, COMBO_END};
const uint16_t PROGMEM combo_sft_cmd_r[] = {LT(2, KC_SPC), KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM combo_sft_alt_r[] = {LT(2, KC_SPC), KC_J, KC_SCLN, COMBO_END};
// Enter lives on TLM+B since the physical TLI key (previously Enter/Gui) was
// removed. Declared after the mod combos so process_combo_event() ignores it.
const uint16_t PROGMEM combo_enter[]     = {MO(1), KC_B, COMBO_END};

combo_t key_combos[] = {
    [COMBO_ALT_L]     = COMBO(combo_alt_l, KC_LALT),
    [COMBO_CTL_L]     = COMBO(combo_ctl_l, KC_LCTL),
    [COMBO_CMD_L]     = COMBO(combo_cmd_l, KC_LGUI),
    [COMBO_SFT_L]     = COMBO(combo_sft_l, KC_LSFT),
    [COMBO_SFT_CMD_L] = COMBO(combo_sft_cmd_l, LSFT(KC_LGUI)),
    [COMBO_SFT_ALT_L] = COMBO(combo_sft_alt_l, LSFT(KC_LALT)),
    [COMBO_ALT_R]     = COMBO(combo_alt_r, KC_LALT),
    [COMBO_CTL_R]     = COMBO(combo_ctl_r, KC_LCTL),
    [COMBO_CMD_R]     = COMBO(combo_cmd_r, KC_LGUI),
    [COMBO_SFT_R]     = COMBO(combo_sft_r, KC_LSFT),
    [COMBO_SFT_CMD_R] = COMBO(combo_sft_cmd_r, LSFT(KC_LGUI)),
    [COMBO_SFT_ALT_R] = COMBO(combo_sft_alt_r, LSFT(KC_LALT)),
    [COMBO_ENTER]     = COMBO(combo_enter, KC_ENT),
};

// A combo-mod must only apply to a key typed with the OPPOSITE hand from the
// one holding the combo (e.g. left-hand TLM+F for Shift only modifies a key
// typed with the right hand) - otherwise it types unmodified. This mirrors
// "chordal hold" and isn't something the combo feature enforces on its own.
static uint16_t active_combo_mod  = KC_NO;
static bool     active_combo_left = false;
// Only one same-hand keystroke is tracked at a time; holding several
// same-hand keys during a combo hold is an edge case this doesn't cover.
static uint16_t suppressed_keycode = KC_NO;

void process_combo_event(uint16_t combo_index, bool pressed) {
    if (combo_index > COMBO_SFT_ALT_R) {
        return;
    }
    if (pressed) {
        active_combo_mod  = key_combos[combo_index].keycode;
        active_combo_left = combo_index <= COMBO_SFT_ALT_L;
    } else {
        active_combo_mod = KC_NO;
    }
}

// crkbd rev4_1 is a direct-pin split matrix with 4 rows per half (see
// matrix_pins in info.json): rows 0-3 are the left half, rows 4-7 the right.
#define KEY_IS_LEFT_HAND(record) ((record)->event.key.row < (MATRIX_ROWS / 2))

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT(
		LT(4, KC_TAB), KC_Q, KC_W, KC_E, KC_R, KC_T,                          KC_Y, KC_U, KC_I, KC_O, KC_P, KC_ESC,
		OSM(MOD_HYPR), KC_A, KC_S, KC_D, KC_F, KC_G,                          KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
		KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,                                KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
		KC_NO, MO(1), KC_NO,                                                  KC_NO, LT(2, KC_SPC), KC_NO
	),
	[1] = LAYOUT(
		QK_BOOT, LSFT(KC_1), LSFT(KC_2), LSFT(KC_3), LSFT(KC_4), LSFT(KC_5),   LSFT(KC_6), LSFT(KC_7), LSFT(KC_8), LSFT(KC_9), LSFT(KC_0), KC_TRNS,
		KC_TRNS, KC_1, KC_2, KC_3, LSFT_T(KC_4), KC_5,                        KC_MINS, RSFT_T(KC_EQL), KC_GRV, KC_LBRC, KC_RBRC, KC_BSLS,
		KC_TRNS, KC_6, KC_7, KC_8, KC_9, KC_0,                                LSFT(KC_MINS), LSFT(KC_EQL), LSFT(KC_GRV), LSFT(KC_LBRC), LSFT(KC_RBRC), KC_TRNS,
		KC_NO, KC_TRNS, KC_NO,                                                KC_NO, TD(TD_BSPC), KC_NO
	),
	[2] = LAYOUT(
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_F2, KC_NO,                           LCTL(LSFT(LALT(LGUI(KC_Y)))), KC_MPRV, KC_MNXT, KC_MPLY, LGUI(LSFT(KC_T)), KC_TRNS,
		KC_TRNS, KC_NO, KC_NO, KC_NO, KC_LSFT, KC_NO,                         KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, LGUI(KC_GRV), LSFT(KC_BSLS),
		KC_TRNS, KC_NO, LGUI(LSFT(KC_4)), KC_NO, KC_NO, KC_NO,                LGUI(KC_PPLS), LGUI(LSFT(KC_LBRC)), LGUI(LSFT(KC_RBRC)), LGUI(KC_PMNS), LGUI(KC_P0), KC_TRNS,
		KC_NO, KC_TRNS, KC_NO,                                                KC_NO, KC_TRNS, KC_NO
	),
	[3] = LAYOUT(
		QK_BOOT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,                           KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		RM_TOGG, RM_HUEU, RM_SATU, RM_VALU, KC_NO, KC_NO,                     LALT(KC_LEFT), LALT(KC_DOWN), LALT(KC_UP), LALT(KC_RGHT), KC_NO, KC_NO,
		KC_NO, RM_HUED, RM_SATD, RM_VALD, KC_NO, KC_NO,                       KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
		KC_TRNS, KC_TRNS, KC_TRNS,                                            KC_TRNS, KC_TRNS, KC_TRNS
	),
	[4] = LAYOUT(
		KC_TRNS, KC_TRNS, LGUI(KC_W), KC_TRNS, LGUI(KC_R), LGUI(KC_T),        LGUI(KC_LBRC), MS_WHLU, MS_WHLD, LGUI(KC_RBRC), KC_TRNS, QK_LLCK,
		KC_TRNS, LGUI(KC_A), KC_TRNS, LGUI(KC_D), LGUI(KC_F), KC_TRNS,       MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_TRNS, KC_TRNS,
		KC_TRNS, KC_TRNS, LGUI(KC_X), LGUI(KC_C), LGUI(KC_V), KC_TRNS,       LGUI(KC_PPLS), LGUI(LSFT(KC_LBRC)), LGUI(LSFT(KC_RBRC)), LGUI(KC_PMNS), LGUI(KC_P0), KC_TRNS,
		KC_NO, KC_TRNS, KC_NO,                                                KC_NO, MS_BTN1, KC_NO
	)
};

// Couleur RGB en fonction du layer actif. On ne synchronise pas layer_state
// vers la moitié droite (cf. historique dans config.h) : à la place on pilote
// le mode + la couleur rgb_matrix, qui sont déjà synchronisés vers l'esclave
// par le RPC intégré de QMK (RGB_MATRIX_SPLIT, activé via rgb_matrix.split_count
// dans keyboard.json). Ce canal existe indépendamment de nos changements et ne
// nécessite donc aucune transaction supplémentaire côté split_common.
// Hyper est porté par PLM (OSM(MOD_HYPR)) : maintenu, ses mods sont "réels"
// (get_mods()) ; tapé, il reste armé en one-shot (get_oneshot_mods()) tant
// qu'il n'a pas été consommé par la touche suivante.
static bool is_hyper_active(void) {
    return ((get_mods() | get_oneshot_mods()) & MOD_HYPR) == MOD_HYPR;
}

static bool is_cmd_active(void) {
    return ((get_mods() | get_oneshot_mods()) & MOD_MASK_GUI) != 0;
}

static bool is_ctrl_active(void) {
    return ((get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL) != 0;
}

static bool is_opt_active(void) {
    return ((get_mods() | get_oneshot_mods()) & MOD_MASK_ALT) != 0;
}

static bool is_shift_active(void) {
    return ((get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) != 0;
}

// Formes RGB par modificateur (voir rgb_matrix_shapes.c) : carré sur Cmd,
// croix multicolore sur Hyper, ^ sur Ctrl, glyphe sur Opt, ligne sur Shift -
// superposées quand plusieurs mods sont actifs en même temps. Les noms des
// constantes RGB_MATRIX_CUSTOM_SHAPE_* doivent rester synchronisés avec les
// noms déclarés dans rgb_matrix_user.inc.
enum {
    SHAPE_MASK_CMD   = 1 << 0,
    SHAPE_MASK_HYPER = 1 << 1,
    SHAPE_MASK_CTRL  = 1 << 2,
    SHAPE_MASK_OPT   = 1 << 3,
    SHAPE_MASK_SHIFT = 1 << 4,
};

// Hyper est défini comme Ctrl+Shift+Alt+Gui combinés (MOD_HYPR) : dès qu'il
// est actif, get_mods()/get_oneshot_mods() porte aussi les bits Ctrl/Shift/
// Alt/Gui, ce qui ferait à tort passer is_cmd_active()/is_ctrl_active()/
// is_opt_active()/is_shift_active() à true et superposerait les 4 formes.
// Hyper doit donc rester exclusif des 4 autres pour le choix de la forme.
static uint8_t active_shape_mask(void) {
    if (is_hyper_active()) {
        return SHAPE_MASK_HYPER;
    }
    uint8_t mask = 0;
    if (is_cmd_active()) mask |= SHAPE_MASK_CMD;
    if (is_ctrl_active()) mask |= SHAPE_MASK_CTRL;
    if (is_opt_active()) mask |= SHAPE_MASK_OPT;
    if (is_shift_active()) mask |= SHAPE_MASK_SHIFT;
    return mask;
}

// Hyper étant exclusif (cf. active_shape_mask()), seules les entrées sans le
// bit HYPER sont jamais utilisées ; les autres ne sont pas renseignées
// ci-dessous.
static const uint8_t shape_mode_for_mask[32] = {
    [SHAPE_MASK_HYPER]                                                    = RGB_MATRIX_CUSTOM_SHAPE_HYPER,
    [SHAPE_MASK_CMD]                                                      = RGB_MATRIX_CUSTOM_SHAPE_CMD,
    [SHAPE_MASK_CTRL]                                                     = RGB_MATRIX_CUSTOM_SHAPE_CTRL,
    [SHAPE_MASK_OPT]                                                      = RGB_MATRIX_CUSTOM_SHAPE_OPT,
    [SHAPE_MASK_SHIFT]                                                    = RGB_MATRIX_CUSTOM_SHAPE_SHIFT,
    [SHAPE_MASK_CMD | SHAPE_MASK_CTRL]                                    = RGB_MATRIX_CUSTOM_SHAPE_CMD_CTRL,
    [SHAPE_MASK_CMD | SHAPE_MASK_OPT]                                     = RGB_MATRIX_CUSTOM_SHAPE_CMD_OPT,
    [SHAPE_MASK_CMD | SHAPE_MASK_SHIFT]                                   = RGB_MATRIX_CUSTOM_SHAPE_CMD_SHIFT,
    [SHAPE_MASK_CTRL | SHAPE_MASK_OPT]                                    = RGB_MATRIX_CUSTOM_SHAPE_CTRL_OPT,
    [SHAPE_MASK_CTRL | SHAPE_MASK_SHIFT]                                  = RGB_MATRIX_CUSTOM_SHAPE_CTRL_SHIFT,
    [SHAPE_MASK_OPT | SHAPE_MASK_SHIFT]                                   = RGB_MATRIX_CUSTOM_SHAPE_OPT_SHIFT,
    [SHAPE_MASK_CMD | SHAPE_MASK_CTRL | SHAPE_MASK_OPT]                   = RGB_MATRIX_CUSTOM_SHAPE_CMD_CTRL_OPT,
    [SHAPE_MASK_CMD | SHAPE_MASK_CTRL | SHAPE_MASK_SHIFT]                 = RGB_MATRIX_CUSTOM_SHAPE_CMD_CTRL_SHIFT,
    [SHAPE_MASK_CMD | SHAPE_MASK_OPT | SHAPE_MASK_SHIFT]                  = RGB_MATRIX_CUSTOM_SHAPE_CMD_OPT_SHIFT,
    [SHAPE_MASK_CTRL | SHAPE_MASK_OPT | SHAPE_MASK_SHIFT]                 = RGB_MATRIX_CUSTOM_SHAPE_CTRL_OPT_SHIFT,
    [SHAPE_MASK_CMD | SHAPE_MASK_CTRL | SHAPE_MASK_OPT | SHAPE_MASK_SHIFT] = RGB_MATRIX_CUSTOM_SHAPE_CMD_CTRL_OPT_SHIFT,
};

static void rgb_matrix_update_layer_color(layer_state_t state) {
    uint8_t mask = active_shape_mask();
    if (mask != 0) {
        rgb_matrix_mode_noeeprom(shape_mode_for_mask[mask]);
        return;
    }
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
            rgb_matrix_sethsv_noeeprom(HSV_YELLOW);
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

// Enforces the opposite-hand-only rule for the combo mods above: while a
// combo mod is held, a key typed with the SAME hand as the combo is sent
// unmodified instead of picking up that mod.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (active_combo_mod == KC_NO) {
        return true;
    }

    if (record->event.pressed) {
        if (keycode == active_combo_mod || suppressed_keycode != KC_NO) {
            return true;
        }
        if (KEY_IS_LEFT_HAND(record) == active_combo_left) {
            unregister_code16(active_combo_mod);
            register_code16(keycode);
            register_code16(active_combo_mod);
            suppressed_keycode = keycode;
            return false;
        }
    } else if (keycode == suppressed_keycode) {
        unregister_code16(keycode);
        suppressed_keycode = KC_NO;
        return false;
    }

    return true;
}

// layer_state_set_user ne se redéclenche pas quand seuls les mods changent
// (hold de PLM ou arm/consommation de son one-shot) : on repère ces
// transitions ici pour rafraîchir la couleur sans repasser par un changement
// de layer.
void matrix_scan_user(void) {
    static uint8_t last_shape_mask = 0;
    uint8_t        mask            = active_shape_mask();
    if (mask != last_shape_mask) {
        last_shape_mask = mask;
        rgb_matrix_update_layer_color(layer_state);
    }
}

// Force la heatmap allumée au boot : RGB_MATRIX_DEFAULT_MODE ne s'applique
// qu'à une EEPROM vierge, donc un mode ou un état enable/disable déjà
// enregistré (ex. cycle_all, RGB éteint via RM_TOGG lors d'un test) ne
// serait sinon jamais remplacé par un flash ultérieur.
void keyboard_post_init_user(void) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_TYPING_HEATMAP);
}
