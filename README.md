 # mini_json

This is a json parser to read (there is no write support) and feed structs and classes. It's an over reduction of the features provided by the great https://github.com/nlohmann/json just for my needs, maybe it fits yours. It compiles faster, has less dependencies and uses less memory. The usage syntax is similar / almost equal.

# Use case

* You want your project to compile fast. Including json.h only add's the include <type_traits>
* You do not need to write json files, you just consume json's.
* You parse a json from files, extract data and forget about the json itself.
* You can use C++17

# Basics

You read the file with:

```cpp
JsonFile jfile( "items.json" );
```

And get the root json value with:

```cpp
json j = jfile;
```

The `sizeof(json) = 8`, so you can copy/move without worries, as the actual data is owned by the JsonFile object. So do not destroy it.

# Usage

Given a custom struct `Item`

```cpp
struct Item {
    std::string name;
    int         count = 1;
};
```

You can declare the public method `load`

```cpp
struct Item {
    std::string name;
    int         count = 1;
    void load( json j ) { ... }
};
```

Or you can declare the global function `load`

```cpp
void load( json j, Item& item ) { ... }
```

Everytime a json is converted to a C++ type, the templates will trigger a call to the `T::load` if exists or a call the global `::load` function otherwise. If not implemented, the linker will complain about it.

The conversion from json to basic types : `bool/float/int/const char*` is already defined. Your types require the implementation of previous method..

Conversion from `json` array to `std::vector<T>` and `std::unordered_map<std::string,T>` is automatic as long as the conversion from json to T is provided and the json object is an array / object.

To convert a json to a type T, just directly assign it:

```cpp
Item item = j;
```

This will trigger the templated cast operator to T and call the method appropiate `load` method.

# Object properties

```cpp
json jobj = j["items"];              // Return a new json
int score = j.value( "score", 5 );   // Return the property 'score' as int 5
```

# Iterating json

I use the function `onEachArr` to iterate over the elements of an array.

```cpp
onEachArr(jobj, [](json j, size_t idx) {
	// j contains the nth item in the jobj
	// ...
	});

onEachArr(j["items"], [](const Item& item, size_t idx) {
	// j has already been converted to Item during the call to the lambda
	// ...
	});
```

And the `onEachObj` when I want to iterate over the properties key/value of an json object.

```cpp
onEachObj( jobj, [](const char* key, json jitem) {
	// ...
	})
```
Or:

```cpp
onEachObj( jobj, [](const std::string& name, const Item& jitem) {
	// ...
	})
```
# Integration

Just add the files json.cpp and json_file.cpp in your project. Check the sample/demo.cpp for more examples.
 
