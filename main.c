#include "header.h"

/**
 * Main entry point for the River Raid game
 * Initializes Raylib, sets up the game, and runs the main game loop
 */
int main(void) {
    // Initialize Raylib window and OpenGL context
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "River Raid - Enhanced Edition");
    SetTargetFPS(FPS);
    
    // Initialize random seed
    srand((unsigned int)time(NULL));
    
    // Initialize game data structure
    GameData game = {0};
    InitializeGame(&game);
    
    // Load high score from file
    LoadHighScore(&game);
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update game logic based on current state
        UpdateGame(&game);
        
        // Begin drawing
        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);
        
        // Draw appropriate screen based on game state
        switch (game.state) {
            case GAME_STATE_MENU:
                DrawMenu(&game);
                break;
            case GAME_STATE_PLAYING:
                DrawGame(&game);
                DrawUI(&game);
                break;
            case GAME_STATE_PAUSED:
                DrawGame(&game);
                DrawUI(&game);
                // Draw pause overlay
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 128});
                DrawText("PAUSED", SCREEN_WIDTH/2 - MeasureText("PAUSED", 60)/2, SCREEN_HEIGHT/2 - 30, 60, COLOR_TEXT);
                DrawText("Press P to resume", SCREEN_WIDTH/2 - MeasureText("Press P to resume", 30)/2, SCREEN_HEIGHT/2 + 30, 30, COLOR_TEXT);
                break;
            case GAME_STATE_GAME_OVER:
                DrawGame(&game);
                DrawGameOver(&game);
                break;
            case GAME_STATE_LOGS:
                DrawLogs(&game);
                break;
            case GAME_STATE_DIFFICULTY:
                DrawDifficultyMenu(&game);
                break;
        }
        
        // Draw FPS counter if enabled
        if (game.showFPS) {
            DrawFPS(10, 10);
        }
        
        EndDrawing();
    }
    
    // Save final score if game was in progress
    if (game.state == GAME_STATE_PLAYING || game.state == GAME_STATE_PAUSED) {
        SaveScore(&game);
    }
    
    // Cleanup and close
    CloseWindow();
    return 0;
}