#pragma once

#include <vector>
#include <cassert>
#include <cstdio>
#include "json.h"

struct Buffer : public std::vector< uint8_t > {
	bool load(const char* filename) {
		FILE* f = fopen(filename, "rb");
		if (!f)
			return false;
		fseek(f, 0, SEEK_END);
		size_t sz = ftell(f);
		fseek(f, 0, SEEK_SET);
		resize(sz);
		size_t bytes_read = fread(data(), 1, size(), f);
		assert(bytes_read == size());
		fclose(f);
		return true;
	}
};

class JsonFile {

	Buffer     buf;
	JsonParser* parser = nullptr;
	json        j;

public:
	JsonFile(const char* filename);
	~JsonFile();
	const char* getParseErrorText() const;
	operator json() const { return j; }
};

class JsonStr {
	JsonParser* parser = nullptr;
	json        j;
public:
	JsonStr(const char* filename);
	~JsonStr();
	operator json() const { return j; }
};