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
int offset = 75;

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
    std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}}; // Create snake body
    Vector2 direction = {1, 0}; // Define direction
    bool addSegment = false;

    // Draw snake body
    void Draw() {
        for (unsigned i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update() {

        body.push_front(Vector2Add(body[0], direction)); // Move snake body

        if (addSegment) {
            addSegment = false;
        }
        else {
            body.pop_back();
        }

    }

    void Reset() {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }

};

// Food class
class Food {
public:
    Vector2 position = {5, 6};
    Texture2D texture;

    // Load image
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
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }

    // Function to generate food in random position
    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    // Generate food in random position when eaten calling the GenerateRandomCell() function
    Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody) {

        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) { // When collision food --> snake occurs
            position = GenerateRandomCell();
        }
        return position;
    }

};

// Game class
class Game {
public:
    Snake snake = Snake(); // Create snake object
    Food food = Food(snake.body); // Create food object
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game() {
        InitAudioDevice();
        eatSound = LoadSound("resources/sounds/eat.mp3");
        wallSound = LoadSound("resources/sounds/wall.mp3");
    }

    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();

    }

    void Draw() {
        food.Draw();
        snake.Draw();
    }

    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
        
    }

    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score ++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges() {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
            GameOver();
        }
        else if (snake.body[0].y == cellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }

    void GameOver() {
        std::cout << "Game over" << '\n';
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void CheckCollisionWithTail() {
        std::deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }

    
};

int main() {

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");

    SetTargetFPS(60);

    Game game = Game();

    while(!WindowShouldClose()) {

            // Update only in intervals of 200ms
            if(eventTriggered(0.2)) {
                game.Update();
            }

            // Snake controls
            // TODO: create a function for this
            if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
                game.snake.direction = {0, -1};
                game.running = true;
            }
            if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
                game.snake.direction = {0, 1};
                game.running = true;
            }
            if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
                game.snake.direction = {-1, 0};
                game.running = true;
            }
            if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
                game.snake.direction = {1, 0};
                game.running = true;
            }

        BeginDrawing();

            ClearBackground(green);

            DrawRectangleLinesEx(Rectangle{(float) offset - 5, (float) offset - 5, (float) cellSize * cellCount + 10, (float) cellSize * cellCount + 10}, 5, darkGreen);

            DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
            DrawText(TextFormat("%d", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);

            game.Draw();

        EndDrawing();
    }

    CloseWindow();


    return 0;
}