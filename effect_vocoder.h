#ifndef effect_vocoder_h_
#define effect_vocoder_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

class AudioEffectVocoder : public AudioStream
{
  public:
    AudioEffectVocoder(void): AudioStream(1, inputQueueArray) {
      arm_cfft_radix4_init_q15(&fft_inst, 1024, 0, 1);
      arm_cfft_radix4_init_q15(&ifft_inst, 1024, 1, 1);
      for (int i = 0; i < 8; ++i){
        ifft_blocklist[i] = allocate();
      }
     };
    ~AudioEffectVocoder(){
        for (int i = 0; i < 8; ++i){
          release(ifft_blocklist[i]);        
        }
      };
    virtual void update(void);
    
    void windowFunction(const int16_t *w) {
      window = w;
    }
  private:
    audio_block_t *inputQueueArray[1];

    const int16_t *window;
    uint8_t state;
    
    audio_block_t *fft_blocklist[8];
    audio_block_t *ifft_blocklist[8];
    int16_t buffer[2048] __attribute__ ((aligned (4)));
    
    arm_cfft_radix4_instance_q15 fft_inst, ifft_inst;
};

#endif
