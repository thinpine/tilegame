#include <stdio.h>
#include "raylib.h"

// --------------------------------------------------------------------------------------
// 1. CONSTANTS, ENUMS & GLOBALS
// --------------------------------------------------------------------------------------
const int SCR_WIDTH = 1200;
const int SCR_HEIGHT = 900;

// Map dimensions
#define MAP_WIDTH 10
#define MAP_HEIGHT 10

typedef enum { DIR_NORTH=0, DIR_EAST=1, DIR_SOUTH=2, DIR_WEST=3 } Direction;

// Scene identifiers
typedef enum { 
    SCENE_MENU_MAIN = 0,
    SCENE_LEVEL_1 = 1,
    SCENE_LEVEL_2 = 2,
    SCENE_LEVEL_3 = 3,
    SCENE_LEVEL_4 = 4,
    SCENE_MENU_PAUSE = 5
} SceneType;

// Global Assets
// Simplified to 1 texture per level for now based on your file list
Texture2D levelTextures[5]; // Indices 1-4 used

// --------------------------------------------------------------------------------------
// 2. DATA STRUCTURES
// --------------------------------------------------------------------------------------

typedef struct PlayerState {
    int x, y;
    Direction facing;
} PlayerState;

PlayerState storedStates[5]; 
int lastActiveLevel = 1; 

struct Scene; 
typedef struct Scene {
    SceneType type;
    PlayerState player;
    void (*Update)(struct Scene* self);
    void (*Draw)(struct Scene* self);
} Scene;

Scene activeScene; 

typedef struct Button {
    Rectangle rect;
    const char* text;
    Color color;
} Button;

// --------------------------------------------------------------------------------------
// 3. FORWARD DECLARATIONS
// --------------------------------------------------------------------------------------
void InitMenuMain(void); void UpdateMenuMain(Scene* s); void DrawMenuMain(Scene* s);
void InitLevel(int levelNum, bool resetState); void UpdateLevel(Scene* s); void DrawLevel(Scene* s);
void InitMenuPause(void); void UpdateMenuPause(Scene* s); void DrawMenuPause(Scene* s);

void ChangeScene(SceneType newType) {
    activeScene.type = newType;
    switch (newType) {
        case SCENE_MENU_MAIN: InitMenuMain(); break;
        case SCENE_LEVEL_1:   InitLevel(1, true); break;
        case SCENE_LEVEL_2:   InitLevel(2, true); break;
        case SCENE_LEVEL_3:   InitLevel(3, true); break;
        case SCENE_LEVEL_4:   InitLevel(4, true); break;
        case SCENE_MENU_PAUSE: InitMenuPause(); break;
    }
}

// --------------------------------------------------------------------------------------
// 4. HELPER FUNCTIONS
// --------------------------------------------------------------------------------------

bool GuiButton(Button btn) {
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, btn.rect);
    DrawRectangleRec(btn.rect, isHover ? LIGHTGRAY : btn.color);
    DrawRectangleLinesEx(btn.rect, 2, DARKGRAY);
    
    int fontSize = 30;
    int textWidth = MeasureText(btn.text, fontSize);
    int drawX = btn.rect.x + (btn.rect.width / 2) - (textWidth / 2);
    int drawY = btn.rect.y + (btn.rect.height / 2) - (fontSize / 2);
    DrawText(btn.text, drawX, drawY, fontSize, BLACK);
    
    return (isHover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
}

bool IsValidMove(int x, int y) {
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
}

// --------------------------------------------------------------------------------------
// 5. SCENE IMPLEMENTATIONS
// --------------------------------------------------------------------------------------

// --- MAIN MENU ---
void InitMenuMain(void) {
    activeScene.Update = UpdateMenuMain;
    activeScene.Draw = DrawMenuMain;
    for(int i=1; i<=4; i++) storedStates[i] = (PlayerState){5, 5, DIR_NORTH};
}

void UpdateMenuMain(Scene* self) { }

void DrawMenuMain(Scene* self) {
    // No texture provided for menu yet, so we use a solid color
    ClearBackground(DARKBLUE);
    
    DrawText("MAIN MENU", 450, 100, 60, WHITE);
    
    Button btnL1 = { (Rectangle){400, 250, 400, 60}, "Level 1: Residence", GRAY };
    Button btnL2 = { (Rectangle){400, 330, 400, 60}, "Level 2: Copse", GRAY };
    Button btnL3 = { (Rectangle){400, 410, 400, 60}, "Level 3: Hospital", GRAY };
    Button btnL4 = { (Rectangle){400, 490, 400, 60}, "Level 4: Dungeon", GRAY };
    Button btnExit = {(Rectangle){400, 650, 400, 60}, "EXIT GAME", MAROON};

    if (GuiButton(btnL1)) ChangeScene(SCENE_LEVEL_1);
    if (GuiButton(btnL2)) ChangeScene(SCENE_LEVEL_2);
    if (GuiButton(btnL3)) ChangeScene(SCENE_LEVEL_3);
    if (GuiButton(btnL4)) ChangeScene(SCENE_LEVEL_4);
    if (GuiButton(btnExit)) CloseWindow();
}

