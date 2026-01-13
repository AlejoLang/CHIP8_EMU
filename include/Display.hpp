#pragma once
#include <cstdint>
#include <sixel.h>

class Display {
  private:
    uint16_t width;
    uint16_t height;
    unsigned char* buffer;
    uint64_t buffer_lenght;

  public:
    Display(uint16_t w, uint16_t h);
    void set_pixel(uint16_t x, uint16_t y, unsigned char r, unsigned char g, unsigned char b);
    void draw();
    void clear();
    void clear_screen();
    ~Display();
};