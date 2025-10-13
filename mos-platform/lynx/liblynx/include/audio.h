// Copyright 2025 LLVM-MOS Project
// Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
// See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
// information.

// Portions originally from cc65.

#ifndef _AUDIO_H
#define _AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Sound support */
void lynx_audio_init(); /* Initialize the sound driver */
void lynx_audio_pause (); /* Pause sound */
void lynx_audio_continue(); /* Continue sound after pause */
void lynx_audio_play(unsigned char channel, const unsigned char *music); /* Play tune on channel */
void lynx_audio_stop(); /* Stop sound on all channels */
void  lynx_audio_stop_channel(unsigned char channel); /* Stop sound on all channels */
unsigned char lynx_audio_active(); /* Show which channels are active */

#ifdef __cplusplus
}
#endif

#endif
