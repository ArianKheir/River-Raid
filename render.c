#include "header.h"

/**
 * Draw the main game screen with all game objects
 * @param game Pointer to the game data structure
 */
void DrawGame(GameData* game) {
    // Draw background with scrolling effect
    DrawBackground(game);
    
    // Draw player
    DrawPlayer(&game->player);
    
    // Draw enemies
    for (int i = 0; i < game->maxEnemies; i++) {
        if (game->enemies[i].isAlive) {
            DrawEnemy(&game->enemies[i]);
        }
    }
    
    // Draw bullets
    for (int i = 0; i < game->maxBullets; i++) {
        if (game->bullets[i].isAlive) {
            DrawBullet(&game->bullets[i]);
        }
    }
    
    // Draw powerups
    for (int i = 0; i < game->maxPowerups; i++) {
        if (game->powerups[i].isAlive) {
            DrawPowerup(&game->powerups[i]);
        }
    }
    
    // Draw particle effects
    DrawParticles(game);
}

/**
 * Draw the main menu screen
 * @param game Pointer to the game data structure
 */
void DrawMenu(GameData* game) {
    // Draw title
    const char* title = "RIVER RAID";
    const char* subtitle = "Enhanced Edition";
    int titleWidth = MeasureText(title, 80);
    int subtitleWidth = MeasureText(subtitle, 30);
    
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 150, 80, COLOR_TEXT);
    DrawText(subtitle, SCREEN_WIDTH/2 - subtitleWidth/2, 240, 30, COLOR_TEXT);
    
    // Draw menu options
    const char* options[] = {"PLAY", "LOGS", "DIFFICULTY", "EXIT"};
    int selectedOption = 0;
    
    // Handle selection logic (simplified for now)
    static int currentSelection = 0;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        currentSelection = (currentSelection - 1 + 4) % 4;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        currentSelection = (currentSelection + 1) % 4;
    }
    selectedOption = currentSelection;
    
    for (int i = 0; i < 4; i++) {
        Color textColor = (i == selectedOption) ? COLOR_PLAYER : COLOR_TEXT;
        const char* option = options[i];
        int textWidth = MeasureText(option, 40);
        
        // Draw selection indicator
        if (i == selectedOption) {
            DrawText("> ", SCREEN_WIDTH/2 - textWidth/2 - 30, 350 + i * 60, 40, COLOR_PLAYER);
            DrawText(" <", SCREEN_WIDTH/2 + textWidth/2 + 10, 350 + i * 60, 40, COLOR_PLAYER);
        }
        
        DrawText(option, SCREEN_WIDTH/2 - textWidth/2, 350 + i * 60, 40, textColor);
    }
    
    // Draw instructions
    const char* instructions = "Use W/S or UP/DOWN to navigate, ENTER to select";
    int instWidth = MeasureText(instructions, 20);
    DrawText(instructions, SCREEN_WIDTH/2 - instWidth/2, SCREEN_HEIGHT - 100, 20, COLOR_TEXT);
    
    // Draw high score
    char highScoreText[64];
    sprintf(highScoreText, "High Score: %d", game->highScore);
    int hsWidth = MeasureText(highScoreText, 25);
    DrawText(highScoreText, SCREEN_WIDTH/2 - hsWidth/2, SCREEN_HEIGHT - 50, 25, COLOR_TEXT);
}

/**
 * Draw the user interface overlay
 * @param game Pointer to the game data structure
 */
