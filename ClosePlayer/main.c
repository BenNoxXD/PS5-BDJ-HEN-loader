/*
 * Copyright (c) 2025 BenNox_XD
 *
 * This file is part of PS5-BDJ-HEN-loader and is licensed under the MIT License.
 * See the LICENSE file in the root of the project for full license information.
 */
#include <stdio.h>

#include "../HENloader_C_part/src/kill_disc_player.h"

const char *target_process = "SceDiscPlayer";
const char *target_title_id = "NPXS40140";
const char *target_friendly_name = "Disc Player";

int main(void) {
  return kill_disc_player(target_process, target_title_id, target_friendly_name);
}
