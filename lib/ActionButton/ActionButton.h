#if !defined(ACTION_BUTTON_H)
#define ACTION_BUTTON_H

#include <Arduino.h>
class ActionButton {
public:
  ActionButton(int pinNumber, int stepCount, int debounceDelay = 50);
  ~ActionButton();
  void addStep(int pressCount, void (*callback)());
  void service();

private:
  int pin;
  int pressCount;
  unsigned long lastPressTime;
  unsigned long lastDebounceTime;
  int debounceDelay;
  int buttonState;
  int lastButtonState;
  int stepCount;
  void (**callbacks)(); // Dynamically allocated array of callbacks

  void buttonPress();
  static void isrWrapper();
  static ActionButton* instance; // Static instance pointer for ISR
};

#endif // ACTION_BUTTON_H
