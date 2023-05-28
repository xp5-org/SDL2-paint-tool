#include <SDL2/SDL.h>
#include <vector>
#include <random>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int BOTTOM_BAR_HEIGHT = 32;
const int NUM_COLORS = 16;
const int COLOR_BOX_WIDTH = SCREEN_WIDTH / NUM_COLORS;
const int MAX_FRAME_RATE = 60;
const int FRAME_DELAY = 1000 / MAX_FRAME_RATE;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool isDrawing = false;
int startX = 0;
int startY = 0;
SDL_Color currentColor = { 255, 255, 255, 255 };
std::vector<SDL_Rect> colorBoxes; // for rectangle to hold palette colors
std::vector<SDL_Color> randomColors;
int lineWidth = 1;
const int MIN_LINE_WIDTH = 1;
const int MAX_LINE_WIDTH = 25;

#include <algorithm>
#include <vector>

void drawLine(SDL_Renderer* renderer, int startX, int startY, int endX, int endY, SDL_Color color)
{
    // Calculate the angle and length of the line
    float angle = atan2(endY - startY, endX - startX);
    float length = hypot(endX - startX, endY - startY);

    // Set the initial line width
    float initialLineWidth = static_cast<float>(lineWidth);

    // Calculate the half line width
    float halfLineWidth = initialLineWidth / 2.0f;

    // Calculate the offsets for drawing lines
    float offsetX = halfLineWidth * sin(angle);
    float offsetY = halfLineWidth * cos(angle);

    // Calculate the coordinates for drawing lines
    float startX1 = startX + offsetX;
    float startY1 = startY - offsetY;
    float endX1 = endX + offsetX;
    float endY1 = endY - offsetY;
    float startX2 = startX - offsetX;
    float startY2 = startY + offsetY;
    float endX2 = endX - offsetX;
    float endY2 = endY + offsetY;

    // Set the drawing color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw the lines
    int maxdY = SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT;
    if (startY1 <= maxdY && endY1 <= maxdY)
        SDL_RenderDrawLine(renderer, static_cast<int>(startX1), static_cast<int>(startY1),
                           static_cast<int>(endX1), static_cast<int>(endY1));
    if (startY2 <= maxdY && endY2 <= maxdY)
        SDL_RenderDrawLine(renderer, static_cast<int>(startX2), static_cast<int>(startY2),
                           static_cast<int>(endX2), static_cast<int>(endY2));

    // Calculate the rectangle coordinates for filling
    std::vector<float> xValues = { startX1, endX1, startX2, endX2 };
    std::vector<float> yValues = { startY1, endY1, startY2, endY2 };
    float minX = *std::min_element(xValues.begin(), xValues.end());
    float minY = *std::min_element(yValues.begin(), yValues.end());
    float maxX = *std::max_element(xValues.begin(), xValues.end());
    float maxY = *std::max_element(yValues.begin(), yValues.end());

    // Draw the filled rectangle
    if (startY1 <= maxdY && endY1 <= maxdY)
    {
        SDL_Rect fillRect = { static_cast<int>(minX), static_cast<int>(minY),
                              static_cast<int>(maxX - minX), static_cast<int>(maxY - minY) };
        SDL_RenderFillRect(renderer, &fillRect);
    }
}







void renderColorBoxes()
{
    for (int i = 0; i < NUM_COLORS; ++i)
    {
        SDL_Rect colorBox = { i * COLOR_BOX_WIDTH, SCREEN_HEIGHT - BOTTOM_BAR_HEIGHT, COLOR_BOX_WIDTH, BOTTOM_BAR_HEIGHT };
        SDL_Color color = randomColors[i];
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &colorBox);
        colorBoxes.push_back(colorBox);
    }
    SDL_RenderPresent(renderer);
}

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_a && lineWidth < MAX_LINE_WIDTH)
                {
                    lineWidth++;
                    SDL_Log("Line width increased: %d", lineWidth);
                }
                else if (event.key.keysym.sym == SDLK_z && lineWidth > MIN_LINE_WIDTH)
                {
                    lineWidth--;
                    SDL_Log("Line width decreased: %d", lineWidth);
                }
                break;
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
                    // check if clicked within color boxes
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
                    drawLine(renderer, startX, startY, mouseX, mouseY, currentColor);
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



int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("QCM Paint Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

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

        if (frameDuration < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frameDuration);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
