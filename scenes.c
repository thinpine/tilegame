#include "scenes.h"
#include "renderer.h"
#include "coremechanics.h"
#include <stdio.h>

// ------------------------------------------------------------------
// GLOBAL DATA STORAGE
// ------------------------------------------------------------------
int globalScore = 0;
bool gameShouldClose = false;

static GameMap storedMaps[5];
static PlayerState storedPlayerStates[5];
static int lastActiveLevel = 1;

static Scene activeScene;

// ------------------------------------------------------------------
// INTERNAL FUNCTION PROTOTYPES
// ------------------------------------------------------------------
void InitMenuMain(void);
void UpdateMenuMain(Scene* s); 
void DrawMenuMain(Scene* s);

void InitLevel(int levelNum, bool resetPosition);
void UpdateLevel(Scene* s); 
void DrawLevel(Scene* s);

void InitMenuPause(void);
void UpdateMenuPause(Scene* s); 
void DrawMenuPause(Scene* s);

// ------------------------------------------------------------------
// PUBLIC FUNCTIONS
// ------------------------------------------------------------------
void InitSceneSystem(void) {
    // Generate Random Points
    for(int l=1; l<=4; l++) {
        for(int x=0; x<MAP_WIDTH; x++) {
            for(int y=0; y<MAP_HEIGHT; y++) {
                if (GetRandomValue(0, 7) == 0) {
                    storedMaps[l].tiles[x][y].hasPoint = true;
                } else {
                    storedMaps[l].tiles[x][y].hasPoint = false;
                }
                storedMaps[l].tiles[x][y].isClaimed = false;
            }
        }
    }
}

Scene* GetActiveScene(void) {
    return &activeScene;
}

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

// ------------------------------------------------------------------
// SCENE IMPLEMENTATIONS
// ------------------------------------------------------------------

// --- MAIN MENU ---
void InitMenuMain(void) {
    activeScene.Update = UpdateMenuMain;
    activeScene.Draw = DrawMenuMain;
}
void UpdateMenuMain(Scene* s) {}
void DrawMenuMain(Scene* s) {
    ClearBackground(DARKBLUE);
    DrawCenteredText("MAIN MENU", SCR_WIDTH/2, 100, 60, WHITE);
    
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

// --- LEVEL ---
void InitLevel(int levelNum, bool resetPosition) {
    activeScene.type = levelNum;
    activeScene.Update = UpdateLevel;
    activeScene.Draw = DrawLevel;
    lastActiveLevel = levelNum;
    activeScene.map = storedMaps[levelNum];

    if (resetPosition) {
        activeScene.player = (PlayerState){5, 5, DIR_NORTH};
    } else {
        activeScene.player = storedPlayerStates[levelNum];
    }
}

void UpdateLevel(Scene* s) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        storedPlayerStates[s->type] = s->player;
        ChangeScene(SCENE_MENU_PAUSE);
        return;
    }
    
    // Core Logic
    HandlePlayerMovement(&s->player);
    CheckPointCollection(s, &globalScore, &storedMaps[s->type]);
}

void DrawLevel(Scene* s) {
    DrawLevelView(s);
    DrawHUD(s, globalScore);
}

// --- PAUSE ---
void InitMenuPause(void) {
    activeScene.Update = UpdateMenuPause;
    activeScene.Draw = DrawMenuPause;
}
void UpdateMenuPause(Scene* s) {
    if (IsKeyPressed(KEY_ESCAPE)) InitLevel(lastActiveLevel, false);
}
void DrawMenuPause(Scene* s) {
    DrawLevelView(s); // Draw background
    DrawRectangle(0,0, SCR_WIDTH, SCR_HEIGHT, Fade(BLACK, 0.6f));
    DrawCenteredText("PAUSED", SCR_WIDTH/2, 100, 60, RAYWHITE);

    Button btnResume = { (Rectangle){400, 250, 400, 60}, "RESUME", LIGHTGRAY };
    Button btnExit = {(Rectangle){400, 650, 400, 60}, "EXIT GAME", MAROON};

    if (GuiButton(btnResume)) InitLevel(lastActiveLevel, false);
    if (GuiButton(btnExit)) gameShouldClose = true;
}