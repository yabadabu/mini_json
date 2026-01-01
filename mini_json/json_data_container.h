#pragma once

#include <vector>
#include "json.h"

namespace MiniJson {

  class Buffer : public std::vector< uint8_t > {
  public:
    bool save(const char* filename) const;
    void append(char c) {
      push_back((uint8_t)c);
    }
    void append(const char* c, size_t sz) {
      std::vector<uint8_t>::insert(end(), (const uint8_t*)c, (const uint8_t*)(c + sz));
    }
  };

  class json::DataContainer {

    Buffer      buf;
    Parser*     parser = nullptr;
    json        j;

  public:
    DataContainer() = default;
    DataContainer(const char* filename);
    ~DataContainer();
    bool loadFromFile(const char* filename);
    const char* getParseErrorText() const;
    operator json() const { return j; }
  };

}
