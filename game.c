#include "header.h"

/**
 * Initialize the game data structure with default values
 * @param game Pointer to the game data structure
 */
void InitializeGame(GameData* game) {
    // Set initial game state
    game->state = GAME_STATE_MENU;
    game->difficulty = DIFFICULTY_EASY;
    game->showFPS = false;
    
    // Initialize player
    game->player.position = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 100.0f};
    game->player.velocity = (Vector2){0, 0};
    game->player.health = PLAYER_MAX_HEALTH;
    game->player.score = 0;
    game->player.invulnerabilityTime = 0;
    game->player.isAlive = true;
    
    // Initialize arrays
    game->maxEnemies = 20;
    game->enemies = (Enemy*)malloc(game->maxEnemies * sizeof(Enemy));
    game->enemyCount = 0;
    
    game->maxBullets = 100;
    game->bullets = (Bullet*)malloc(game->maxBullets * sizeof(Bullet));
    game->bulletCount = 0;
    
    game->maxPowerups = 10;
    game->powerups = (PowerUp*)malloc(game->maxPowerups * sizeof(PowerUp));
    game->powerupCount = 0;
    
    // Initialize timers
    game->gameTime = 0;
    game->enemySpawnTimer = 0;
    game->powerupSpawnTimer = 0;
    game->highScore = 0;
    
    // Initialize all enemies as inactive
    for (int i = 0; i < game->maxEnemies; i++) {
        game->enemies[i].isAlive = false;
    }
    
    // Initialize all bullets as inactive
    for (int i = 0; i < game->maxBullets; i++) {
        game->bullets[i].isAlive = false;
    }
    
    // Initialize all powerups as inactive
    for (int i = 0; i < game->maxPowerups; i++) {
        game->powerups[i].isAlive = false;
    }
}

/**
 * Update game logic based on current state
 * @param game Pointer to the game data structure
 */
void UpdateGame(GameData* game) {
    float deltaTime = GetFrameTime();
    
    switch (game->state) {
        case GAME_STATE_MENU:
            UpdateMenu(game);
            break;
        case GAME_STATE_PLAYING:
            game->gameTime += deltaTime;
            UpdatePlayer(game);
            UpdateEnemies(game);
            UpdateBullets(game);
            UpdatePowerups(game);
            CheckCollisions(game);
            
            // Spawn enemies periodically
            game->enemySpawnTimer += deltaTime;
            float spawnInterval = 2.0f / GetDifficultyMultiplier(game->difficulty);
            if (game->enemySpawnTimer >= spawnInterval) {
                SpawnEnemy(game);
                game->enemySpawnTimer = 0;
            }
            
            // Spawn powerups periodically
            game->powerupSpawnTimer += deltaTime;
            if (game->powerupSpawnTimer >= 15.0f) {
                SpawnPowerup(game);
                game->powerupSpawnTimer = 0;
            }
            
            // Check for game over
            if (!game->player.isAlive) {
                LogGameEnd(game);
                game->state = GAME_STATE_GAME_OVER;
                SaveScore(game);
            }
            break;
        case GAME_STATE_PAUSED:
            // Handle pause menu input
            if (IsKeyPressed(KEY_P)) {
                game->state = GAME_STATE_PLAYING;
            }
            break;
        case GAME_STATE_GAME_OVER:
            UpdateGameOver(game);
            break;
        case GAME_STATE_LOGS:
            UpdateLogs(game);
            break;
        case GAME_STATE_DIFFICULTY:
            UpdateDifficultyMenu(game);
            break;
    }
}

/**
 * Update menu input handling
 * @param game Pointer to the game data structure
 */
