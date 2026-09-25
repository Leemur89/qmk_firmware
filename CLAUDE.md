# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repo context

This is a fork of QMK Firmware (github.com/qmk/qmk_firmware). It is used to build custom firmware for a **Corne V4.1** split keyboard (`keyboards/crkbd/rev4_1/standard`), a 3x6 PCB (36 keys + 3 thumb keys per side) on an RP2040 microcontroller. The PCB supports 2 extra keys per half (`LAYOUT_split_3x6_3_ex2`) but they are **not wired** on this build — the keymap uses the plain `LAYOUT` (aliased to `LAYOUT_split_3x6_3`), which excludes them.

- Primary development branch: `raph`
- `master` tracks the upstream/official QMK repo — don't develop directly on it, only use it as a merge/rebase base when syncing upstream changes.
- Nearly all custom work lives under `keyboards/crkbd/rev4_1/standard/keymaps/raphaelsmadja/` (`keymap.c`, `rules.mk`). Treat this as the actual "project" directory; the rest of the tree is upstream QMK vendored code that should rarely need edits.

**Pull requests always target this fork, never upstream.** This repo (`Leemur89/qmk_firmware`) is a fork of `qmk/qmk_firmware`, and every PR for work here must have base owner/repo `Leemur89/qmk_firmware` and base branch `raph` — never `qmk/qmk_firmware`. GitHub's own "Compare & pull request" flow (including the `pull/new/<branch>` link `git push` prints) defaults the base repo to the upstream parent for a fork, not the fork itself, so this has to be set explicitly every time, not assumed from a printed link or a UI default. When creating a PR via the GitHub MCP tools/API, always pass `owner: "Leemur89"`, `repo: "qmk_firmware"`, `base: "raph"` explicitly rather than relying on any default. If a PR ever does land against `qmk/qmk_firmware` by mistake, close it and re-open against `Leemur89/qmk_firmware:raph` instead of trying to redirect it (a PR's base repo can't be changed after creation).

## Key naming convention

When discussing this keymap, keys are referred to by these names instead of raw matrix/column coordinates. Each half has 6 columns x 3 rows plus 3 thumb keys; the naming splits the 6 columns into a 5-wide "querty" block and a 1-wide "pinky" column per side, plus the 3-key "thumb" cluster.

**Querty keys** — the standard QWERTY letters and punctuation (`;` `,` `.` `/`), i.e. left columns 1-5 and right columns 0-4 of each row:
- Row 0: `Q W E R T` / `Y U I O P`
- Row 1 (home row, has the F/J home-row shift mods): `A S D F G` / `H J K L ;`
- Row 2: `Z X C V B` / `N M , . /`

**Pinky keys** — the outer column of each half (left column 0, right column 5), 3 per side named by row. Each also has a 3-letter acronym (Pinky/Left-Right/Up-Middle-Down):
| | Gauche (left) | Droite (right) |
|---|---|---|
| **Haut** (top) | `PLU` = `LT(4, KC_TAB)` — Tab tap / layer 4 hold; on layer 1, `PLU` is overridden to `QK_BOOT` (bootloader entry) | `PRU` — **physically removed**, `KC_NO` on every layer (Enter and Escape are combos, the layer 4 lock moved to `TLM`) |
| **Milieu** (home row) | `PLM` — `KC_NO` on layers 0/1/2/4 (Hyper moved to the `TLM`+`G` / `TRM`+`H` combos); only `RM_TOGG` on layer 3 | `PRM` = `KC_QUOT` — the `'` key |
| **Bas** (bottom) | `PLD` = `KC_LSFT` | `PRD` = `KC_RSFT` |

**Thumb keys** — the 3-key mod cluster per side, named by distance from the keyboard's center gap. Each also has a 3-letter acronym (Thumb/Left-Right/Inner-Middle-Outer):
| | Gauche (left) | Droit (right) |
|---|---|---|
| **Extérieur** (outermost, away from center) | `TLO` — **physically removed**, `KC_NO` on every layer | `TRO` = `KC_BSPC` — Backspace, no hold function; on layer 1, `TRO` is `LALT(KC_BSPC)` (word delete) |
| **Milieu** | `TLM` = `MO(1)` — layer 1 hold, no tap function; `TLM`+`B` combo = Enter, `TLM`+`G` combo = Hyper; on layer 4, `TLM` is `QK_LLCK` (layer lock) | `TRM` = `LT(2, KC_SPC)` — Space tap / layer 2 hold; `TRM`+`P` combo = Escape, `TRM`+`H` combo = Hyper |
| **Intérieur** (innermost, next to center) | `TLI` — **physically removed**, `KC_NO` on every layer | `TRI` — **physically removed**, `KC_NO` on every layer |

These names describe layer 0 (the base layer); the same position names apply on other layers even when the keycode there differs (e.g. "pinky droite haut" is `QK_BOOT` on layer 3, `KC_ESC` on layer 0).

## Build / compile

The `qmk` CLI (Python) and the `arm-none-eabi-gcc` toolchain are usually not preinstalled in a fresh environment; `make` shells out to `qmk` for a preflight check (`qmk hello`) and fails immediately without it. To set both up:

