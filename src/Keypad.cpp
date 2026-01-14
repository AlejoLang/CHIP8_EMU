#include "../include/Keypad.hpp"
#include <iostream>

Keypad::Keypad() {
    this->keys = new bool[16]();
}

void Keypad::set_key_status(int index, bool status) {
    if (index < 0 || index > 15) {
        std::cerr << "Key index out of range " << index << std::endl;
        return;
    }
    this->keys[index] = status;
}

bool Keypad::get_key_status(int index) {
    if (index < 0 || index > 15) {
        std::cerr << "Key index out of range " << index << std::endl;
        return;
    }
    return this->keys[index];
}

Keypad::~Keypad() {
    delete[] this->keys;
}