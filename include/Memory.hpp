#pragma once
#include <cstdint>

class Memory {
  private:
    uint16_t size;
    uint8_t* start;

  public:
    Memory(uint16_t size);
    void set_byte(uint16_t address, uint8_t data);
    uint8_t get_byte(uint16_t address);
    ~Memory();
};