void UpdateMenu(GameData* game) {
    static int selectedOption = 0;
    static float inputDelay = 0;
    
    inputDelay += GetFrameTime();
    if (inputDelay < 0.15f) return; // Prevent too fast input
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption = (selectedOption - 1 + 4) % 4;
        inputDelay = 0;
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption = (selectedOption + 1) % 4;
        inputDelay = 0;
    }
    else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        switch (selectedOption) {
            case 0: // Play
                ResetGame(game);
                game->state = GAME_STATE_PLAYING;
                break;
            case 1: // Logs
                game->state = GAME_STATE_LOGS;
                break;
            case 2: // Difficulty
                game->state = GAME_STATE_DIFFICULTY;
                break;
            case 3: // Exit
                CloseWindow();
                break;
        }
        inputDelay = 0;
    }
}

/**
 * Update player movement and actions
 * @param game Pointer to the game data structure
 */
void UpdatePlayer(GameData* game) {
    Player* player = &game->player;
    
    // Reset velocity
    player->velocity = (Vector2){0, 0};
    
    // Handle input
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->velocity.x = -PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->velocity.x = PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->velocity.y = -PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->velocity.y = PLAYER_SPEED;
    }
    
    // Update position
    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;
    
    // Keep player within screen bounds
    if (player->position.x < PLAYER_SIZE) player->position.x = PLAYER_SIZE;
    if (player->position.x > SCREEN_WIDTH - PLAYER_SIZE) player->position.x = SCREEN_WIDTH - PLAYER_SIZE;
    if (player->position.y < PLAYER_SIZE) player->position.y = PLAYER_SIZE;
    if (player->position.y > SCREEN_HEIGHT - PLAYER_SIZE) player->position.y = SCREEN_HEIGHT - PLAYER_SIZE;
    
    // Handle shooting
    static float lastShotTime = 0;
    float currentTime = GetTime();
    if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_X)) && currentTime - lastShotTime > 0.2f) {
        // Find an available bullet slot
        bool bulletCreated = false;
        for (int i = 0; i < game->maxBullets && !bulletCreated; i++) {
            if (!game->bullets[i].isAlive) {
                Bullet* bullet = &game->bullets[i];
                bullet->position = (Vector2){player->position.x, player->position.y - PLAYER_SIZE};
                bullet->velocity = (Vector2){0, -BULLET_SPEED};
                bullet->damage = BULLET_DAMAGE;
                bullet->isPlayerBullet = true;
                bullet->isAlive = true;
                bullet->trailTime = 0;
                bullet->spawnTime = GetTime();
                bulletCreated = true;
            }
        }
        if (!bulletCreated) {
            // No bullet slots available!
        }
        lastShotTime = currentTime;
    }
    
    // Update invulnerability
    if (player->invulnerabilityTime > 0) {
        player->invulnerabilityTime -= GetFrameTime();
    }
    
    // Pause game
    if (IsKeyPressed(KEY_P)) {
        game->state = GAME_STATE_PAUSED;
    }
}

/**
 * Update enemy movement and behavior
 * @param game Pointer to the game data structure
 */
void UpdateEnemies(GameData* game) {
    float deltaTime = GetFrameTime();
    
    for (int i = 0; i < game->maxEnemies; i++) {
        Enemy* enemy = &game->enemies[i];
        if (!enemy->isAlive) continue;
        
        // Update position
        enemy->position.x += enemy->velocity.x * deltaTime;
        enemy->position.y += enemy->velocity.y * deltaTime;
        
        // Update animation time
        enemy->animationTime += deltaTime;
        
        // Remove enemies that go off screen
        if (enemy->position.y > SCREEN_HEIGHT + ENEMY_SIZE || 
            enemy->position.x < -ENEMY_SIZE || 
            enemy->position.x > SCREEN_WIDTH + ENEMY_SIZE) {
            enemy->isAlive = false;
            game->enemyCount--;
        }
        
        // Enemy shooting (only for certain enemy types)
        if ((enemy->type == 1 || enemy->type == 3 || enemy->type == 4) && GetTime() - enemy->lastShotTime > 3.0f) {
            // Find an available bullet slot
            bool enemyBulletCreated = false;
            for (int j = 0; j < game->maxBullets && !enemyBulletCreated; j++) {
                if (!game->bullets[j].isAlive) {
                    Bullet* bullet = &game->bullets[j];
                    bullet->position = enemy->position;
                    bullet->velocity = (Vector2){0, BULLET_SPEED * 0.5f};
                    bullet->damage = enemy->damage;
                    bullet->isPlayerBullet = false;
                    bullet->isAlive = true;
                    bullet->trailTime = 0;
                    bullet->spawnTime = GetTime();
                    enemy->lastShotTime = GetTime();
                    enemyBulletCreated = true;
                }
            }
        }
    }
}

