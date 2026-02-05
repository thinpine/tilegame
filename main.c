#include "raylib.h"
#include "types.h"
#include "resources.h"
#include "scenes.h"

int main(void) {
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "First Person C Game");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    LoadGameAssets();
    InitSceneSystem();
    
    ChangeScene(SCENE_MENU_MAIN);

    while (!WindowShouldClose() && !gameShouldClose) {
        Scene* active = GetActiveScene();
        
        if (active->Update) active->Update(active);

        BeginDrawing();
        ClearBackground(BLACK);
        if (active->Draw) active->Draw(active);
        EndDrawing();
    }

    UnloadGameAssets();
    CloseWindow();
    return 0;
}