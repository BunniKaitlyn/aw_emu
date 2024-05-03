#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace aw_emu {

class util {
public:
  static bool read_binary(const char* filename, long* out_size, char** out_buffer) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
      return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size * sizeof(char));
    fread(buffer, size, 1, file);
    fclose(file);

    *out_size = size;
    *out_buffer = buffer;
    return true;
  }

};

}
