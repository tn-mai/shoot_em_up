/**
* @file ShootEmUp.cpp
*/
#include <objbase.h>
#include "ShootEmUp.h"
#include "GLFWEW.h"
#include "Sprite.h"
#include "Font.h"
#include "Audio.h"
#include "Log.h"
#include <glm/gtc/constants.hpp>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <iostream>
#include <stdarg.h>
#include <algorithm>
#include <unordered_map>
#include <thread>
#include <chrono>

namespace ShootEmUp {

namespace /* unnamed */ {

const double unitDeltaTime = 1.0 / 60.0;
double remainingDeltaTime = 0;

const size_t maxSpriteCount = 1024; // �ő�X�v���C�g��.
Sprite rootNode; // �`�擙�̑�{�ɂȂ�X�v���C�g.
std::vector<Sprite> spriteBuffer; // ���ۂɕ`�悳���X�v���C�g����.
SpriteRenderer spriteRenderer; // �X�v���C�g�`��p�ϐ�.

std::unordered_map<std::string, TexturePtr> textureCache; // �ǂݍ��񂾃e�N�X�`����ێ�����ϐ�.

/**
* �e�L�X�g�\�����.
*/
struct text_info
{
  glm::vec2 pos; // �\��������W.
  std::wstring text; // �\������e�L�X�g.
};
std::vector<text_info> textList; // �e�L�X�g���̃��X�g.
Font::Renderer fontRenderer;

std::mt19937 randomEngine; // ����������.

// ��������ϐ�.
std::string bgmFilename;
Audio::SoundPtr bgm;
float seVolume = 0.8f;
float bgmVolume = 0.8f;

/**
* sjis�����񂩂�utf16������𓾂�.
*/
std::wstring sjis_to_utf16(const char* p)
{
  std::wstring tmp;
  tmp.resize(strlen(p) + 1);
  mbstowcs(&tmp[0], p, tmp.size());
  return tmp;
}

/**
* utf16�����񂩂�sjis������𓾂�.
*/
std::string utf16_to_sjis(const std::wstring& p)
{
  std::string tmp;
  tmp.resize(p.size() * 2);
  wcstombs(&tmp[0], p.c_str(), tmp.size());
  return tmp;
}

/**
* �X�N���[�����W�ɑΉ�����N���b�v���W�𓾂�.
*/
glm::vec2 screen_coord_to_clip_coord(const glm::vec2& pos)
{
  const GLFWEW::Window& window = GLFWEW::Window::Instance();
  const glm::vec2 ss(window.Width() / 2, window.Height() / 2);
  return pos / ss;
}

/**
* �E�B���h�E���W�n(���㌴�_)����OpenGL���W�n(�������_)�ɕϊ�����.
*/
glm::vec2 win_to_ogl_coord(double x, double y)
{
  const GLFWEW::Window& window = GLFWEW::Window::Instance();
  return glm::vec2(x - window.Width() / 2, window.Height() / 2 - y);
}

} // unnamed namespace

/**
* Easy�����Command���C�u�����̏�����.
*/
void initialize(const char* title)
{
  FreeConsole();

  const int wcs_size = MultiByteToWideChar(CP_ACP, 0, title, -1, nullptr, 0);
  std::vector<wchar_t> wcs_title(wcs_size, L'\0');
  MultiByteToWideChar(CP_ACP, 0, title, -1, wcs_title.data(), wcs_title.size());
  const int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wcs_title.data(), wcs_title.size(), nullptr, 0, nullptr, nullptr);
  std::vector<char> utf8_title(utf8_size, u8'\0');
  WideCharToMultiByte(CP_UTF8, 0, wcs_title.data(), wcs_title.size(), utf8_title.data(), utf8_title.size(), nullptr, nullptr);

  if (CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED) != S_OK) {
    LOG("WARNING: CoInitializeEx�Ɏ��s\n.");
    exit(1);
  }
  GLFWEW::Window& window = GLFWEW::Window::Instance();
  if (!window.Init(800, 600, utf8_title.data())) {
    exit(1);
  }

  if (!Audio::Engine::Get().Initialize()) {
    LOG("WARNING: Audio Engine�̏������Ɏ��s\n.");
  }

  // �X�v���C�g�y�уe�N�X�`���̏����ݒ�.
  setlocale(LC_CTYPE, "JPN");
  Texture::Initialize();
  textureCache.reserve(maxSpriteCount);

  spriteRenderer.Init(maxSpriteCount);
  spriteBuffer.reserve(maxSpriteCount);

  fontRenderer.Init(1024, glm::vec2(window.Width(), window.Height()));
  fontRenderer.LoadFromFile("Res/Font/font.fnt");

  randomEngine.seed(std::random_device()());
  std::srand(std::random_device()());
}

void update()
{
  GLFWEW::Window& window = GLFWEW::Window::Instance();

  // �f���^�^�C����unitDeltaTime�𒴂���܂őҋ@.
  bool updateKey = true;
  for (;;) {
    window.Update(updateKey);
    remainingDeltaTime += window.DeltaTime();
    if (remainingDeltaTime >= unitDeltaTime) {
      break;
    }
    updateKey = false;
    //std::this_thread::sleep_for(std::chrono::duration<double>(unitDeltaTime - remainingDeltaTime));
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }

  Audio::Engine::Get().Update();
  textList.clear();

  rootNode.ClearChildren();
  spriteBuffer.clear();

  const GamePad gamepad = window.GetGamePad();
  if (window.ShouldClose() || (gamepad.buttonDown & GamePad::ESC)) {
    finalize();
    exit(0);
    return;
  }
}

