#include "FanControl.h"

FanControl *FanControl::instance = nullptr;

const uint8_t FAN_PWM_CHANNELS[] = {PWM_CH_1, PWM_CH_2, PWM_CH_3, PWM_CH_4, PWM_CH_5};
const pcnt_unit_t PCNT_UNITS[] = {PCNT_UNIT_0, PCNT_UNIT_1, PCNT_UNIT_2, PCNT_UNIT_3};

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
  if (!instance)
    return;
  unsigned long currentTime = millis();
  if (currentTime - instance->last_pulse_times[4] >= FAN_FILTER_TIME)
  {
    instance->pwm_freqs[4]++;
  }
  instance->last_pulse_times[4] = currentTime;
}

void IRAM_ATTR FanControl::pulseInterrupt(void *arg)
{
  if (!instance)
    return;
  int fanIndex = (int)arg;
  unsigned long currentTime = millis();
  if (currentTime - instance->last_pulse_times[fanIndex] >= 10)
  {
    instance->pwm_freqs[fanIndex]++;
  }
  instance->last_pulse_times[fanIndex] = currentTime;
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
  // for (int i = 0; i < 5; i++)
  // {
  //   pinMode(TAC_PINS[i], INPUT_PULLUP);
  //   attachInterruptArg(digitalPinToInterrupt(TAC_PINS[i]), pulseInterrupt, (void *)i, FALLING);
  // }

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

void FanControl::initPcnt(uint8_t gpio_pin, pcnt_unit_t pcnt_unit)
{
  pcnt_config_t pcnt = {
      .pulse_gpio_num = gpio_pin,
      .ctrl_gpio_num = PCNT_PIN_NOT_USED,
      .lctrl_mode = PCNT_MODE_KEEP,
      .hctrl_mode = PCNT_MODE_KEEP,
      .pos_mode = PCNT_COUNT_INC,
      .neg_mode = PCNT_COUNT_DIS,
      .counter_h_lim = 0,
      .counter_l_lim = 0,
      .unit = pcnt_unit,
      .channel = PCNT_CHANNEL_0};

  pcnt_unit_config(&pcnt);
  pcnt_set_filter_value(pcnt_unit, 100);
  pcnt_filter_enable(pcnt_unit);
  pcnt_event_enable(pcnt_unit, PCNT_EVT_H_LIM);
  pcnt_counter_pause(pcnt_unit);
  pcnt_counter_clear(pcnt_unit);
  pcnt_counter_resume(pcnt_unit);
}

FanControl::FanControl(int pwm_freq, int pwm_res)
{
  this->pwm_freq = pwm_freq;
  this->pwm_res = pwm_res;
  instance = this;
}
void FanControl::service()
{
  if (!(millis() - lastCalcTime >= 1000))
  {
    return;
  }
  for (int i = 0; i < 5; i++)
  {
    noInterrupts();
    pwm_freq_buffers[i] = pwm_freqs[i];
    pwm_freqs[i] = 0;
    interrupts();
  }
  lastCalcTime = millis();
}

void FanControl::finalize()
{
  for (int i = 0; i < 5; i++)
  {
    noInterrupts();
    pwm_freq_buffers[i] = pwm_freqs[i];
    pwm_freqs[i] = 0;
    interrupts();
  }
}

void FanControl::finalizePcnt()
{
  noInterrupts();
  pwm_freq_buffers[4] = pwm_freqs[4];
  pwm_freqs[4] = 0;
  interrupts();
  
  for (int i = 0; i < 4; i++)
  {
    pcnt_counter_pause(PCNT_UNITS[i]);
    int16_t cache;
    pcnt_get_counter_value(PCNT_UNITS[i], &cache);
    pwm_freq_buffers[i] = cache;
    pcnt_counter_clear(PCNT_UNITS[i]);
  }

  for (int i = 0; i < 4; i++)
  {
    pcnt_counter_resume(PCNT_UNITS[i]);
  }

}

void FanControl::begin()
{
  initPwmGenerator();
  DEBUG_PRINTLN("FanControl ready");
  for (int i = 0; i < 4; i++)
  {
    initPcnt(TAC_PINS[i], PCNT_UNITS[i]);
  }
  pinMode(PIN_TAC_5, INPUT_PULLUP);
  attachInterrupt(PIN_TAC_5, TAC_5_ISR, FALLING);
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
  int32_t duty = ledcRead(FAN_PWM_CHANNELS[ch]);
  if (duty > 255)
  {
    return 255;
  }
  return duty;
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
    fanData->freq[i] = pwm_freq_buffers[i];
    fanData->rpm[i] = (pwm_freq_buffers[i] / 2) * 60;
    fanData->duty[i] = getDuty(i);
  }
}

void FanControl::readFanDataPcnt(FanData *fanData)
{
  for (int i = 0; i < 5; i++)
  {
    fanData->freq[i] = pwm_freq_buffers[i];
    fanData->rpm[i] = (pwm_freq_buffers[i] / 2) * 60;
    fanData->duty[i] = getDuty(i);
  }
}

void FanControl::resetFreqs()
{
  for (int i = 0; i < 5; i++)
  {
    pwm_freqs[i] = 0;
  }
}
