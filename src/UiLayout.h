#pragma once

#include "DisplayConfig.h"

static constexpr int UI_SCREEN_W = SCREEN_WIDTH;
static constexpr int UI_SCREEN_H = SCREEN_HEIGHT;
static constexpr int UI_HEADER_H = (SCREEN_HEIGHT >= 272) ? 48 : 40;
static constexpr int UI_KEYBOARD_H = (SCREEN_HEIGHT >= 272) ? 140 : 120;
static constexpr int UI_PAD = (SCREEN_WIDTH >= 480) ? 12 : 8;
