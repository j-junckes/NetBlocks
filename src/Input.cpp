#include "Input.hpp"
#include <SDL2/SDL.h>

Input::Input() {
  keyboardState = SDL_GetKeyboardState(nullptr);
  previousKeyboardState = new Uint8[SDL_NUM_SCANCODES];
  memcpy(previousKeyboardState, keyboardState, SDL_NUM_SCANCODES);

  mouseX = mouseY = previousMouseX = previousMouseY = deltaMouseX = deltaMouseY = 0;

  mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  previousMouseX = mouseX;
  previousMouseY = mouseY;
  previousMouseState = mouseState;
}

Input::~Input() {
  delete[] previousKeyboardState;
}

void Input::update() {
  memcpy(previousKeyboardState, keyboardState, SDL_NUM_SCANCODES);
  previousMouseState = mouseState;
  SDL_PumpEvents();

  keyboardState = SDL_GetKeyboardState(nullptr);
  previousMouseX = mouseX;
  previousMouseY = mouseY;
  mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  SDL_GetRelativeMouseState(&deltaMouseX, &deltaMouseY);
}

bool Input::isKeyDown(SDL_Scancode scancode) {
  return keyboardState[scancode] && !previousKeyboardState[scancode];
}

bool Input::isKeyUp(SDL_Scancode scancode) {
  return !keyboardState[scancode] && previousKeyboardState[scancode];
}

bool Input::isKey(SDL_Scancode scancode) {
  return keyboardState[scancode] && previousKeyboardState[scancode];
}

bool Input::isMouseButtonDown(int button) const {
  return mouseState & SDL_BUTTON(button) && !(previousMouseState & SDL_BUTTON(button));
}

bool Input::isMouseButtonUp(int button) const {
  return !(mouseState & SDL_BUTTON(button)) && previousMouseState & SDL_BUTTON(button);
}

bool Input::isMouseButton(int button) const {
  return mouseState & SDL_BUTTON(button);
}

int Input::getMouseX() const {
  return mouseX;
}

int Input::getMouseY() const {
  return mouseY;
}

int Input::getMouseDeltaX() const {
  return deltaMouseX;
}

int Input::getMouseDeltaY() const {
  return deltaMouseY;
}
