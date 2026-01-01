#pragma once

#include <vector>
#include <cassert>
#include <cstdio>
#include "json.h"

class JsonDataContainer {

	using Buffer = std::vector< uint8_t >;

	Buffer      buf;
	JsonParser* parser = nullptr;
	json        j;

public:
	JsonDataContainer() = default;
	JsonDataContainer(const char* filename);
	~JsonDataContainer();
	bool loadFromFile(const char* filename);
	const char* getParseErrorText() const;
	operator json() const { return j; }
};

