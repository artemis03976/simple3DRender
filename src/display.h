#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdbool.h>

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

extern bool RenderMode_Vertex;
extern bool RenderMode_Wireframe;
extern bool RenderMode_Fill;
extern bool RenderMode_Texture;
extern bool CullMode_Back;

int getWindowWidth(void);
int getWindowHeight(void);
float getZBufferAt(int x, int y);

void setZBufferAt(int x, int y, float value);

bool initialize_window(void);
void draw_grid(void);
void draw_rect(int x, int y, int width, int height, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_z_buffer(void);
void destroy_window(void);

#endif //DISPLAY_H
