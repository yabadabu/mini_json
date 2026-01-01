#pragma once

#include <vector>
#include "json.h"

namespace MiniJson {

  class json::DataContainer {

    using Buffer = std::vector< uint8_t >;

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
