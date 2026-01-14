#include "../include/Display.hpp"
#include <SDL3/SDL.h>
#include <iostream>

Display::Display(const char* name, uint16_t window_w, uint16_t window_h, uint16_t texture_w,
                 uint16_t texture_h) {
    this->window_width = window_w;
    this->window_height = window_h;
    this->texture_width = texture_w;
    this->texture_height = texture_h;
    this->window = SDL_CreateWindow(name, window_w, window_h, SDL_WINDOW_RESIZABLE);
    this->rederer = SDL_CreateRenderer(this->window, NULL);
    this->texture = SDL_CreateTexture(this->rederer, SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_STREAMING, texture_w, texture_h);
    SDL_SetTextureScaleMode(this->texture, SDL_SCALEMODE_PIXELART);
}

void Display::set_texture(uint32_t* texture) {
    SDL_UpdateTexture(this->texture, NULL, texture, sizeof(uint32_t) * this->texture_width);
}

void Display::draw() {
    SDL_RenderClear(this->rederer);
    SDL_RenderTexture(this->rederer, this->texture, NULL, NULL);
    SDL_RenderPresent(this->rederer);
}

Display::~Display() {
    SDL_DestroyTexture(this->texture);
    SDL_DestroyRenderer(this->rederer);
    SDL_DestroyWindow(this->window);
}