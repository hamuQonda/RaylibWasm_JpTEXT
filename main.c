#include "raylib.h"
#include <stdlib.h> // malloc, free
#include <string.h> // strlen, strcat
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// 表示したい日本語TEXTたち
const char* texts[] = {
    u8"こんにちは ( ^ω^ )\nｒａｙｌｉｂの世界へ ようこそ",
    u8"レイリブは楽しい！",
    u8"漢字・ひらがな・カタカナを使えます"
};
int textCount = 3;
Font font;
int codepointCount = 0;
int* codepoints = NULL;

bool swColor = false;
int textY = 560;
float textSpeed = 15.0f;

// Prototype Declaration : 文字列配列から重複を除いたコードポイント配列を作成
int* GetUniqueCodepointsFromStrings(const char** texts, int textCount, int* outCount);
// Prototype Declaration : Update and draw one frame
void UpdateDrawFrame(void);

// Program main entry point
int main(void) {
    InitWindow(800, 600, u8"raylib で日本語表示するサンプルプログラム");
    //textCount = sizeof(texts) / sizeof(texts[0]);
    // ユニークなコードポイントを取得
    codepoints = GetUniqueCodepointsFromStrings(texts, textCount, &codepointCount);
    if (codepointCount == 0)    // 取得失敗
        SetWindowTitle(u8"コードポイントの読み込みに失敗しました！！！！！！！！！！！！");

    // フォント読み込み（例: "NotoSansJP-Regular.otf" を使用）
    font = LoadFontEx("font/NotoSansJP-Regular.otf", 60, codepoints, codepointCount);

    free(codepoints); // 不要になったコードポイント配列を解放

    SetTextLineSpacing(55);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    UnloadFont(font);
    CloseWindow();
    return 0;
}

// Update and draw one frame
void UpdateDrawFrame(void) {
    // Update
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) swColor = !swColor;
    textY = textY - ((textY > 40) ? textSpeed * GetFrameTime() : -560);

    // Draw
    BeginDrawing();
    ClearBackground(swColor ? RAYWHITE : PINK);
    for (int i = 0; i < textCount; i++) {
        DrawTextEx(font, texts[i], (Vector2) { 60.0f, textY + i * 160.0f }, 60, 1, BLACK);
    }
    //DrawRectangle(20, 40, 760, 520, swColor ? YELLOW : RAYWHITE);
    DrawRectangleLinesEx((Rectangle) { 0, 0, GetScreenWidth(), GetScreenHeight() }, 40, swColor ? YELLOW : RAYWHITE);
    DrawText("Below, Japanese text is displayed.", 20, 8, 24, DARKGRAY);
    DrawText("Click to change color", 500, 570, 24, DARKGRAY);
    EndDrawing();
}

/// 文字列配列から重複を除いたコードポイント配列を作成  
int* GetUniqueCodepointsFromStrings(const char** texts, int textCount, int* outCount) {
    int totalCodes = 0;
    int capacity = 1024;
    int* allCodepoints = (int*)malloc(sizeof(int) * capacity);
    if (allCodepoints == NULL) {
        *outCount = 0;
        return NULL;
    }

    for (int i = 0; i < textCount; i++) {
        if (texts[i][0] == '\0' || texts[i] == NULL) continue;
        int count = 0;
        int* codes = LoadCodepoints(texts[i], &count);

        for (int j = 0; j < count; j++) {
            int cp = codes[j];
            // 重複チェック  
            bool exists = false;
            for (int k = 0; k < totalCodes; k++) {
                if (allCodepoints[k] == cp) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                if (totalCodes >= capacity) {
                    capacity *= 2;
                    int* temp = (int*)realloc(allCodepoints, sizeof(int) * capacity);
                    if (temp == NULL) {
                        free(allCodepoints);
                        UnloadCodepoints(codes);
                        *outCount = 0;
                        return NULL;
                    }
                    allCodepoints = temp;
                }
                allCodepoints[totalCodes++] = cp;
            }
        }
        UnloadCodepoints(codes);
    }

    *outCount = totalCodes;
    return allCodepoints;
}
