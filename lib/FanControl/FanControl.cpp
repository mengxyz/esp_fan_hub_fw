#include "FanControl.h"

FanControl *FanControl::instance = nullptr;

const uint8_t FAN_PWM_CHANNELS[] = {PWM_CH_1, PWM_CH_2, PWM_CH_3, PWM_CH_4, PWM_CH_5};

void IRAM_ATTR FanControl::TAC_1_ISR()
{
  if (instance)
  {
    instance->pwm_freqs[0]++;
  }
}

void IRAM_ATTR FanControl::TAC_2_ISR()
{
  if (instance)
  {
    instance->pwm_freqs[1]++;
  }
}

void IRAM_ATTR FanControl::TAC_3_ISR()
{
  if (instance)
  {
    instance->pwm_freqs[2]++;
  }
}

void IRAM_ATTR FanControl::TAC_4_ISR()
{
  if (instance)
  {
    instance->pwm_freqs[3]++;
  }
}

void IRAM_ATTR FanControl::TAC_5_ISR()
{
  if (instance)
  {
    instance->pwm_freqs[4]++;
  }
}

void IRAM_ATTR FanControl::BTN_UTIL_1_ISR()
{
  if (instance)
  {
    instance->setAllDuty(instance->getDuty(0) + 10);
  }
}

void IRAM_ATTR FanControl::BTN_UTIL_2_ISR()
{
  if (instance)
  {
    instance->setAllDuty(instance->getDuty(0) - 10);
  }
}

void FanControl::initPwmGenerator()
{
  ledcSetup(PWM_CH_1, pwm_freq, pwm_res);
  ledcSetup(PWM_CH_2, pwm_freq, pwm_res);
  ledcSetup(PWM_CH_3, pwm_freq, pwm_res);
  ledcSetup(PWM_CH_4, pwm_freq, pwm_res);
  ledcSetup(PWM_CH_5, pwm_freq, pwm_res);

  ledcAttachPin(PIN_PWM_1, PWM_CH_1);
  ledcAttachPin(PIN_PWM_2, PWM_CH_2);
  ledcAttachPin(PIN_PWM_3, PWM_CH_3);
  ledcAttachPin(PIN_PWM_4, PWM_CH_4);
  ledcAttachPin(PIN_PWM_5, PWM_CH_4);
}

void FanControl::initTachometer()
{
  pinMode(PIN_TAC_1, INPUT_PULLUP);
  pinMode(PIN_TAC_2, INPUT_PULLUP);
  pinMode(PIN_TAC_3, INPUT_PULLUP);
  pinMode(PIN_TAC_4, INPUT_PULLUP);
  pinMode(PIN_TAC_5, INPUT_PULLUP);

  attachInterrupt(PIN_TAC_1, TAC_1_ISR, FALLING);
  attachInterrupt(PIN_TAC_2, TAC_2_ISR, FALLING);
  attachInterrupt(PIN_TAC_3, TAC_3_ISR, FALLING);
  attachInterrupt(PIN_TAC_4, TAC_4_ISR, FALLING);
  attachInterrupt(PIN_TAC_5, TAC_5_ISR, FALLING);
}

FanControl::FanControl(int pwm_freq, int pwm_res)
{
  this->pwm_freq = pwm_freq;
  this->pwm_res = pwm_res;
  instance = this;
}

void FanControl::begin()
{
  initTachometer();
  initPwmGenerator();
  Serial.println("FanControl ready");
}

void FanControl::beginBtnUtils()
{
  pinMode(BTN_UTIL_PIN_1, INPUT_PULLUP);
  pinMode(BTN_UTIL_PIN_2, INPUT_PULLUP);
  attachInterrupt(BTN_UTIL_PIN_1, BTN_UTIL_1_ISR, FALLING);
  attachInterrupt(BTN_UTIL_PIN_2, BTN_UTIL_2_ISR, FALLING);
}

void FanControl::setAllDuty(uint8_t duty)
{
  if (duty > 255 || duty < 0)
  {
    return;
  }
  for (int i = 0; i < 5; i++)
  {
    ledcWrite(FAN_PWM_CHANNELS[i], duty);
  }
}

uint32_t FanControl::getDuty(uint8_t ch)
{
  return ledcRead(FAN_PWM_CHANNELS[ch]);
}

void FanControl::initAllDuty(uint8_t (&duty)[5])
{
  for (int i = 0; i < 5; i++)
  {
    ledcWrite(FAN_PWM_CHANNELS[i], duty[i]);
  }
}

bool FanControl::setDuty(uint8_t ch, uint8_t duty)
{
  if (duty > 255 || duty < 0)
  {
    return false;
  }
  ledcWrite(FAN_PWM_CHANNELS[ch], duty);
  return true;
}

void FanControl::readFanData(FanData *fanData)
{
  for (int i = 0; i < 5; i++)
  {
    fanData->freq[i] = pwm_freqs[i];
  }
}

void FanControl::resetFreqs()
{
  for (int i = 0; i < 5; i++)
  {
    pwm_freqs[i] = 0;
  }
}
