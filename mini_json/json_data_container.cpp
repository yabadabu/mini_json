#include "json_data_container.h"
#include <cstdio>
#include <cassert>

namespace MiniJson
{

  Parser* allocParser();
  void freeParser(Parser* p);
  json parseJson(Parser* p, const char* buf, size_t nbytes);
  const char* getParseErrorText(Parser* p);
  void setParseErrorText(Parser* p, const char* new_error_msg);

  static bool unscapeUnicodes(unsigned char* data, size_t data_size, size_t& new_size) {

    size_t spos = 0;
    unsigned char* dst = data;
    while (spos < data_size) {
      if (data[spos] == '\\' && spos + 1 < data_size && data[spos + 1] == 'u') {
        // Extract the Unicode code point (4 hexadecimal digits)
        unsigned unicode = 0;
        for (int i = 0; i < 4; ++i) {
          int char_value = 0;
          int hex_char = data[spos + 2 + i];
          if (hex_char >= '0' && hex_char <= '9') char_value = hex_char - '0';
          else if (hex_char >= 'A' && hex_char <= 'F') char_value = 10 + hex_char - 'A';
          else if (hex_char >= 'a' && hex_char <= 'f') char_value = 10 + hex_char - 'a';
          else {
            printf("Invalid unicode found in json\n");
            return false;
          }
          unicode += char_value * (1ULL << ((3 - i) * 4));
        }

        // Convert the code point to UTF-8
        if (unicode <= 0x7F) {
          // Convert unicode \ and " in escaped \\ and \"
          if (unicode == '\"' || unicode == '\\')
            *dst++ = '\\';
          *dst++ = static_cast<char>(unicode & 0xFF);
        }
        else if (unicode <= 0x7FF) {
          *dst++ = static_cast<char>(0xC0 | ((unicode >> 6) & 0xFF));
          *dst++ = static_cast<char>(0x80 | (unicode & 0x3F));
        }
        else if (unicode <= 0xFFFF) {
          *dst++ = static_cast<char>(0xE0 | ((unicode >> 12) & 0xFF));
          *dst++ = static_cast<char>(0x80 | ((unicode >> 6) & 0x3F));
          *dst++ = static_cast<char>(0x80 | (unicode & 0x3F));
        }

        // Move the position past the Unicode escape sequence
        spos += 6;
      }
      else {

        // Copy regular characters
        *dst++ = data[spos];
        ++spos;
      }
    }
    new_size = dst - data;
    return true;
  }

  json::DataContainer::~DataContainer() {
    freeParser(parser);
  }

  const char* json::DataContainer::getParseErrorText() const {
    return MiniJson::getParseErrorText(parser);
  }

  bool json::DataContainer::loadFromFile(const char* filename) {
    parser = allocParser();

    FILE* f = fopen(filename, "rb");
    if (!f) {
      setParseErrorText(parser, "Failed to open input json file");
      return false;
    }
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf.resize(sz);
    size_t bytes_read = fread(buf.data(), 1, buf.size(), f);
    assert(bytes_read == buf.size());
    fclose(f);

    //printf( "Read %zu\n", buf.size() );
    size_t new_size = 0;
    if (!unscapeUnicodes(buf.data(), buf.size(), new_size)) {
      setParseErrorText(parser, "Invalid unicode characters in input json");
      return false;
    }
    j = parseJson(parser, (const char*)buf.data(), new_size);
    if (!j)
      return false;
    return true;
  }

  json::DataContainer::DataContainer(const char* filename) {
    loadFromFile(filename);
  }

}