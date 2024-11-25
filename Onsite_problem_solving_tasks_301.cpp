#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

int cell_size = 30;
int cell_number = 25;
int direct = 2;
int set = 75;

double lastUpdateTime = 0;

Color background = {144, 238, 144, 255};
Color snake_color = {0, 100, 0, 255};
Color food_color = {255, 0, 0, 255};
Color border = {2, 48, 32, 255};
Color obstacle_color = {139, 69, 19, 255};

bool event(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool TailHead(Vector2 tail, deque<Vector2> deq)
{
    for (unsigned int i = 0; i < deq.size(); i++)
    {
        if (Vector2Equals(deq[i], tail))
        {
            return true;
        }
    }
    return false;
}

class Snake
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool growing = false;

    void draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;

            Rectangle segment = Rectangle{set + x * cell_size, set + y * cell_size, (float)cell_size, (float)cell_size};
            DrawRectangleRounded(segment, 0.5, 6, snake_color);
        }
    }

    void update()
    {
        body.push_front(Vector2Add(body[0], direction));

        if (growing)
        {
            growing = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food
{
public:
    Vector2 position;

    Food()
    {
        position = random();
    }

    void draw()
    {
        DrawRectangle(set + position.x * cell_size, set + position.y * cell_size, cell_size, cell_size, food_color);
    }

    Vector2 random()
    {
        float x = GetRandomValue(0, cell_number - 1);
        float y = GetRandomValue(0, cell_number - 1);
        return Vector2{x, y};
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food();

    deque<Vector2> obstacle1;
    deque<Vector2> obstacle2;

    bool running = true;
    bool gameOver = false;
    bool paused = false; // To handle the pause state
    int score = 0;

    Game()
    {
        obstacle1 = create_L_obstacle(5, 1, false);  // First standard L-shaped obstacle
        obstacle2 = create_L_obstacle(15, 15, true); // Second reversed L-shaped obstacle
    }

    void draw()
    {
        snake.draw();
        food.draw();
        draw_obstacles();

        if (gameOver)
        {
            DrawText("GAME OVER", set + 250, set + (cell_size * cell_number) / 2, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set + 535, set + (cell_size * cell_number) / 2, 40, border);
        }

        if (paused)
        {
            DrawRectangle(set + 100, set + 150, 500, 200, Fade(border, 0.9f));
            DrawText("PAUSED", set + 150, set + 180, 40, food_color);
            DrawText("CONTINUE? (Y/N)", set + 120, set + 240, 30, food_color);
        }
    }

    void update()
    {
        if (paused)
        {
            if (IsKeyPressed(KEY_Y))
            {
                paused = false;
                running = true;
                score = max(0, score - 10); // Reduce 10 points, ensuring score doesn't go negative
            }
            else if (IsKeyPressed(KEY_N))
            {
                game_over();
            }
            return; // Skip further updates while paused
        }

        if (running)
        {
            snake.update();
            collision_food();
            collision_wall();
            collision_tail();
            collision_obstacles();
        }
    }

    void collision_food()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.random();
            snake.growing = true;
            score++;
        }
    }

    void collision_wall()
    {
        if (snake.body[0].x == cell_number || snake.body[0].x == -1 ||
            snake.body[0].y == cell_number || snake.body[0].y == -1)
        {
            game_over();
        }
    }

    void collision_tail()
    {
        deque<Vector2> tail = snake.body;
        tail.pop_front();
        if (TailHead(snake.body[0], tail))
        {
            game_over();
        }
    }

    void collision_obstacles()
    {
        for (Vector2 pos : obstacle1)
        {
            if (Vector2Equals(snake.body[0], pos))
            {
                pause_game();
                return;
            }
        }

        for (Vector2 pos : obstacle2)
        {
            if (Vector2Equals(snake.body[0], pos))
            {
                pause_game();
                return;
            }
        }
    }

    void draw_obstacles()
    {
        for (Vector2 pos : obstacle1)
        {
            DrawRectangle(set + pos.x * cell_size, set + pos.y * cell_size, cell_size, cell_size, border);
        }

        for (Vector2 pos : obstacle2)
        {
            DrawRectangle(set + pos.x * cell_size, set + pos.y * cell_size, cell_size, cell_size, border);
        }
    }

    void pause_game()
    {
        running = false;
        paused = true;
    }

    void game_over()
    {
        // Draw "GAME OVER" screen and wait for key press
        while (true)
        {
            BeginDrawing();
            ClearBackground(background);

            DrawRectangleLinesEx(Rectangle{(float)set - 5, (float)set - 5, (float)cell_size * cell_number + 10, (float)cell_size * cell_number + 10}, 5, border);
            DrawText("PROJECT-150 --> SNAKE GAME", set - 5, 20, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set - 5, set + (cell_size * cell_number) + 10, 40, border);

            // Draw "GAME OVER" message
            DrawText("GAME OVER", set + 250, set + (cell_size * cell_number) / 2, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set + 250, set + (cell_size * cell_number) / 2 + 50, 40, border);
            DrawText("PRESS ANY KEY TO CONTINUE...", set + 100, set + (cell_size * cell_number) / 2 + 100, 30, food_color);

            EndDrawing();

            if (GetKeyPressed() != 0) // Detect any key press
            {
                break;
            }
        }

        // Reset the game state after "GAME OVER" is cleared
        snake.reset();
        food.position = food.random();
        score = 0;
        running = false;
        paused = false;
        gameOver = false;
    }

    deque<Vector2> create_L_obstacle(int start_x, int start_y, bool reversed)
    {
        deque<Vector2> obstacle;

        if (reversed)
        {
            // Vertical part of the reversed L
            for (int i = 0; i < 5; i++)
            {
                obstacle.push_back(Vector2{(float)(start_x + 4), (float)(start_y + i)});
            }

            // Horizontal part of the reversed L
            for (int i = 0; i < 4; i++)
            {
                obstacle.push_back(Vector2{(float)(start_x + i), (float)start_y});
            }
        }
        else
        {
            // Vertical part of the L
            for (int i = 0; i < 5; i++)
            {
                obstacle.push_back(Vector2{(float)start_x, (float)(start_y + i)});
            }

            // Horizontal part of the L
            for (int i = 1; i < 5; i++)
            {
                obstacle.push_back(Vector2{(float)(start_x + i), (float)(start_y + 4)});
            }
        }

        return obstacle;
    }
};

int main()
{
    InitWindow(2 * set + cell_size * cell_number, 2 * set + cell_size * cell_number, "Snake Game");
    SetTargetFPS(10);

    Game game = Game();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        if (event(0.1))
        {
            game.update();
        }

        if(IsKeyPressed(KEY_N))
        {
            game.game_over();
        }

        if (game.paused && IsKeyPressed(KEY_Y))
        {
            game.paused = false;
            game.running = true;
            game.score = max(0, game.score - 10); // Reduce 10 points, ensuring score doesn't go negative
        }

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }

        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }

        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }

        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }

        ClearBackground(background);

        DrawRectangleLinesEx(Rectangle{(float)set - 5, (float)set - 5, (float)cell_size * cell_number + 10, (float)cell_size * cell_number + 10}, 5, border);
        DrawText("PROJECT-150 --> SNAKE GAME", set - 5, 20, 40, border);
        DrawText(TextFormat("SCORE = %i", game.score), set - 5, set + (cell_size * cell_number) + 10, 40, border);

        game.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
