#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_OBSTACLES 5

// define hiscore e playername como variáveis globais
int hiscore = 0;
char playername[25];

// Cria o objeto obstaculo 
typedef struct Obstacle {
    float x;
    float y;
    float width;
    float height;
} Obstacle;

// Cria o objeto ranking
typedef struct {
    char playerName[25];
    int playerScore;
} Ranking;

// Reseta o estado do jogo, assim as variaveis voltam ao padrao original
void ResetGame(float *xPos, float *yPos, float *velY, int *score, Obstacle obstacles[], int screenWidth, float groundY, int size) {
    *xPos = 100;
    *velY = 0;
    *score = 0;
    *yPos = groundY - size;  
    
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].width = 20;
        obstacles[i].height = 50;
        obstacles[i].x = screenWidth + i * 750 + GetRandomValue(0, 200);
        obstacles[i].y = groundY - obstacles[i].height;
    }
}

// Função para a comparação dos pontos dos jogadores
int comparePlayersByScore(const void *a, const void *b) {
    const Ranking *rankingA = (const Ranking *)a;
    const Ranking *rankingB = (const Ranking *)b;
    return rankingB->playerScore - rankingA->playerScore;
}

// Mostra a tela de ranking quando pressionado a tecla c 
void RankingScreen(const char *playername, int hiscore) {
    Ranking ranking[25] = {
        {"vinizin", 1203},
        {"gabeVerdin", 980},
        {"Willian", 345},
        {"", 0},
        {"leozao", 200},
    };
    
    strncpy(ranking[3].playerName, playername, sizeof(ranking[3].playerName) - 1);
    ranking[3].playerName[sizeof(ranking[3].playerName) - 1] = '\0';
    ranking[3].playerScore = hiscore;
    
    int sizeStruct = sizeof(ranking) / sizeof(ranking[0]);
    
    // Ordenação do ranking
    qsort(ranking, sizeStruct, sizeof(Ranking), comparePlayersByScore);
    
    while (!IsKeyPressed(KEY_Q)) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Ranking", 260, 20, 40, GOLD);
            for (int i = 0; i < 5; i++) {
                DrawText(TextFormat("%d. %s %d", i + 1, ranking[i].playerName, ranking[i].playerScore), 240, 120 + i * 40, 30, WHITE);
            }
            
            DrawText("Press Q to return", 260, 400, 20, GRAY);
        EndDrawing();
    }
    
}

// Tela inicial Start e Exit
void StartGame(int screenWidth, int screenHeight) {
    while (!IsKeyPressed(KEY_S)) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("PetRun", screenWidth/2 - 160, screenHeight/2.5 - 70,40, WHITE);
            DrawText("Start Game (S)", screenWidth/2 - 160, screenHeight/2.5,40, WHITE);
            DrawText("Ranking (C)", screenWidth/2 - 160, screenHeight/2.5 + 80, 40, WHITE);
            DrawText("Exit Game (E)", screenWidth/2 - 160, screenHeight/2.5 + 40, 40, WHITE);
        EndDrawing();
        
        if (IsKeyPressed(KEY_S)) break;
        if (IsKeyPressed(KEY_E)) CloseWindow();
        if (IsKeyPressed(KEY_C)) RankingScreen(playername, hiscore);
    }
} 


