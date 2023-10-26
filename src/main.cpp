#include <iostream>

#ifdef PLATFORM_WEB

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#elif PLATFORM_DESKTOP

#include <windows.h>

#endif

#include "gl.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"
#include "Exit.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Input.hpp"
#include "Camera.hpp"
#include <SDL2/SDL.h>

bool isGameRunning = true;
SDL_Window *window = nullptr;
SDL_GLContext gl_context;
std::shared_ptr<Chunk> chunk;
std::shared_ptr<Shader> standardShader;
std::shared_ptr<Shader> simpleShader;
std::shared_ptr<Input> input;
std::shared_ptr<Camera> camera;
int windowWidth, windowHeight;

bool isMouseLocked = false;
GLuint squareVAO, squareVBO, squareEBO;

float vertices[] = {
  0.5f, 0.5f, 0.0f,  // top right
  0.5f, -0.5f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  // bottom left
  -0.5f, 0.5f, 0.0f   // top left
};

unsigned int indices[] = {  // note that we start from 0!
  0, 1, 3,   // first triangle
  1, 2, 3    // second triangle
};

void initSquare() {
  glGenVertexArrays(1, &squareVAO);
  glGenBuffers(1, &squareVBO);
  glGenBuffers(1, &squareEBO);

  glBindVertexArray(squareVAO);

  glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);
}

#ifdef PLATFORM_WEB

EMSCRIPTEN_RESULT pointerLockChangeCallback(int eventType, const EmscriptenPointerlockChangeEvent *e, void *userData) {
  if (!e->isActive && isMouseLocked) {
    isMouseLocked = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
  return EMSCRIPTEN_RESULT_SUCCESS;
}

#endif

Uint64 NOW = SDL_GetPerformanceCounter();
Uint64 LAST = 0;
double deltaTime = 0;

void mainLoop() {
  if (!isGameRunning) {
    exitGame(EXIT_SUCCESS);
  }

  LAST = NOW;
  NOW = SDL_GetPerformanceCounter();
  deltaTime = (double) ((NOW - LAST) * 1000 / (double) SDL_GetPerformanceFrequency()) / 1000.0;

  input->update();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    SDL_PumpEvents();

    if (event.type == SDL_QUIT) {
      isGameRunning = false;
    } else if (event.type == SDL_WINDOWEVENT_LEAVE) {
      std::cout << "Mouse left window" << std::endl;
    } else if (event.type == SDL_MOUSEBUTTONDOWN && !isMouseLocked) {
      isMouseLocked = true;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      SDL_GetWindowSize(window, &windowWidth, &windowHeight);

      glViewport(0, 0, windowWidth, windowHeight);
    }
  }

#ifdef PLATFORM_DESKTOP
  if (isMouseLocked && input->isKeyDown(SDL_SCANCODE_ESCAPE)) {
    isMouseLocked = false;
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
#endif

  if (isMouseLocked) {
    camera->processKeyboard(input, deltaTime);
    camera->processMouseMovement(input);
  }

  chunk->tryUpdateMesh();

  glClearColor(0x98 / 255.0f, 0xd6 / 255.0f, 0xff / 255.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  standardShader->use();

  auto view = camera->getViewMatrix();
  standardShader->setMat4("view", view);

  auto projection = camera->getProjectionMatrix(windowWidth, windowHeight);
  standardShader->setMat4("projection", projection);

  glm::mat4 model = glm::mat4(1.0f);
  standardShader->setMat4("model", model);

  chunk->render();

//  simpleShader->use();
//
//  simpleShader->setMat4("view", view);
//  simpleShader->setMat4("projection", projection);
//
//  glBindVertexArray(squareVAO);
//  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//  glBindVertexArray(0);

  SDL_GL_SwapWindow(window);
}

void initialize() {
  std::cout << "Initializing game..." << std::endl;

#ifdef PLATFORM_WEB
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

  SDL_Renderer *renderer = nullptr;
  SDL_CreateWindowAndRenderer(854, 480, SDL_WINDOW_OPENGL, &window, &renderer);
  std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;

  emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, pointerLockChangeCallback);

  std::cout << "Game initialized." << std::endl;

  standardShader = std::make_shared<Shader>("assets/shaders/standard.es3.vsh", "assets/shaders/standard.es3.fsh");
#elif PLATFORM_DESKTOP
  auto system = SDL_Init(SDL_INIT_VIDEO);
  if (system != 0) {
    std::cerr << "Error initializing SDL: " << SDL_GetError() << std::endl;
    exitGame(EXIT_FAILURE);
  }

  window = SDL_CreateWindow("NetBlocks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 854, 480,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    std::cerr << "Error creating window: " << SDL_GetError() << std::endl;
    exitGame(EXIT_FAILURE);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
    exitGame(EXIT_FAILURE);
  }

  SDL_GL_MakeCurrent(window, gl_context);

  auto glew = glewInit();
  if (glew != GLEW_OK) {
    std::cerr << "Error initializing GLEW: " << glewGetErrorString(glew) << std::endl;
    exitGame(EXIT_FAILURE);
  }

  std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;

  std::cout << "Game initialized." << std::endl;

  standardShader = std::make_shared<Shader>("assets/shaders/standard.gl46.vsh", "assets/shaders/standard.gl46.fsh");
  simpleShader = std::make_shared<Shader>("assets/shaders/simple.gl46.vsh", "assets/shaders/simple.gl46.fsh");
#endif

  SDL_GetWindowSize(window, &windowWidth, &windowHeight);

  camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

  chunk = std::make_shared<Chunk>(0, 0, time(nullptr) % 1000);

  input = std::make_shared<Input>();

  //initSquare();

  //glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  //render wireframe
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

[[noreturn]] int main(int argv, char *argc[]) {
  initialize();

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(mainLoop, 0, true);
#else
  while (true) {
    mainLoop();
  }
#endif
}
