/**
* @file ShootEmUp.h
*/
#ifndef SHOOTEMUP_H_INCLUDED
#define SHOOTEMUP_H_INCLUDED
#include <string>

namespace ShootEmUp {

/**
* ShootEmUp���C�u����������������.
*
* @param title  �^�C�g���o�[�ɕ\������镶��.
*/
void initialize(const char* title);

void update();
void render();

/**
* ShootEmUp���C�u�������I������.
*/
void finalize();

/**
*
*/
void draw(double x, double y, const char* image);
inline void draw(double x, double y, const std::string& image) { draw(x, y, image.c_str()); }
void draw(double x, double y, const char* image, double scale);

/**
* �w�肳�ꂽ�L�[�̏�Ԃ��擾����.
*
* @param key ��Ԃ��擾����L�[�ԍ�.
*
* @retval 0  �L�[�͉�����Ă��Ȃ�.
* @retval 1  �L�[�������ꂽ�u��.
* @retval 2  �L�[��������Ă���.
*/
int get_key(int key);

void xyprintf(double x, double y, const char* format, ...);

/**
* �����𓾂�.
*
* @return 0�ȏ�2^31-1�ȉ��̃����_���Ȓl.
*/
//int rand();

/**
* �������Đ�����.
*
* @param filename �Đ����鉹���t�@�C��.
*/
void play_sound(const char* filename);

/**
* �w�i���y���Đ�����.
*
* @param filename �Đ����鉹���t�@�C��.
*/
void play_bgm(const char* filename);

/**
* �w�i���y���~����.
*/
void stop_bgm();

/**
* �����̉��ʂ�ݒ肷��.
*
* @param volume ����
*               0.0 ����
*               1.0 �����f�[�^���̂܂܂̉���.
*               2.0 �����f�[�^��2�{�̉���.
*/
void set_sound_volume(double volume);

/**
* �w�i���y�̉��ʂ�ݒ肷��.
*
* @param volume ����
*               0.0 ����
*               1.0 �����f�[�^���̂܂܂̉���.
*               2.0 �����f�[�^��2�{�̉���.
*/
void set_bgm_volume(double volume);

// ����L�[ID.
static int vk_enter = 257;
static int vk_right = 262;
static int vk_left  = 263;
static int vk_down  = 264;
static int vk_up    = 265;

} // namespace ShootEmUp

#endif // SHOOTEMUP_H_INCLUDED
