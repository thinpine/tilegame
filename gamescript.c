#include <stdio.h>
#include "raylib.h"

// --------------------------------------------------------------------------------------
// 1. CONSTANTS, ENUMS & GLOBALS
// --------------------------------------------------------------------------------------
const int SCR_WIDTH = 1200;
const int SCR_HEIGHT = 900;

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

typedef enum { DIR_NORTH=0, DIR_EAST=1, DIR_SOUTH=2, DIR_WEST=3 } Direction;

typedef enum { 
    SCENE_MENU_MAIN = 0,
    SCENE_LEVEL_1 = 1,
    SCENE_LEVEL_2 = 2,
    SCENE_LEVEL_3 = 3,
    SCENE_LEVEL_4 = 4,
    SCENE_MENU_PAUSE = 5
} SceneType;

Texture2D levelTextures[5]; 
bool gameShouldClose = false;

// --- NEW: Global Score ---
int globalScore = 0;

// --------------------------------------------------------------------------------------
// 2. DATA STRUCTURES
// --------------------------------------------------------------------------------------

// --- NEW: Tile & Map Structures ---
typedef struct Tile {
    bool hasPoint;
    bool isClaimed;
} Tile;

typedef struct GameMap {
    Tile tiles[MAP_WIDTH][MAP_HEIGHT];
} GameMap;

// Global persistent storage for maps (Points state)
GameMap storedMaps[5]; 

typedef struct PlayerState {
    int x, y;
    Direction facing;
} PlayerState;

// "Save Slot" for player state (needed for Pause/Resume functionality)
PlayerState storedPlayerStates[5]; 
int lastActiveLevel = 1; 

struct Scene; 
typedef struct Scene {
    SceneType type;
    PlayerState player;
    // The scene holds a COPY of the map for active use
    GameMap map; 
    
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
void InitLevel(int levelNum, bool resetPosition); void UpdateLevel(Scene* s); void DrawLevel(Scene* s);
void InitMenuPause(void); void UpdateMenuPause(Scene* s); void DrawMenuPause(Scene* s);

// --- NEW: Debug Printer ---
void PrintLevelDebugInfo(int levelNum) {
    printf("\n--- DEBUG: LEVEL %d POINT STATUS ---\n", levelNum);
    int count = 0;
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (storedMaps[levelNum].tiles[x][y].hasPoint) {
                const char* status = storedMaps[levelNum].tiles[x][y].isClaimed ? "[CLAIMED]" : "[AVAILABLE]";
                printf("Point at (%d, %d): %s\n", x, y, status);
                count++;
            }
        }
    }
    if (count == 0) printf("No points generated for this level.\n");
    printf("------------------------------------\n");
}

