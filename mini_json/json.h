#pragma once

#include <type_traits>

class json;
struct JObj;

template< typename T>
void load(json j, T& t);

class json {

	const JObj* obj = nullptr;
	friend class JsonParser;

	json(const JObj* in_obj) : obj(in_obj) {}

public:

	// Can be called even when this is null
	bool isArray() const;
	bool isObject() const;
	bool isLiteral() const;
	bool is_bool() const;
	bool is_string() const;
	bool is_number() const;
	size_t size() const;

	int count(const char* key) const;

	template< typename T>
	T value(const char* key, const T default_value) const {
		if (!isObject())
			return default_value;

		json jkey = operator[](key);
		if( !jkey ) 
			return default_value;

		T t( default_value );
		tryLoad(jkey, t);
		return t;
	}

	template< typename T>
	T get() const {
		T t;
		tryLoad(*this, t);
		return t;
	}

	const char* key(size_t idx) const;

	json operator[](const char* key) const;
	json operator[](size_t idx) const;

	explicit operator bool() const;
	operator const char*() const;

	json() = default;
	static json empty_obj();

	template< typename T>
	operator T() const {
		return get<T>();
	}

	friend void load<float>(json j, float& v);
	friend void load<int>(json j, int& v);
	friend void load<bool>(json j, bool& v);
	friend void load<const char*>(json j, const char*& v);
};

template< typename T>
void onEachArr(const json& jarr, T fn) {
	if (jarr.isArray()) {
		size_t sz = jarr.size();
		for (size_t i = 0; i < sz; ++i)
			fn(jarr[i], i);
	}
}

template< typename T>
void onEachObj(const json& jobj, T fn) {
	if (jobj.isObject()) {
		size_t sz = jobj.size();
		for (size_t i = 0; i < sz; ++i)
			fn(jobj.key(i), jobj[i] );
	}
}

// SFINAE check for the existence of the load method. Default is you don't have
template <typename T, typename = void>
struct has_load_method : std::false_type {};

// But if T.load(json) is not an error, you have it
template <typename T>
struct has_load_method<T, std::void_t<decltype(std::declval<T>().load(std::declval<const json&>()))>> : std::true_type {};

template< typename T>
void tryLoad(json j, T& t) {
	// Sometimes it might be more convenient
	if constexpr (has_load_method<T>::value) 
		t.load( j );
	else 
		load( j, t );
}

class JsonParser;
JsonParser* allocJsonParser();
void freeJsonParser(JsonParser* p);
json parseJson(JsonParser* p, const char* buf, size_t nbytes);
const char* getParseErrorText(JsonParser* p);

// Extensions for vector and unordered map
#include <vector>
template<typename T>
void load(json j, std::vector<T>& v) {
	size_t sz = j.size();
	v.resize(sz);
	for (size_t i = 0; i < sz; ++i)
		tryLoad(j[i], v[i]);
}

#include <string>
#include <unordered_map>
#include <cassert>
template<typename T>
void load(json j, std::unordered_map<std::string,T>& v) {
	assert(j.isObject());
	for (size_t i = 0; i < j.size(); ++i) {
		const char* key = j.key(i);
		json value = j[i];
		tryLoad(value, v[key]);
	}
}