void DrawUI(GameData* game) {
    // Draw health bar
    DrawHealthBar(&game->player);
    
    // Draw score
    char scoreText[32];
    sprintf(scoreText, "Score: %d", game->player.score);
    DrawText(scoreText, 20, 20, 25, COLOR_TEXT);
    
    // Draw game time
    char timeText[32];
    sprintf(timeText, "Time: %02d:%02d", (int)game->gameTime / 60, (int)game->gameTime % 60);
    DrawText(timeText, 20, 50, 20, COLOR_TEXT);
    
    // Draw difficulty
    const char* diffText = GetDifficultyName(game->difficulty);
    DrawText(diffText, SCREEN_WIDTH - MeasureText(diffText, 20) - 20, 20, 20, COLOR_TEXT);
    
    // Draw bullet count (debug info)
    char bulletText[32];
    sprintf(bulletText, "Bullets: %d/%d", game->bulletCount, game->maxBullets);
    DrawText(bulletText, SCREEN_WIDTH - MeasureText(bulletText, 15) - 20, 45, 15, COLOR_TEXT);
    
    // Draw invulnerability indicator
    if (game->player.invulnerabilityTime > 0) {
        DrawText("INVULNERABLE", SCREEN_WIDTH/2 - MeasureText("INVULNERABLE", 30)/2, 100, 30, COLOR_PLAYER);
    }
}

/**
 * Draw the logs/high scores screen
 * @param game Pointer to the game data structure
 */
void DrawLogs(GameData* game) {
    (void)game; // Suppress unused parameter warning
    // Draw title
    const char* title = "GAME LOGS";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 50, 60, COLOR_TEXT);
    
    // Read and display scores from file
    const char* locations[] = {"scores.txt", "/app/scores.txt", "./scores.txt"};
    FILE* file = NULL;
    char* usedLocation = NULL;
    
    for (int i = 0; i < 3 && !file; i++) {
        file = fopen(locations[i], "r");
        if (file) {
            usedLocation = (char*)locations[i];
            break;
        }
    }
    if (file) {
        char line[256];
        int yPos = 150;
        int lineCount = 0;
        char displayLine[256];
        
        while (fgets(line, sizeof(line), file) && lineCount < 20) {
            // Remove newline character
            line[strcspn(line, "\n")] = 0;
            
            // Parse and format the line for better display
            int score, time;
            char diff[20], date[50];
            
            if (sscanf(line, "score = %d diff = %s time = %ds date = %[^\n]", 
                      &score, diff, &time, date) == 4) {
                // Format for display
                sprintf(displayLine, "Score: %d | %s | %ds | %s", score, diff, time, date);
            } else if (sscanf(line, "Score: %d", &score) == 1) {
                // New format
                strcpy(displayLine, line);
            } else {
                // Unknown format, use as is
                strcpy(displayLine, line);
            }
            
            // Truncate long lines
            if (strlen(displayLine) > 80) {
                displayLine[77] = '.';
                displayLine[78] = '.';
                displayLine[79] = '.';
                displayLine[80] = '\0';
            }
            
            DrawText(displayLine, 50, yPos, 18, COLOR_TEXT);
            yPos += 25;
            lineCount++;
        }
        fclose(file);
        
        // Show which file was used
        char infoText[100];
        sprintf(infoText, "Reading from: %s", usedLocation ? usedLocation : "unknown");
        DrawText(infoText, 50, SCREEN_HEIGHT - 80, 15, COLOR_TEXT);
        
        if (lineCount == 0) {
            DrawText("No scores recorded yet!", SCREEN_WIDTH/2 - MeasureText("No scores recorded yet!", 30)/2, 200, 30, COLOR_TEXT);
        }
    } else {
        DrawText("No scores file found!", SCREEN_WIDTH/2 - MeasureText("No scores file found!", 30)/2, 200, 30, COLOR_TEXT);
    }
    
    // Draw back instruction
    const char* backText = "Press ESC or BACKSPACE to return to menu";
    int backWidth = MeasureText(backText, 25);
    DrawText(backText, SCREEN_WIDTH/2 - backWidth/2, SCREEN_HEIGHT - 50, 25, COLOR_TEXT);
}

/**
 * Draw the difficulty selection menu
 * @param game Pointer to the game data structure
 */
