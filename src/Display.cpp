#include "../include/Display.hpp"
#include "fstream"
#include <iostream>

static std::string base64_encode(const unsigned char* data, size_t len) {
    static const char base64_chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (len--) {
        char_array_3[i++] = *(data++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }

    return ret;
}

Display::Display(uint16_t w, uint16_t h) {
    this->width = w;
    this->height = h;
    this->buffer_lenght = w * h * 3;
    this->buffer = new unsigned char[this->buffer_lenght]();
}

void Display::set_pixel(uint16_t x, uint16_t y, unsigned char r, unsigned char g, unsigned char b) {
    int pos = (x + (y * this->width)) * 3;
    if (pos < 0 || pos >= (this->buffer_lenght)) {
        std::cout << "Error: Trying to write outside buffer limits [" << x << ", " << y << "]"
                  << std::endl;
        return;
    }
    this->buffer[pos] = r;
    this->buffer[pos + 1] = g;
    this->buffer[pos + 2] = b;
}

void Display::draw() {
    std::string encoded = base64_encode(this->buffer, this->buffer_lenght);

    const size_t chunk_size = 4096;
    size_t offset = 0;

    while (offset < encoded.size()) {
        size_t remaining = encoded.size() - offset;
        size_t current_chunk = (remaining > chunk_size) ? chunk_size : remaining;
        bool is_last = (offset + current_chunk >= encoded.size());

        if (offset == 0) {

            printf("\033_Ga=T,f=24,s=%d,v=%d,m=%d;", width, height, is_last ? 0 : 1);
        } else {

            printf("\033_Gm=%d;", is_last ? 0 : 1);
        }

        std::cout << encoded.substr(offset, current_chunk);
        printf("\033\\");

        offset += current_chunk;
    }

    std::cout << std::endl;
}

void Display::clear() {
    for (int i = 0; i < this->buffer_lenght; i++) {
        this->buffer[i] = 0;
    }
}

void Display::clear_screen() {
    printf("\033[2J\033[H");
    printf("\033_Ga=d;\033\\");
    std::cout.flush();
}

Display::~Display() {
    delete[] this->buffer;
}