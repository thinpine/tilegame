#include "coremechanics.h"
#include <stdio.h>

bool IsValidMove(int x, int y) {
    return (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT);
}

bool HandlePlayerMovement(PlayerState* p) {
    bool inputDetected = false;

    // Turning
    if (IsKeyPressed(KEY_A)) {
        p->facing = (p->facing - 1 + 4) % 4;
        inputDetected = true;
    }
    if (IsKeyPressed(KEY_D)) {
        p->facing = (p->facing + 1) % 4;
        inputDetected = true;
    }

    // Moving
    if (IsKeyPressed(KEY_W)) {
        int tx = p->x; 
        int ty = p->y;
        
        switch (p->facing) {
            case DIR_NORTH: ty--; break;
            case DIR_EAST:  tx++; break;
            case DIR_SOUTH: ty++; break;
            case DIR_WEST:  tx--; break;
        }

        if (IsValidMove(tx, ty)) {
            p->x = tx;
            p->y = ty;
            inputDetected = true;
        }
    }
    return inputDetected;
}

void CheckPointCollection(Scene* scene, int* globalScore, GameMap* globalStoredMap) {
    int x = scene->player.x;
    int y = scene->player.y;

    if (scene->map.tiles[x][y].hasPoint && !scene->map.tiles[x][y].isClaimed) {
        // Update local scene map
        scene->map.tiles[x][y].isClaimed = true;
        // Update global persistence map
        globalStoredMap->tiles[x][y].isClaimed = true;
        
        (*globalScore)++;
        printf("Point collected! New Score: %d\n", *globalScore);
    }
}