void DrawDifficultyMenu(GameData* game) {
    (void)game; // Suppress unused parameter warning
    // Draw title
    const char* title = "SELECT DIFFICULTY";
    int titleWidth = MeasureText(title, 50);
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 100, 50, COLOR_TEXT);
    
    // Draw difficulty options
    const char* difficulties[] = {"EASY", "MEDIUM", "HARD", "NIGHTMARE"};
    static int selectedDifficulty = 0;
    
    // Handle selection logic
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedDifficulty = (selectedDifficulty - 1 + 4) % 4;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedDifficulty = (selectedDifficulty + 1) % 4;
    }
    
    for (int i = 0; i < 4; i++) {
        Color textColor = (i == selectedDifficulty) ? COLOR_PLAYER : COLOR_TEXT;
        const char* diff = difficulties[i];
        int textWidth = MeasureText(diff, 40);
        
        // Draw selection indicator
        if (i == selectedDifficulty) {
            DrawText("> ", SCREEN_WIDTH/2 - textWidth/2 - 30, 250 + i * 60, 40, COLOR_PLAYER);
            DrawText(" <", SCREEN_WIDTH/2 + textWidth/2 + 10, 250 + i * 60, 40, COLOR_PLAYER);
        }
        
        DrawText(diff, SCREEN_WIDTH/2 - textWidth/2, 250 + i * 60, 40, textColor);
    }
    
    // Draw difficulty description
    const char* descriptions[] = {
        "Enemies spawn slowly, easy to dodge",
        "Moderate enemy spawn rate and speed",
        "Fast enemies, frequent spawns",
        "Extreme difficulty - for experts only!"
    };
    
    int descWidth = MeasureText(descriptions[selectedDifficulty], 20);
    DrawText(descriptions[selectedDifficulty], SCREEN_WIDTH/2 - descWidth/2, 500, 20, COLOR_TEXT);
    
    // Draw instructions
    const char* instructions = "Press ENTER to confirm, ESC to cancel";
    int instWidth = MeasureText(instructions, 25);
    DrawText(instructions, SCREEN_WIDTH/2 - instWidth/2, SCREEN_HEIGHT - 50, 25, COLOR_TEXT);
}

/**
 * Draw the game over screen
 * @param game Pointer to the game data structure
 */
void DrawGameOver(GameData* game) {
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
    
    // Draw game over text
    const char* gameOverText = "GAME OVER";
    int goWidth = MeasureText(gameOverText, 80);
    DrawText(gameOverText, SCREEN_WIDTH/2 - goWidth/2, SCREEN_HEIGHT/2 - 150, 80, COLOR_TEXT);
    
    // Draw final score
    char scoreText[64];
    sprintf(scoreText, "Final Score: %d", game->player.score);
    int scoreWidth = MeasureText(scoreText, 40);
    DrawText(scoreText, SCREEN_WIDTH/2 - scoreWidth/2, SCREEN_HEIGHT/2 - 80, 40, COLOR_TEXT);
    
    // Draw time survived
    char timeText[64];
    sprintf(timeText, "Time Survived: %02d:%02d", (int)game->gameTime / 60, (int)game->gameTime % 60);
    int timeWidth = MeasureText(timeText, 30);
    DrawText(timeText, SCREEN_WIDTH/2 - timeWidth/2, SCREEN_HEIGHT/2 - 30, 30, COLOR_TEXT);
    
    // Draw difficulty
    char diffText[64];
    sprintf(diffText, "Difficulty: %s", GetDifficultyName(game->difficulty));
    int diffWidth = MeasureText(diffText, 25);
    DrawText(diffText, SCREEN_WIDTH/2 - diffWidth/2, SCREEN_HEIGHT/2 + 10, 25, COLOR_TEXT);
    
    // Draw high score comparison
    if (game->player.score >= game->highScore) {
        const char* newRecord = "NEW HIGH SCORE!";
        int nrWidth = MeasureText(newRecord, 35);
        DrawText(newRecord, SCREEN_WIDTH/2 - nrWidth/2, SCREEN_HEIGHT/2 + 50, 35, COLOR_PLAYER);
    } else {
        char hsText[64];
        sprintf(hsText, "High Score: %d", game->highScore);
        int hsWidth = MeasureText(hsText, 25);
        DrawText(hsText, SCREEN_WIDTH/2 - hsWidth/2, SCREEN_HEIGHT/2 + 50, 25, COLOR_TEXT);
    }
    
    // Draw restart instruction
    const char* restartText = "Press ENTER or SPACE to return to menu";
    int restartWidth = MeasureText(restartText, 25);
    DrawText(restartText, SCREEN_WIDTH/2 - restartWidth/2, SCREEN_HEIGHT/2 + 100, 25, COLOR_TEXT);
}

