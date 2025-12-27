#include <stdio.h>
#include "mini_json/json.h"
#include "mini_json/json_file.h"

int main(int argc, char** argv ) {
  if( argc < 2 ) {
    printf( "Usage: %s <in_json>\n", argv[0] );
    return -1;
  }
  const char* infilename = argv[1];
  JsonFile jfile( infilename );
  json j = jfile;
  if( !j ) {
    printf( "Parse failed\n");
    return -1;
  }
  printf( "Parse success\n");
  return 0;
  // if( j.isArray() ) {
  //   printf( "Array of %ld elems\n", j.size() );
  // }
  // else if( j.isObject() ) {
  //   printf( "Object of %ld elems\n", j.size() );
  // }
  // else if( j.isLiteral() ) {
  //   if( j.is_bool() ) {
  //     printf( "bool\n" );
  //   }
  //   else if( j.is_string() ) {
  //     printf( "str\n" );
  //   }
  //   else if( j.is_number() ) {
  //     printf( "number\n" );
  //   }
  //   else {
  //     printf( "literal\n" );
  //   }
  // }
  // else {
  //   printf( "Other\n" );
  // }
  // return 0;
}