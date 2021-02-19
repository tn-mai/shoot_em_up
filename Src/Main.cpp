#include "ShootEmUp.h"

using namespace ShootEmUp;
using namespace std;

// ���@�̕ϐ�
double jiki_x;
double jiki_y;

// �G�̕ϐ�
double teki_x;
double teki_y;

// ���݂̉�ʔԍ�.
//   0 = �^�C�g��
//   1 = �Q�[��
//   2 = �Q�[���I�[�o�[
int scene_number = 0;

//
// �^�C�g�����
//
void title()
{
  if (get_key(' ') == 1) {
    scene_number = 1; // �Q�[����ʂɐ؂�ւ���

    // �Q�[���J�n���̐ݒ�
    teki_x = 0;
    teki_y = 0;
    jiki_x = 0;
    jiki_y = 0;
  }

  // �摜��\��
  draw(400, 300, "logo_title.png");
  draw(400, 500, "logo_push_space_key.png");
}

//
// �Q�[�����
//
void game()
{
  if (get_key(' ') == 1) {
    scene_number = 2; // �Q�[���I�[�o�[��ʂɐ؂�ւ���.
  }

  // �摜��\��
  draw(teki_x, teki_y, "asteroid_a.png");
  draw(jiki_x, jiki_y, "space_fighter.png");
}

//
// �Q�[���I�[�o�[���
//
void gameover()
{
  if (get_key(' ') == 1) {
    scene_number = 0; // �^�C�g����ʂɐ؂�ւ���
  }

  // �摜��\��
  draw(400, 300, "logo_gameover.png");
}

//
// ��������v���O�����̎��s���J�n�����
//
int main()
{
  // �v���O�����̏���������
  initialize("�V���[�e�B���O�Q�[��");

  // �Q�[�����[�v
  for (;;) {
    update();

    // �V�[���ԍ��ɑΉ�����֐������s
    if (scene_number == 0) {
      title();
    } else if (scene_number == 1) {
      game();
    } else if (scene_number == 2) {
      gameover();
    }

    render();
  }

  // �v���O�����̏I������
  finalize();
}
