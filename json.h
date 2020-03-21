#ifndef JSON_H
#define JSON_H

struct JsonString {
  unsigned long lenght;
  char *str;
};

struct JsonArray;
struct JsonObject;

enum JsonBool {
  JSON_TRUE,
  JSON_FALSE
};

struct JsonValue {
  enum {
    JSON_NULL,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
  } type;
  union {
    struct JsonObject *object;
    struct JsonArray *array;
    struct JsonString string;
    enum JsonBool jbool;
    double number;
  } value;
};

struct JsonObject {
  struct JsonString name;
  struct JsonValue element;
  struct JsonObject *next;
};

struct JsonArray {
  struct JsonValue element;
  struct JsonArray *next;
};

struct JsonValue json_parse(const char *json);
void json_print(struct JsonValue *json);
void json_free(struct JsonValue *json);

#endif
