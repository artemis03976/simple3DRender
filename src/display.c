#include <stdio.h>
#include <math.h>
#include "display.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

static uint32_t* color_buffer = NULL;
static float *z_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;

static int window_width = 800;
static int window_height = 600;

bool RenderMode_Vertex = false;
bool RenderMode_Wireframe = true;
bool RenderMode_Fill = false;
bool RenderMode_Texture = false;
bool CullMode_Back = true;

int getWindowWidth(void){
    return window_width;
}

int getWindowHeight(void){
    return window_height;
}

float getZBufferAt(int x, int y){
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) return 1.0;
    return z_buffer[y * window_width + x];
}

void setZBufferAt(int x, int y, float value){
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) return;
    z_buffer[y * window_width + x] = value;
}

bool initialize_window(void) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query fullscreen resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create a SDL window
    window = SDL_CreateWindow(
        "My Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (renderer == NULL) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // allocate color buffer
    color_buffer = (uint32_t*) malloc(window_width * window_height * sizeof(uint32_t));
    // allocate z buffer
    z_buffer = (float*) malloc(window_width * window_height * sizeof(float));

    color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
    );

    return true;
}

void draw_grid(void){
    for (int j = 0; j < window_height; j += 10){
        for (int i = 0; i < window_width; i += 10){
            color_buffer[(j * window_width) + i] = 0xFF444444;
        }
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color){
    for (int j = y; j < y + height; j++){
        for (int i = x; i < x + width; i++){
            draw_pixel(i, j, color);
        }
    }
}

void draw_pixel(int x, int y, uint32_t color){
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) return;
    color_buffer[(y * window_width) + x] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color){
    int dx = x1 - x0;
    int dy = y1 - y0;

    int side_length = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float x_inc = dx / (float)side_length;
    float y_inc = dy / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++){
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void render_color_buffer(void){
    SDL_UpdateTexture(
            color_buffer_texture,
            NULL,
            color_buffer,
            (int)window_width * sizeof(uint32_t)
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void clear_color_buffer(uint32_t color){
    for (int i = 0; i < window_width * window_height; i++){
        color_buffer[i] = color;
    }
}

void clear_z_buffer(void){
    for (int i = 0; i < window_width * window_height; i++){
        z_buffer[i] = 1.0;
    }
}

void destroy_window(void){
    free(color_buffer);
    free(z_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
