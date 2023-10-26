#pragma once

#include <SDL2/SDL.h>

class Input {
public:
  Input();

  ~Input();

  void update();

  [[nodiscard]] bool isKeyDown(SDL_Scancode scancode);

  [[nodiscard]] bool isKeyUp(SDL_Scancode scancode);

  [[nodiscard]] bool isKey(SDL_Scancode scancode);

  [[nodiscard]] bool isMouseButtonDown(int button) const;

  [[nodiscard]] bool isMouseButtonUp(int button) const;

  [[nodiscard]] bool isMouseButton(int button) const;

  [[nodiscard]] int getMouseX() const;

  [[nodiscard]] int getMouseY() const;

  [[nodiscard]] int getMouseDeltaX() const;

  [[nodiscard]] int getMouseDeltaY() const;

private:
  const Uint8 *keyboardState;
  Uint8 *previousKeyboardState;

  Uint32 mouseState;
  Uint32 previousMouseState;
  int mouseX, mouseY;
  int previousMouseX, previousMouseY;
  int deltaMouseX, deltaMouseY;
};
