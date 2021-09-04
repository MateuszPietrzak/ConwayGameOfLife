#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
// cell size should be n * gcd(w,h)
#define CELL_SIZE 10
#define FRAMES_BETWEEN_UPDATE 2

bool present[WINDOW_HEIGHT/CELL_SIZE][WINDOW_WIDTH/CELL_SIZE];
bool calculating[WINDOW_HEIGHT/CELL_SIZE][WINDOW_WIDTH/CELL_SIZE];



int getNeighbours(int i, int j);

int main() {
    
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Event event;
    SDL_Rect rect;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;
    bool running = true;
    bool mouseDownLeft = false;
    enum State {simulating, paused};
    State state = paused;
    
    const int FPS = 60;
    const int frameDelay = 1000/FPS;
    
    Uint32 frameStart;
    int frameTime;
    
    int ticks = 0;
    int neighbours;
    
    int mx, my;
    
    srand(time(NULL));
    
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        return 1;
    }
    
    window = SDL_CreateWindow("Gaem", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(window == NULL)
    {
        SDL_Quit();
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer == NULL)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    while(running)
    {
        frameStart = SDL_GetTicks();
        
        SDL_PollEvent(&event);
        SDL_GetMouseState(&mx, &my);
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT && !mouseDownLeft)
                {
                    present[my/CELL_SIZE][mx/CELL_SIZE] ? present[my/CELL_SIZE][mx/CELL_SIZE] = false : present[my/CELL_SIZE][mx/CELL_SIZE] = true;
                    mouseDownLeft = true;
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseDownLeft = false;
                }
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        std::cout << "SPACE\n";
                        state == paused ? state = simulating : state = paused;
                        break;
                    
                    case SDLK_c:
                        for(int i = 0; i< WINDOW_HEIGHT/CELL_SIZE; ++i){
                            for(int j = 0; j< WINDOW_WIDTH/CELL_SIZE; ++j){
                                present[i][j] = 0;
                            }
                        }
                        break;
                    
                    case SDLK_r:
                        for(int i = 0; i< WINDOW_HEIGHT/CELL_SIZE; ++i){
                            for(int j = 0; j< WINDOW_WIDTH/CELL_SIZE; ++j){
                                present[i][j] = rand()%2;
                            }
                        }
                        break;
                        
                    default:
                        break;
                }
                break;
                
            default:
                break;
        }
        
        state == simulating ? ticks++ : ticks = 0;
        if(ticks >= FRAMES_BETWEEN_UPDATE)
        {
            std::cout << "CHANGING\n";
            ticks %= FRAMES_BETWEEN_UPDATE;
            for(int i = 0; i< WINDOW_HEIGHT/CELL_SIZE; ++i){
                for(int j = 0; j< WINDOW_WIDTH/CELL_SIZE; ++j){
                    neighbours = getNeighbours(i,j);
                    
                    if(neighbours)std::cout << neighbours << std::endl;
                    
                    if(present[i][j]){
                        if(neighbours < 2 || neighbours > 3){
                            calculating[i][j] = false;
                        }else{
                            calculating[i][j] = true;
                        }
                    }else{
                        if(neighbours == 3){
                            calculating[i][j] = true;
                        }else{
                            calculating[i][j] = false;
                        }
                    }
                }
            }
            
            for(int i = 0; i< WINDOW_HEIGHT/CELL_SIZE; ++i){
                for(int j = 0; j< WINDOW_WIDTH/CELL_SIZE; ++j){
                    present[i][j] = calculating[i][j];
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        
        for(int i = 0; i< WINDOW_HEIGHT/CELL_SIZE; ++i){
            for(int j = 0; j< WINDOW_WIDTH/CELL_SIZE; ++j){
                
                SDL_SetRenderDrawColor(renderer, 75, 75, 75, 255);
                
                SDL_RenderDrawPoint(renderer, j*CELL_SIZE, i*CELL_SIZE);
                SDL_RenderDrawPoint(renderer, j*CELL_SIZE-1, i*CELL_SIZE);
                SDL_RenderDrawPoint(renderer, j*CELL_SIZE-1, i*CELL_SIZE-1);
                SDL_RenderDrawPoint(renderer, j*CELL_SIZE, i*CELL_SIZE-1);
                
                SDL_SetRenderDrawColor(renderer, 10, 10, 10, 255);
                
                if(present[i][j]){
                    rect.x = j * CELL_SIZE;
                    rect.y = i * CELL_SIZE;
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        
        SDL_RenderPresent(renderer);
        
        frameTime = SDL_GetTicks() - frameStart;
        if(frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
        
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

int getNeighbours(int i, int j)
{
    int res = 0;
    
    if(present[i ? i-1 : WINDOW_HEIGHT/CELL_SIZE - 1][j ? j-1 : WINDOW_WIDTH/CELL_SIZE - 1]) res++;
    if(present[i ? i-1 : WINDOW_HEIGHT/CELL_SIZE - 1][j]) res++;
    if(present[i ? i-1 : WINDOW_HEIGHT/CELL_SIZE - 1][j < WINDOW_WIDTH/CELL_SIZE - 1 ? j+1 : 0]) res++;
    if(present[i][j ? j-1 : WINDOW_WIDTH/CELL_SIZE - 1]) res++;
    if(present[i][j < WINDOW_WIDTH/CELL_SIZE - 1 ? j+1 : 0]) res++;
    if(present[i < WINDOW_HEIGHT/CELL_SIZE - 1 ? i+1 : 0][j ? j-1 : WINDOW_WIDTH/CELL_SIZE - 1]) res++;
    if(present[i < WINDOW_HEIGHT/CELL_SIZE - 1 ? i+1 : 0][j]) res++;
    if(present[i < WINDOW_HEIGHT/CELL_SIZE - 1 ? i+1 : 0][j < WINDOW_WIDTH/CELL_SIZE - 1 ? j+1 : 0]) res++;
    
    return res;
}
