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
    bool running = true;
    bool gameOver = false;
    int score = 0;

    void draw()
    {
        snake.draw();
        food.draw();

        if (gameOver)
        {
            DrawText("GAME OVER", set + 250, set + (cell_size * cell_number) / 2, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set + 250, set + (cell_size * cell_number) / 2 + 50, 40, border);
        }
    }

    void update()
    {
        if (running)
        {
            snake.update();
            collision_food();
            collision_wall();
            collision_tail();
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

    void game_over()
    {
        gameOver = true;
        running = false;

        
        while (true)
        {
            BeginDrawing();
            ClearBackground(background);

            DrawRectangleLinesEx(Rectangle{(float)set - 5, (float)set - 5, (float)cell_size * cell_number + 10, (float)cell_size * cell_number + 10}, 5, border);
            DrawText("PROJECT-150 --> SNAKE GAME", set - 5, 20, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set - 5, set + (cell_size * cell_number) + 10, 40, border);

            DrawText("GAME OVER", set + 250, set + (cell_size * cell_number) / 2, 40, border);
            DrawText(TextFormat("SCORE = %i", score), set + 250, set + (cell_size * cell_number) / 2 + 50, 40, border);
            DrawText("PRESS ANY KEY TO RESTART", set + 100, set + (cell_size * cell_number) / 2 + 100, 30, food_color);

            EndDrawing();

            if (GetKeyPressed() != 0)
            {
                break;
            }
        }

    
        snake.reset();
        food.position = food.random();
        score = 0;
        running = true;
        gameOver = false;
    }
};

int main()
{
    InitWindow(2 * set + cell_number * cell_size, 2 * set + cell_number * cell_size, "project-150");
    SetTargetFPS(65);

    Game game = Game();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        if (event(0.2))
        {
            game.update();
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
