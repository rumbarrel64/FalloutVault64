// General file to hold all libraries
#include "libs.h"
#include "menu.h"
#include "gameState.h"
#include "gameaudio.h"

static bool menu_init = false;
#define JOY_THRESHOLD 32  // Adjust this if needed

void menu_loop() {   

        // Get memory usage
        heap_stats_t heap_stats;
        sys_get_heap_stats(&heap_stats);

        // Load and Start Menu Music
        if (!menu_init) {
            music_load("rom:/menuMusicShort.wav64");
            music_play(); // Start playing immediately
            menu_init = true;
        };

        static int selected_option = 0;
        
        // Lock display and clear screen
        surface_t *disp = display_get();

        // 1. Calculate Score
        int final_score = 0;
        if (last_player_health > 0) {
            // Basic formula: Health is weighted heavily, Time reduces score
            final_score = (last_player_health * 500) - (int)(last_tutorial_time * 5);
            if (final_score < 0) final_score = 0;
        };

        // Drawing
        graphics_fill_screen(disp, graphics_make_color(0, 0, 50, 255));
        graphics_set_color(graphics_make_color(255, 255, 0, 255), 0);

        graphics_draw_text(disp, 80, 40, "FALLOUT: VAULT 64");
        graphics_draw_text(disp, 100, 100, (selected_option == 0) ? "> START" : "  START");
        graphics_draw_text(disp, 100, 120, (selected_option == 1) ? "> TUTORIAL" : "  TUTORIAL");
        graphics_draw_text(disp, 100, 140, (selected_option == 2) ? "> REVIEWS" : "  REVIEWS");
        char score_text[32];
        snprintf(score_text, sizeof(score_text), "LAST SCORE: %d", final_score);
        graphics_draw_text(disp, 80, 175, score_text);


        // Display Memory
        char mem_text[32];
        snprintf(mem_text, sizeof(mem_text), "Mem: %d KiB", heap_stats.used / 1024);
        graphics_draw_text(disp, 10, 220, mem_text);

        // Display
        display_show(disp);

        // Audio Update (handles all audio processing)
        audio_update();

        // Input Handling
        joypad_poll();
        joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_inputs_t joypad = joypad_get_inputs(JOYPAD_PORT_1);

        // Navagation
        if (btn.d_up || joypad.stick_y > JOY_THRESHOLD) {
            if (selected_option == 1)      selected_option = 0; // Move from Tutorial to Start
            else if (selected_option == 2) selected_option = 1; // Move from Reviews to Tutorial
        }

        if (btn.d_down || joypad.stick_y < -JOY_THRESHOLD) {
            if (selected_option == 0)      selected_option = 1; // Move from Start to Tutorial
            else if (selected_option == 1) selected_option = 2; // Move from Tutorial to Reviews
        }
        
        // Selection Logic (Checks for A OR Start)
        if (btn.a || btn.start) {
            // Free Assests to prevent memory leaks
            audio_cleanup_all();  // Updated from music_stop() - now stops and unloads

            menu_init = false;

            if (selected_option == 0) {
                state = STATE_GAME;      // Jump straight to game
                return;
            } 
            else if (selected_option == 1) {
                state = STATE_TUTORIAL;  // Jump to your new tutorial screen
                return;
            }
            else if (selected_option == 2) {
                state = STATE_REVIEWS; // This triggers your new video loop
                return;
    }
        }

};