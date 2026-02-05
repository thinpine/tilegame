#include "renderer.h"
#include "resources.h" // Needs this to get the background images
#include <stdio.h>

bool GuiButton(Button btn) {
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, btn.rect);
    
    DrawRectangleRec(btn.rect, isHover ? LIGHTGRAY : btn.color);
    DrawRectangleLinesEx(btn.rect, 2, DARKGRAY);
    
    DrawCenteredText(btn.text, 
                     btn.rect.x + btn.rect.width/2, 
                     btn.rect.y + btn.rect.height/2 - 15, // -15 is half of font size 30
                     30, BLACK);
    
    return (isHover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
}

void DrawCenteredText(const char* text, int centerX, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, centerX - textWidth/2, y, fontSize, color);
}

void DrawLevelView(Scene* scene) {
    // 1. Get the background texture from Resources
    Texture2D tex = GetLevelTexture(scene->type);
    
    // 2. Draw it scaled to screen
    Rectangle sourceRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT };
    DrawTexturePro(tex, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
}

void DrawHUD(Scene* scene, int score) {
    char coordText[100];
    char* dirStrs[] = {"North", "East", "South", "West"};
    sprintf(coordText, "Lvl: %d | X: %d Y: %d | Facing: %s | Score: %d", 
            scene->type, scene->player.x, scene->player.y, dirStrs[scene->player.facing], score);

    int fontSize = 40;
    int textWidth = MeasureText(coordText, fontSize);
    int drawX = (SCR_WIDTH / 2) - (textWidth / 2);
    int drawY = 20;

    DrawRectangle(drawX - 20, drawY - 5, textWidth + 40, fontSize + 10, Fade(BLACK, 0.6f));
    DrawText(coordText, drawX, drawY, fontSize, RAYWHITE);
}