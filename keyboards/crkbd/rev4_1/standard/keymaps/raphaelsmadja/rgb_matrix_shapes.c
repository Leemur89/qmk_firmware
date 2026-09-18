// Formes RGB affichées quand Cmd/Hyper/Ctrl/Opt est actif (voir keymap.c,
// active_shape_mask()). Les indices LED sont ceux de keyboard.json
// (rgb_matrix.layout), numérotés globalement sur les deux moitiés ; chaque
// moitié n'allume que les indices qui lui appartiennent (RGB_MATRIX_USE_LIMITS
// s'en charge), donc les mêmes tableaux dessinent la forme identiquement des
// deux côtés sans synchronisation supplémentaire - le seul état propagé au
// clavier esclave est le numéro de mode ci-dessous, déjà synchronisé par le
// RPC RGB_MATRIX_SPLIT intégré à QMK.
enum {
    SHAPE_MASK_CMD   = 1 << 0,
    SHAPE_MASK_HYPER = 1 << 1,
    SHAPE_MASK_CTRL  = 1 << 2,
    SHAPE_MASK_OPT   = 1 << 3,
};

typedef struct {
    const uint8_t *leds;
    uint8_t        count;
    hsv_t          hsv;
} rgb_shape_t;

// Carré (Cmd) : W E R / S...F / X C V sur chaque moitié.
static const uint8_t shape_leds_cmd[] = {
    12, 11, 4, 13, 5, 14, 9, 6,
    35, 34, 27, 36, 28, 37, 32, 29,
};
// ^ (Ctrl) : E en pointe, S et F en pieds.
static const uint8_t shape_leds_ctrl[] = {
    11, 13, 5,
    34, 36, 28,
};
// Opt : Q W en haut à gauche, T en haut à droite, D au centre, V B en bas à droite.
static const uint8_t shape_leds_opt[] = {
    17, 12, 3, 10, 6, 1,
    40, 35, 26, 33, 29, 24,
};

static const rgb_shape_t shape_cmd  = {shape_leds_cmd, sizeof(shape_leds_cmd) / sizeof(shape_leds_cmd[0]), {HSV_CYAN}};
static const rgb_shape_t shape_ctrl = {shape_leds_ctrl, sizeof(shape_leds_ctrl) / sizeof(shape_leds_ctrl[0]), {HSV_GREEN}};
static const rgb_shape_t shape_opt  = {shape_leds_opt, sizeof(shape_leds_opt) / sizeof(shape_leds_opt[0]), {HSV_MAGENTA}};

static void paint_shape(const rgb_shape_t *shape, uint8_t led_min, uint8_t led_max) {
    rgb_t rgb = rgb_matrix_hsv_to_rgb(shape->hsv);
    for (uint8_t i = 0; i < shape->count; i++) {
        uint8_t led = shape->leds[i];
        if (led >= led_min && led < led_max) {
            rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
        }
    }
}

// Croix fine (Hyper = Shift+Cmd+Ctrl+Opt) : chaque touche reprend la couleur
// du mod qu'elle symbolise (S=Shift, E=Cmd, F=Ctrl, C=Opt) ; D (le centre)
// n'appartient à aucun des 4 mods et reste dans une couleur à part.
typedef struct {
    uint8_t led;
    hsv_t   hsv;
} rgb_shape_point_t;

static const rgb_shape_point_t shape_points_hyper[] = {
    {13, {HSV_RED}},     // S = Shift
    {11, {HSV_CYAN}},    // E = Cmd
    {5, {HSV_GREEN}},    // F = Ctrl
    {9, {HSV_MAGENTA}},  // C = Opt
    {10, {HSV_WHITE}},   // D = à part
    {36, {HSV_RED}},     // S = Shift
    {34, {HSV_CYAN}},    // E = Cmd
    {28, {HSV_GREEN}},   // F = Ctrl
    {32, {HSV_MAGENTA}}, // C = Opt
    {33, {HSV_WHITE}},   // D = à part
};

static void paint_hyper(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = 0; i < sizeof(shape_points_hyper) / sizeof(shape_points_hyper[0]); i++) {
        uint8_t led = shape_points_hyper[i].led;
        if (led >= led_min && led < led_max) {
            rgb_t rgb = rgb_matrix_hsv_to_rgb(shape_points_hyper[i].hsv);
            rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
        }
    }
}

// Superpose les formes actives ; en cas de chevauchement, l'ordre
// cmd -> ctrl -> opt fait gagner la dernière dessinée. Hyper est exclusif
// des 3 autres (voir active_shape_mask() dans keymap.c) donc ne se combine
// jamais avec elles.
static bool render_shapes(effect_params_t *params, uint8_t mask) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }
    if (mask & SHAPE_MASK_CMD) paint_shape(&shape_cmd, led_min, led_max);
    if (mask & SHAPE_MASK_CTRL) paint_shape(&shape_ctrl, led_min, led_max);
    if (mask & SHAPE_MASK_OPT) paint_shape(&shape_opt, led_min, led_max);
    return rgb_matrix_check_finished_leds(led_max);
}

static bool SHAPE_HYPER(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }
    paint_hyper(led_min, led_max);
    return rgb_matrix_check_finished_leds(led_max);
}

#define SHAPE_EFFECT(name, mask) \
    static bool name(effect_params_t *params) { return render_shapes(params, (mask)); }

SHAPE_EFFECT(SHAPE_CMD, SHAPE_MASK_CMD)
SHAPE_EFFECT(SHAPE_CTRL, SHAPE_MASK_CTRL)
SHAPE_EFFECT(SHAPE_OPT, SHAPE_MASK_OPT)
SHAPE_EFFECT(SHAPE_CMD_CTRL, SHAPE_MASK_CMD | SHAPE_MASK_CTRL)
SHAPE_EFFECT(SHAPE_CMD_OPT, SHAPE_MASK_CMD | SHAPE_MASK_OPT)
SHAPE_EFFECT(SHAPE_CTRL_OPT, SHAPE_MASK_CTRL | SHAPE_MASK_OPT)
SHAPE_EFFECT(SHAPE_CMD_CTRL_OPT, SHAPE_MASK_CMD | SHAPE_MASK_CTRL | SHAPE_MASK_OPT)

#undef SHAPE_EFFECT
