#include <iostream>
#include <raylib.h>
#include<deque>
#include<raymath.h>

using namespace std;

int cell_size=30;

int cell_number=25;

int direct=2;

double lastUpdateTime=0;

Color background={144, 238, 144,255};

Color snake_color={0, 0, 255,255};

Color food_color={255,0,0,255};

bool event(double interval)
{
    double currentTime=GetTime();
    if(currentTime-lastUpdateTime>=interval)  
    {
        lastUpdateTime=currentTime;
        return true;
    }
    return false;
}

class Snake
{

public:

    deque<Vector2> body={Vector2{6,9},Vector2{5,9},Vector2{4,9}};

    Vector2 direction={1,0};

    bool growing=false;

    void draw()
    {
        for(unsigned i=0 ; i<body.size() ; i++)
        {
            float x=body[i].x;
            float y=body[i].y;
            
            DrawRectangle(x*cell_size,y*cell_size,cell_size,cell_size,snake_color);
        }
    }

    void update()
    {
        body.push_front(Vector2Add(body[0],direction));
        
        if(growing)
        {
            growing=false;
        }
        else
        {
            body.pop_back();
        }
    }
};

class Food
{
    public:
        Vector2 position;

        Food()
        {
            position=random();
        }

        void draw()
        {
            DrawRectangle(position.x*cell_size,position.y*cell_size,cell_size,cell_size,food_color);
        }

        Vector2 random()
        {
            float x=GetRandomValue(0,cell_number-1);
            float y=GetRandomValue(0,cell_number-1);
            return Vector2{x,y};
        }
};

class Game
{
public:
    Snake snake=Snake();

    Food food=Food();

    void draw()
    {
        snake.draw();

        food.draw();
    }

    void update()
    {
        snake.update();
        collition();
    }

    void collition()
    {
        if(Vector2Equals(snake.body[0],food.position))
        {
            food.position=food.random();
            snake.growing = true;
        }
    }
};

int main () {

    InitWindow(750,750,"project-150");

    SetTargetFPS(65);

    Game game=Game();

    while(WindowShouldClose()==false)
    {
        BeginDrawing();

        if(event(0.2))
        {
            game.update();
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y!=1)
        {
            game.snake.direction={0,-1};
        }

        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y!=-1)
        {
            game.snake.direction={0,1};
        }

        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x!=-1)
        {
            game.snake.direction={1,0};
        }

        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x!=1)
        {
            game.snake.direction={-1,0};
        }

        ClearBackground(background);

        game.draw();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}