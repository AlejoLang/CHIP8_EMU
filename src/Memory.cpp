#include "../include/Memory.hpp"
#include <iostream>

Memory::Memory(uint16_t size) {
    this->size = size;
    this->start = new uint8_t[size];
}

void Memory::set_byte(uint16_t address, uint8_t data) {
    if (address < 0 || address > this->size) {
        std::cerr << std::hex << "Address out of bounds " << address << std::endl;
        return;
    }
    this->start[address] = data;
}

uint8_t Memory::get_byte(uint16_t address) {
    if (address < 0 || address > this->size) {
        std::cerr << std::hex << "Address out of bounds " << address << std::endl;
        return 0;
    }
    return this->start[address];
}

u_int16_t Memory::get_size() {
    return this->size;
}

Memory::~Memory() {
    delete[] this->start;
}