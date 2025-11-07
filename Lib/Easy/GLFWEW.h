/**
* @file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include "GamePad.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GLFWEW {

/**
* GLFWとGLEWのラッパークラス.
*/
class Window
{
public:
  static Window& Instance();
  bool Init(int w, int h, const char* title);
  bool ShouldClose() const;
  void SwapBuffers() const;
  const GamePad& GetGamePad() const;
  bool GetKeyPressed(int n) const { return keyPressed[n]; }
  bool GetKeyPressedInLastFrame(int n) const { return keyPressedInLastFrame[n]; }
  void Update();
  void ResetDeltaTime();
  float DeltaTime() const { return deltaTime; }
  int Width() const { return width; }
  int Height() const { return height; }

private:
  Window() = default;
  ~Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  void UpdateGamePad();
  static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

  bool isGLFWInitialized = false;
  bool isInitialized = false;
  GLFWwindow* window = nullptr;
  int width;
  int height;
  GamePad gamepad;
  bool keyValid[GLFW_KEY_LAST];
  bool keyPressed[GLFW_KEY_LAST];
  bool keyPressedInLastFrame[GLFW_KEY_LAST];

  double prevTime = 0;
  float deltaTime = 0;
};

} // namespace GLFWEW

#endif // GLFWEW_H_INCLUDED
