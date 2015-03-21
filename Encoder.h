#ifndef _ENCODERS_H_
#define _ENCODERS_H_
  #include <avr/io.h>

  #define ENC_1A (1<<4)
  #define ENC_1B (1<<6)
  #define ENC_2A (1<<0)
  #define ENC_2B (1<<1)

  uint8_t encoder_1;
  uint8_t encoder_2;
  uint8_t encoder_tmp;
  static const uint8_t encoder_states_1 [] = {0, ENC_1A, ENC_1A | ENC_1B, ENC_1B};
  static const uint8_t encoder_states_2 [] = {0, ENC_2A, ENC_2A | ENC_2B, ENC_2B};

  uint8_t Encoder1Status_LCL;
  uint8_t Encoder2Status_LCL;

  void Encoders_Init(void);
  void Encoders_Disable(void);
  uint8_t Encoder_1_GetStatus(void);
  uint8_t Encoder_2_GetStatus(void);
#endif