/**
 * Update bullet movement
 * @param game Pointer to the game data structure
 */
void UpdateBullets(GameData* game) {
    float deltaTime = GetFrameTime();
    float currentTime = GetTime();
    
    // Reset bullet count and recount active bullets
    game->bulletCount = 0;
    int bulletsTerminated = 0;
    
    for (int i = 0; i < game->maxBullets; i++) {
        Bullet* bullet = &game->bullets[i];
        if (!bullet->isAlive) continue;
        
        // Update position
        bullet->position.x += bullet->velocity.x * deltaTime;
        bullet->position.y += bullet->velocity.y * deltaTime;
        
        // Update trail time
        bullet->trailTime += deltaTime;
        
        bool shouldTerminate = false;
        char* reason = "unknown";
        
        // Check if bullet has exceeded lifetime
        if (currentTime - bullet->spawnTime > BULLET_LIFETIME) {
            shouldTerminate = true;
            reason = "lifetime expired";
        }
        // Remove bullets that go off screen
        else if (bullet->position.y < -BULLET_SIZE) {
            shouldTerminate = true;
            reason = "off screen top";
        }
        else if (bullet->position.y > SCREEN_HEIGHT + BULLET_SIZE) {
            shouldTerminate = true;
            reason = "off screen bottom";
        }
        else if (bullet->position.x < -BULLET_SIZE) {
            shouldTerminate = true;
            reason = "off screen left";
        }
        else if (bullet->position.x > SCREEN_WIDTH + BULLET_SIZE) {
            shouldTerminate = true;
            reason = "off screen right";
        }
        
        if (shouldTerminate) {
            bullet->isAlive = false;
            bulletsTerminated++;
        } else {
            // Count this bullet as active
            game->bulletCount++;
        }
    }
    
    // Emergency cleanup: if too many bullets are active, force cleanup
    if (game->bulletCount > game->maxBullets * 0.8f) {
        for (int i = 0; i < game->maxBullets; i++) {
            if (game->bullets[i].isAlive && currentTime - game->bullets[i].spawnTime > BULLET_LIFETIME * 0.5f) {
                game->bullets[i].isAlive = false;
            }
        }
    }
}

/**
 * Update powerup behavior
 * @param game Pointer to the game data structure
 */
void UpdatePowerups(GameData* game) {
    for (int i = 0; i < game->maxPowerups; i++) {
        PowerUp* powerup = &game->powerups[i];
        if (!powerup->isAlive) continue;
        
        // Remove powerups that have been on screen too long
        if (GetTime() - powerup->spawnTime > POWERUP_DURATION) {
            powerup->isAlive = false;
            game->powerupCount--;
        }
    }
}

/**
 * Spawn a new enemy
 * @param game Pointer to the game data structure
 */
