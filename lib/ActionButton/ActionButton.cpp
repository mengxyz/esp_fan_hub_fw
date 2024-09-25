#include "ActionButton.h"

ActionButton* ActionButton::instance = nullptr; // Initialize static instance pointer

ActionButton::ActionButton(int pinNumber, int stepCount, int debounceDelay) 
  : pin(pinNumber), pressCount(0), lastPressTime(0), lastDebounceTime(0), debounceDelay(debounceDelay), buttonState(LOW), lastButtonState(LOW), stepCount(stepCount) {
  pinMode(pin, INPUT);
  instance = this; // Set the static instance pointer
  attachInterrupt(digitalPinToInterrupt(pin), ActionButton::isrWrapper, RISING);

  // Dynamically allocate the callbacks array based on stepCount
  callbacks = new void(*[stepCount])();
  for (int i = 0; i < stepCount; i++) {
    callbacks[i] = nullptr;
  }
}

ActionButton::~ActionButton() {
  // Free the dynamically allocated memory
  delete[] callbacks;
}

void ActionButton::addStep(int pressCount, void (*callback)()) {
  if (pressCount >= 1 && pressCount <= stepCount) {
    callbacks[pressCount - 1] = callback;
  }
}

void ActionButton::service() {
  // Check if the button is released and the press count is valid
  if (pressCount > 0 && (millis() - lastPressTime) > 500) {
    if (pressCount >= 1 && pressCount <= stepCount && callbacks[pressCount - 1] != nullptr) {
      callbacks[pressCount - 1]();
    }
    pressCount = 0; // Reset the press count
  }
}

void ActionButton::buttonPress() {
  unsigned long currentTime = millis();

  // Debounce check
  if (currentTime - lastDebounceTime < debounceDelay) {
    return;
  }
  lastDebounceTime = currentTime;

  // Check if the press is within the allowed time window (e.g., 500ms)
  if (currentTime - lastPressTime <= 500) {
    pressCount++;
  } else {
    pressCount = 1; // Reset count if the press is outside the window
  }

  lastPressTime = currentTime;
}

void ActionButton::isrWrapper() {
  if (instance) {
    instance->buttonPress();
  }
}