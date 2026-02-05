#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

// Draws the immediate-mode UI button
bool GuiButton(Button btn);

// Draws the main 3D-style view
void DrawLevelView(Scene* scene);

// Draws the HUD (text, score, etc.)
void DrawHUD(Scene* scene, int score);

// Helper to center text
void DrawCenteredText(const char* text, int centerX, int y, int fontSize, Color color);

#endif // RENDERER_H