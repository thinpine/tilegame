#include "resources.h"
#include <stdio.h>

// Private storage for textures
// static means they are only visible in this file
static Texture2D levelTextures[5]; 

void LoadGameAssets(void) {
    levelTextures[1] = LoadTexture("assets/bg_residence.png");
    levelTextures[2] = LoadTexture("assets/bg_copse.png");
    levelTextures[3] = LoadTexture("assets/bg_hospital.png");
    levelTextures[4] = LoadTexture("assets/bg_dungeon.png");
    
    printf("Assets Loaded.\n");
}

void UnloadGameAssets(void) {
    for(int i=1; i<=4; i++) {
        UnloadTexture(levelTextures[i]);
    }
    printf("Assets Unloaded.\n");
}

Texture2D GetLevelTexture(int levelIndex) {
    if (levelIndex < 1 || levelIndex > 4) {
        // Return a default "error" texture or the first one to prevent crash
        return levelTextures[1]; 
    }
    return levelTextures[levelIndex];
}