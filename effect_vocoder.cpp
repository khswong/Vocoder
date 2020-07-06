#include <Arduino.h>
#include "effect_vocoder.h"
#include "utility/sqrt_integer.h"
#include "utility/dspinst.h"

static void copy_to_fft_buffer(void *destination, const void *source)
{
  const uint16_t *src = (const uint16_t *)source;
  uint32_t *dst = (uint32_t *)destination;

  for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
    *dst++ = *src++;  // real sample plus a zero for imaginary
  }
}

static void apply_window_to_fft_buffer(void *buffer, const void *window)
{
  int16_t *buf = (int16_t *)buffer;
  const int16_t *win = (int16_t *)window;;

  for (int i=0; i < 1024; i++) {
    int32_t val = *buf * *win++;
    //*buf = signed_saturate_rshift(val, 16, 15);
    *buf = val >> 15;
    buf += 2;
  }

}

void AudioEffectVocoder::update(void)
{
  audio_block_t *block;
  block = receiveReadOnly();
  if (!block) return;

    switch (state) {
  case 0:
    fft_blocklist[0] = block;
    state = 1;
    break;
  case 1:
    fft_blocklist[1] = block;
    state = 2;
    break;
  case 2:
    fft_blocklist[2] = block;
    state = 3;
    break;
  case 3:
    fft_blocklist[3] = block;
    state = 4;
    break;
  case 4:
    fft_blocklist[4] = block;
    state = 5;
    break;
  case 5:
    fft_blocklist[5] = block;
    state = 6;
    break;
  case 6:
    fft_blocklist[6] = block;
    state = 7;
    break;
  case 7:
    fft_blocklist[7] = block;
    // TODO: perhaps distribute the work over multiple update() ??
    //       github pull requsts welcome......
    copy_to_fft_buffer(buffer+0x000, fft_blocklist[0]->data);
    copy_to_fft_buffer(buffer+0x100, fft_blocklist[1]->data);
    copy_to_fft_buffer(buffer+0x200, fft_blocklist[2]->data);
    copy_to_fft_buffer(buffer+0x300, fft_blocklist[3]->data);
    copy_to_fft_buffer(buffer+0x400, fft_blocklist[4]->data);
    copy_to_fft_buffer(buffer+0x500, fft_blocklist[5]->data);
    copy_to_fft_buffer(buffer+0x600, fft_blocklist[6]->data);
    copy_to_fft_buffer(buffer+0x700, fft_blocklist[7]->data);
    if (window) apply_window_to_fft_buffer(buffer, window);
    arm_cfft_radix4_q15(&fft_inst, buffer);
   
    arm_cfft_radix4_q15(&ifft_inst, buffer);
    copy_to_fft_buffer(ifft_blocklist[0]->data, buffer+0x000);
    copy_to_fft_buffer(ifft_blocklist[1]->data, buffer+0x100);
    copy_to_fft_buffer(ifft_blocklist[2]->data, buffer+0x200);
    copy_to_fft_buffer(ifft_blocklist[3]->data, buffer+0x300);
    copy_to_fft_buffer(ifft_blocklist[4]->data, buffer+0x400);
    copy_to_fft_buffer(ifft_blocklist[5]->data, buffer+0x500);
    copy_to_fft_buffer(ifft_blocklist[6]->data, buffer+0x600);
    copy_to_fft_buffer(ifft_blocklist[7]->data, buffer+0x700);
    transmit(ifft_blocklist[0]);
    transmit(ifft_blocklist[1]);
    transmit(ifft_blocklist[2]);
    transmit(ifft_blocklist[3]);
    release(fft_blocklist[0]);
    release(fft_blocklist[1]);
    release(fft_blocklist[2]);
    release(fft_blocklist[3]);
    fft_blocklist[0] = fft_blocklist[4];
    fft_blocklist[1] = fft_blocklist[5];
    fft_blocklist[2] = fft_blocklist[6];
    fft_blocklist[3] = fft_blocklist[7];
    state = 4;
    break;
  }
}
