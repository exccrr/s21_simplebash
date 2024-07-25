#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "CatFlags.h"

typedef struct {
  bool number_nonblank;
  bool end_of_line_character;
  bool numbering_of_all_lines;
  bool compresses_strings;
  bool display_tab;
  bool non_printtable;
} Flags;

Flags ReadFlags(int argc, char *argv[]) {
  struct option long_options[] = {{"number-nonblank", 0, NULL, 'b'},
                                  {"number", 0, NULL, 'n'},
                                  {"squeeze-blank", 0, NULL, 's'},
                                  {NULL, 0, NULL, 0}};
  int current_flag = getopt_long(argc, argv, "bevEnstT", long_options, NULL);
  Flags flags = {false, false, false, false, false, false};
  for (; current_flag != -1;
       current_flag = getopt_long(argc, argv, "bevEnstT", long_options, NULL)) {
    switch (current_flag) {
      break;
      case 'b':
        flags.number_nonblank = true;
        break;
      case 'e':
        flags.end_of_line_character = true;
        flags.non_printtable = true;
        break;
      case 'v':
        flags.non_printtable = true;
        break;
      case 'E':
        flags.end_of_line_character = true;
        break;
      case 'n':
        flags.numbering_of_all_lines = true;
        break;
      case 's':
        flags.compresses_strings = true;
        break;
      case 't':
        flags.display_tab = true;
        flags.non_printtable = true;
        break;
      case 'T':
        flags.display_tab = true;
    }
  }
  return flags;
}

void CatFile(FILE *file, Flags flags, const char *table[static 256]) {
  int c = 0;
  int last = 0;
  int number_lines = 0;
  bool squeeze = false;
  last = '\n';
  (void)flags;
  while (fread(&c, 1, 1, file) > 0) {
    if (last == '\n') {
      if (flags.compresses_strings && c == '\n') {
        if (squeeze) {
          continue;
        }
        squeeze = true;

      } else {
        squeeze = false;
      }
      if (flags.number_nonblank) {
        if (c != '\n') {
          printf("%6i\t", ++number_lines);
        }
      } else {
        if (flags.numbering_of_all_lines) {
          printf("%6i\t", ++number_lines);
        }
      }
    }
    if (!*table[c]) {
      printf("%c", '\0');
    } else {
      printf("%s", table[c]);
    }

    last = c;
  }
}

void Cat(int argc, char *argv[], Flags flags, const char *table[static 256]) {
  for (char **filename = &argv[1], **end = &argv[argc]; filename != end;
       ++filename) {
    if (**filename == '-') {
      continue;
    }
    FILE *file = freopen(*filename, "rb", stdin);
    if (errno) {
      fprintf(stderr, "%s", argv[0]);
      perror(*filename);
      continue;
    }
    CatFile(file, flags, table);
    fclose(file);
  }
}

int main(int argc, char *argv[]) {
  Flags flags = ReadFlags(argc, argv);
  const char *table[256];
  CatSetTable(table);
  if (flags.end_of_line_character) {
    CatSetEnd(table);
  }
  if (flags.display_tab) {
    CatSetTAB(table);
  }
  if (flags.non_printtable) {
    CatSetNonPrintable(table);
  }

  Cat(argc, argv, flags, table);

  return 0;
}
