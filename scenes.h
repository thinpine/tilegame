#ifndef SCENES_H
#define SCENES_H

#include "types.h"

// Global System State
extern int globalScore;
extern bool gameShouldClose;

// Initializes the Scene System (maps, etc.)
void InitSceneSystem(void);

// The Main Scene Switcher
void ChangeScene(SceneType newType);

// Access to the active scene for Main loop
Scene* GetActiveScene(void);

#endif // SCENES_H