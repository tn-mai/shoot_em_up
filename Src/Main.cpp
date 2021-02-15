#include "ShootEmUp.h"

using namespace ShootEmUp;
using namespace std;

namespace Test {
int main();
}

// 自機の変数
double player_x;
double player_y;
double player_shot_x;
double player_shot_y;

// 敵の変数
double enemy_x[3];
double enemy_y[3];

int score;

string number[10] = {
  "chr_0.png",
  "chr_1.png",
  "chr_2.png",
  "chr_3.png",
  "chr_4.png",
  "chr_5.png",
  "chr_6.png",
  "chr_7.png",
  "chr_8.png",
  "chr_9.png",
};

double explosion_x;
double explosion_y;
int explosion_counter;

string explosion_anime[4] = {
  "explosion_0.png",
  "explosion_1.png",
  "explosion_2.png",
  "explosion_3.png",
};

// 現在の画面番号.
// 0 = タイトル
// 1 = ゲーム
// 2 = ゲームオーバー
int scene_number = 0;

//
// タイトル画面
//
void title()
{
  draw(400, 300, "logo_title.png");

  if (get_key(' ') == 1) {
    // ゲーム画面で使う変数を初期化する
    player_x = 400;
    player_y = 500;
    player_shot_x = 0;
    player_shot_y = -100;
    
    for (int i = 0; i < 3; i += 1) {
      enemy_x[i] = rand() % 800;
      enemy_y[i] = -50;
    }

    score = 0;
    
    explosion_x = 0;
    explosion_y = 0;

    explosion_counter = 100;

    // ゲーム画面に切り替える.
    scene_number = 1;
  }
}

//
// ゲーム画面
//
void game()
{
  // 自機を動かす
  if (get_key('a')) {
    player_x -= 6;
  }
  if (get_key('d')) {
    player_x += 6;
  }
  if (get_key('w')) {
    player_y -= 6;
  }
  if (get_key('s')) {
    player_y += 6;
  }

  // 自機の移動範囲を制限する
  if (player_x < 0) {
    player_x = 0;
  }
  if (player_x > 800) {
    player_x = 800;
  }
  if (player_y < 0) {
    player_y = 0;
  }
  if (player_y > 600) {
    player_y = 600;
  }

  // ビーム発射
  if (get_key(' ') == 1) {
    if (player_shot_y <= -30) {
      player_shot_x = player_x;
      player_shot_y = player_y;
    }
  }

  // ビームを動かす
  if (player_shot_y >= -30) {
    player_shot_y -= 30;
  }

  // 敵を動かす
  for (int i = 0; i < 3; i += 1) {
    enemy_y[i] += 5;
    // 敵が画面の下まで移動したら、画面の上に戻す
    if (enemy_y[i] > 830) {
      enemy_x[i] = rand() % 800;
      enemy_y[i] = -50;
    }
  }

  // ビームと敵の当たり判定
  for (int i = 0; i < 3; i += 1) {
    if (std::abs(enemy_x[i] - player_shot_x) < 20) {
      if (std::abs(enemy_y[i] - player_shot_y) < 20) {
        // 爆発を表示
        explosion_x = enemy_x[i];
        explosion_y = enemy_y[i];
        explosion_counter = 0;

        // 敵を画面の上に戻す
        enemy_x[i] = rand() % 800;
        enemy_y[i] = -50;

        // ビームを消す
        player_shot_y = -100;

        // スコアを加算
        score += 1;
      }
    }
  }

  // 敵と自機の当たり判定
  for (int i = 0; i < 3; i += 1) {
    if (std::abs(player_x - enemy_x[i]) < 8) {
      if (std::abs(player_y - enemy_y[i]) < 8) {
        // 自機を消す
        player_x = -100;

        // 敵を画面の上に戻す
        enemy_x[i] = rand() % 800;
        enemy_y[i] = -50;

        // ゲームオーバー画面に切り替える
        scene_number = 2;
      }
    }
  }

  // 画像を表示する
  draw(400, 300, "bg_unknown_planet.png");
  for (int i = 0; i < 3; i += 1) {
    draw(enemy_x[i], enemy_y[i], "asteroid.png");
  }
  if (explosion_counter < 16) {
    int n = explosion_counter / 4;
    draw(explosion_x, explosion_y, explosion_anime[n]);
    explosion_counter += 1;
  }
  draw(player_shot_x, player_shot_y, "shot_beam.png");
  draw(player_x, player_y, "space_fighter.png");

  // スコアを表示する
  draw(400, 30, number[score % 10]);
  draw(360, 30, number[(score / 10) % 10]);
}

//
// ゲームオーバー画面
//
void gameover()
{
  draw(400, 300, "logo_gameover.png");
  if (get_key(' ') == 1) {
    scene_number = 0;
  }
}

//
// ここからプログラムの実行が開始される
//
int main()
{
#if 1
  Test::main();
#else
  // プログラムの初期化処理
  initialize("シューティングゲーム");

  // メインループ
  for (;;) {
    update();

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
#endif
}
