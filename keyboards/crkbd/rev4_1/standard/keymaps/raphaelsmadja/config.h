/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

// Layer 0 utilise l'effet "typing heatmap" (typing_heatmap est déjà activé
// par info.json ; forcé au boot dans keymap.c car RGB_MATRIX_DEFAULT_MODE
// ne s'applique qu'à une EEPROM vierge)
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_TYPING_HEATMAP

// Essai n°3 pour synchroniser le layer actif vers la moitié droite (esclave).
//
// SPLIT_LAYER_STATE_ENABLE (essais 1 et 2, y compris avec un watchdog
// rallongé à 15s) a cassé la sync de la matrice de touches côté droit à
// chaque fois (LEDs OK, plus aucune touche ne remonte) : ce mécanisme
// tourne dans la chaîne de transactions périodique (transactions_master()),
// qui semble déjà tourner à la limite sur ce clavier. On utilise à la place
// un appel RPC "custom data sync" explicite et limité en fréquence (voir
// keymap.c), hors de cette chaîne critique, pour rester sans effet sur le
// scan clavier même si le lien série est juste.
#define SPLIT_TRANSACTION_IDS_USER RPC_LAYER_SYNC
