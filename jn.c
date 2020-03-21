#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "json.h"

static void error(const char *msg) {
  fprintf(stderr, "jn: %s\n", msg);
  exit(2);
}

static char *read_file(const char *path) {
  FILE *f = fopen(path, "rb");

  if (f == NULL) 
    error("Can not open file");

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *data = calloc(1, fsize + 1);
  if (data == NULL)
    error("calloc fail");

  if (fread(data, 1, fsize, f) != (size_t)fsize)
    error("Reading error");

  fclose(f);
  return data;
}

int main(int argc, char **argv) {
  int ch;
  while ((ch = getopt(argc, argv, "n")) != -1)
    switch (ch) {
      default: goto using;
    }
  argc -= optind; argv += optind;
  if (argc == 0) goto using;
  char *test = read_file(argv[0]);
  struct JsonValue j = json_parse(test);
  free(test);

  struct JsonValue *tmp = &j;
  for (int i = 1; i < argc; i++) {
    if (tmp -> type == JSON_OBJECT) {
      struct JsonValue *val = tmp;
      for (struct JsonObject *key = tmp -> value.object;
          key != NULL; key = key -> next) {
        if (strcmp(key -> name.str, argv[i]) == 0) {
          val = &key -> element;
          break;
        }
      }
      if (val == tmp) error("wrong key.");
      tmp = val;
    } else if (tmp -> type == JSON_ARRAY) {
      int index = atoi(argv[i]);
      for (struct JsonArray *key = tmp -> value.array;
          key != NULL; key = key -> next) {
        if (--index == 0) {
          tmp = &key -> element;
          break;
        }
      }
      if (index != 0) error("index out of range.");
    } else error("unexpected type.");
  }

  if (tmp -> type == JSON_STRING) 
    printf("%s\n", tmp -> value.string.str);
  else if (tmp -> type == JSON_NUMBER) 
    printf("%.0f\n", tmp -> value.number);
  else json_print(tmp);

  json_free(&j);
  return 0;
using:
  printf("using\n\tjn <file> [path]\n");
  return 2;
}
