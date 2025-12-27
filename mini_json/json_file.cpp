#include "json_file.h"

bool unscapeUnicodes(unsigned char* data, size_t data_size, size_t& new_size) {

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

JsonFile::~JsonFile() {
	freeJsonParser(parser);
}

const char* JsonFile::getParseErrorText() const {
	return ::getParseErrorText(parser);
}

JsonFile::JsonFile(const char* filename) {

	parser = allocJsonParser();

	if (!buf.load(filename)) {
		printf("Failed to load json file %s\n", filename);
		return;
	}
	//printf( "Read %zu\n", buf.size() );
	size_t new_size = 0;
	if( !unscapeUnicodes(buf.data(), buf.size(), new_size )) {
		//parseJsonSetError( parser, "Invalid unicode characters in input json");
		return;
	}
	// for( size_t k=0; k<new_size; ++k ) {
	// 	printf( "unscaped byte %zu is %c\n", k, buf[k]);
	// }

	j = parseJson(parser, (const char*)buf.data(), new_size);
	if (!j) {
		printf("Invalid json read from file %s: %s\n", filename, getParseErrorText());
		return;
	}
}
