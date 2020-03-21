#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

static void error(const char *msg) {
  fprintf(stderr, "Json error: %s\n", msg);
  exit(1);
}

static void *json_alloc(size_t size) {
  void *res = calloc(size, 1);
  if (res == NULL)
    error("malloc fail.");
  return res;
}

static void skip_ws(char **json) {
  while ( **json == ' '  ||  **json == '\t'
       || **json == '\r' ||  **json == '\n') (*json)++;
}

static struct JsonString pares_string(char **json) {
  int len = 0;
  while (**json != '"' && *(*json - 1) != '\\') {
    (*json)++;
    len++;
  }
  (*json)++;
  return (struct JsonString) {
    .lenght = len,
    .str = strndup(*json - 1 - len, len)
  };
}

static struct JsonValue json_parse_impl(char **json) {
  struct JsonValue res = { 0 };
  skip_ws(json);
  switch(**json) {
    case '[': {
        res.type = JSON_ARRAY;
        (*json)++;
        skip_ws(json);
        if (**json == ']') {
          (*json)++;
          break;
        }
        struct JsonArray *now = json_alloc(sizeof(struct JsonArray));
        res.value.array = now;
        for (;;) {
          skip_ws(json);

          now -> element = json_parse_impl(json);

          skip_ws(json);
          if (**json == ']') {
            (*json)++;
            break;
          }
          if (**json != ',')
            error("unexspete charactre.");
          (*json)++;
          struct JsonArray *new = json_alloc(sizeof(struct JsonArray));        
          now -> next = new;
          now = new;
        }
      } break;
    case '{': {
        res.type = JSON_OBJECT;
        (*json)++;
        skip_ws(json);
        if (**json == '}') {
          (*json)++;
          break;
        }
        struct JsonObject *now = json_alloc(sizeof(struct JsonObject));
        res.value.object = now;
        for (;;) {
          skip_ws(json);
          if (**json != '"')
            error("unexspete charactre.");
          (*json)++;

          now -> name = pares_string(json);

          skip_ws(json);
          if (**json != ':')
            error("unexspete charactre.");
          (*json)++;

          now -> element = json_parse_impl(json);

          skip_ws(json);
          if (**json == '}') {
            (*json)++;
            break;
          }
          if (**json != ',')
            error("unexspete charactre.");
          (*json)++;

          struct JsonObject *new = json_alloc(sizeof(struct JsonObject));        
          now -> next = new;
          now = new;
          now -> next = NULL;
        }
      } break;
    case '"':
      (*json)++;
      res.type = JSON_STRING;
      res.value.string = pares_string(json);
      break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      res.type = JSON_NUMBER;
      res.value.number = strtod(*json, json);
      break;
    case 't':
      if (strncmp(*json, "true", 4) == 0) {
        res.type = JSON_BOOL;
        res.value.jbool = JSON_TRUE;
        (*json) += 4;
      } else error("unexspete value.");
      break;
    case 'f':
      if (strncmp(*json, "false", 5) == 0) {
        res.type = JSON_BOOL;
        res.value.jbool = JSON_FALSE;
        (*json) += 5;
      } else error("unexspete value.");
      break;
    case 'n':
      if (strncmp(*json, "null", 4) == 0) {
        res.type = JSON_NULL;
        (*json) += 4;
      } else error("unexspete value.");
      break;
    case 0:
      return res;
    default:
      error("unexspete charactre.");
  }
  return res;
}

struct JsonValue json_parse(const char *json) {
  char *_json = (char *)json;
  return json_parse_impl(&_json);
}

static void intend(int i) {
  while (i--) printf("  ");
}

static void json_print_impl(struct JsonValue *json, int deep) {
  switch (json -> type) {
    case JSON_STRING:
      printf("\"%s\"", json -> value.string.str);
      break;
    case JSON_NUMBER:
      printf("%.0f", json -> value.number);
      break;
    case JSON_BOOL:
      printf("%s", json -> value.jbool == JSON_TRUE ? "true" : "fales");
      break;
    case JSON_NULL:
      printf("null");
      break;
    case JSON_ARRAY:
      printf("[\n");
      for (struct JsonArray *key = json -> value.array;
         key != NULL; key = key -> next) {
        intend(deep);
        json_print_impl(&key -> element, deep + 1);
        if (key -> next != NULL)
          printf(",");
        printf("\n");
      }
      intend(deep - 1);
      printf("]");
      break;
    case JSON_OBJECT:
      printf("{\n");
      for (struct JsonObject *key = json -> value.object;
         key != NULL; key = key -> next) {
        intend(deep);
        printf("\"%s\": ", key -> name.str);
        json_print_impl(&key -> element, deep + 1);
        if (key -> next != NULL)
          printf(",");
        printf("\n");
      }
      intend(deep - 1);
      printf("}");
      break;
  }
}

void json_print(struct JsonValue *json) {
  json_print_impl(json, 1);
  puts("");
}

void json_free(struct JsonValue *json) {
  switch (json -> type) {
    case JSON_STRING:
      free(json -> value.string.str);
      break;
    case JSON_ARRAY:
      for (struct JsonArray *key = json -> value.array;
         key != NULL; key = key -> next) {
        json_free(&key -> element);
      };
      break;
    case JSON_OBJECT:
      for (struct JsonObject *key = json -> value.object;
         key != NULL; key = key -> next) {
        free(key -> name.str);
        json_free(&key -> element);
      };
      break;
    case JSON_NUMBER:
    case JSON_BOOL:
    case JSON_NULL:
      break;
  }
}