// --- GAME LEVELS ---
void InitLevel(int levelNum, bool resetState) {
    activeScene.Update = UpdateLevel;
    activeScene.Draw = DrawLevel;
    lastActiveLevel = levelNum;

    if (resetState) {
         activeScene.player = storedStates[levelNum];
    }
}

void UpdateLevel(Scene* self) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        storedStates[activeScene.type] = self->player;
        ChangeScene(SCENE_MENU_PAUSE);
        return; 
    }

    PlayerState* p = &self->player;
    if (IsKeyPressed(KEY_A)) p->facing = (p->facing - 1 + 4) % 4;
    if (IsKeyPressed(KEY_D)) p->facing = (p->facing + 1) % 4;

    if (IsKeyPressed(KEY_W)) {
        int tx = p->x; int ty = p->y;
        switch (p->facing) {
            case DIR_NORTH: ty--; break; case DIR_EAST: tx++; break;
            case DIR_SOUTH: ty++; break; case DIR_WEST: tx--; break;
        }
        if (IsValidMove(tx, ty)) { p->x = tx; p->y = ty; }
    }
}

void DrawLevel(Scene* self) {
    int lvlIdx = activeScene.type;
    
    // 1. Draw the background image for this level
    // We scale the texture to fit the screen just in case the image isn't exactly 1200x900
    // sourceRec is the full image, destRec is the full screen
    Texture2D tex = levelTextures[lvlIdx];
    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT };
    DrawTexturePro(tex, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);

    // 2. Draw Text (Aligned Top-Middle)
    char coordText[100];
    char* dirStrs[] = {"North", "East", "South", "West"};
    sprintf(coordText, "Lvl: %d | X: %d Y: %d | Facing: %s", 
            lvlIdx, self->player.x, self->player.y, dirStrs[self->player.facing]);

    int fontSize = 40;
    int textWidth = MeasureText(coordText, fontSize);
    int drawX = (SCR_WIDTH / 2) - (textWidth / 2);
    int drawY = 20; // Top margin

    // Draw background box for text
    DrawRectangle(drawX - 20, drawY - 5, textWidth + 40, fontSize + 10, Fade(BLACK, 0.6f));
    DrawText(coordText, drawX, drawY, fontSize, RAYWHITE);
}

// --- PAUSE MENU ---
void InitMenuPause(void) {
    activeScene.Update = UpdateMenuPause;
    activeScene.Draw = DrawMenuPause;
}

void UpdateMenuPause(Scene* self) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        InitLevel(lastActiveLevel, false);
    }
}

void DrawMenuPause(Scene* self) {
    DrawLevel(self); // Draw game behind menu
    DrawRectangle(0,0, SCR_WIDTH, SCR_HEIGHT, Fade(BLACK, 0.6f));
    
    DrawText("PAUSED", 500, 100, 60, RAYWHITE);

    Button btnResume = { (Rectangle){400, 250, 400, 60}, "RESUME", LIGHTGRAY };
    
    char txtL[3][20];
    int count = 0;
    int levelIDs[3];
    for(int i=1; i<=4; i++) {
        if(i == lastActiveLevel) continue;
        sprintf(txtL[count], "Go to Level %d", i);
        levelIDs[count] = i;
        count++;
    }

    Button btnOther1 = { (Rectangle){400, 350, 400, 60}, txtL[0], GRAY };
    Button btnOther2 = { (Rectangle){400, 430, 400, 60}, txtL[1], GRAY };
    Button btnOther3 = { (Rectangle){400, 510, 400, 60}, txtL[2], GRAY };
    Button btnExit = {(Rectangle){400, 650, 400, 60}, "EXIT GAME", MAROON};

    if (GuiButton(btnResume)) InitLevel(lastActiveLevel, false);
    if (GuiButton(btnOther1)) ChangeScene(levelIDs[0]);
    if (GuiButton(btnOther2)) ChangeScene(levelIDs[1]);
    if (GuiButton(btnOther3)) ChangeScene(levelIDs[2]);
    if (GuiButton(btnExit)) CloseWindow();
}

// --------------------------------------------------------------------------------------
// 6. MAIN & ASSET LOADING
// --------------------------------------------------------------------------------------
int main(void) {
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "First Person C Game");
    SetTargetFPS(60);

    // --- LOAD ASSETS ---
    // Loading the specific files you requested
    levelTextures[1] = LoadTexture("assets/bg_residence.png");
    levelTextures[2] = LoadTexture("assets/bg_copse.png");
    levelTextures[3] = LoadTexture("assets/bg_hospital.png");
    levelTextures[4] = LoadTexture("assets/bg_dungeon.png");

    // Start game at main menu
    ChangeScene(SCENE_MENU_MAIN);

    while (!WindowShouldClose()) {
        if (activeScene.Update) activeScene.Update(&activeScene);

        BeginDrawing();
        // Clear background is technically not needed if drawing full screen texture, 
        // but good for the Menu which has no texture.
        ClearBackground(BLACK); 
        
        if (activeScene.Draw) activeScene.Draw(&activeScene);
        EndDrawing();
    }

    // --- CLEANUP ASSETS ---
    for(int i=1; i<=4; i++) {
        UnloadTexture(levelTextures[i]);
    }
    
    CloseWindow();
    return 0;
}