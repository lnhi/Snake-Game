#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "painter.h"
#include "PlayGround.h"
#include "Gallery.h"

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const string WINDOW_TITLE = "Snake Game";

SDL_Window* window;
SDL_Renderer* renderer;
const int GROUND_WIDTH = 30;
const int GROUND_HEIGHT = 20;
const int CELL_SIZE = 30;

const double STEP_DELAY = 0.15;
#define CLOCK_NOW chrono::system_clock::now
typedef chrono::duration<double> ElapsedTime;

void logSDLError(std::ostream& os,const std:: string &msg, bool fatal = false);
void initSDL(SDL_Window* &window, SDL_Renderer* &renderer);
void quitSDL(SDL_Window* window, SDL_Renderer* renderer);
void waitUntilKeyPressed();

void renderSplashScreen();
void renderGamePlay(Painter&, const PlayGround& playGround);
void renderGameOver(Painter&, const PlayGround& playGround);
UserInput interpretEvent(SDL_Event e);
void updateRankingTable(const PlayGround& playGround);

Gallery* gallery = nullptr; 

int main(int argc, char* argv[])
{
    srand(time(0));
    
    initSDL(window, renderer);
    Painter painter(window, renderer);
    gallery = new Gallery(painter);

    renderSplashScreen();
    PlayGround playGround(GROUND_WIDTH, GROUND_HEIGHT);

    SDL_Event e;
    auto start = CLOCK_NOW();
    renderGamePlay(painter, playGround);
    SDL_Texture* newTexture = painter.loadTexture("bk_gr.png");
    while (playGround.isGameRunning()) 
    {
        SDL_RenderCopy(renderer, newTexture, NULL, NULL);
        while (SDL_PollEvent(&e)) 
        {
            UserInput input = interpretEvent(e);
            playGround.processUserInput(input);
        }

        auto end = CLOCK_NOW();
        ElapsedTime elapsed = end-start;
        if (elapsed.count() > STEP_DELAY) {
            playGround.nextStep();
            renderGamePlay(painter, playGround);
            start = end;
        }
        SDL_Delay(1);
    }
    renderGameOver(painter, playGround);
    updateRankingTable(playGround);

    delete gallery;
    quitSDL(window, renderer);
    return 0;
}

void logSDLError(std::ostream& os, const std:: string &msg, bool fatal)
{
    os << msg << " Error: " << SDL_GetError() << std::endl;
    if (fatal) {
        SDL_Quit();
        exit(1);
    }
}

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        logSDLError(std::cout, "SDL_Init", true);

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  
    if (window == nullptr)
        logSDLError(std::cout, "CreateWindow", true);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  
    if (renderer == nullptr)
        logSDLError(std::cout, "CreateRenderer", true);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void quitSDL(SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);
	SDL_Quit();
}
/*bool loadMedia()
{
	//Loading success flag
	bool success = true;
	//Load PNG surface
	gPNGSurface = loadSurface( "bk_gr.png" );
	if( gPNGSurface == NULL )
	{
		printf( "Failed to load PNG image!\n" );
		success = false;
	}
	return success;
}
SDL_Surface* loadSurface(string path )
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
		if( optimizedSurface == NULL )
		{
			printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return optimizedSurface;
}*/
void waitUntilKeyPressed()
{
    SDL_Event e;
    while (true) {
        if ( SDL_WaitEvent(&e) != 0 &&
             (e.type == SDL_KEYDOWN || e.type == SDL_QUIT) )
            return;
        SDL_Delay(100);
    }
}

float generateRandomNumber()
{
    return (float) rand() / RAND_MAX;
}

void renderSplashScreen()
{
    waitUntilKeyPressed();
}