void SpawnEnemy(GameData* game) {
    if (game->enemyCount >= game->maxEnemies) return;
    
    // Find an inactive enemy slot
    for (int i = 0; i < game->maxEnemies; i++) {
        if (!game->enemies[i].isAlive) {
            Enemy* enemy = &game->enemies[i];
            
            // Random enemy type and position (now includes new enemy types)
            enemy->type = GetRandomValue(0, 4);
            enemy->position.x = GetRandomValue(ENEMY_SIZE, SCREEN_WIDTH - ENEMY_SIZE);
            enemy->position.y = -ENEMY_SIZE;
            
            // Set enemy properties based on type
            switch (enemy->type) {
                case 0: // Basic enemy
                    enemy->health = 30;
                    enemy->damage = 20;
                    enemy->velocity = (Vector2){0, ENEMY_SPEED_MIN + GetRandomValue(0, 100) / 100.0f * (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN)};
                    enemy->animationTime = 0;
                    break;
                case 1: // Shooter enemy
                    enemy->health = 50;
                    enemy->damage = 15;
                    enemy->velocity = (Vector2){0, ENEMY_SPEED_MIN * 0.7f};
                    enemy->lastShotTime = GetTime();
                    enemy->animationTime = 0;
                    break;
                case 2: // Fast enemy
                    enemy->health = 20;
                    enemy->damage = 30;
                    enemy->velocity = (Vector2){0, ENEMY_SPEED_MAX * 1.5f};
                    enemy->animationTime = 0;
                    break;
                case 3: // Elite enemy (new!)
                    enemy->health = 80;
                    enemy->damage = 25;
                    enemy->velocity = (Vector2){0, ENEMY_SPEED_MIN * 0.8f};
                    enemy->lastShotTime = GetTime();
                    enemy->animationTime = 0;
                    break;
                case 4: // Boss enemy (new!)
                    enemy->health = 150;
                    enemy->damage = 40;
                    enemy->velocity = (Vector2){0, ENEMY_SPEED_MIN * 0.5f};
                    enemy->lastShotTime = GetTime();
                    enemy->animationTime = 0;
                    break;
            }
            
            enemy->isAlive = true;
            game->enemyCount++;
            break;
        }
    }
}

/**
 * Spawn a new powerup
 * @param game Pointer to the game data structure
 */
void SpawnPowerup(GameData* game) {
    if (game->powerupCount >= game->maxPowerups) return;
    
    // Find an inactive powerup slot
    for (int i = 0; i < game->maxPowerups; i++) {
        if (!game->powerups[i].isAlive) {
            PowerUp* powerup = &game->powerups[i];
            
            powerup->type = GetRandomValue(0, 2);
            powerup->position.x = GetRandomValue(POWERUP_SIZE, SCREEN_WIDTH - POWERUP_SIZE);
            powerup->position.y = -POWERUP_SIZE;
            powerup->isAlive = true;
            powerup->spawnTime = GetTime();
            
            game->powerupCount++;
            break;
        }
    }
}

/**
 * Check for collisions between game objects
 * @param game Pointer to the game data structure
 */
