#pragma once

#include "json_data_container.h"

// Extensions for vector and unordered map
#include <vector>
template<typename T>
void load(MiniJson::json j, std::vector<T>& v) {
  size_t sz = j.size();
  v.resize(sz);
  for (size_t i = 0; i < sz; ++i)
    tryLoad(j[i], v[i]);
}

#include <string>
#include <unordered_map>
#include <cassert>
template<typename T>
void load(MiniJson::json j, std::unordered_map<std::string, T>& v) {
  assert(j.isObject());
  for (size_t i = 0; i < j.size(); ++i) {
    const char* key = j.key(i);
    MiniJson::json value = j[i];
    tryLoad(value, v[key]);
  }
}

