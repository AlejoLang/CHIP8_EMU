#pragma once

class Keypad {
  private:
    bool* keys;

  public:
    Keypad();
    void set_key_status(int index, bool status);
    bool get_key_status(int index);
    ~Keypad();
};