/**
 * Draw the player ship with beautiful detailed graphics
 * @param player Pointer to the player structure
 */
void DrawPlayer(Player* player) {
    Color playerColor = COLOR_PLAYER_PRIMARY;
    
    // Flash player when invulnerable
    if (player->invulnerabilityTime > 0) {
        if ((int)(player->invulnerabilityTime * 10) % 2 == 0) {
            playerColor = COLOR_PLAYER_ACCENT; // Flash white
        }
    }
    
    // Draw main ship body (detailed triangle)
    Vector2 points[3] = {
        {player->position.x, player->position.y - PLAYER_SIZE},
        {player->position.x - PLAYER_SIZE * 0.7f, player->position.y + PLAYER_SIZE * 0.5f},
        {player->position.x + PLAYER_SIZE * 0.7f, player->position.y + PLAYER_SIZE * 0.5f}
    };
    
    // Draw ship body with gradient effect
    DrawTriangle(points[0], points[1], points[2], playerColor);
    DrawTriangleLines(points[0], points[1], points[2], COLOR_PLAYER_ACCENT);
    
    // Draw cockpit (small circle)
    DrawCircle(player->position.x, player->position.y - PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.2f, COLOR_PLAYER_SECONDARY);
    DrawCircleLines(player->position.x, player->position.y - PLAYER_SIZE * 0.3f, PLAYER_SIZE * 0.2f, COLOR_PLAYER_ACCENT);
    
    // Draw wings with detail
    Vector2 leftWing[3] = {
        {player->position.x - PLAYER_SIZE * 0.7f, player->position.y + PLAYER_SIZE * 0.5f},
        {player->position.x - PLAYER_SIZE * 1.2f, player->position.y + PLAYER_SIZE * 0.3f},
        {player->position.x - PLAYER_SIZE * 0.8f, player->position.y + PLAYER_SIZE * 0.1f}
    };
    
    Vector2 rightWing[3] = {
        {player->position.x + PLAYER_SIZE * 0.7f, player->position.y + PLAYER_SIZE * 0.5f},
        {player->position.x + PLAYER_SIZE * 1.2f, player->position.y + PLAYER_SIZE * 0.3f},
        {player->position.x + PLAYER_SIZE * 0.8f, player->position.y + PLAYER_SIZE * 0.1f}
    };
    
    DrawTriangle(leftWing[0], leftWing[1], leftWing[2], COLOR_PLAYER_SECONDARY);
    DrawTriangle(rightWing[0], rightWing[1], rightWing[2], COLOR_PLAYER_SECONDARY);
    DrawTriangleLines(leftWing[0], leftWing[1], leftWing[2], COLOR_PLAYER_ACCENT);
    DrawTriangleLines(rightWing[0], rightWing[1], rightWing[2], COLOR_PLAYER_ACCENT);
    
    // Draw engine exhaust with animated effect
    float time = GetTime();
    float exhaustLength = 15.0f + 5.0f * sinf(time * 10.0f);
    
    Vector2 exhaust[2] = {
        {player->position.x - PLAYER_SIZE * 0.3f, player->position.y + PLAYER_SIZE * 0.5f},
        {player->position.x + PLAYER_SIZE * 0.3f, player->position.y + PLAYER_SIZE * 0.5f}
    };
    
    for (int i = 0; i < 2; i++) {
        // Main exhaust
        DrawLineEx(exhaust[i], (Vector2){exhaust[i].x, exhaust[i].y + exhaustLength}, 4, COLOR_PLAYER_ENGINE);
        // Secondary exhaust
        DrawLineEx(exhaust[i], (Vector2){exhaust[i].x, exhaust[i].y + exhaustLength * 0.6f}, 2, COLOR_BULLET_SPECIAL);
    }
    
    // Draw weapon ports
    DrawCircle(player->position.x - PLAYER_SIZE * 0.4f, player->position.y - PLAYER_SIZE * 0.1f, 3, COLOR_PLAYER_ACCENT);
    DrawCircle(player->position.x + PLAYER_SIZE * 0.4f, player->position.y - PLAYER_SIZE * 0.1f, 3, COLOR_PLAYER_ACCENT);
}

