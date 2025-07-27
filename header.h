#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "raylib.h"

// Game window dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define FPS 60

// Game area dimensions (keeping similar to original)
#define GAME_WIDTH 90
#define GAME_HEIGHT 40
#define CELL_SIZE 8

// Player settings
#define PLAYER_SPEED 5.0f
#define PLAYER_SIZE 30.0f  // Increased from 20.0f to make player bigger
#define PLAYER_MAX_HEALTH 100

// Enemy settings
#define ENEMY_SPEED_MIN 2.0f
#define ENEMY_SPEED_MAX 4.0f
#define ENEMY_SIZE 25.0f  // Increased from 15.0f to make enemies bigger

// Bullet settings - MUCH FASTER NOW!
#define BULLET_SPEED 15.0f  // Increased from 8.0f
#define BULLET_SIZE 6.0f  // Increased from 4.0f to make bullets bigger
#define BULLET_DAMAGE 25
#define BULLET_LIFETIME 10.0f  // Bullets disappear after 10 seconds

// Power-up settings
#define POWERUP_SIZE 18.0f  // Increased from 12.0f to make powerups bigger
#define POWERUP_DURATION 10.0f

// Color definitions
#define COLOR_PLAYER CLITERAL(Color){ 0, 150, 255, 255 }      // Blue
#define COLOR_ENEMY CLITERAL(Color){ 255, 50, 50, 255 }       // Red
#define COLOR_BULLET CLITERAL(Color){ 255, 255, 0, 255 }      // Yellow
#define COLOR_ENEMY_BULLET CLITERAL(Color){ 255, 100, 0, 255 } // Orange
#define COLOR_POWERUP CLITERAL(Color){ 0, 255, 0, 255 }       // Green
#define COLOR_BACKGROUND CLITERAL(Color){ 20, 20, 40, 255 }   // Dark blue
#define COLOR_UI CLITERAL(Color){ 50, 50, 80, 255 }           // UI background
#define COLOR_TEXT CLITERAL(Color){ 255, 255, 255, 255 }      // White

// Enhanced color palette for beautiful graphics
#define COLOR_PLAYER_PRIMARY CLITERAL(Color){ 0, 200, 255, 255 }    // Bright blue
#define COLOR_PLAYER_SECONDARY CLITERAL(Color){ 100, 220, 255, 255 } // Light blue
#define COLOR_PLAYER_ACCENT CLITERAL(Color){ 255, 255, 255, 255 }   // White
#define COLOR_PLAYER_ENGINE CLITERAL(Color){ 255, 100, 0, 255 }     // Orange engine

#define COLOR_ENEMY_BASIC CLITERAL(Color){ 255, 50, 50, 255 }       // Red
#define COLOR_ENEMY_SHOOTER CLITERAL(Color){ 255, 100, 100, 255 }   // Light red
#define COLOR_ENEMY_FAST CLITERAL(Color){ 255, 0, 255, 255 }        // Magenta
#define COLOR_ENEMY_ELITE CLITERAL(Color){ 255, 200, 0, 255 }       // Gold
#define COLOR_ENEMY_BOSS CLITERAL(Color){ 150, 0, 255, 255 }        // Purple

#define COLOR_BULLET_PLAYER CLITERAL(Color){ 0, 255, 255, 255 }     // Cyan
#define COLOR_BULLET_ENEMY CLITERAL(Color){ 255, 100, 0, 255 }      // Orange
#define COLOR_BULLET_SPECIAL CLITERAL(Color){ 255, 255, 0, 255 }    // Yellow

// Game states
typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER,
    GAME_STATE_LOGS,
    GAME_STATE_DIFFICULTY
} GameState;

// Difficulty levels
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD,
    DIFFICULTY_NIGHTMARE
} Difficulty;

// Player structure
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float health;
    int score;
    float invulnerabilityTime;
    bool isAlive;
} Player;

// Enemy structure
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float health;
    int damage;
    int type;
    bool isAlive;
    float lastShotTime;
    float animationTime;
} Enemy;

// Bullet structure
typedef struct {
    Vector2 position;
    Vector2 velocity;
    int damage;
    bool isPlayerBullet;
    bool isAlive;
    float trailTime;
    float spawnTime;  // When the bullet was created
} Bullet;

// Power-up structure
typedef struct {
    Vector2 position;
    int type; // 0: health, 1: speed, 2: damage
    bool isAlive;
    float spawnTime;
} PowerUp;

// Game data structure
typedef struct {
    GameState state;
    Difficulty difficulty;
    Player player;
    Enemy* enemies;
    int enemyCount;
    int maxEnemies;
    Bullet* bullets;
    int bulletCount;
    int maxBullets;
    PowerUp* powerups;
    int powerupCount;
    int maxPowerups;
    float gameTime;
    float enemySpawnTimer;
    float powerupSpawnTimer;
    int highScore;
    bool showFPS;
} GameData;

// Function declarations
void InitializeGame(GameData* game);
void UpdateGame(GameData* game);
void DrawGame(GameData* game);
void DrawMenu(GameData* game);
void DrawUI(GameData* game);
void DrawLogs(GameData* game);
void DrawDifficultyMenu(GameData* game);
void DrawGameOver(GameData* game);
void UpdatePlayer(GameData* game);
void UpdateEnemies(GameData* game);
void UpdateBullets(GameData* game);
void UpdatePowerups(GameData* game);
void SpawnEnemy(GameData* game);
void SpawnPowerup(GameData* game);
void CheckCollisions(GameData* game);
void ResetGame(GameData* game);
void SaveScore(GameData* game);
void LoadHighScore(GameData* game);
const char* GetDifficultyName(Difficulty difficulty);
float GetDifficultyMultiplier(Difficulty difficulty);

// Additional function declarations
void UpdateMenu(GameData* game);
void UpdateGameOver(GameData* game);
void UpdateLogs(GameData* game);
void UpdateDifficultyMenu(GameData* game);
void DrawPlayer(Player* player);
void DrawEnemy(Enemy* enemy);
void DrawBullet(Bullet* bullet);
void DrawPowerup(PowerUp* powerup);
void DrawHealthBar(Player* player);
void DrawBackground(GameData* game);
void DrawParticles(GameData* game);

// Logging function declarations
void WriteGameLog(const char* event, const char* details);
void LogGameStart(GameData* game);
void LogGameEnd(GameData* game);
void LogScoreSave(GameData* game);
void LogEnemyKill(GameData* game, int enemyType, int score);
void LogPowerupCollect(GameData* game, int powerupType);
void logs(); // Legacy function for compatibility

#endif