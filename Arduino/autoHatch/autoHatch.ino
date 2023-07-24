/**
 * ボックスに眠っているタマゴを順に孵化していくスケッチ
 *
 * ※このスケッチはタマゴの孵化のみでタマゴ受け取りは行いません。タマゴは事前に用意しておいてください。
 *
 * 初期条件は以下の通り
 * 1.「プラトタウン」に空を飛ぶを行い、その着地点にいること
 * 2.マップを開いた際に北が上にくるようになっていること
 * 3.ポケモンが連れ歩き状態でないこと
 * 4.ライド状態でないこと
 * 5.手持ちが「ほのおのからだ」の特性持ちであり、かつ、キャモメを倒せるモーションの短い攻撃技を一番上にした通常色の最終進化系ポケモン1匹のみであること
 * 6.孵化させたいタマゴの預けてあるボックスが最初に開くこと（複数ボックス孵化させる場合は、ボックスが連続するような配置にしておいてください）
 * 7.孵化させるボックスにタマゴを敷き詰めてあること（空きがあると不安定になるかも）
 * 8.オフライン状態であること
 * 9.無線のコントローラーが接続されていないこと
 * 10.「設定」から「話の速さ」を「はやい」に、「わざ覚えスキップ」を「する」に、「ニックネーム登録」を「しない」にしておくこと
 */

////////////////// libraries ////////////////////////////////

// ライブラリを読み込むためのコード
#include <NintendoSwitchControlLibrary.h>

////////////////// options ////////////////////////////////

// 何ボックス分ループさせるか？
const int BOX_COUNT = 5;

// 1ボックスの孵化を終えるごとにレポートを書くかどうか（書く: 1, 書かない: 0）
const int REPORT_OPTION = 0;

////////////////// constant parameters ////////////////////////////////

// ボックスの列数
const int BOX_COLUMN_LENGTH = 6;

// 手持ちに入れられるタマゴの最大数
const int MAX_EGG_COUNT = 5;

// ボタン押下時の標準的な待機時間
const int BASE_PUSH_BUTTON_TIME_MS = 1000;

// カーソル操作時の標準的な待機時間
const int BASE_CURSOR_OPERATION_TIME_MS = 200;
const int HALF_CURSOR_OPERATION_TIME_MS = 100;

// メニューの開閉にかかる待機時間
const int MENU_OPEN_OR_CLOSE_TIME_MS = 1500;

////////////////// variable parameters ////////////////////////////////

// ループをカウントするための変数
int loopCount = 0;

////////////////// methods ////////////////////////////////

/**
 * Switchをスリープさせる
 *
 * @return void
 */
void sleepGame() {
    holdButton(Button::HOME, 1500);
    pushButton(Button::A, BASE_PUSH_BUTTON_TIME_MS);
}

/**
 * レポートを書く
 *
 * @return void
 */
void writeReport() {
    pushButton(Button::X, MENU_OPEN_OR_CLOSE_TIME_MS);
    pushButton(Button::R, 1500);
    pushButton(Button::A, 4000);
    pushButton(Button::A, BASE_PUSH_BUTTON_TIME_MS);
    pushButton(Button::B, MENU_OPEN_OR_CLOSE_TIME_MS);
}

/**
 * ボックスを開く
 *
 * @param bool is_init
 * @return void
 */
void openBox(bool is_init = false) {
    pushButton(Button::X, MENU_OPEN_OR_CLOSE_TIME_MS);

    // 初回のみカーソルを合わせる
    if (is_init) {
        holdHat(Hat::RIGHT, BASE_CURSOR_OPERATION_TIME_MS);
        holdHat(Hat::UP, 2000);
        pushHat(Hat::DOWN, BASE_CURSOR_OPERATION_TIME_MS);
    }

    pushButton(Button::A, 4000);
}

/**
 * ボックスを閉じる
 *
 * @return void
 */
void closeBox() {
    pushButton(Button::B, 3000);
    pushButton(Button::B, MENU_OPEN_OR_CLOSE_TIME_MS);
}

/**
 * ポケモンを預ける
 *
 * @param int box_line
 * @return void
 */
void putInPokemons(int box_line) {
    // 手持ちのポケモンを範囲選択する
    pushHat(Hat::LEFT, HALF_CURSOR_OPERATION_TIME_MS);
    pushHat(Hat::DOWN, HALF_CURSOR_OPERATION_TIME_MS);
    pushButton(Button::MINUS, BASE_PUSH_BUTTON_TIME_MS);
    holdHat(Hat::DOWN, 1000);
    pushButton(Button::A, BASE_CURSOR_OPERATION_TIME_MS);

    // 範囲選択したポケモンをボックスに預ける
    pushHat(Hat::RIGHT, BASE_CURSOR_OPERATION_TIME_MS, box_line);
    pushHat(Hat::UP, BASE_CURSOR_OPERATION_TIME_MS);
    pushButton(Button::A, BASE_CURSOR_OPERATION_TIME_MS);

    // ボックスがいっぱいになったら、次のボックスに移動させる
    if (box_line == BOX_COLUMN_LENGTH) {
        pushButton(Button::R, BASE_CURSOR_OPERATION_TIME_MS);

        // 最後のボックスである場合、その後のカーソル操作は行わない
        if (loopCount == BOX_COUNT - 1) return;

        pushHat(Hat::RIGHT, HALF_CURSOR_OPERATION_TIME_MS);
    }

    // 次の処理に繋げやすくするため、カーソルを移動させる
    pushHat(Hat::RIGHT, BASE_CURSOR_OPERATION_TIME_MS);
}