/**
 * Draw an enemy ship with beautiful pixel art spaceship designs
 * @param enemy Pointer to the enemy structure
 */
void DrawEnemy(Enemy* enemy) {
    Color enemyColor = COLOR_ENEMY_BASIC;
    Color accentColor = COLOR_PLAYER_ACCENT;
    Color detailColor = COLOR_PLAYER_ACCENT;
    
    // Animated scaling effect
    float scale = 1.0f + 0.05f * sinf(enemy->animationTime * 3.0f);
    float size = ENEMY_SIZE * scale;
    float pixelSize = size / 8.0f; // 8x8 pixel grid
    
    // Different pixel art designs for different enemy types
    switch (enemy->type) {
        case 0: // Basic Fighter - Simple red spaceship
            enemyColor = COLOR_ENEMY_BASIC;
            accentColor = (Color){255, 200, 200, 255};
            detailColor = (Color){200, 50, 50, 255};
            
            // Main body (pixel art)
            DrawRectangle(enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 3, 
                         pixelSize * 4, pixelSize * 6, enemyColor);
            
            // Nose cone
            DrawTriangle((Vector2){enemy->position.x, enemy->position.y - pixelSize * 4},
                        (Vector2){enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 2},
                        (Vector2){enemy->position.x + pixelSize * 2, enemy->position.y - pixelSize * 2}, detailColor);
            
            // Wings
            DrawRectangle(enemy->position.x - pixelSize * 4, enemy->position.y - pixelSize * 1, 
                         pixelSize * 2, pixelSize * 3, accentColor);
            DrawRectangle(enemy->position.x + pixelSize * 2, enemy->position.y - pixelSize * 1, 
                         pixelSize * 2, pixelSize * 3, accentColor);
            
            // Cockpit
            DrawRectangle(enemy->position.x - pixelSize, enemy->position.y - pixelSize * 2, 
                         pixelSize * 2, pixelSize * 2, detailColor);
            break;
            
        case 1: // Shooter - Light red with weapon pods
            enemyColor = COLOR_ENEMY_SHOOTER;
            accentColor = (Color){255, 150, 150, 255};
            detailColor = (Color){180, 80, 80, 255};
            
            // Main body
            DrawRectangle(enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 3, 
                         pixelSize * 4, pixelSize * 6, enemyColor);
            
            // Weapon pods (side turrets)
            DrawCircle(enemy->position.x - pixelSize * 3, enemy->position.y - pixelSize * 1, pixelSize * 1.5f, accentColor);
            DrawCircle(enemy->position.x + pixelSize * 3, enemy->position.y - pixelSize * 1, pixelSize * 1.5f, accentColor);
            DrawCircleLines(enemy->position.x - pixelSize * 3, enemy->position.y - pixelSize * 1, pixelSize * 1.5f, detailColor);
            DrawCircleLines(enemy->position.x + pixelSize * 3, enemy->position.y - pixelSize * 1, pixelSize * 1.5f, detailColor);
            
            // Nose
            DrawTriangle((Vector2){enemy->position.x, enemy->position.y - pixelSize * 4},
                        (Vector2){enemy->position.x - pixelSize * 1.5f, enemy->position.y - pixelSize * 2},
                        (Vector2){enemy->position.x + pixelSize * 1.5f, enemy->position.y - pixelSize * 2}, detailColor);
            
            // Engine exhaust
            DrawRectangle(enemy->position.x - pixelSize, enemy->position.y + pixelSize * 3, 
                         pixelSize * 2, pixelSize * 1, (Color){255, 100, 0, 255});
            break;
            
        case 2: // Fast Interceptor - Sleek magenta design
            enemyColor = COLOR_ENEMY_FAST;
            accentColor = (Color){255, 150, 255, 255};
            detailColor = (Color){200, 100, 200, 255};
            
            // Sleek body
            DrawTriangle((Vector2){enemy->position.x, enemy->position.y - pixelSize * 4},
                        (Vector2){enemy->position.x - pixelSize * 3, enemy->position.y + pixelSize * 2},
                        (Vector2){enemy->position.x + pixelSize * 3, enemy->position.y + pixelSize * 2}, enemyColor);
            
            // Wings (swept back)
            DrawTriangle((Vector2){enemy->position.x - pixelSize * 3, enemy->position.y + pixelSize * 2},
                        (Vector2){enemy->position.x - pixelSize * 5, enemy->position.y + pixelSize * 1},
                        (Vector2){enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 1}, accentColor);
            DrawTriangle((Vector2){enemy->position.x + pixelSize * 3, enemy->position.y + pixelSize * 2},
                        (Vector2){enemy->position.x + pixelSize * 5, enemy->position.y + pixelSize * 1},
                        (Vector2){enemy->position.x + pixelSize * 2, enemy->position.y - pixelSize * 1}, accentColor);
            
            // Cockpit
            DrawRectangle(enemy->position.x - pixelSize, enemy->position.y - pixelSize * 2, 
                         pixelSize * 2, pixelSize * 2, detailColor);
            
            // Engine trails
            DrawRectangle(enemy->position.x - pixelSize * 1.5f, enemy->position.y + pixelSize * 2, 
                         pixelSize * 3, pixelSize * 2, (Color){255, 0, 255, 150});
            break;
            
        case 3: // Elite Destroyer - Gold armored design
            enemyColor = COLOR_ENEMY_ELITE;
            accentColor = (Color){255, 255, 200, 255};
            detailColor = (Color){200, 200, 100, 255};
            
            // Main armored body
            DrawRectangle(enemy->position.x - pixelSize * 3, enemy->position.y - pixelSize * 4, 
                         pixelSize * 6, pixelSize * 8, enemyColor);
            
            // Armor plates
            DrawRectangle(enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 3, 
                         pixelSize * 4, pixelSize * 2, accentColor);
            DrawRectangle(enemy->position.x - pixelSize * 2, enemy->position.y + pixelSize * 1, 
                         pixelSize * 4, pixelSize * 2, accentColor);
            
            // Weapon turrets
            DrawCircle(enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 2, pixelSize * 1.2f, detailColor);
            DrawCircle(enemy->position.x + pixelSize * 2, enemy->position.y - pixelSize * 2, pixelSize * 1.2f, detailColor);
            DrawCircle(enemy->position.x, enemy->position.y + pixelSize * 2, pixelSize * 1.5f, detailColor);
            
            // Bridge
            DrawRectangle(enemy->position.x - pixelSize * 1.5f, enemy->position.y - pixelSize * 1, 
                         pixelSize * 3, pixelSize * 2, detailColor);
            break;
            
        case 4: // Boss Battleship - Massive purple design
            enemyColor = COLOR_ENEMY_BOSS;
            accentColor = (Color){200, 150, 255, 255};
            detailColor = (Color){150, 100, 200, 255};
            
            // Massive main body
            DrawRectangle(enemy->position.x - pixelSize * 5, enemy->position.y - pixelSize * 6, 
                         pixelSize * 10, pixelSize * 12, enemyColor);
            
            // Command bridge
            DrawRectangle(enemy->position.x - pixelSize * 2, enemy->position.y - pixelSize * 7, 
                         pixelSize * 4, pixelSize * 3, accentColor);
            
            // Heavy weapon turrets
            DrawCircle(enemy->position.x - pixelSize * 4, enemy->position.y - pixelSize * 4, pixelSize * 2, detailColor);
            DrawCircle(enemy->position.x + pixelSize * 4, enemy->position.y - pixelSize * 4, pixelSize * 2, detailColor);
            DrawCircle(enemy->position.x - pixelSize * 3, enemy->position.y + pixelSize * 3, pixelSize * 2, detailColor);
            DrawCircle(enemy->position.x + pixelSize * 3, enemy->position.y + pixelSize * 3, pixelSize * 2, detailColor);
            DrawCircle(enemy->position.x, enemy->position.y + pixelSize * 4, pixelSize * 2.5f, detailColor);
            
            // Engine section
            DrawRectangle(enemy->position.x - pixelSize * 3, enemy->position.y + pixelSize * 6, 
                         pixelSize * 6, pixelSize * 2, accentColor);
            
            // Engine exhaust
            for (int i = 0; i < 3; i++) {
                DrawRectangle(enemy->position.x - pixelSize * 2 + i * pixelSize * 2, enemy->position.y + pixelSize * 8, 
                             pixelSize, pixelSize * 2, (Color){255, 100, 0, 255});
            }
            break;
    }
    
    // Draw health bar for enemies with more health
    if (enemy->health < 50) {
        float healthPercent = enemy->health / 50.0f;
        DrawRectangle(enemy->position.x - size, enemy->position.y - size - 10, 
                     size * 2 * healthPercent, 3, COLOR_POWERUP);
        DrawRectangleLines(enemy->position.x - size, enemy->position.y - size - 10, 
                          size * 2, 3, accentColor);
    } else if (enemy->health >= 80) {
        // Boss health bar
        float healthPercent = enemy->health / 150.0f;
        DrawRectangle(enemy->position.x - size * 1.2f, enemy->position.y - size - 15, 
                     size * 2.4f * healthPercent, 5, COLOR_ENEMY);
        DrawRectangleLines(enemy->position.x - size * 1.2f, enemy->position.y - size - 15, 
                          size * 2.4f, 5, accentColor);
    }
}

