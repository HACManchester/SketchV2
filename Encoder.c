#include "Encoder.h"

void Encoders_Init(void)
{
  DDRD &= ~(ENC_1A | ENC_1B | ENC_2A | ENC_2B);
  encoder_1 = 0;
  encoder_2 = 0;
}

void Encoders_Disable(void)
{
}

uint8_t Encoder_1_GetStatus(void)
{
  uint8_t ret = 0;
  encoder_tmp = (PIND & (ENC_1A | ENC_1B)) ^ (ENC_1A | ENC_1B);
  if (encoder_tmp != encoder_1)
  {
    if (encoder_1 == encoder_states_1[0])
    {
      if (encoder_tmp == encoder_states_1[3])
        ret = -1;
      else if (encoder_tmp == encoder_states_1[1])
        ret = 1;
    }
    else if (encoder_1 == encoder_states_1[1])
    {
      if (encoder_tmp == encoder_states_1[0])
        ret = -1;
      else if (encoder_tmp == encoder_states_1[2])
        ret = 1;
    }
    else if (encoder_1 == encoder_states_1[2])
    {
      if (encoder_tmp == encoder_states_1[1])
        ret = -1;
      else if (encoder_tmp == encoder_states_1[3])
        ret = 1;
    }
    else if (encoder_1 == encoder_states_1[3])
    {
      if (encoder_tmp == encoder_states_1[2])
        ret = -1;
      else if (encoder_tmp == encoder_states_1[0])
        ret = 1;
    }
    encoder_1 = encoder_tmp;
  }
  return ret;
}

uint8_t Encoder_2_GetStatus(void)
{
  uint8_t ret = 0;
  encoder_tmp = (PIND & (ENC_2A | ENC_2B)) ^ (ENC_2A | ENC_2B);
  if (encoder_tmp != encoder_2)
  {
    if (encoder_2 == encoder_states_2[0])
    {
      if (encoder_tmp == encoder_states_2[3])
        ret = -1;
      else if (encoder_tmp == encoder_states_2[1])
        ret = 1;
    }
    else if (encoder_2 == encoder_states_2[1])
    {
      if (encoder_tmp == encoder_states_2[0])
        ret = -1;
      else if (encoder_tmp == encoder_states_2[2])
        ret = 1;
    }
    else if (encoder_2 == encoder_states_2[2])
    {
      if (encoder_tmp == encoder_states_2[1])
        ret = -1;
      else if (encoder_tmp == encoder_states_2[3])
        ret = 1;
    }
    else if (encoder_2 == encoder_states_2[3])
    {
      if (encoder_tmp == encoder_states_2[2])
        ret = -1;
      else if (encoder_tmp == encoder_states_2[0])
        ret = 1;
    }
    encoder_2 = encoder_tmp;
  }
  return ret;
}
