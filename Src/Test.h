//#include "ShootEmUp.h"

using namespace ShootEmUp;
using namespace std;

// 自機の変数
double jiki_x;
double jiki_y;
double jiki_beam_x;
double jiki_beam_y;

// 敵の変数
const int teki_kazu = 30;
double teki_x[teki_kazu];
double teki_y[teki_kazu];

// 現在の画面番号.
//   0 = タイトル
//   1 = ゲーム
//   2 = ゲームオーバー
int scene_number = 0;

//
// タイトル画面
//
void title()
{
  if (get_key(' ') == 1) {
    scene_number = 1; // ゲーム画面に切り替える

    // ゲーム開始時の変数の設定
    for (int n = 0; n < teki_kazu; n += 1) {
      teki_x[n] = 400;
      teki_y[n] = 100;
    }
    jiki_x = 400;
    jiki_y = 500;
    jiki_beam_x = 0;
    jiki_beam_y = -100;
  }

  // 画像を表示
  draw(400, 300, "logo_title.png");
  draw(400, 500, "logo_push_space_key.png");
}

//
// ゲーム画面
//
void game()
{
  // 隕石の移動
  for (int n = 0; n < teki_kazu; n += 1) {
    teki_y[n] += 3;
    if (teki_y[n] > 600) {
      teki_x[n] = rand() % 800;
      teki_y[n] = 0;
    }
  }

  // 自機の移動
  if (get_key('a') > 0) {
    jiki_x -= 6;
  }
  if (get_key('d') > 0) {
    jiki_x += 6;
  }

  // 自機と隕石の衝突
  for (int n = 0; n < teki_kazu; n += 1) {
    if (abs(teki_x[n] - jiki_x) < 32 + 8) {
      if (abs(teki_y[n] - jiki_y) < 32 + 8) {
        scene_number = 2; // ゲームオーバーに切り替える
        stop_bgm();
      }
    }
  }

  // ビーム発射
  if (get_key(' ') == 1) {
    play_sound("shot_0.wav");
    jiki_beam_x = jiki_x;
    jiki_beam_y = jiki_y;
  }

  // ビームの移動
  if (jiki_beam_y > -100) {
    jiki_beam_y -= 32;
  }

  // ビームと隕石の衝突
  for (int n = 0; n < teki_kazu; n += 1) {
    if (abs(teki_x[n] - jiki_beam_x) < 32 + 8) {
      if (abs(teki_y[n] - jiki_beam_y) < 32 + 24) {
        play_sound("bomb_1.wav");
        jiki_beam_y = -100; // ビームを消す
        teki_x[n] = rand() % 800;
        teki_y[n] = 0; // 隕石を上に戻す
      }
    }
  }

  // 画像を表示
  for (int n = 0; n < teki_kazu; n += 1) {
    draw(teki_x[n], teki_y[n], "asteroid_a.png");
  }
  draw(jiki_x, jiki_y, "space_fighter.png");
  draw(jiki_beam_x, jiki_beam_y, "shot_beam.png");
}

//
// ゲームオーバー画面
//
void gameover()
{
  if (get_key(' ') == 1) {
    scene_number = 0; // タイトル画面に切り替える
  }

  // 画像を表示
  draw(400, 300, "logo_gameover.png");
}

//
// ここからプログラムの実行が開始される
//
int main()
{
  // プログラムの初期化処理
  initialize("シューティングゲーム");

  // ゲームループ
  for (;;) {
    update();

    // シーン番号に対応する関数を実行
    if (scene_number == 0) {
      title();
    } else if (scene_number == 1) {
      game();
    } else if (scene_number == 2) {
      gameover();
    }

    render();
  }

  // プログラムの終了処理
  finalize();
}