/**
 * Draw a bullet with beautiful trail effects
 * @param bullet Pointer to the bullet structure
 */
void DrawBullet(Bullet* bullet) {
    Color bulletColor = bullet->isPlayerBullet ? COLOR_BULLET_PLAYER : COLOR_BULLET_ENEMY;
    Color trailColor = bullet->isPlayerBullet ? COLOR_BULLET_SPECIAL : COLOR_BULLET_ENEMY;
    
    // Draw bullet trail (longer for faster bullets)
    float trailLength = 20.0f + 10.0f * sinf(bullet->trailTime * 5.0f);
    Vector2 trailStart = {
        bullet->position.x - bullet->velocity.x * trailLength / BULLET_SPEED,
        bullet->position.y - bullet->velocity.y * trailLength / BULLET_SPEED
    };
    
    // Main trail
    DrawLineEx(trailStart, bullet->position, 3, trailColor);
    
    // Secondary trail
    Vector2 trailStart2 = {
        bullet->position.x - bullet->velocity.x * trailLength * 0.5f / BULLET_SPEED,
        bullet->position.y - bullet->velocity.y * trailLength * 0.5f / BULLET_SPEED
    };
    DrawLineEx(trailStart2, bullet->position, 1, bulletColor);
    
    // Bullet core
    DrawCircle(bullet->position.x, bullet->position.y, BULLET_SIZE, bulletColor);
    DrawCircleLines(bullet->position.x, bullet->position.y, BULLET_SIZE, COLOR_PLAYER_ACCENT);
    
    // Glow effect
    DrawCircle(bullet->position.x, bullet->position.y, BULLET_SIZE + 2, (Color){bulletColor.r, bulletColor.g, bulletColor.b, 100});
}

