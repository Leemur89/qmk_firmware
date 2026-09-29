/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

// Layer 0 utilise une heatmap persistante (rgb_matrix_heatmap.c, variante de
// la "typing heatmap" QMK qui survit aux changements de layer ; forcée au
// boot dans keymap.c car RGB_MATRIX_DEFAULT_MODE ne s'applique qu'à une
// EEPROM vierge)
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CUSTOM_PERSISTENT_HEATMAP

// NOTE HISTORIQUE : synchroniser layer_state vers la moitié droite (pour les
// couleurs par layer) avait cassé le scan clavier côté droit à 5 reprises,
// jusqu'à isoler que le simple appel à transaction_register_rpc() suffit
// (même sans jamais envoyer de donnée) — voir la PR pour le détail des
// essais. La solution retenue évite d'ajouter une transaction split_common :
// keymap.c pilote directement rgb_matrix_config (mode + HSV) sur le master
// depuis layer_state_set_user(), et laisse le RPC RGB_MATRIX_SPLIT déjà
// intégré à QMK (activé via rgb_matrix.split_count dans keyboard.json)
// propager ça vers l'esclave — aucune transaction supplémentaire n'est donc
// ajoutée au split_common.

// Réglages de la heatmap (lus par rgb_matrix_heatmap.c ; les couleurs sont
// dans la palette heat_palette[] de ce fichier).
//
// Chauffe : chaleur ajoutée à la touche appuyée (INCREASE_STEP, défaut QMK
// 32) et au maximum à ses voisines (AREA_LIMIT, défaut QMK 16). Divisés par 2
// ici : il faut deux fois plus d'appuis pour passer d'une couleur à la
// suivante (saturation à 16 appuis au lieu de 8).
#define RGB_MATRIX_TYPING_HEATMAP_INCREASE_STEP 16
#define RGB_MATRIX_TYPING_HEATMAP_AREA_LIMIT 8

// Refroidissement : chaque touche perd 1 point de chaleur toutes les N ms
// (défaut QMK : 25 ms). À 200 ms, une zone saturée (255) met ~50 s à
// refroidir, et un appui isolé (16) reste visible ~3 s.
#define RGB_MATRIX_TYPING_HEATMAP_DECREASE_DELAY_MS 200