/**
 * タマゴを引き出す
 *
 * @param int box_line
 * @return void
 */
void pullOutEggs(int box_line) {
    pushButton(Button::MINUS, BASE_PUSH_BUTTON_TIME_MS);
    holdHat(Hat::DOWN, 1000);
    pushButton(Button::A, BASE_CURSOR_OPERATION_TIME_MS);
    pushHat(Hat::LEFT, BASE_CURSOR_OPERATION_TIME_MS, (box_line % BOX_COLUMN_LENGTH) + 1);
    pushHat(Hat::DOWN, BASE_CURSOR_OPERATION_TIME_MS);
    pushButton(Button::A, BASE_CURSOR_OPERATION_TIME_MS);
}

/**
 * 孵化した手持ちのポケモンを全てボックスに預け、同時にタマゴを手持ちに加える
 *
 * @param int box_line
 * @return void
 */
void doBoxOperations(int box_line) {
    openBox();
    putInPokemons(box_line);

    // 最後の孵化のタイミングでは、タマゴの引き出しは行わない
    if (!(box_line == BOX_COLUMN_LENGTH && loopCount == BOX_COUNT - 1)) {
        pullOutEggs(box_line);
    }

    // ボックスを跨いだときは処理が重くて閉じる操作に失敗することがあるため、回避するための遅延&連打処理を用意
    if (box_line == BOX_COLUMN_LENGTH) {
        delay(4000);
        pushButton(Button::B, 100, 3);
    }

    closeBox();
}

/**
 * 「プラトタウン」から「コサジの小道灯台」に空を飛ぶで移動する（同じ場所に空を飛ぼうとするとAボタンが反応しないことがあるため、「プラトタウン」からのスタートとした）
 *
 * @return void
 */
void goFromLosPlatosToLighthouse() {
    pushButton(Button::Y, 3000);
    tiltLeftStick(204, Stick::MAX, 380);
    delay(1000);
    pushButton(Button::A, BASE_PUSH_BUTTON_TIME_MS);
    pushButton(Button::A, 2000);
    pushButton(Button::A, 9000);
}

/**
 * 灯台に登る
 *
 * @return void
 */
void climbLighthouse() {
    tiltLeftStick(Stick::NEUTRAL, Stick::MIN, 5000);
    delay(5000);
}

/**
 * ライドのON/OFFを切り替える（処理落ちで稀にボタン操作を認識しないことがあるため、連打で回避を狙う）
 *
 * @return void
 */
void switchRide() {
    pushButton(Button::PLUS, 10, 6);
    delay(BASE_PUSH_BUTTON_TIME_MS);
}

/**
 * キャモメがいる場合、あらかじめ飛ばしておく
 *
 * @return void
 */
void flyWingulls() {
    tiltLeftStick(Stick::MAX, Stick::NEUTRAL, 2400);
    delay(1000);
}

/**
 * 走り回って手持ちの全てのタマゴを孵化させる（ダッシュを使うと飛び出しやすい）
 *
 * @param long run_time_ms
 * @return void
 */
void hatchAllEggs(unsigned long run_time_ms) {
    int battle_time_ms = 20000;
    tiltLeftStick(Stick::MAX, Stick::NEUTRAL, run_time_ms - battle_time_ms, Button::A);
    pushButton(Button::A, 500, 40);  // キャモメ対策で最後は移動せずにA連打する
}

////////////////// execute ////////////////////////////////

void setup() {
    // Switchがマイコンを認識するまでは信号を受け付けないため、適当な処理をさせておく
    pushButton(Button::LCLICK, 500, 5);

    // ポジションへ移動
    goFromLosPlatosToLighthouse();
    climbLighthouse();
    switchRide();
    flyWingulls();

    // 初回のボックス操作
    openBox(true);
    pullOutEggs(0);
    closeBox();
}

void loop() {
    // 指定のボックス分の孵化を終えた後、スリープ状態にして処理を終了する
    if (loopCount == BOX_COUNT) {
        sleepGame();
        exit(0);
    }

    for (int box_line = 1; box_line <= BOX_COLUMN_LENGTH; box_line++) {
        // タマゴの孵化
        hatchAllEggs(260000);  // 260秒で孵化サイクル40が全て孵化できる？

        // ボックス操作
        doBoxOperations(box_line);
    }

    // レポートオプション
    if (REPORT_OPTION == 1) {
        writeReport();
    }

    loopCount++;
}
