/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

// Layer 0 utilise l'effet "typing heatmap" (typing_heatmap est déjà activé
// par info.json ; forcé au boot dans keymap.c car RGB_MATRIX_DEFAULT_MODE
// ne s'applique qu'à une EEPROM vierge)
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_TYPING_HEATMAP

// TEST DIAGNOSTIC (pas un fix) : déclare un transaction ID RPC custom sans
// jamais l'enregistrer/l'utiliser (keymap.c est identique à la version qui
// fonctionne). But : voir si le simple fait d'agrandir la table de
// transactions split (via SPLIT_TRANSACTION_RPC, déclenché par cette
// define) suffit à casser le scan clavier côté droit sur ce clavier, même
// sans jamais s'en servir — ce qui écarterait toute question de fréquence/
// volume de sync et pointerait vers une limite plus fondamentale du lien
// série de cette carte.
//
// Essais précédents, tous cassé le clavier droit à chaque fois :
// 1) SPLIT_LAYER_STATE_ENABLE seul
// 2) idem + SPLIT_WATCHDOG_TIMEOUT 15000 (LEDs OK, heatmap visible, mais
//    plus aucune touche ne remonte)
// 3) sync via transaction_rpc_send()/register_rpc() explicite et limité à
//    500ms, hors de la chaîne périodique transactions_master() (toujours
//    cassé)
#define SPLIT_TRANSACTION_IDS_USER RPC_LAYER_SYNC
