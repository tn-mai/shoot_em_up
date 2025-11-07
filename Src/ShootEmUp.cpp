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
#include <xmmintrin.h>

namespace ShootEmUp {

namespace /* unnamed */ {

const double unitDeltaTime = 1.0 / 60.0;
double remainingDeltaTime = 0;

const size_t maxSpriteCount = 1024; // 最大スプライト数.
Sprite rootNode; // 描画等の大本になるスプライト.
std::vector<Sprite> spriteBuffer; // 実際に描画されるスプライトたち.
SpriteRenderer spriteRenderer; // スプライト描画用変数.

std::unordered_map<std::string, TexturePtr> textureCache; // 読み込んだテクスチャを保持する変数.

/**
* テキスト表示情報.
*/
struct text_info
{
  glm::vec2 pos; // 表示する座標.
  std::wstring text; // 表示するテキスト.
};
std::vector<text_info> textList; // テキスト情報のリスト.
Font::Renderer fontRenderer;

std::mt19937 randomEngine; // 乱数生成器.

// 音声制御変数.
std::string bgmFilename;
Audio::SoundPtr bgm;
float seVolume = 0.8f;
float bgmVolume = 0.8f;

/**
* sjis文字列からutf16文字列を得る.
*/
std::wstring sjis_to_utf16(const char* p)
{
  std::wstring tmp;
  tmp.resize(strlen(p) + 1);
  mbstowcs(&tmp[0], p, tmp.size());
  return tmp;
}

/**
* utf16文字列からsjis文字列を得る.
*/
std::string utf16_to_sjis(const std::wstring& p)
{
  std::string tmp;
  tmp.resize(p.size() * 2);
  wcstombs(&tmp[0], p.c_str(), tmp.size());
  return tmp;
}

/**
* スクリーン座標に対応するクリップ座標を得る.
*/
glm::vec2 screen_coord_to_clip_coord(const glm::vec2& pos)
{
  const GLFWEW::Window& window = GLFWEW::Window::Instance();
  const glm::vec2 ss(window.Width() / 2, window.Height() / 2);
  return pos / ss;
}

/**
* ウィンドウ座標系(左上原点)からOpenGL座標系(中央原点)に変換する.
*/
glm::vec2 win_to_ogl_coord(double x, double y)
{
  const GLFWEW::Window& window = GLFWEW::Window::Instance();
  return glm::vec2(x - window.Width() / 2, window.Height() / 2 - y);
}

} // unnamed namespace

/**
* EasyおよびCommandライブラリの初期化.
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
    LOG("WARNING: CoInitializeExに失敗\n.");
    exit(1);
  }
  GLFWEW::Window& window = GLFWEW::Window::Instance();
  if (!window.Init(800, 600, utf8_title.data())) {
    exit(1);
  }

  if (!Audio::Engine::Get().Initialize()) {
    LOG("WARNING: Audio Engineの初期化に失敗\n.");
  }

  // スプライト及びテクスチャの初期設定.
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

  // デルタタイムがunitDeltaTimeを超えるまで待機.
  const double startTime = glfwGetTime();
  window.Update();
  for (;;) {
    const double diffTime = glfwGetTime() - startTime;
    if (remainingDeltaTime + diffTime >= unitDeltaTime) {
      remainingDeltaTime += diffTime;
      break;
    }

    // Sleep関数はコンテキストスイッチング等で大量のCPUサイクルを消費するため、案外CPUに優しくないらしい
    // ia-optimization-reference-manual-2022-02-jp.pdf によると、 PAUSE 命令で置き換えるのが良いらしい
    //
    // ところで、PAUSE 1回につき10～140サイクル待機させられるので、複数回呼び出すことで待機時間を制御する
    // CPUが2GHzで駆動すると仮定すると 140/2,000,000,000秒 = 70ns
    // PAUSE を10回呼び出すと0.7us、1000回呼び出すと70us(0.07ms)となる
    //
    // さすがにこの回数のループは無意味だと思うので、待つべき時間によって呼び分けることにした
    //
    // ただ Sleep(0)は自分以上の優先度のスレッドがない場合は何もしない
    // そこで、Sleep(0)の代わりに、優先度に関係なくスイッチする SwitchToThread を使うことにした
    // こいつはスイッチが起こらなかったら0を返すので、そのときは PAUSE ループにフォールバックする

    const double t = unitDeltaTime - remainingDeltaTime + diffTime;
    if (t < 0.001 || SwitchToThread() == 0) {
      for (int i = 0; i < 100; i++) {
        _mm_pause();
      }
    }
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
      LOG("これ以上スプライトを追加できません(最大%zu個まで)\n.", maxSpriteCount);
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
    str += "Res/画像/";
    str += image;
    tex = Texture::LoadFromFile(str.c_str());
    if (!tex) {
      //LOG("画像ファイル名%sが見つかりません. ファイル名を確認してください.\n.", image);
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
* EasyおよびCommandライブラリの終了.
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
  str += "Res/音声/";
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
    str += "Res/音声/";
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
