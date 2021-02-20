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

/**
* ShootEmUp���C�u�����̏�Ԃ��X�V.
*/
void update();

/**
* ShootEmUp���C�u�����ɂ��`��.
*/
void render();

/**
* ShootEmUp���C�u�������I������.
*/
void finalize();

/**
* �摜��`�悷��.
*
* @param x     ���̍��W.
* @param y     �c�̍��W.
* @param image �`�悷��摜�t�@�C����.
* @@aram scale �g�嗦(1=�g��k���Ȃ�).
* @@aram rotation ��](0�`360).
*/
void draw(double x, double y, const char* image, double scale, double rotation);
void draw(double x, double y, const char* image);
void draw(double x, double y, const std::string& image);

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

/**
* ������`�悷��.
*
* @param x      ���̍��W.
* @param y      �c�̍��W.
* @param format �`�悷�镶����A�܂��͏����t��������.
* @param ...    (�����t��������̏ꍇ)�����ɑΉ�����ǉ��p�����[�^.
*/
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
const int vk_enter = 257;
const int vk_right = 262;
const int vk_left  = 263;
const int vk_down  = 264;
const int vk_up    = 265;

} // namespace ShootEmUp

#endif // SHOOTEMUP_H_INCLUDED
