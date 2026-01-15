#pragma once
#include "Keypad.hpp"
#include "Memory.hpp"
#include <cstdint>

class CHIP8 {
  private:
    uint16_t pc; // Program counter
    uint16_t I;  // Index register

    uint8_t* registers; // V registers

    uint16_t* stack; // Stack (only holds return addresses)
    uint16_t sp;     // Stack pointer

    uint8_t delay_timer;
    uint8_t sound_timer;

    bool print_signal;
    bool key_halted;

  public:
    CHIP8();
    void clear_state();
    void run_cycle(Memory* main_memory, Memory* graphics_memory, Keypad* keypad);
    unsigned char* get_video_mem(uint16_t& size);
    bool get_print_signal();
    void set_print_signal(bool v);
    void update_timers();
    ~CHIP8();
};