void ChangeScene(SceneType newType) {
    activeScene.type = newType;
    switch (newType) {
        case SCENE_MENU_MAIN: InitMenuMain(); break;
        // When switching to a level from menu, we RESET position (true)
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
    // Note: We do NOT reset globalScore or storedMaps here, so they persist.
}

void UpdateMenuMain(Scene* self) { }

void DrawMenuMain(Scene* self) {
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
    if (GuiButton(btnExit)) gameShouldClose = true;
}

// --- GAME LEVELS ---
void InitLevel(int levelNum, bool resetPosition) {
    activeScene.type = levelNum; 
    activeScene.Update = UpdateLevel;
    activeScene.Draw = DrawLevel;
    lastActiveLevel = levelNum;

    // Load the persistent map state (Points)
    activeScene.map = storedMaps[levelNum];

    if (resetPosition) {
         // Fresh Start logic: 
         // 1. Reset Position to default (5,5)
         activeScene.player.x = 5;
         activeScene.player.y = 5;
         activeScene.player.facing = DIR_NORTH;
         
         // 2. Print Debug Info since we are entering the level fresh
         PrintLevelDebugInfo(levelNum);
         
         printf("Level %d initialized. State loaded. Position reset to 5,5.\n", levelNum);
    } else {
         // Resume logic: Load the player's position from the "Pause Save"
         activeScene.player = storedPlayerStates[levelNum];
         printf("Resuming Level %d at X:%d Y:%d\n", levelNum, activeScene.player.x, activeScene.player.y);
    }
}

void UpdateLevel(Scene* self) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        storedPlayerStates[activeScene.type] = self->player;
        ChangeScene(SCENE_MENU_PAUSE);
        return; 
    }

    PlayerState* p = &self->player;
    
    // --- NEW: Check for Point Collection ---
    // If the tile has a point and it hasn't been claimed yet
    if (self->map.tiles[p->x][p->y].hasPoint && !self->map.tiles[p->x][p->y].isClaimed) {
        // 1. Update Active Scene
        self->map.tiles[p->x][p->y].isClaimed = true;
        
        // 2. Update Global Persistent Storage (Important!)
        storedMaps[activeScene.type].tiles[p->x][p->y].isClaimed = true;
        
        // 3. Increment Global Score
        globalScore++;
        printf("Point collected at %d,%d! Total Score: %d\n", p->x, p->y, globalScore);
    }

    // Movement Logic
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
    
    Texture2D tex = levelTextures[lvlIdx];
    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT };
    DrawTexturePro(tex, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);

    // --- UPDATED: Display Score ---
    char coordText[100];
    char* dirStrs[] = {"North", "East", "South", "West"};
    sprintf(coordText, "Lvl: %d | X: %d Y: %d | Facing: %s | Score: %d", 
            lvlIdx, self->player.x, self->player.y, dirStrs[self->player.facing], globalScore);

    int fontSize = 40;
    int textWidth = MeasureText(coordText, fontSize);
    int drawX = (SCR_WIDTH / 2) - (textWidth / 2);
    int drawY = 20;

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
        InitLevel(lastActiveLevel, false); // false = Do NOT reset position (Resume)
    }
}

void DrawMenuPause(Scene* self) {
    DrawLevel(self); 
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

    if (GuiButton(btnResume)) InitLevel(lastActiveLevel, false); // Resume
    if (GuiButton(btnOther1)) ChangeScene(levelIDs[0]); // Reset Position
    if (GuiButton(btnOther2)) ChangeScene(levelIDs[1]); // Reset Position
    if (GuiButton(btnOther3)) ChangeScene(levelIDs[2]); // Reset Position
    
    if (GuiButton(btnExit)) gameShouldClose = true;
}

// --------------------------------------------------------------------------------------
// 6. MAIN & ASSET LOADING
// --------------------------------------------------------------------------------------
int main(void) {
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "First Person C Game");
    SetExitKey(KEY_NULL); 
    SetTargetFPS(60);

    // --- NEW: Initialize Random Points ---
    // We do this once at startup so the points are fixed for the session
    for(int l=1; l<=4; l++) {
        for(int x=0; x<MAP_WIDTH; x++) {
            for(int y=0; y<MAP_HEIGHT; y++) {
                // 1/8 probability (approx 12.5%)
                if (GetRandomValue(0, 7) == 0) {
                    storedMaps[l].tiles[x][y].hasPoint = true;
                    storedMaps[l].tiles[x][y].isClaimed = false;
                } else {
                    storedMaps[l].tiles[x][y].hasPoint = false;
                    storedMaps[l].tiles[x][y].isClaimed = false;
                }
            }
        }
    }
    // -------------------------------------

    levelTextures[1] = LoadTexture("assets/bg_residence.png");
    levelTextures[2] = LoadTexture("assets/bg_copse.png");
    levelTextures[3] = LoadTexture("assets/bg_hospital.png");
    levelTextures[4] = LoadTexture("assets/bg_dungeon.png");

    ChangeScene(SCENE_MENU_MAIN);

    while (!WindowShouldClose() && !gameShouldClose) {
        if (activeScene.Update) activeScene.Update(&activeScene);

        BeginDrawing();
        ClearBackground(BLACK); 
        
        if (activeScene.Draw) activeScene.Draw(&activeScene);
        EndDrawing();
    }

    for(int i=1; i<=4; i++) {
        UnloadTexture(levelTextures[i]);
    }
    
    CloseWindow();
    return 0;
}