void draw(double x, double y, const char* image, double scale, double rotation)
{
  if (spriteBuffer.size() >= maxSpriteCount) {
    static bool logged = false;
    if (!logged) {
      LOG("����ȏ�X�v���C�g��ǉ��ł��܂���(�ő�%zu�܂�)\n.", maxSpriteCount);
      logged;
    }
    return;
  }

  TexturePtr tex;
  auto itr = textureCache.find(image);
  if (itr != textureCache.end()) {
    tex = itr->second;
  } else {
    std::string str;
    str.reserve(1024);
    str += "Res/�摜/";
    str += image;
    tex = Texture::LoadFromFile(str.c_str());
    if (!tex) {
      //LOG("�摜�t�@�C����%s��������܂���. �t�@�C�������m�F���Ă�������.\n.", image);
      return;
    }
    textureCache.emplace(std::string(image), tex);
  }

  Sprite sprite(tex);

  sprite.Position(glm::vec3(win_to_ogl_coord(x, y), 0));
  sprite.Scale(glm::vec2(static_cast<float>(scale)));
  sprite.Rotation(static_cast<float>(glm::radians(rotation)));
  //target.Shear(0);
  //sprite.Color(glm::vec4(1, 1, 1, a));
  //sprite.ColorMode(BlendMode_Multiply);

  spriteBuffer.push_back(sprite);
  rootNode.AddChild(&spriteBuffer.back());
}

void draw(double x, double y, const char* image) { draw(x, y, image, 1, 0); }

void draw(double x, double y, const std::string& image) { draw(x, y, image.c_str(), 1, 0); }

void render()
{
  GLFWEW::Window& window = GLFWEW::Window::Instance();
  const double d = remainingDeltaTime / unitDeltaTime;
  if (d >= 1) {
    remainingDeltaTime -= unitDeltaTime;
    if (d < 2) {
      rootNode.UpdateTransform();
      rootNode.UpdateRecursive(static_cast<float>(unitDeltaTime));
      spriteRenderer.Update(rootNode);

      fontRenderer.Color(glm::vec4(1));
      fontRenderer.MapBuffer();
      for (const auto& e : textList) {
        fontRenderer.AddString(e.pos, e.text.c_str());
      }
      fontRenderer.UnmapBuffer();

      glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      const glm::vec2 windowSize(window.Width(), window.Height());
      spriteRenderer.Draw(windowSize);
      fontRenderer.Draw();

      window.SwapBuffers();
    }
  }
}

/**
* Easy�����Command���C�u�����̏I��.
*/
void finalize()
{
  bgm.reset();
  rootNode = Sprite();
  spriteBuffer.clear();
  textureCache.clear();
  Texture::Finalize();

  Audio::Engine::Get().Destroy();

  CoUninitialize();
  std::cout << "Finish." << std::endl;
}

/**
*
*/
void xyprintf(double x, double y, const char* format, ...)
{
  char tmp[1024];

  va_list ap;
  va_start(ap, format);
  vsnprintf(tmp, sizeof(tmp), format, ap);
  va_end(ap);

  const glm::vec2 opengl_pos = win_to_ogl_coord(x, y);
  const std::wstring ws = sjis_to_utf16(tmp);
  textList.push_back({ screen_coord_to_clip_coord(opengl_pos), ws });
}

/**
*
*/
int get_key(int key)
{
  key = std::toupper(key);
  const GLFWEW::Window& window = GLFWEW::Window::Instance();
  return window.GetKeyPressed(key) * 2 - window.GetKeyPressedInLastFrame(key);
}

/**
*
*/
int rand()
{
  return std::uniform_int_distribution<int>(0, std::numeric_limits<int>::max())(randomEngine);
}

void play_sound(const char* filename)
{
  std::string str;
  str.reserve(1024);
  str += "Res/����/";
  str += filename;
  const std::wstring ws = sjis_to_utf16(str.c_str());
  Audio::SoundPtr p = Audio::Engine::Get().PrepareMFStream(ws.c_str());
  p->Play(Audio::Flag_None);
  p->SetVolume(seVolume);
}

void play_bgm(const char* filename)
{
  if (bgmFilename != filename || !bgm || !(bgm->GetState() & Audio::State_Playing)) {
    if (bgm) {
      bgm->Stop();
    }
    bgmFilename = filename;
    std::string str;
    str.reserve(1024);
    str += "Res/����/";
    str += filename;
    const std::wstring ws = sjis_to_utf16(str.c_str());
    bgm = Audio::Engine::Get().PrepareMFStream(ws.c_str());
    bgm->Play(Audio::Flag_Loop);
    bgm->SetVolume(bgmVolume);
  }
}

void stop_bgm()
{
  bgmFilename.clear();
  if (bgm) {
    bgm->Stop();
    bgm.reset();
  }
}

void set_sound_volume(double volume)
{
  seVolume = static_cast<float>(volume);
}

void set_bgm_volume(double volume)
{
  bgmVolume = static_cast<float>(volume);
  if (bgm) {
    bgm->SetVolume(bgmVolume);
  }
}

} // namespace ShootEmUp
