#ifndef COREMECHANICS_H
#define COREMECHANICS_H

#include "types.h"

// Checks map boundaries
bool IsValidMove(int x, int y);

// Handles W/A/D input and updates player position
// Returns true if the player actually moved/turned
bool HandlePlayerMovement(PlayerState* p);

// Checks if player is standing on a point and updates map/score
void CheckPointCollection(Scene* scene, int* globalScore, GameMap* globalStoredMap);

#endif // COREMECHANICS_H