#include <cstdio>
#include <string>
#include <unordered_map>
#include "mini_json/json.h"
#include "mini_json/json_file.h"

/*
{
 "inventory": [
	{
	 "name": "Water",
	 "count": 3
	},
	{
	 "name": "Food",
	 "count": 5
	}
 ],
 "weapon": {
	"name": "Axe"
 },
 "my_score": {
	"score": 100
 },
 "obj.properties": {
	"color": "blue",
	"age": 99,
	"saved" : true,
	"country" : "Spain"
 },
 "dict": {
	"north": {
	 "name": "Sword"
	},
	"south": {
	 "name": "Spear"
	},
	"west": {
	 "name": "Mace"
	}
 }
}
*/

// --------------------------------------
// My class, I want to read from a json
struct Item {
	std::string name;
	int         count = 1;

	void load(const json& j) {
		// Override values, if the json does not provide 
		// the attributes, the current values are kept
		name = j.value("name", name.c_str());
		count = j.value("count", count);
	}
};

// --------------------------------------
struct ExistingScoreClass {
	int score = 2;
};

void load(const json& j, ExistingScoreClass& obj ) {
	obj.score = j.value("score", obj.score);
}

// --------------------------------------
int main(int argc, char** argv) {
	JsonFile jfile( "items.json" );
	json j = jfile;
	if (!j) {
		printf("Json parse failed : %s\n", jfile.getParseErrorText());
		return -1;
	}

	// Will use the Item::load method because it exists
	Item weapon = j["weapon"];
	assert(weapon.name == "Axe");
	assert(weapon.count == 1);		// As provided in the member default value

	// Will try to find the global method: void load( json, ExistingScoreClass& );
	ExistingScoreClass score = j["my_score"];
	assert( score.score == 100 );

	// Access obj property by name, provide a default value which forces the type
	std::vector< Item > items;
	items = j.value( "inventory", items );
	assert(items.size() == 2);
	assert(items[0].name == "Water");
	assert(items[0].count == 3);
	assert(items[1].name == "Food");
	assert(items[1].count == 5);

	// Mapping of std::vector is automatically provided
	std::vector< Item > items2 = j["inventory"].get< std::vector<Item> >();

	// Iterate over the items when inventory is an array
	printf("onEachArr:\n");
	onEachArr(j["inventory"], [](json jitem, size_t idx) {
		// jitem contains the idx-nth item in the json
		const char* item_name = jitem["name"];
		int count = jitem["count"];
		printf("  [%d] x%d : %s\n", (int)idx, count, item_name);
		});

	printf("onEachArr (direct Conversion):\n");
	onEachArr(j["inventory"], [](const Item& item, size_t idx) {
		printf("  [%d] x%d : %s\n", (int)idx, item.count, item.name.c_str());
		});

	// Iterate over the properties when j is an object
	printf( "Obj.Properties:\n");
	onEachObj(j["obj.properties"], [](const char* key, json jitem) {
		if (jitem.is_string()) {
			const char* str_value = jitem;
			printf("  %s : %s\n", key, str_value);
		}
		else if (jitem.is_number()) {
			float v = jitem;
			printf("  %s : %f\n", key, v);
		}
		else if (jitem.is_bool()) {
			bool v = jitem;
			printf("  %s : %s\n", key, v ? "true" : "false");
		}

		});
	
	// Populate the dictionary
	std::unordered_map< std::string, Item > dict = j["dict"];
	assert( dict[ "north"].name == "Sword" );

	// Errors should provide enough information about the problem
	JsonFile jferr("broken.json");
	json j2 = jferr;
	if (!j2) 
		printf("Json parse failed : %s\n", jferr.getParseErrorText());

	printf( "sizeof(json) = %d\n", (int) sizeof(json));

	return 0;
}