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
// Wi-Fi情報
// ------------------------------------------------------
struct WiFiInfo {
  String ssid;   // Wi-Fi名
  String bssid;  // 固有ID
  int rssi;      // 電波強度
  int channel;   // チャンネル
};

// ------------------------------------------------------
// 最大32件保存
// ------------------------------------------------------
WiFiInfo wifiList[32];


// ------------------------------------------------------
// ペンタトニックスケール
//
// Wi-Fiチャンネル(CH1～CH13)を
// MIDIノート番号へ変換するテーブル
//
// 60 = ド(C4)
// 62 = レ(D4)
// 64 = ミ(E4)
// 67 = ソ(G4)
// 69 = ラ(A4)
// ------------------------------------------------------
const int scaleTable[13] = {
  60,  // CH1  → ド(C4)
  62,  // CH2  → レ(D4)
  64,  // CH3  → ミ(E4)
  67,  // CH4  → ソ(G4)
  69,  // CH5  → ラ(A4)

  72,  // CH6  → ド(C5)
  74,  // CH7  → レ(D5)
  76,  // CH8  → ミ(E5)
  79,  // CH9  → ソ(G5)
  81,  // CH10 → ラ(A5)

  84,  // CH11 → ド(C6)
  86,  // CH12 → レ(D6)
  88   // CH13 → ミ(E6)
};


// ------------------------------------------------------
// Wi-FiチャンネルをMIDIノート番号へ変換する
//
// 引数
//   channel : Wi-Fiチャンネル(1～13)
//
// 戻り値
//   MIDIノート番号
// ------------------------------------------------------
int channelToMidiNote(int channel) {
  // 範囲外なら真ん中のド(C4)
  if (channel < 1 || channel > 13) {
    return 60;
  }

  // テーブルから取得(配列の添字は0から始まり、Wi-Fiチャネルは1からのため-1を実施)
  return scaleTable[channel - 1];
}


// ------------------------------------------------------
// MIDIノート番号を音名へ変換する
//
// 引数
//   note : MIDIノート番号
//
// 戻り値
//   音名（C4, D4など）
// ------------------------------------------------------
String midiNoteToName(int note) {
  switch (note) {
    case 60: return "C4";
    case 62: return "D4";
    case 64: return "E4";
    case 67: return "G4";
    case 69: return "A4";

    case 72: return "C5";
    case 74: return "D5";
    case 76: return "E5";
    case 79: return "G5";
    case 81: return "A5";

    case 84: return "C6";
    case 86: return "D6";
    case 88: return "E6";

    default: return "---";
  }
}


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

  // スキャン失敗時
  if (n < 0) {

    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);

    M5.Display.setTextColor(RED);
    M5.Display.println("Scan Error");

    delay(3000);
    return;
  }

  // ----------------------------------------------------
  // Wi-Fi情報を構造体へ保存する
  // 最大32件まで保存
  // ----------------------------------------------------
  int count = min(n, 32);
  for (int i = 0; i < count; i++) {
    wifiList[i].ssid = WiFi.SSID(i);
    wifiList[i].bssid = WiFi.BSSIDstr(i);
    wifiList[i].rssi = WiFi.RSSI(i);
    wifiList[i].channel = WiFi.channel(i);
  }

  // ----------------------------------------------------
  // LCD表示
  // ----------------------------------------------------
  // 画面クリア
  M5.Display.fillScreen(BLACK);

  // カーソルを左上へ移動
  M5.Display.setCursor(0, 0);

  // 検出数表示
  M5.Display.setTextColor(YELLOW);
  M5.Display.printf("%d AP Found\n", count);


  // 最大8件まで表示
  for (int i = 0; i < count && i < 8; i++) {
    // SSID取得
    String ssid = wifiList[i].ssid;

    // RSSI取得
    int rssi = wifiList[i].rssi;

    // BSSID取得
    String bssid = wifiList[i].bssid;

    // Channnel取得
    int channel = wifiList[i].channel;

    // Wi-FiチャンネルからMIDIノート番号へ変換
    int note = channelToMidiNote(channel);

    // MIDIノート番号を音名へ変換
    String noteName = midiNoteToName(note);

    // SSID・RSSI表示
    M5.Display.setTextColor(WHITE);
    M5.Display.printf("%s : %d dBm : \n",
                      ssid.c_str(),
                      rssi);

    // BSSID表示
    M5.Display.setTextColor(CYAN);
    M5.Display.printf("%s,  ",
                      bssid.c_str());

    // チャンネルとMIDIノート番号を表示
    M5.Display.setTextColor(GREEN);
    M5.Display.printf("CH=%2d NOTE=%2d(%s)\n",
                      channel,
                      note,
                      noteName.c_str());
  }

  // 3秒待って再スキャン
  delay(3000);
}