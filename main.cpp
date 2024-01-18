#include <raylib.h>
#include <iostream>

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;
bool game_paused = false;

class Ball {
 public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    // Méthode pour dessiner la balle
    void Draw() {
        DrawCircle(x, y, radius, Yellow);
    }

    // Méthode pour mettre à jour la position de la balle
    void Update() {
        x += speed_x;
        y += speed_y;

        // Gestion des scores et réinitialisation de la balle
        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
    
        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0) {
            player_score++;
            ResetBall();
        }
    }

    // Méthode pour réinitialiser la position de la balle
    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;
    // Choix aléatoire de la direction de la balle
        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle {
 protected:
    //  limite le mouvement de la raquette pour qu'elle ne puisse pas dépasser les bords de l'écran
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

 public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
 public:
    // Surcharge de fonoction update de class paddle
    // Elle prend la position en y de la balle comme paramètre pour ajuster son mouvement
    void Update(int ball_y){
        if (y + height / 2 > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) {
            y = y + speed;
        }
        LimitMovement();  // Appeler la méthode pour limiter le mouvement de la raquette CPU
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
    std::cout << "Starting the game" << std::endl;
    const int screen_width = 1080;
    const int screen_height = 600;
    InitWindow(screen_width, screen_height, "Play Pong !");
    SetTargetFPS(60);

    Ball ball;
    Paddle player;
    CpuPaddle cpu;

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 117;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 5;



    bool game_started = false;

    while (WindowShouldClose() == false) {
        BeginDrawing();

        // Draw Start Game button if the game has not started
        if (!game_started) {
            ClearBackground(Dark_Green);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            ball.Draw();
            cpu.Draw();
            player.Draw();
            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
            DrawText("START ", screen_width / 2 - 100, screen_height / 2 - 20, 40, WHITE);


            // Check if Start Game button is clicked
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                CheckCollisionPointRec(GetMousePosition(), {screen_width / 2 - 80, screen_height / 2 - 10, 160, 20})) {
                game_started = true;
            }
        } else {
            // Check for the pause key and toggle the game_paused variable
            if (IsKeyPressed(KEY_P)) {
                game_paused = !game_paused;                
            }

            // Only update the game elements when the game is not paused
            if (!game_paused) {
                // Updating
                if (player_score < 5 && cpu_score < 5) {
                    ball.Update();
                    player.Update();
                    cpu.Update(ball.y);

                    // Checking for collisions
                    if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height})) {
                        ball.speed_x *= -1;
                    }

                    if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
                        ball.speed_x *= -1;
                    }
                }
            }
            // Drawing
            ClearBackground(Dark_Green);
            DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            ball.Draw();
            cpu.Draw();
            player.Draw();
            DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
            if (game_paused) {
                DrawText("GAME PAUSED", screen_width / 2 - 120, screen_height / 2 - 20, 40, WHITE); 
            }   
            // Check for winning condition
            if (player_score == 5 || cpu_score == 5) {
                DrawText("Game Over!", screen_width / 2 - 120, screen_height / 2 - 60, 60, WHITE);

                // Display winner
                if (player_score == 5) {
                    DrawRectangle(screen_width / 2 - 120, screen_height / 2 - 10, 240, 40, Green);
                    DrawText("Player Wins!", screen_width / 2 - 110, screen_height / 2 - 5, 20, WHITE);
                } else {
                    DrawRectangle(screen_width / 2 - 120, screen_height / 2 + 10, 240, 40, Green);
                    DrawText("CPU Wins!", screen_width / 2 - 100, screen_height / 2 + 15, 20, WHITE);
                }
}           
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


