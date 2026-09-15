/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

// Layer 0 utilise l'effet "typing heatmap" (typing_heatmap est déjà activé
// par info.json ; forcé au boot dans keymap.c car RGB_MATRIX_DEFAULT_MODE
// ne s'applique qu'à une EEPROM vierge)
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_TYPING_HEATMAP

// Synchronise le layer actif vers la moitié droite (esclave) pour que
// rgb_matrix_indicators_user() y applique aussi les couleurs par layer.
//
// Essai n°2 : la 1ère tentative rendait la moitié droite injoignable après
// flash. Piste : ce clavier a SPLIT_WATCHDOG_ENABLE actif (config du board),
// avec un timeout par défaut de 3s. transactions_master() abandonne le reste
// de la chaîne (donc le ping watchdog, qui est en tout dernier) dès qu'une
// transaction échoue après 10 essais ; SPLIT_LAYER_STATE_ENABLE ajoute 2
// transactions de plus avant ce ping, augmentant le risque qu'une liaison
// série un peu juste déclenche un reset côté esclave (potentiellement en
// boucle). On teste ici avec un timeout bien plus tolérant pour confirmer
// ou infirmer cette hypothèse avant d'aller plus loin.
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_WATCHDOG_TIMEOUT 15000