void drawCherry(Painter& painter, int left, int top)
{
    SDL_Rect dst = { left+5, top+5, CELL_SIZE+5, CELL_SIZE+5};
    painter.createImage(gallery->getImage(PIC_CHERRY), NULL, &dst);
}
void drawBox(Painter& painter, int left, int top)
{
    SDL_Rect dst = { left+5, top+5, CELL_SIZE+5, CELL_SIZE+5};
    painter.createImage(gallery->getImage(PIC_BOX), NULL, &dst);
}
void drawSnake(Painter& painter, int left, int top, vector<Position> pos)
{
    for (size_t i = 0; i < pos.size(); i++)
    {
        SDL_Rect dst = { left+pos[i].x*CELL_SIZE+1, top+pos[i].y*CELL_SIZE+1, CELL_SIZE+5, CELL_SIZE+5};
        SDL_Texture* texture = NULL;
        if (i > 0)
        {
            if (pos[i].y == pos[i-1].y)
                {
                    texture = gallery->getImage(PIC_SNAKE_HORIZONTAL);
                }
            /* if(pos[i].y == pos[i-1].y && pos[i].y==pos[i+1].y-1&&pos[i].x==pos[i+1].x)
            {
                 texture = gallery->getImage(PIC_SNAKE_CORNER1);
            }
             if(pos[i].y == pos[i-1].y && pos[i].y==pos[i+1].y-1&&pos[i].x==pos[i+1].x)
            {
                 texture = gallery->getImage(PIC_SNAKE_CORNER2);
            }
             if(pos[i].y == pos[i-1].y && pos[i].y==pos[i+1].y-1&&pos[i].x==pos[i+1].x)
            {
                 texture = gallery->getImage(PIC_SNAKE_CORNER1);
            }
             if(pos[i].y == pos[i-1].y && pos[i].y==pos[i+1].y-1&&pos[i].x==pos[i+1].x)
            {
                 texture = gallery->getImage(PIC_SNAKE_CORNER1);
            }*/
            if (pos[i].y != pos[i-1].y)
                texture = gallery->getImage(PIC_SNAKE_VERTICAL);
        }
        else 
        { 
            texture = gallery->getImage(PIC_SNAKE_HEAD);
        }
        painter.createImage(texture, NULL, &dst);
    }
    
}

/*void drawVerticalLine(Painter& painter, int left, int top, int cells)
{
    painter.setColor(GREEN_COLOR);
    painter.setAngle(-90);
    painter.setPosition(left, top);
    painter.moveForward(cells * CELL_SIZE);
}

void drawHorizontalLine(Painter& painter, int left, int top, int cells)
{
    painter.setColor(GREEN_COLOR);
    painter.setAngle(0);
    painter.setPosition(left, top);
    painter.moveForward(cells * CELL_SIZE);
}
*/
void renderGamePlay(Painter& painter, const PlayGround& playGround)
{
    int top = 0, left = 0;
    int width = playGround.getWidth();
    int height = playGround.getHeight();
    drawBox(painter, CELL_SIZE, CELL_SIZE);
   
  //  painter.loadTexture("bk_gr.png");
   // SDL_BlitSurface( gPNGSurface, NULL, gScreenSurface, NULL);
   /*painter.setColor(WHITE_COLOR);
   for (int i = 0; i <= width; i++)
        drawVerticalLine(painter, left+i*CELL_SIZE, top+0, height);

    for (int i = 0; i <= height; i++)
        drawHorizontalLine(painter, left+0, top+i * CELL_SIZE, width);
*/
    const vector<vector<CellType> >& squares = playGround.getSquares();
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (squares[i][j] == CELL_CHERRY)
                drawCherry(painter, left+j*CELL_SIZE, top+i*CELL_SIZE);

    vector<Position> snakePositions = playGround.getSnakePositions();
    drawSnake(painter, left, top, snakePositions);

    SDL_RenderPresent(painter.getRenderer());
}
void renderGameOver(Painter& painter, const PlayGround& playGround)
{
    SDL_Texture* newTexture = painter.loadTexture("gameover.png");
    bool quit = false;
    SDL_Event e;
        while(!quit)
    {
        while(SDL_PollEvent(&e)!=0)
        {
            if( e.type == SDL_QUIT )
            {
                quit=true;
            }
        }
    SDL_RenderCopy(renderer, newTexture, NULL, NULL);
    SDL_Delay(1);
    }
    //painter.loadTexture("gameover.png");
    //SDL_Delay(100);
}

UserInput interpretEvent(SDL_Event e)
{
    if (e.type == SDL_KEYUP)
     {
        switch (e.key.keysym.sym) 
        {
        case SDLK_UP: return KEY_UP;
        case SDLK_DOWN: return KEY_DOWN;
        case SDLK_LEFT: return KEY_LEFT;
        case SDLK_RIGHT: return KEY_RIGHT;
        }
    }
    return NO_INPUT;
}

void updateRankingTable(const PlayGround& playGround)
{

}