/**
 * Draw a powerup with beautiful effects
 * @param powerup Pointer to the powerup structure
 */
void DrawPowerup(PowerUp* powerup) {
    Color powerupColor = COLOR_POWERUP;
    
    // Different colors for different powerup types
    switch (powerup->type) {
        case 0: // Health
            powerupColor = (Color){0, 255, 0, 255}; // Green
            break;
        case 1: // Speed
            powerupColor = (Color){0, 255, 255, 255}; // Cyan
            break;
        case 2: // Damage
            powerupColor = (Color){255, 255, 0, 255}; // Yellow
            break;
    }
    
    // Draw powerup with pulsing and rotating effect
    float pulse = 1.0f + 0.3f * sinf(GetTime() * 5.0f);
    float size = POWERUP_SIZE * pulse;
    
    // Outer glow
    DrawCircle(powerup->position.x, powerup->position.y, size + 4, (Color){powerupColor.r, powerupColor.g, powerupColor.b, 100});
    
    // Main powerup
    DrawCircle(powerup->position.x, powerup->position.y, size, powerupColor);
    DrawCircleLines(powerup->position.x, powerup->position.y, size, COLOR_PLAYER_ACCENT);
    
    // Inner detail
    DrawCircle(powerup->position.x, powerup->position.y, size * 0.6f, (Color){255, 255, 255, 150});
    
    // Powerup symbol with rotation
    const char* symbols[] = {"H", "S", "D"};
    // float rotation = GetTime() * 2.0f; // Unused variable - commented out
    DrawText(symbols[powerup->type], 
             powerup->position.x - MeasureText(symbols[powerup->type], 15)/2,
             powerup->position.y - 7, 15, COLOR_TEXT);
}

