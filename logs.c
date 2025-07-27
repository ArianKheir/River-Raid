#include"header.h"

// New function to write game logs
void WriteGameLog(const char* event, const char* details) {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    
    // Create logs directory if it doesn't exist
    #ifdef _WIN32
    system("if not exist logs mkdir logs");
    #else
    system("mkdir -p logs");
    #endif
    
    // Create log filename with current date
    char logFilename[256];
    sprintf(logFilename, "logs/game_%04d-%02d-%02d.log", 
            timeinfo->tm_year + 1900, 
            timeinfo->tm_mon + 1, 
            timeinfo->tm_mday);
    
    FILE* logFile = fopen(logFilename, "a");
    if (logFile) {
        fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n",
                timeinfo->tm_year + 1900,
                timeinfo->tm_mon + 1,
                timeinfo->tm_mday,
                timeinfo->tm_hour,
                timeinfo->tm_min,
                timeinfo->tm_sec,
                event, details);
        fclose(logFile);
    }
}

// Function to log game start
void LogGameStart(GameData* game) {
    char details[256];
    sprintf(details, "Game started - Difficulty: %s", GetDifficultyName(game->difficulty));
    WriteGameLog("GAME_START", details);
}

// Function to log game end
void LogGameEnd(GameData* game) {
    char details[256];
    sprintf(details, "Game ended - Score: %d, Time: %.1fs, Difficulty: %s", 
            game->player.score, game->gameTime, GetDifficultyName(game->difficulty));
    WriteGameLog("GAME_END", details);
}

// Function to log score save
void LogScoreSave(GameData* game) {
    char details[256];
    sprintf(details, "Score saved - Score: %d, Time: %.1fs, Difficulty: %s", 
            game->player.score, game->gameTime, GetDifficultyName(game->difficulty));
    WriteGameLog("SCORE_SAVE", details);
}

// Function to log enemy kills
void LogEnemyKill(GameData* game, int enemyType, int score) {
    char details[256];
    sprintf(details, "Enemy killed - Type: %d, Score: %d, Total Score: %d", 
            enemyType, score, game->player.score);
    WriteGameLog("ENEMY_KILL", details);
}

// Function to log powerup collection
void LogPowerupCollect(GameData* game, int powerupType) {
    char details[256];
    sprintf(details, "Powerup collected - Type: %d, Score: %d", 
            powerupType, game->player.score);
    WriteGameLog("POWERUP_COLLECT", details);
}

// Legacy logs function - replaced by DrawLogs in render.c
void logs() {
    // This function is kept for compatibility but the actual logs display
    // is now handled by DrawLogs() in render.c
}