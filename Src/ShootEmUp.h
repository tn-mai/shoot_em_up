/**
* @file ShootEmUp.h
*/
#ifndef SHOOTEMUP_H_INCLUDED
#define SHOOTEMUP_H_INCLUDED
#include <string>

namespace ShootEmUp {

/**
* ShootEmUpライブラリを初期化する.
*
* @param title  タイトルバーに表示される文章.
*/
void initialize(const char* title);

void update();
void render();

/**
* ShootEmUpライブラリを終了する.
*/
void finalize();

/**
*
*/
void draw(double x, double y, const char* image);
inline void draw(double x, double y, const std::string& image) { draw(x, y, image.c_str()); }
void draw(double x, double y, const char* image, double scale);

/**
* 指定されたキーの状態を取得する.
*
* @param key 状態を取得するキー番号.
*
* @retval 0  キーは押されていない.
* @retval 1  キーが押された瞬間.
* @retval 2  キーが押されている.
*/
int get_key(int key);

void xyprintf(double x, double y, const char* format, ...);

/**
* 乱数を得る.
*
* @return 0以上2^31-1以下のランダムな値.
*/
//int rand();

/**
* 音声を再生する.
*
* @param filename 再生する音声ファイル.
*/
void play_sound(const char* filename);

/**
* 背景音楽を再生する.
*
* @param filename 再生する音声ファイル.
*/
void play_bgm(const char* filename);

/**
* 背景音楽を停止する.
*/
void stop_bgm();

/**
* 音声の音量を設定する.
*
* @param volume 音量
*               0.0 無音
*               1.0 音声データそのままの音量.
*               2.0 音声データの2倍の音量.
*/
void set_sound_volume(double volume);

/**
* 背景音楽の音量を設定する.
*
* @param volume 音量
*               0.0 無音
*               1.0 音声データそのままの音量.
*               2.0 音声データの2倍の音量.
*/
void set_bgm_volume(double volume);

// 特殊キーID.
static int vk_enter = 257;
static int vk_right = 262;
static int vk_left  = 263;
static int vk_down  = 264;
static int vk_up    = 265;

} // namespace ShootEmUp

#endif // SHOOTEMUP_H_INCLUDED