/**
 * Draw the health bar
 * @param player Pointer to the player structure
 */
void DrawHealthBar(Player* player) {
    float healthPercent = player->health / PLAYER_MAX_HEALTH;
    int barWidth = 200;
    int barHeight = 20;
    int x = SCREEN_WIDTH - barWidth - 20;
    int y = 20;
    
    // Draw background
    DrawRectangle(x, y, barWidth, barHeight, COLOR_UI);
    DrawRectangleLines(x, y, barWidth, barHeight, COLOR_TEXT);
    
    // Draw health
    Color healthColor = (healthPercent > 0.5f) ? COLOR_POWERUP : 
                       (healthPercent > 0.25f) ? COLOR_BULLET : COLOR_ENEMY;
    DrawRectangle(x + 2, y + 2, (barWidth - 4) * healthPercent, barHeight - 4, healthColor);
    
    // Draw health text
    char healthText[16];
    sprintf(healthText, "Health: %.0f", player->health);
    DrawText(healthText, x, y + barHeight + 5, 18, COLOR_TEXT);
}

/**
 * Draw the background with scrolling effect
 * @param game Pointer to the game data structure
 */
void DrawBackground(GameData* game) {
    // Draw stars (simple scrolling effect)
    for (int i = 0; i < 100; i++) {
        int x = (i * 37) % SCREEN_WIDTH;
        int y = ((int)(game->gameTime * 50) + i * 23) % SCREEN_HEIGHT;
        
        Color starColor = (i % 3 == 0) ? COLOR_TEXT : (Color){100, 100, 100, 255};
        DrawPixel(x, y, starColor);
    }
    
    // Draw river banks (simple lines)
    DrawLine(0, 0, SCREEN_WIDTH, 0, (Color){50, 100, 50, 255});
    DrawLine(0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1, (Color){50, 100, 50, 255});
}

/**
 * Draw particle effects (placeholder for future enhancement)
 * @param game Pointer to the game data structure
 */
void DrawParticles(GameData* game) {
    (void)game; // Suppress unused parameter warning
    // This function can be enhanced with explosion effects, engine trails, etc.
    // For now, it's a placeholder for future particle system implementation
} 