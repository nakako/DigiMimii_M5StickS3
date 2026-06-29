// ======================================================
// デジみみぃ
// 周囲のWi-Fiアクセスポイントを検出して表示する
// M5StickS3 + M5Unified
// ======================================================

// M5StickS3制御ライブラリ
#include <M5Unified.h>

// ESP32 Wi-Fiライブラリ
#include <WiFi.h>

// ------------------------------------------------------
// 初期化処理
// 電源投入時に1回だけ実行される
// ------------------------------------------------------
void setup() {

  // M5StickS3の設定を取得
  auto cfg = M5.config();

  // M5StickS3初期化
  M5.begin(cfg);

  // LCDを横向き表示
  M5.Display.setRotation(3);

  // 文字サイズ設定
  M5.Display.setTextSize(1);

  // Wi-Fiをステーションモードに設定
  // （アクセスポイントにはしない）
  WiFi.mode(WIFI_STA);

  // 念のため接続状態を解除
  WiFi.disconnect();

  delay(100);

  // タイトル表示
  M5.Display.println("DIGIMIMII");
  M5.Display.println("WiFi Scanner");

  // 起動画面を2秒表示
  delay(2000);
}

// ------------------------------------------------------
// メインループ
// setup()終了後に繰り返し実行される
// ------------------------------------------------------
void loop() {
  // 前回のスキャン結果を解放
  WiFi.scanDelete();

  // Wi-FiタスクへCPU時間を渡す
  delay(100);

  // 周囲のWi-Fiをスキャン
  int n = WiFi.scanNetworks();

  // Wi-FiタスクへCPU時間を渡す
  delay(10);

  // スキャン失敗時
  if (n < 0) {

    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);

    M5.Display.setTextColor(RED);
    M5.Display.println("Scan Error");

    delay(3000);
    return;
  }

   // 画面クリア
  M5.Display.fillScreen(BLACK);

  // カーソルを左上へ移動
  M5.Display.setCursor(0, 0);

  // 検出数表示
  M5.Display.printf("%d AP Found\n", n);

  // 最大8件まで表示
  for (int i = 0; i < n && i < 8; i++) {
    // SSID取得
    String ssid = WiFi.SSID(i);

    // RSSI取得
    int rssi = WiFi.RSSI(i);

    // BSSID取得
    String bssid = WiFi.BSSIDstr(i);

    M5.Display.setTextColor(WHITE);
    M5.Display.printf("%s : %d dBm\n",
                      ssid.c_str(),
                      rssi);

    M5.Display.setTextColor(CYAN);
    M5.Display.printf("%s\n",
                      bssid.c_str());
  }

  // 3秒待って再スキャン
  delay(3000);
}