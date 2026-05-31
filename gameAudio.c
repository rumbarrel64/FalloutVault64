// gameAudio.c
#include "gameaudio.h"
#include <audio.h>
#include <mixer.h>

// Mixer channel allocation (More Channels, More Audio!!)
// Max channles 16
// Stereo files use 2 consecutive channels (left + right) 
#define CHANNEL_MUSIC 0  // Load music in and out into to this channel. Uses channels 0 (left) and 1 (right)
#define CHANNEL_SFX1 2   // Load sound effect for Fight (maybe others later). Uses channels 2 (left) and 3 (right) 
#define CHANNEL_SFX2 4   // Additional channel for future sounds. Uses channels 4 (left) and 5 (right)

// Music
static wav64_t music_wav; // Game Music

void audio_system_init(void) {
    // Initialize audio system
    audio_init(22050, 4);           // 22.05 kHz, 4 buffers
    mixer_init(16);                 // 16 mixer channels
    wav64_init_compression(1);      // compression level 1
    mixer_ch_set_limits(CHANNEL_MUSIC, 0, 48000, 0);
}

// ========== LOADING FUNCTIONS (prepare audio, don't play yet) ==========
void music_load(const char *filename) {
    wav64_open(&music_wav, filename);
    wav64_set_loop(&music_wav, true);
}

// ========== PLAYING FUNCTIONS (actually start the sound) ==========
void music_play(void) {
    wav64_play(&music_wav, CHANNEL_MUSIC);
}

// ========== AUDIO PROCESSING (handles ALL audio - music + SFX) ==========
void audio_update(void) {

    mixer_try_play();
}

// ========== STOPPING FUNCTIONS ==========
void music_stop(void) {
    mixer_ch_stop(CHANNEL_MUSIC);
}

// ========== CLEANUP FUNCTIONS ==========
void music_cleanup(void) {
    mixer_ch_stop(CHANNEL_MUSIC);
    wav64_close(&music_wav);
}

void audio_cleanup_all(void) {
    music_cleanup();
}