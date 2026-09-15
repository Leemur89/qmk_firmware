/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

// Layer 0 utilise l'effet "typing heatmap" (typing_heatmap est déjà activé
// par info.json ; forcé au boot dans keymap.c car RGB_MATRIX_DEFAULT_MODE
// ne s'applique qu'à une EEPROM vierge)
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_TYPING_HEATMAP

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
