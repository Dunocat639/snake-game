#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

// Define colors
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

// Define cells
int cellSize = 30;
int cellCount = 25;

double lastUpdateTime = 0;

// Function to update in time
bool eventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    } else {
        return false;
    }
}

// Function to determine collision between snake and food
bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

// Snake class
class Snake {
public:
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};

    void Draw() {
        for (unsigned i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{x * cellSize, y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() {
        body.pop_back();
        body.push_front(Vector2Add(body[0], direction));
    }

};

// Food class
class Food {
public:
    Vector2 position = {5, 6};
    Texture2D texture;

    Food(std::deque<Vector2> snakeBody) {
        Image image = LoadImage("resources/images/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);
    }

    ~Food(){
        UnloadTexture(texture);
    }

    void Draw() {
        DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
    }

    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody) {

        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }

};

// Game class
class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        snake.Update();
        CheckCollisionWithFood();
    }

    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
        }
    }
};

int main() {

    InitWindow(cellSize * cellCount, cellSize * cellCount, "Snake game");

    SetTargetFPS(60);

    Game game = Game();

    while(!WindowShouldClose()) {

            // Snake controls
            // TODO: create a function for this
            if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
                game.snake.direction = {0, -1};
            }
            if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
                game.snake.direction = {0, 1};
            }
            if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
                game.snake.direction = {-1, 0};
            }
            if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
                game.snake.direction = {1, 0};
            }

        BeginDrawing();

            // Update only in intervals of 200ms
            if(eventTriggered(0.2)) {
                game.Update();
            }

            ClearBackground(green);

            game.Draw();

        EndDrawing();
    }

    CloseWindow();


    return 0;
}