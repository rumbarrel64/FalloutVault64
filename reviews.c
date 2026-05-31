#include "libs.h"
#include "gameState.h"
#include "reviews.h"
#include "gameaudio.h"

static video_t *video = NULL;
static yuv_blitter_t yuvBlitter;
static bool review_init = false;
static bool playing_video = false;
static int selected_review = 0; // 0 = Review 1, 1 = Review 2

void review_loop() {
    
    joypad_poll();
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if (!playing_video) {
        // ===== MENU MODE =====
        
        // Handle left/right navigation
        if (inputs.stick_x > 40 || btn.d_right) {
            selected_review = 1; // Move to Review 2
        } else if (inputs.stick_x < -40 || btn.d_left) {
            selected_review = 0; // Move to Review 1
        }

        // Handle A button to play selected video
        if (btn.a) {
            playing_video = true;
            review_init = false;
        }

        // Handle B or START to exit back to menu
        if (btn.start || btn.b) {
            state = STATE_MENU;
            return;
        }

        // ===== DRAW MENU =====
        surface_t *disp = display_get();
        rdpq_attach(disp, NULL);
        rdpq_clear(RGBA32(0, 0, 0, 255));

        // Draw review options
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 80, 100, "Review 1");
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 200, 100, "Review 2");

        // Draw selection indicator (>)
        if (selected_review == 0) {
            rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 60, 100, ">");
        } else {
            rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 180, 100, ">");
        }

        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 60, 140, "Press A to play");
        rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 60, 160, "Press B to exit");

        rdpq_detach_show();
        return;
    }

    // ===== VIDEO PLAYBACK MODE =====
    
    if (!review_init) {
        // 1. Setup Video Codec and File
        video_register_codec(&mpeg1_codec);
        
        // Load the appropriate video and audio files
        const char *video_file = (selected_review == 0) ? "rom:/review_1.m1v" : "rom:/review_2.m1v";
        const char *audio_file = (selected_review == 0) ? "rom:/review_1.wav64" : "rom:/review_2.wav64";
        
        video = video_open(video_file, NULL);
        video_info_t vinfo = video_get_info(video);

        // 2. Setup Blitter (Width/Height of video -> Width/Height of screen)
        yuvBlitter = yuv_blitter_new_fmv(
            vinfo.width, vinfo.height,
            320, 240, NULL
        );

        // 3. Load and play audio
        music_load(audio_file);
        music_play();


        review_init = true;
    }

    // 3. Update Video Frame
    if (!video_next_frame(video)) {
        goto exit_video;
    }

    audio_update();

    // 4. Input Handling (Skip video with START or B)
    if (btn.start || btn.b) {
        goto exit_video;
    }

    // 5. Drawing to Screen
    surface_t *disp = display_get();
    rdpq_attach(disp, NULL);

    yuv_frame_t frame = video_get_frame(video);
    yuv_blitter_run(&yuvBlitter, &frame);

    rdpq_detach_show();
    return;

    exit_video:
        // Cleanup video
        if (video) {
            video_close(video);
            video = NULL;
        }
        yuv_blitter_free(&yuvBlitter);
        
        // Cleanup audio
        music_cleanup();
        
        review_init = false;
        playing_video = false;
}