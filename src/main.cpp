#include "../include/CHIP8.hpp"
#include "../include/Display.hpp"
#include "../include/Keypad.hpp"
#include "../include/Memory.hpp"
#include <SDL3/SDL.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

uint32_t keymap[16] = {
    // Maps SDL Key events to the keypads index
    SDLK_0, SDLK_1, SDLK_S, SDLK_3, SDLK_A, SDLK_5, SDLK_D, SDLK_7,
    SDLK_W, SDLK_9, SDLK_E, SDLK_R, SDLK_T, SDLK_Y, SDLK_U, SDLK_I};

static int load_program(const char* file_path, Memory* main_memory) {
    std::ifstream rom(file_path, std::ios::binary);
    if (!rom.is_open()) {
        std::cerr << "File couldn't be opened" << std::endl;
        return 1;
    }
    rom.seekg(0, std::ios::end);
    std::streampos size = rom.tellg();
    rom.seekg(0, std::ios::beg);
    if (size > (4096 - 512)) {
        std::cerr << "ROM too large" << std::endl;
        rom.close();
        return 1;
    }

    char* mem_buffer = new char[size];
    rom.read(mem_buffer, size);
    rom.close();

    for (int i = 0; i < size; ++i) {
        main_memory->set_byte(512 + i, (uint8_t)mem_buffer[i]);
    }

    delete[] mem_buffer;
    return 0;
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Error intitializing SDL" << std::endl;
        return 1;
    }
    Display* disp = new Display("CHIP8", 1024, 720, 64, 32);
    Memory* main_memory = new Memory(4096);
    Memory* graphics_memory = new Memory(64 * 32); // Holds B/W values for a 64x32 screen
    Keypad* keypad = new Keypad();
    CHIP8* chip = new CHIP8();
    bool exit = false;

    uint32_t display_buffer[64 * 32];

    if (argc < 2) {
        std::cout << "Path needed" << std::endl;
        return 1;
    }
    char* path = argv[1];
    if (load_program(path, main_memory) != 0) {
        return 1;
    }

    while (!exit) {
        uint64_t ticks_start = SDL_GetTicks();
        for (int i = 0; i < 10; ++i) {
            chip->run_cycle(main_memory, graphics_memory, keypad);
        }
        chip->update_timers();

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_KEY_DOWN: {
                switch (e.key.key) {
                case SDLK_Q:
                    exit = true;
                    break;
                default:
                    for (int i = 0; i < 15; ++i) {
                        if (e.key.key == keymap[i]) {
                            keypad->set_key_status(i, 1);
                        }
                    }
                    break;
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                for (int i = 0; i < 15; ++i) {
                    if (e.key.key == keymap[i]) {
                        keypad->set_key_status(i, 0);
                    }
                }
                break;
            }
            case SDL_EVENT_QUIT: {
                exit = true;
            }
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
                exit = true;
            }
            default:
                break;
            }
        }

        if (chip->get_print_signal()) {
            chip->set_print_signal(false);
            for (int row = 0; row < 32; ++row) {
                for (int col = 0; col < 64; ++col) {
                    uint8_t pixel = graphics_memory->get_byte((row * 64) + col);
                    if (pixel == 1) {
                        display_buffer[(row * 64) + col] = 0xFFFFFFFF;
                    } else {
                        display_buffer[(row * 64) + col] = 0x00000000;
                    }
                }
            }
            disp->set_texture(display_buffer);
            disp->draw();
        }
        uint64_t ticks_elapsed = SDL_GetTicks() - ticks_start;
        if ((1000 / 60) > ticks_elapsed) {
            SDL_Delay((1000 / 60) - ticks_elapsed);
        }
    }

    delete disp;
    delete main_memory;
    delete graphics_memory;
    delete keypad;
    delete chip;
    return 0;
}