int main(void) {
    // Variaveis
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    const float GRAVITY = 800;
    const float JUMP = -490;
    
    float xPos = 100;
    float yPos = 0;
    float velY = 0;
    
    const int size = 40;
    const float groundY = 300;
    
    int score = 0;
    
    Obstacle obstacles[NUM_OBSTACLES];
    float obstacleSpeed = 200;
    
    srand(time(NULL));
    yPos = groundY - size;
    bool gameOver = false;
    
    printf("Digite o nome do jogador: ");
    scanf("%24s", playername);
    
    // Gera obstaculos aleatorios
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].width = 20;
        obstacles[i].height = 50;
        obstacles[i].x = screenWidth + i * 750 + GetRandomValue(0, 200);
        obstacles[i].y = groundY - obstacles[i].height;
    }
    
    // Inicia a tela do jogo
    InitWindow(screenWidth, screenHeight, "PetRun");
    SetTargetFPS(60);
    
    // Reseta o jogo
    ResetGame(&xPos, &yPos, &velY, &score, obstacles, screenWidth, groundY, size);
    
    // Tela de start e exit do jogo   
    StartGame(screenWidth, screenHeight);
    
    // Deixa o jogo em loop ate ser fechado
    while(!WindowShouldClose()){
        
        float dt = GetFrameTime();
        
        if (!gameOver) {
            // Pulo
            if (IsKeyPressed(KEY_SPACE) && yPos >= groundY - size) {
                velY = JUMP;
            }
            
            // Gravidade
            velY += GRAVITY * dt;
            yPos += velY * dt;
            
            // Impede o jogador de cair para fora
            if (yPos > groundY - size) {
                yPos = groundY - size;
                velY = 0;    
            }
            
            // Quando pressionado a tecla R, o jogo reinicia
            if (IsKeyPressed(KEY_R)) {
                ResetGame(&xPos, &yPos, &velY, &score, obstacles, screenWidth, groundY, size);
            }
            
            // Quando pressionado a tecla Q, a tela volta para o menu inicial e reinicia o jogo
            if (IsKeyPressed(KEY_Q)) {
                ResetGame(&xPos, &yPos, &velY, &score, obstacles, screenWidth, groundY, size);
                StartGame(screenWidth, screenHeight);
            }
            
            // Movimenta obstáculos
            for (int i = 0; i < NUM_OBSTACLES; i++) {
                obstacles[i].x -= obstacleSpeed * dt;
                
                if (obstacles[i].x + obstacles[i].width < 0) {
                    obstacles[i].x = screenWidth + GetRandomValue(0, 200);
                }
                
                // Checa colisão
                if (CheckCollisionRecs(
                    (Rectangle){ xPos, yPos, size, size },
                    (Rectangle){ obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height }
                )) {
                    gameOver = true;
                }
            }
            
            // Score aumenta somente se não estiver em game over
            score ++;
            
            // Atualiza highscore
            if (score > hiscore) {
                hiscore = score;
            }
        }        
        // Desenha
        BeginDrawing();
            ClearBackground(BLACK);
            
            if (!gameOver) {
                DrawRectangle((int)xPos, (int)yPos, size, size, RED);
                DrawLine(0, groundY, screenWidth, groundY, WHITE);
                DrawText(TextFormat("Score: %d", score), 25, 20, 20, WHITE);
                DrawText(TextFormat("High Score: %d", hiscore), 25, 50, 20, GREEN);
                DrawText("Press R to restart", 600, 25, 14, WHITE);
                DrawText("Press Q to go to the menu", 600, 5, 14, WHITE);
                
                for (int i = 0; i < NUM_OBSTACLES; i++) {
                    DrawRectangle(
                        (int)obstacles[i].x,
                        (int)obstacles[i].y,
                        (int)obstacles[i].width,
                        (int)obstacles[i].height,
                        GREEN
                    );
                }
            } else {
                DrawText("Game Over!", screenWidth/2 - 120, screenHeight/2.5, 40, RED);
                DrawText("Press R to Restart", screenWidth/2 - 120, screenHeight/2.5 + 40, 20, WHITE);
                DrawText("Press Q to return", screenWidth/2 - 120, screenHeight/2.5 + 60, 20, WHITE);
                DrawText(TextFormat("Your Score: %d", score), screenWidth/2 - 120, screenHeight/2.5 + 100, 20, BLUE);
                DrawText(TextFormat("High Score: %d", hiscore), screenWidth/2 - 120, screenHeight/2.5 + 120, 20, GREEN);
                
                if (IsKeyPressed(KEY_R)) {
                    ResetGame(&xPos, &yPos, &velY, &score, obstacles, screenWidth, groundY, size);
                    gameOver = false;
                }
                
                if (IsKeyPressed(KEY_Q)) {
                    ResetGame(&xPos, &yPos, &velY, &score, obstacles, screenWidth, groundY, size);
                    StartGame(screenWidth, screenHeight);
                    gameOver = false;
                } 
            }
        EndDrawing();
    }
    CloseWindow();
    
    return 0;
}