void CheckCollisions(GameData* game) {
    Player* player = &game->player;
    
    // Check player-bullet collisions
    for (int i = 0; i < game->maxBullets; i++) {
        Bullet* bullet = &game->bullets[i];
        if (!bullet->isAlive || bullet->isPlayerBullet) continue;
        
        if (CheckCollisionCircles(player->position, PLAYER_SIZE, bullet->position, BULLET_SIZE)) {
            if (player->invulnerabilityTime <= 0) {
                player->health -= bullet->damage;
                player->invulnerabilityTime = 2.0f; // 2 seconds of invulnerability
                
                if (player->health <= 0) {
                    player->health = 0;
                    player->isAlive = false;
                    
                    // Update high score if current score is higher
                    if (player->score > game->highScore) {
                        game->highScore = player->score;
                    }
                    
                    // Save score immediately when player dies
                    SaveScore(game);
                }
            }
            bullet->isAlive = false;
        }
    }
    
    // Check player-enemy collisions
    for (int i = 0; i < game->maxEnemies; i++) {
        Enemy* enemy = &game->enemies[i];
        if (!enemy->isAlive) continue;
        
        if (CheckCollisionCircles(player->position, PLAYER_SIZE, enemy->position, ENEMY_SIZE)) {
            if (player->invulnerabilityTime <= 0) {
                player->health -= enemy->damage;
                player->invulnerabilityTime = 2.0f;
                
                if (player->health <= 0) {
                    player->health = 0;
                    player->isAlive = false;
                    
                    // Update high score if current score is higher
                    if (player->score > game->highScore) {
                        game->highScore = player->score;
                    }
                    
                    // Save score immediately when player dies
                    SaveScore(game);
                }
            }
            enemy->isAlive = false;
            game->enemyCount--;
        }
    }
    
    // Check bullet-enemy collisions
    for (int i = 0; i < game->maxBullets; i++) {
        Bullet* bullet = &game->bullets[i];
        if (!bullet->isAlive || !bullet->isPlayerBullet) continue;
        
        for (int j = 0; j < game->maxEnemies; j++) {
            Enemy* enemy = &game->enemies[j];
            if (!enemy->isAlive) continue;
            
            if (CheckCollisionCircles(bullet->position, BULLET_SIZE, enemy->position, ENEMY_SIZE)) {
                enemy->health -= bullet->damage;
                bullet->isAlive = false;
                
                if (enemy->health <= 0) {
                    enemy->isAlive = false;
                    game->enemyCount--;
                    int scoreGained = 100 * (enemy->type + 1); // More points for harder enemies
                    player->score += scoreGained;
                    
                    // Log enemy kill
                    LogEnemyKill(game, enemy->type, scoreGained);
                    
                    // Update high score if current score is higher
                    if (player->score > game->highScore) {
                        game->highScore = player->score;
                    }
                }
                break;
            }
        }
    }
    
    // Check player-powerup collisions
    for (int i = 0; i < game->maxPowerups; i++) {
        PowerUp* powerup = &game->powerups[i];
        if (!powerup->isAlive) continue;
        
        if (CheckCollisionCircles(player->position, PLAYER_SIZE, powerup->position, POWERUP_SIZE)) {
            switch (powerup->type) {
                case 0: // Health
                    player->health = fmin(player->health + 30, PLAYER_MAX_HEALTH);
                    player->score += 50; // Bonus points for health pickup
                    break;
                case 1: // Speed (temporary)
                    player->score += 25; // Bonus points for speed pickup
                    // Could implement temporary speed boost
                    break;
                case 2: // Damage (temporary)
                    player->score += 25; // Bonus points for damage pickup
                    // Could implement temporary damage boost
                    break;
            }
            
            // Log powerup collection
            LogPowerupCollect(game, powerup->type);
            
            // Update high score if current score is higher
            if (player->score > game->highScore) {
                game->highScore = player->score;
            }
            powerup->isAlive = false;
            game->powerupCount--;
        }
    }
}

/**
 * Reset the game to initial state
 * @param game Pointer to the game data structure
 */
void ResetGame(GameData* game) {
    // Reset player
    game->player.position = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 100.0f};
    game->player.velocity = (Vector2){0, 0};
    game->player.health = PLAYER_MAX_HEALTH;
    game->player.score = 0;
    game->player.invulnerabilityTime = 0;
    game->player.isAlive = true;
    
    // Log game start
    LogGameStart(game);
    
    // Reset enemies
    for (int i = 0; i < game->maxEnemies; i++) {
        game->enemies[i].isAlive = false;
    }
    game->enemyCount = 0;
    
    // Reset bullets
    for (int i = 0; i < game->maxBullets; i++) {
        game->bullets[i].isAlive = false;
    }
    game->bulletCount = 0;
    
    // Reset powerups
    for (int i = 0; i < game->maxPowerups; i++) {
        game->powerups[i].isAlive = false;
    }
    game->powerupCount = 0;
    
    // Reset timers
    game->gameTime = 0;
    game->enemySpawnTimer = 0;
    game->powerupSpawnTimer = 0;
}

/**
 * Save the current score to file
 * @param game Pointer to the game data structure
 */
