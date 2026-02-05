#ifndef RESOURCES_H
#define RESOURCES_H

#include "raylib.h"

// Load all textures (Call once at startup)
void LoadGameAssets(void);

// Unload all textures (Call once at exit)
void UnloadGameAssets(void);

// Getters for specific assets
Texture2D GetLevelTexture(int levelIndex);
// You can add GetDaemonTexture(int id) here later

#endif // RESOURCES_H