```sh
# qmk CLI: requirements.txt's own deps don't provide the `qmk` command itself
# (that's the separate `qmk` PyPI package), and a modern setuptools breaks
# building the `halo` dependency's wheel — pin setuptools first.
pip install --break-system-packages "setuptools<72" qmk

# ARM cross-compiler toolchain, needed to actually compile for the RP2040
apt-get install -y gcc-arm-none-eabi
```

Once both are on `PATH`, the top-level `Makefile` works directly:

```sh
# Build the firmware for this keymap
make crkbd/rev4_1/standard:raphaelsmadja

# Verbose build (see full compiler output)
make crkbd/rev4_1/standard:raphaelsmadja VERBOSE=true

# Clean build artifacts
make clean
```

The output is a `.uf2` file (RP2040 bootloader format) at the repo root, named like `crkbd_rev4_1_standard_raphaelsmadja.uf2`. Flashing requires physical access to the keyboard (put it in bootloader mode, then copy the `.uf2` to the mass-storage device) — this can't be done from this environment.

There is no build step needed to just edit the keymap; only run `make` to verify the firmware compiles after changes to `keymap.c` or `rules.mk`.

## Tests

Core QMK has its own unit test suite (C++ tests under `tests/`, plus Python tests under `lib/python/qmk/tests` run via `nose2`/`nose2.cfg`). These test core QMK behavior (tap-dance, tap-hold, combos, etc.), not this specific keymap — there is no dedicated test target for a single keymap's `keymap.c`. Compiling successfully (`make crkbd/rev4_1/standard:raphaelsmadja`) is the practical correctness check for changes in this repo.

If a change touches shared/core QMK code (rare — normally out of scope here), run the relevant test target, e.g.:

```sh
make test:tap_dance
```

## Keymap architecture (`keyboards/crkbd/rev4_1/standard/keymaps/raphaelsmadja/`)

- `keymap.c` defines a 5-layer `keymaps[]` array using the `LAYOUT()` macro (36 main keys + 6 thumb keys, split 18/18 + 3/3):
  - Layer 0: base QWERTY layer. Mods (Alt/Ctrl/Cmd/Shift on the home row, Hyper on `G`/`H`) are all combos with `TLM`/`TRM`; `PLM` is empty. `TRM` is `LT(2, KC_SPC)` (Space tap / layer 2 hold), Enter is the `TLM`+`B` combo and Escape the `TRM`+`P` combo (`TLO`/`TLI`/`TRI`/`PRU` are physically removed).
  - Layer 1: symbols/numbers (accessed by holding `TLM`, `MO(1)`, on layer 0; `TLM` has no tap function). `TRM` on this layer is `KC_TRNS` (falls through to `LT(2, KC_SPC)`); `TRO` on this layer is `LALT(KC_BSPC)` (word delete). `PLU` on this layer is `QK_BOOT` (bootloader entry) instead of a symbol.
  - Layer 2: navigation/media/screenshot keys (accessed by holding `TRM`, `LT(2, KC_SPC)`, on layer 0; tapping it types Space).
  - Layer 3: reached automatically when layers 1+2 are both active (tri-layer) — bootloader entry (`QK_BOOT`), RGB matrix controls, and window-management arrow keys.
  - Layer 4: mouse keys layer, held via `PLU` (`LT(4, KC_TAB)`); tapping `TLM` (`QK_LLCK`) while on it locks/unlocks the layer; `TRM` is left click (`MS_BTN1`) and `TRO` is right click (`MS_BTN2`) while on this layer.
- `layer_state_set_user()` wires up the tri-layer behavior (`update_tri_layer_state(state, 1, 2, 3)`), making layer 3 accessible by holding both layer-1 and layer-2 triggers together. It also drives the per-layer RGB color (`rgb_matrix_update_layer_color()`): layer 0 = a custom persistent typing heatmap (`rgb_matrix_heatmap.c`: its heat buffer is fed from `housekeeping_task_user()` on both halves regardless of the displayed mode, so it survives layer/mod-shape color switches instead of being reset like QMK's built-in `TYPING_HEATMAP`), layers 1-4 = solid blue/orange/purple/green via `rgb_matrix_mode_noeeprom()`/`rgb_matrix_sethsv_noeeprom()`. This applies to **both halves** — it piggybacks on QMK's built-in `RGB_MATRIX_SPLIT` sync RPC (auto-enabled by `rgb_matrix.split_count` in `keyboard.json`), which was already syncing `rgb_matrix_config` to the slave, rather than syncing `layer_state` itself. Syncing `layer_state` directly (a new split transaction) was tried repeatedly in the past and broke matrix scanning on the right half every time — don't reintroduce that approach; see the history in `config.h`.
- `rules.mk` enables `MOUSEKEY_ENABLE`, `RGB_MATRIX_ENABLE`, `COMBO_ENABLE` and `LAYER_LOCK_ENABLE` for this keymap specifically (no tap-dance anymore).

Keyboard-level config (matrix pins, RGB matrix LED positions, split/handedness config, available `LAYOUT_*` macros) lives one level up in `keyboards/crkbd/rev4_1/standard/keyboard.json` and `keyboards/crkbd/rev4_1/info.json` — only touch these if changing the physical hardware config (not needed for keymap/layer/behavior changes).

## Contributing back upstream

`.github/copilot-instructions.md` documents QMK's own PR review checklist (branch targeting, `info.json` requirements, license headers, keymap conventions, etc.). It's only relevant if opening a PR against the official `qmk/qmk_firmware` repo — not needed for personal changes on `raph`.