void SaveScore(GameData* game) {
    // Log score save
    LogScoreSave(game);
    
    // Try multiple locations to ensure the file is saved
    const char* locations[] = {"scores.txt", "/app/scores.txt", "./scores.txt"};
    bool saved = false;
    
    for (int i = 0; i < 3 && !saved; i++) {
        FILE* file = fopen(locations[i], "a");
        if (file) {
            time_t now = time(NULL);
            struct tm* timeinfo = localtime(&now);
            char* difficultyName = (char*)GetDifficultyName(game->difficulty);
            
            // Write in the format that matches the existing file
            // Convert difficulty name to lowercase manually
            char lowerDiff[20];
            strcpy(lowerDiff, difficultyName);
            for (int j = 0; lowerDiff[j]; j++) {
                lowerDiff[j] = tolower(lowerDiff[j]);
            }
            
            fprintf(file, "score = %d diff = %s time = %ds date = %s", 
                    game->player.score, 
                    lowerDiff, // Convert to lowercase to match existing format
                    (int)game->gameTime,
                    asctime(timeinfo));
            fclose(file);
            
            saved = true;
        } else {
            // Failed to open file, continue trying other locations
        }
    }
    
    if (!saved) {
        printf("ERROR: Could not save score to any location!\n");
    }
}

/**
 * Load the high score from file
 * @param game Pointer to the game data structure
 */
void LoadHighScore(GameData* game) {
    const char* locations[] = {"scores.txt", "/app/scores.txt", "./scores.txt"};
    int maxScore = 0;
    
    for (int i = 0; i < 3; i++) {
        FILE* file = fopen(locations[i], "r");
        if (file) {
            char line[256];
            
            while (fgets(line, sizeof(line), file)) {
                int score;
                // Try both formats: new format and existing format
                if (sscanf(line, "Score: %d", &score) == 1 || 
                    sscanf(line, "score = %d", &score) == 1) {
                    if (score > maxScore) {
                        maxScore = score;
                    }
                }
            }
            
            fclose(file);
            break; // Found a file, no need to try others
        } else {
            // Could not open file, continue trying other locations
        }
    }
    
    game->highScore = maxScore;
}

/**
 * Get the name of a difficulty level
 * @param difficulty The difficulty level
 * @return String representation of the difficulty
 */
const char* GetDifficultyName(Difficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY: return "Easy";
        case DIFFICULTY_MEDIUM: return "Medium";
        case DIFFICULTY_HARD: return "Hard";
        case DIFFICULTY_NIGHTMARE: return "Nightmare";
        default: return "Unknown";
    }
}

/**
 * Get the multiplier for a difficulty level
 * @param difficulty The difficulty level
 * @return Multiplier value
 */
float GetDifficultyMultiplier(Difficulty difficulty) {
    switch (difficulty) {
        case DIFFICULTY_EASY: return 1.0f;
        case DIFFICULTY_MEDIUM: return 1.5f;
        case DIFFICULTY_HARD: return 2.0f;
        case DIFFICULTY_NIGHTMARE: return 3.0f;
        default: return 1.0f;
    }
}

// Additional update functions for different game states
void UpdateGameOver(GameData* game) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        // Reload high score before returning to menu
        LoadHighScore(game);
        game->state = GAME_STATE_MENU;
    }
}

void UpdateLogs(GameData* game) {
    // Reload high score when entering logs screen
    static bool logsInitialized = false;
    if (!logsInitialized) {
        LoadHighScore(game);
        logsInitialized = true;
    }
    
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        game->state = GAME_STATE_MENU;
        logsInitialized = false; // Reset for next time
    }
}

void UpdateDifficultyMenu(GameData* game) {
    static int selectedDifficulty = 0;
    static float inputDelay = 0;
    
    inputDelay += GetFrameTime();
    if (inputDelay < 0.15f) return;
    
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedDifficulty = (selectedDifficulty - 1 + 4) % 4;
        inputDelay = 0;
    }
    else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedDifficulty = (selectedDifficulty + 1) % 4;
        inputDelay = 0;
    }
    else if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        game->difficulty = (Difficulty)selectedDifficulty;
        game->state = GAME_STATE_MENU;
        inputDelay = 0;
    }
    else if (IsKeyPressed(KEY_ESCAPE)) {
        game->state = GAME_STATE_MENU;
        inputDelay = 0;
    }
} 