// gameState.h
#pragma once

typedef enum {
    STATE_MENU,
    STATE_TUTORIAL,
    STATE_GAME,
    STATE_REVIEWS
} GameState;

extern GameState state;

// For Score Keeping
extern float last_tutorial_time;
extern int last_player_health;