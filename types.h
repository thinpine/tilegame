#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include "raylib.h"

// --------------------------------------------------------------------------------------
// CONSTANTS
// --------------------------------------------------------------------------------------
#define SCR_WIDTH 1200
#define SCR_HEIGHT 900
#define MAP_WIDTH 10
#define MAP_HEIGHT 10

// --------------------------------------------------------------------------------------
// ENUMS
// --------------------------------------------------------------------------------------
typedef enum { 
    DIR_NORTH=0, 
    DIR_EAST=1, 
    DIR_SOUTH=2, 
    DIR_WEST=3 
} Direction;

typedef enum { 
    SCENE_MENU_MAIN = 0,
    SCENE_LEVEL_1 = 1,
    SCENE_LEVEL_2 = 2,
    SCENE_LEVEL_3 = 3,
    SCENE_LEVEL_4 = 4,
    SCENE_MENU_PAUSE = 5
} SceneType;

// NEW: The State Machine for the game flow
typedef enum {
    STATE_EXPLORE,
    STATE_DIALOGUE,
    STATE_COMBAT
} GameState;

// --------------------------------------------------------------------------------------
// STRUCTS
// --------------------------------------------------------------------------------------

// -- UI --
typedef struct Button {
    Rectangle rect;
    const char* text;
    Color color;
} Button;

// -- MAP & TILES --
typedef struct Tile {
    bool hasPoint;
    bool isClaimed;
} Tile;

typedef struct GameMap {
    Tile tiles[MAP_WIDTH][MAP_HEIGHT];
} GameMap;

// -- PLAYER --
typedef struct PlayerState {
    int x, y;
    Direction facing;
} PlayerState;

// -- SCENE --
// Forward declaration so the function pointers inside can reference "Scene"
typedef struct Scene Scene; 

struct Scene {
    SceneType type;
    PlayerState player;
    GameMap map; 
    
    // Logic Pointers
    void (*Update)(Scene* self);
    void (*Draw)(Scene* self);
};

#endif // TYPES_H