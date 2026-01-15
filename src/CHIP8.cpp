#include "../include/CHIP8.hpp"
#include <cmath>
#include <iostream>

CHIP8::CHIP8() {
    this->pc = 0x200;
    this->I = 0;
    this->registers = new uint8_t[16]();

    this->sp = 0;
    this->stack = new uint16_t[16]();

    this->delay_timer = 0;
    this->sound_timer = 0;
}

void CHIP8::clear_state() {
    this->pc = 0x200;
    this->I = 0;

    for (uint8_t i = 0; i < 16; ++i) {
        this->registers[i] = 0;
    }

    this->sp = 0;

    for (uint8_t i = 0; i < 16; ++i) {
        this->stack[i] = 0;
    }

    this->delay_timer = 0;
    this->sound_timer = 0;
}

void CHIP8::run_cycle(Memory* main_memory, Memory* graphics_memory, Keypad* keypad) {
    if (this->print_signal) {
        return;
    }
    uint16_t opcode = main_memory->get_byte(this->pc) << 8;
    opcode = opcode | main_memory->get_byte(this->pc + 1);
    switch (opcode & 0xF000) {
    case 0x0000: {
        switch (opcode & 0x000F) {
        case 0x0000: { // Clear screen 0x00E0
            this->print_signal = 1;
            for (uint16_t i = 0; i < graphics_memory->get_size(); ++i) {
                graphics_memory->set_byte(i, 0);
            }
            this->pc += 2;
            break;
        }
        case 0x000E: { // Return from subrutine 0x00EE
            this->sp -= 1;
            this->pc = this->stack[this->sp];
            this->pc += 2;
            break;
        }
        default: {
            std::cerr << std::hex << "Unknown opcode " << opcode << std::endl;
            break;
        }
        }
        break;
    }
    case 0x1000: { // 1NNN - Jump to address NNN
        this->pc = opcode & 0xFFF;
        break;
    }
    case 0x2000: { // 2NNN - Jump to subrutine NNN
        this->stack[this->sp] = this->pc;
        this->sp++;
        this->pc = opcode & 0xFFF;
        break;
    }
    case 0x3000: { // 3XNN - Skips next instruction if Vx equals NN
        if (this->registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
            this->pc += 4;
        } else {
            this->pc += 2;
        }
        break;
    }
    case 0x4000: { // 4XNN - Skips the next instruction if VX does not equal NN
        if (this->registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
            this->pc += 4;
        } else {
            this->pc += 2;
        }
        break;
    }
    case 0x5000: { // 5XY0 - Skips the next instruction if VX equals VY
        if (this->registers[(opcode & 0x0F00) >> 8] == this->registers[(opcode & 0x0F0) >> 4]) {
            this->pc += 4;
        } else {
            this->pc += 2;
        }
        break;
    }
    case 0x6000: { // 6XNN - Sets VX to NN
        this->registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        this->pc += 2;
        break;
    }
    case 0x7000: { // 7XNN - Adds NN to VX (Carry flag doesn't change)
        this->registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        this->pc += 2;
        break;
    }
    case 0x8000: {
        switch (opcode & 0x000F) {
        case 0x0000: { // 8XY0 - Sets VX to the value of VY
            this->registers[(opcode & 0x0F00) >> 8] = this->registers[(opcode & 0x00F0) >> 4];
            this->pc += 2;
            break;
        }
        case 0x0001: { // 8XY1 - Sets VX to VX or VY (bitwise or)
            this->registers[(opcode & 0x0F00) >> 8] |= this->registers[(opcode & 0x00F0) >> 4];
            this->registers[0xF] = 0;
            this->pc += 2;
            break;
        }
        case 0x0002: { // 8XY2 - Sets VX to VX and VY (bitwise and)
            this->registers[(opcode & 0x0F00) >> 8] &= this->registers[(opcode & 0x00F0) >> 4];
            this->registers[0xF] = 0;
            this->pc += 2;
            break;
        }
        case 0x0003: { // 8XY3 - Sets VX to VX xor VY (bitwise xor)
            this->registers[(opcode & 0x0F00) >> 8] ^= this->registers[(opcode & 0x00F0) >> 4];
            this->registers[0xF] = 0;
            this->pc += 2;
            break;
        }
        case 0x0004: { // 8XY4 - Adds VY to VX, sets VF to 1 if there's an overflow
            this->registers[(opcode & 0x0F00) >> 8] += this->registers[(opcode & 0x00F0) >> 4];
            this->registers[0xF] =
                this->registers[(opcode & 0x00F0) >> 4] > (this->registers[(opcode & 0x0F00) >> 8]);
            this->pc += 2;
            break;
        }
        case 0x0005: { // 8XY5 - Substracts VY from VX, sets VF to 0 if there's an underflow
            this->registers[(opcode & 0x0F00) >> 8] -= this->registers[(opcode & 0x00F0) >> 4];
            this->registers[0xF] = this->registers[(opcode & 0x00F0) >> 4] <=
                                   (0xFF - this->registers[(opcode & 0x0F00) >> 8]);
            this->pc += 2;
            break;
        }
        case 0x0006: { // 8XY6 - Shifts VX to the right by 1 bit, stores the LSB bit on VF
            this->registers[0xF] = this->registers[(opcode & 0x00F0) >> 4] & 0x1;
            this->registers[(opcode & 0x0F00) >> 8] = this->registers[(opcode & 0x00F0) >> 4] >> 1;
            this->pc += 2;
            break;
        }
        case 0x0007: { // 8XY7 - Sets VX to VY minus VX, VF is set to 0 when there's an underflow
            this->registers[(opcode & 0x0F00) >> 8] =
                this->registers[(opcode & 0x00F0) >> 4] - this->registers[(opcode & 0x0F00) >> 8];
            this->registers[0xF] =
                this->registers[(opcode & 0x0F00) >> 8] <= this->registers[(opcode & 0x00F0) >> 4];
            this->pc += 2;
            break;
        }
        case 0x000E: { // 8XYE - Shifts VX to the left by 1 bit, stores the MSB bit on VF
            this->registers[0xF] = this->registers[(opcode & 0x00F0) >> 4] >> 7;
            this->registers[(opcode & 0x0F00) >> 8] = this->registers[(opcode & 0x00F0) >> 4] << 1;
            this->pc += 2;
            break;
        }
        default:
            std::cerr << std::hex << "Unknown opcode " << opcode << std::endl;
            break;
        }
        break;
    }
    case 0x9000: { // 9XY0 - Skips the next instruction if VX != VY
        if (this->registers[(opcode & 0x0F00) >> 8] != this->registers[(opcode & 0x00F0) >> 4]) {
            this->pc += 4;
        } else {
            this->pc += 2;
        }
        break;
    }
    case 0xA000: { // ANNN - Sets I to the address NNN
        this->I = opcode & 0x0FFF;
        this->pc += 2;
        break;
    }
    case 0xB000: { // BNNN - Jumps to address V0 + NNN
        this->pc = this->registers[0] + (opcode & 0x0FFF);
        break;
    }
    case 0xC000: { // CXNN Sets VX to the result of a bitwise and operation on a random number
                   // (Typically: 0 to 255) and NN
        this->registers[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
        this->pc += 2;
        break;
    }
    case 0xD000: { // DXYN - Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and
        // a height of N pixels. Each row of 8 pixels is read as bit-coded starting from
        // memory location I; I value does not change after the execution of this
        // instruction. VF is set to 1 if any screen pixels are flipped from set to unset
        // Sprites are clipped at screen boundaries (no wrapping of individual pixels)
        uint8_t height = opcode & 0x000F;
        uint8_t x =
            this->registers[(opcode & 0x0F00) >> 8] % 64; // Clip if starting pos is outside screen
        uint8_t y = this->registers[(opcode & 0x00F0) >> 4] % 32;
        this->registers[0xF] = 0;
        for (uint8_t row = 0; row < height; ++row) {
            uint8_t py = y + row;
            if (py >= 32)
                break; // Clip at bottom edge
            uint8_t sprite_byte = main_memory->get_byte(this->I + row);
            for (uint8_t column = 0; column < 8; ++column) {
                if ((sprite_byte & (0x80 >> column)) != 0) {
                    uint8_t px = x + column;
                    if (px >= 64)
                        break; // Clip at right edge
                    uint16_t pixel_index = py * 64 + px;
                    uint8_t pixel = graphics_memory->get_byte(pixel_index);
                    if (pixel == 1) {
                        this->registers[0xF] = 1;
                    }
                    graphics_memory->set_byte(pixel_index, pixel ^ 1);
                }
            }
        }
        this->print_signal = true;
        this->pc += 2;
        break;
    }
    case 0xE000: {
        switch (opcode & 0x00FF) {
        case 0x009e: { // Skips the next instruction if the key stored in VX is pressed
            if (keypad->get_key_status(this->registers[(opcode & 0x0F00) >> 8]) != 0) {
                this->pc += 4;
            } else {
                this->pc += 2;
            }
            break;
        }
        case 0x00A1: { // Skips the next instruction if the key stored in VX is not pressed
            if (keypad->get_key_status(this->registers[(opcode & 0x0F00) >> 8]) == 0) {
                this->pc += 4;
            } else {
                this->pc += 2;
            }
            break;
        }
        default:
            std::cerr << std::hex << "Unknown opcode " << opcode << std::endl;
            break;
        }
        break;
    }
    case 0xF000: {
        switch (opcode & 0x00FF) {
        case 0x0007: { // FX07 - Sets VX to the value of the delay timer
            this->registers[(opcode & 0x0F00) >> 8] = this->delay_timer;
            this->pc += 2;
            break;
        }
        case 0x000A: { // FX0A - A key press is awaited, and then stored in VX (blocking operation,
                       // all instruction halted until next key event, delay and sound timers should
                       // continue processing)
            if (!this->key_halted) {
                for (uint8_t i = 0; i < 16; ++i) {
                    if (keypad->get_key_status(i) == 1) {
                        this->registers[(opcode & 0x0F00) >> 8] = i;
                        this->key_halted = true;
                        break;
                    }
                }
            } else {
                uint8_t key_index = this->registers[(opcode & 0x0F00) >> 8];
                if (keypad->get_key_status(key_index) == 0) {
                    this->key_halted = false;
                    this->pc += 2;
                }
            }
            break;
        }
        case 0x0015: { // FX15 - Sets the delay timer to VX
            this->delay_timer = this->registers[(opcode & 0x0F00) >> 8];
            this->pc += 2;
            break;
        }
        case 0x0018: { // FX18 - Sets the sound timer to VX
            this->sound_timer = this->registers[(opcode & 0x0F00) >> 8];
            this->pc += 2;
            break;
        }
        case 0x001E: { // FX1E - Adds VX to I. VF is not affected
            this->I += this->registers[(opcode & 0x0F00) >> 8];
            this->pc += 2;
            break;
        }
        case 0x0029: { // FX29 - Sets I to the location of the sprite for the character in VX.
                       // Characters
            // 0-F (in hexadecimal) are represented by a 4x5 font.
            this->I = this->registers[(opcode & 0x0F00) >> 8] * 0x05;
            this->pc += 2;
            break;
        }
        case 0x0033: { // FX33 - Stores the binary-coded decimal representation of VX, with the
                       // hundreds digit in memory at location in I, the tens digit at location I+1,
                       // and the ones digit at location I+2
            main_memory->set_byte(this->I, this->registers[(opcode & 0x0F00) >> 8] / 100);
            main_memory->set_byte(this->I + 1,
                                  (this->registers[(opcode & 0x0F00) >> 8] % 100) / 10);
            main_memory->set_byte(this->I + 2, (this->registers[(opcode & 0x0F00) >> 8] % 10));
            this->pc += 2;
            break;
        }
        case 0x0055: { // FX55 - Stores from V0 to VX (including VX) in memory, starting at address
                       // I. I is changed to the end offset
            for (uint8_t i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
                main_memory->set_byte(this->I + i, this->registers[i]);
            }
            this->I += ((opcode & 0x0F00) >> 8) + 1;
            this->pc += 2;
            break;
        }
        case 0x0065: { // FX65 - Fills from V0 to VX (including VX) with values from memory,
                       // starting at address I. The offset from I is increased by 1 for each value
                       // read, but I itself is left unmodified
            for (uint8_t i = 0; i <= ((opcode & 0x0F00) >> 8); ++i) {
                this->registers[i] = main_memory->get_byte(this->I + i);
            }
            this->I += ((opcode & 0x0F00) >> 8) + 1;
            this->pc += 2;
            break;
        }
        default:
            std::cerr << std::hex << "Unknown opcode " << opcode << std::endl;
            break;
        }
        break;
    }
    default:
        std::cerr << std::hex << "Unknown opcode " << opcode << std::endl;
        break;
    }
}

bool CHIP8::get_print_signal() {
    return this->print_signal;
}

void CHIP8::set_print_signal(bool v) {
    this->print_signal = v;
}

void CHIP8::update_timers() {
    if (this->delay_timer > 0) {
        --this->delay_timer;
    }
    if (this->sound_timer > 0) {
        --this->sound_timer;
    }
}

CHIP8::~CHIP8() {
    delete[] this->stack;
    delete[] this->registers;
}