//#include "ShootEmUp.h"

using namespace ShootEmUp;
using namespace std;

// ���@�̕ϐ�
double jiki_x;
double jiki_y;
double jiki_beam_x;
double jiki_beam_y;

// �G�̕ϐ�
const int teki_kazu = 30;
double teki_x[teki_kazu];
double teki_y[teki_kazu];

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

    // �Q�[���J�n���̕ϐ��̐ݒ�
    for (int n = 0; n < teki_kazu; n += 1) {
      teki_x[n] = 400;
      teki_y[n] = 100;
    }
    jiki_x = 400;
    jiki_y = 500;
    jiki_beam_x = 0;
    jiki_beam_y = -100;
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
  // 覐΂̈ړ�
  for (int n = 0; n < teki_kazu; n += 1) {
    teki_y[n] += 3;
    if (teki_y[n] > 600) {
      teki_x[n] = rand() % 800;
      teki_y[n] = 0;
    }
  }

  // ���@�̈ړ�
  if (get_key('a') > 0) {
    jiki_x -= 6;
  }
  if (get_key('d') > 0) {
    jiki_x += 6;
  }

  // ���@��覐΂̏Փ�
  for (int n = 0; n < teki_kazu; n += 1) {
    if (abs(teki_x[n] - jiki_x) < 32 + 8) {
      if (abs(teki_y[n] - jiki_y) < 32 + 8) {
        scene_number = 2; // �Q�[���I�[�o�[�ɐ؂�ւ���
        stop_bgm();
      }
    }
  }

  // �r�[������
  if (get_key(' ') == 1) {
    play_sound("shot_0.wav");
    jiki_beam_x = jiki_x;
    jiki_beam_y = jiki_y;
  }

  // �r�[���̈ړ�
  if (jiki_beam_y > -100) {
    jiki_beam_y -= 32;
  }

  // �r�[����覐΂̏Փ�
  for (int n = 0; n < teki_kazu; n += 1) {
    if (abs(teki_x[n] - jiki_beam_x) < 32 + 8) {
      if (abs(teki_y[n] - jiki_beam_y) < 32 + 24) {
        play_sound("bomb_1.wav");
        jiki_beam_y = -100; // �r�[��������
        teki_x[n] = rand() % 800;
        teki_y[n] = 0; // 覐΂���ɖ߂�
      }
    }
  }

  // �摜��\��
  for (int n = 0; n < teki_kazu; n += 1) {
    draw(teki_x[n], teki_y[n], "asteroid_a.png");
  }
  draw(jiki_x, jiki_y, "space_fighter.png");
  draw(jiki_beam_x, jiki_beam_y, "shot_beam.png");
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

