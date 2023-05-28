#include <SDL2/SDL.h>
#include <vector>
#include <random>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BOTTOM_BAR_HEIGHT = 16;
const int NUM_COLORS = 32;
const int COLOR_BOX_WIDTH = SCREEN_WIDTH / NUM_COLORS;
const int MAX_FRAME_RATE = 60;
const int FRAME_DELAY = 1000 / MAX_FRAME_RATE;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool isDrawing = false;
int startX = 0;
int startY = 0;
SDL_Color currentColor = { 255, 255, 255, 255 };
std::vector<SDL_Rect> colorBoxes; //for rectangle to hold pallete colors 
std::vector<SDL_Color> randomColors;

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                SDL_Quit();
                exit(0);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    isDrawing = true;
                    startX = event.motion.x;
                    startY = event.motion.y;
                    // Check if clicked within color boxes
                    int colorBoxIndex = event.motion.x / COLOR_BOX_WIDTH;
                    if (event.motion.y >= SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT && colorBoxIndex >= 0 && colorBoxIndex < NUM_COLORS)
                    {
                        // Update the current color
                        currentColor = randomColors[colorBoxIndex];
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                    isDrawing = false;
                break;
            case SDL_MOUSEMOTION:
                if (isDrawing)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;
                    SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
                    SDL_RenderDrawLine(renderer, startX, startY, mouseX, mouseY);
                    startX = mouseX;
                    startY = mouseY;
                    SDL_RenderPresent(renderer);
                }
                break;
        }
    }
}

void generateRandomColors()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 255);
    for (int i = 0; i < NUM_COLORS; ++i)
    {
        SDL_Color color = {
            static_cast<Uint8>(dis(gen)),   // Red component
            static_cast<Uint8>(dis(gen)),   // Green component
            static_cast<Uint8>(dis(gen)),   // Blue component
            255                             // Alpha component
        };
        randomColors.push_back(color);
    }
}

void renderColorBoxes() //function creates random paint pallete at bottom of screen
{
    for (int i = 0; i < NUM_COLORS; ++i)
    {
        SDL_Rect colorBox = { i * COLOR_BOX_WIDTH, SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT, COLOR_BOX_WIDTH, BOTTOM_BAR_HEIGHT };
        SDL_Color color = randomColors[i];
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); //red, green, blue, and alpha values
        SDL_RenderFillRect(renderer, &colorBox);
        colorBoxes.push_back(colorBox);
    }
    SDL_RenderPresent(renderer);
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Mouse Drawing Tool", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    generateRandomColors();
    renderColorBoxes();

    Uint32 frameStartTime = 0;
    Uint32 frameEndTime = 0;
    Uint32 frameDuration = 0;

    while (true)
    {
        frameStartTime = SDL_GetTicks();
        handleEvents();
        frameEndTime = SDL_GetTicks();
        frameDuration = frameEndTime - frameStartTime;
        
        if (frameDuration < FRAME_DELAY) //framerate control dont waste cpu
        {
            SDL_Delay(FRAME_DELAY - frameDuration);
        }
    }

    SDL_DestroyRenderer(renderer); //cleanup steps 
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}