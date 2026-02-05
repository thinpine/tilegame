#include <stdio.h>
#include "raylib.h"

// --------------------------------------------------------------------------------------
// 1. CONSTANTS & ENUMS
// --------------------------------------------------------------------------------------
#define MAP_WIDTH 10
#define MAP_HEIGHT 10
#define TILE_SIZE 60

// Direction Enum for readable logic
typedef enum {
    DIR_NORTH = 0,
    DIR_EAST  = 1,
    DIR_SOUTH = 2,
    DIR_WEST  = 3
} Direction;

// --------------------------------------------------------------------------------------
// 2. DATA STRUCTURES (The "Blueprint")
// --------------------------------------------------------------------------------------

// -- Loot and Inventory --
typedef struct Stash {
    int goldCount;
    int items[10]; // Placeholder for item IDs
} Stash;

// -- NPCs --
typedef struct Daemon {
    int x, y;           // Grid position
    int hp;
    bool isHostile;
    int typeID;         // To link to specific assets/sprites
} Daemon;

// -- The Player --
typedef struct Player {
    int x, y;           // Grid position
    Direction facing;   // 0:N, 1:E, 2:S, 3:W
    Stash inventory;
    int hp;
    
    // Animation State
    bool isMoving;
    float animTimer;    // For handling asset animation frames
} Player;

// -- The Map Components --
typedef struct Tile {
    int typeID;         // 0=Grass, 1=Water, 2=Wall, etc.
    bool isWalkable;
    Color debugColor;   // Placeholder until you load textures
} Tile;

typedef struct GameMap {
    Tile tiles[MAP_WIDTH][MAP_HEIGHT];
} GameMap;

// -- The Scene System --
// Forward declaration so we can use Scene* in function pointers
struct Scene; 

typedef struct Scene {
    int type; // 0 = Menu, 1 = Gameplay
    
    // Scene Data
    GameMap map;
    Player player;
    Daemon daemons[5]; // Array for up to 5 NPCs for now
    
    // Logic Pointers (passing self reference)
    void (*Update)(struct Scene* self);
    void (*Draw)(struct Scene* self);
} Scene;

// --------------------------------------------------------------------------------------
// 3. LOGIC & HELPER FUNCTIONS
// --------------------------------------------------------------------------------------

// Helper: Check if coordinates are valid
bool IsValidMove(int x, int y, GameMap* map) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return false;
    if (!map->tiles[x][y].isWalkable) return false;
    return true;
}

// -- Gameplay Logic --
void UpdateGameplay(Scene* self) {
    Player* p = &self->player;

    // A: Turn Left 90 degrees
    if (IsKeyPressed(KEY_A)) {
        p->facing = (p->facing - 1 + 4) % 4; // Adding 4 ensures result is positive
        // TODO: Trigger "Turn Left" animation from assets
    }

    // D: Turn Right 90 degrees
    if (IsKeyPressed(KEY_D)) {
        p->facing = (p->facing + 1) % 4;
        // TODO: Trigger "Turn Right" animation from assets
    }

    // W: Move Forward (Dependent on Facing)
    if (IsKeyPressed(KEY_W)) {
        int targetX = p->x;
        int targetY = p->y;

        switch (p->facing) {
            case DIR_NORTH: targetY -= 1; break;
            case DIR_EAST:  targetX += 1; break;
            case DIR_SOUTH: targetY += 1; break;
            case DIR_WEST:  targetX -= 1; break;
        }

        if (IsValidMove(targetX, targetY, &self->map)) {
            p->x = targetX;
            p->y = targetY;
            // TODO: Trigger "Move Forward" animation
        } else {
            // TODO: Trigger "Bump/Blocked" sound or animation
        }
    }
}

// -- Gameplay Drawing --
void DrawGameplay(Scene* self) {
    // 1. Draw Map (Grid Iteration)
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            Tile t = self->map.tiles[x][y];
            Rectangle dest = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            DrawRectangleRec(dest, t.debugColor);
            DrawRectangleLinesEx(dest, 1, LIGHTGRAY); // Grid lines
        }
    }

    // 2. Draw Player
    // Calculate pixel position based on Grid X/Y
    Vector2 pixelPos = { 
        self->player.x * TILE_SIZE + (TILE_SIZE/2), 
        self->player.y * TILE_SIZE + (TILE_SIZE/2) 
    };
    
    // Draw Player Body (Circle for now)
    DrawCircleV(pixelPos, TILE_SIZE/3, MAROON);

    // Draw "Facing" Indicator (Small line showing direction)
    Vector2 endPos = pixelPos;
    if (self->player.facing == DIR_NORTH) endPos.y -= 20;
    if (self->player.facing == DIR_SOUTH) endPos.y += 20;
    if (self->player.facing == DIR_EAST)  endPos.x += 20;
    if (self->player.facing == DIR_WEST)  endPos.x -= 20;
    DrawLineEx(pixelPos, endPos, 3, YELLOW);
    
    DrawText("WASD Controls: W to Move, A/D to Turn", 10, 620, 20, DARKGRAY);
}

// -- Menu Logic (Simplified) --
void UpdateMenu(Scene* self) {
    // Just a placeholder to start game
    if (IsKeyPressed(KEY_ENTER)) {
        // In a real engine, you'd trigger a global state change here
        // For this demo, we are just handling the logic frame
    }
}

void DrawMenu(Scene* self) {
    DrawText("MAIN MENU", 300, 200, 40, DARKBLUE);
    DrawText("[ENTER] (Logic placeholder)", 280, 260, 20, GRAY);
}

// --------------------------------------------------------------------------------------
// 4. INITIALIZATION
// --------------------------------------------------------------------------------------

void InitGameplayScene(Scene* scene) {
    scene->type = 1;
    scene->Update = UpdateGameplay;
    scene->Draw = DrawGameplay;

    // Initialize Player
    scene->player.x = 5;
    scene->player.y = 5;
    scene->player.facing = DIR_NORTH;
    scene->player.hp = 100;

    // Initialize Map (Simple Checkerboard for debug)
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            scene->map.tiles[x][y].isWalkable = true;
            // Simple visual variation
            if ((x + y) % 2 == 0) scene->map.tiles[x][y].debugColor = LIGHTGRAY;
            else scene->map.tiles[x][y].debugColor = WHITE;
            
            // Add a "Wall" for testing collision
            if (x == 3 && y == 3) {
                scene->map.tiles[x][y].isWalkable = false;
                scene->map.tiles[x][y].debugColor = DARKGRAY;
            }
        }
    }
}

void InitMenuScene(Scene* scene) {
    scene->type = 0;
    scene->Update = UpdateMenu;
    scene->Draw = DrawMenu;
}

// --------------------------------------------------------------------------------------
// 5. MAIN
// --------------------------------------------------------------------------------------

int main(void) {
    InitWindow(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE + 50, "C Tile Game");
    SetTargetFPS(60);

    // Create the Active Scene
    Scene activeScene;
    
    // Initialize it as Gameplay for immediate testing
    InitGameplayScene(&activeScene);

    while (!WindowShouldClose()) {
        
        // Execute Logic
        if (activeScene.Update) activeScene.Update(&activeScene);

        // Execute Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (activeScene.Draw) activeScene.Draw(&activeScene);
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}