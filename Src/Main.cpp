#include "ShootEmUp.h"

using namespace ShootEmUp;
using namespace std;

// 自機の変数
double jiki_x;
double jiki_y;

// 敵の変数
double teki_x;
double teki_y;

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

    // ゲーム開始時の設定
    teki_x = 0;
    teki_y = 0;
    jiki_x = 0;
    jiki_y = 0;
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
  if (get_key(' ') == 1) {
    scene_number = 2; // ゲームオーバー画面に切り替える.
  }

  // 画像を表示
  draw(teki_x, teki_y, "asteroid_a.png");
  draw(jiki_x, jiki_y, "space_fighter.png");
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
