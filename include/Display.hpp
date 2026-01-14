#pragma once
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cstdint>

class Display {
  private:
    uint16_t window_width;
    uint16_t window_height;
    uint16_t texture_width;
    uint16_t texture_height;
    SDL_Window* window;
    SDL_Renderer* rederer;
    SDL_Texture* texture;
    uint64_t buffer_lenght;

  public:
    Display(const char* name, uint16_t window_w, uint16_t window_h, uint16_t texture_w,
            uint16_t texture_h);
    void set_texture(uint32_t* texture);
    void draw();
    ~Display();
};