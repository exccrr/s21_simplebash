#define _GNU_SOURCE
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *pattern;
  size_t size;
  int regex_flag;
  bool invert_search;
  bool quantity_lines;
  bool name_file;
  bool number_lines;
  bool no_filename;
  bool no_error;
  bool regular;
  bool only_coincidence;
} Flags;

void *xmalloc(size_t size) {
  void *temp;
  temp = malloc(size);
  if (!temp) {
    exit(errno);
  }
  return temp;
}

void *xrealloc(void *block, size_t size) {
  void *temp;
  temp = realloc(block, size);
  if (!temp) {
    exit(errno);
  }
  return temp;
}

char *string_append_expr(char *string, size_t *size, char const *expr,
                         size_t size_expr) {
  string = xrealloc(string, *size + size_expr + 7);
  string[*size] = '\\';
  string[*size + 1] = '|';
  string[*size + 2] = '\\';
  string[*size + 3] = '(';
  memcpy(string + *size + 4, expr, size_expr);
  *size += size_expr + 4;
  string[*size] = '\\';
  string[*size + 1] = ')';
  string[*size + 2] = '\0';
  *size += 2;
  return string;
}

Flags ReadFlags(int argc, char *argv[]) {
  Flags flags = {NULL,  0,     0,     false, false, false,
                 false, false, false, false, false};
  int current_flag = getopt_long(argc, argv, "e:ivclnhsf:o", 0, 0);
  flags.pattern = xmalloc(2);
  flags.pattern[0] = '\\';
  flags.pattern[1] = '\0';
  size_t patern_size = 0;
  for (; current_flag != -1;
       current_flag = getopt_long(argc, argv, "e:ivclnhsf:o", 0, 0)) {
    switch (current_flag) {
      break;
      case 'e':
        flags.pattern = string_append_expr(flags.pattern, &patern_size, optarg,
                                           strlen(optarg));
        break;
      case 'i':
        flags.regex_flag |= REG_ICASE;
        break;
      case 'v':
        flags.invert_search = true;
        break;
      case 'c':
        flags.quantity_lines = true;
        break;
      case 'l':
        flags.name_file = true;
        break;
      case 'n':
        flags.number_lines = true;
        break;
      case 'h':
        flags.no_filename = true;
        break;
      case 's':
        flags.no_error = true;
        break;
      case 'f':
        flags.regular = true;
        break;
      case 'o':
        flags.only_coincidence = true;
    }
  }
  flags.size = patern_size;
  return flags;
}

void GrepFile(FILE *file, char const *filename, Flags flags, regex_t *preg,
              int count_f) {
  (void)flags;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  while (getline(&line, &length, file) > 0) {
    if (!regexec(preg, line, 1, &match, 0)) {
      if (flags.no_filename) {
        printf("%s", line);
      } else {
        if (count_f - 1 > 1) {
          printf("%s:%s", filename, line);
        } else {
          printf("%s", line);
        }
      }
    }
  }
  free(line);
}

void GrepCount(FILE *file, char const *filename, Flags flags, regex_t *preg,
               int count_f) {
  (void)flags;
  (void)filename;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    if (!regexec(preg, line, 1, &match, 0)) {
      count++;
    }
  }
  if (count_f - 1 > 1) {
    printf("%s:%d\n", filename, count);
  } else {
    printf("%d\n", count);
  }
  free(line);
}

void GrepInvert(FILE *file, char const *filename, Flags flags, regex_t *preg,
                int count_f) {
  (void)flags;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    count++;
    if (regexec(preg, line, 1, &match, 0)) {
      if (flags.only_coincidence) {
      } else {
        if (flags.no_filename) {
          if (flags.number_lines) {
            printf("%d:%s", count, line);
          } else {
            printf("%s", line);
          }

        } else {
          if (flags.number_lines) {
            if (count_f - 1 > 1) {
              printf("%s:%d:%s", filename, count, line);
            } else {
              printf("%d:%s", count, line);
            }
          } else {
            if (count_f - 1 > 1) {
              printf("%s:%s", filename, line);
            } else {
              printf("%s", line);
            }
          }
        }
      }
    }
  }
  free(line);
}

void GrepNameFile(FILE *file, char const *filename, Flags flags,
                  regex_t *preg) {
  (void)flags;
  (void)filename;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    if (!regexec(preg, line, 1, &match, 0)) {
      count++;
    }
  }
  if (count > 0) {
    printf("%s\n", filename);
  }

  free(line);
}

void GrepNumberLine(FILE *file, char const *filename, Flags flags,
                    regex_t *preg, int count_f) {
  (void)flags;
  (void)filename;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    count++;
    if (!regexec(preg, line, 1, &match, 0)) {
      if (flags.no_filename) {
        printf("%d:%s", count, line);
      } else {
        if (count_f - 1 > 1) {
          printf("%s:%d:%s", filename, count, line);
        } else {
          printf("%d:%s", count, line);
        }
      }
    }
  }
  free(line);
}

void GrepTemplate(FILE *file, char const *filename, Flags flags, regex_t *preg,
                  int count_f) {
  (void)flags;
  (void)filename;
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;
  while (getline(&line, &length, file) > 0) {
    count++;
    if (!regexec(preg, line, 1, &match, 0)) {
      if (flags.no_filename) {
        if (flags.number_lines) {
          printf("%d:%.*s\n", count, (int)(match.rm_eo - match.rm_so),
                 line + match.rm_so);
          char *remaining = line + match.rm_eo;
          while (!regexec(preg, remaining, 1, &match, 0)) {
            printf("%d:%.*s\n", count, (int)(match.rm_eo - match.rm_so),
                   remaining + match.rm_so);
            remaining = remaining + match.rm_eo;
          }
        } else {
          printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                 line + match.rm_so);
          char *remaining = line + match.rm_eo;
          while (!regexec(preg, remaining, 1, &match, 0)) {
            printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                   remaining + match.rm_so);
            remaining = remaining + match.rm_eo;
          }
        }
      } else {
        if (flags.number_lines) {
          if (count_f - 1 > 1) {
            printf("%s:%d:%.*s\n", filename, count,
                   (int)(match.rm_eo - match.rm_so), line + match.rm_so);
            char *remaining = line + match.rm_eo;
            while (!regexec(preg, remaining, 1, &match, 0)) {
              printf("%s:%d:%.*s\n", filename, count,
                     (int)(match.rm_eo - match.rm_so), remaining + match.rm_so);
              remaining = remaining + match.rm_eo;
            }
          } else {
            printf("%d:%.*s\n", count, (int)(match.rm_eo - match.rm_so),
                   line + match.rm_so);
            char *remaining = line + match.rm_eo;
            while (!regexec(preg, remaining, 1, &match, 0)) {
              printf("%d:%.*s\n", count, (int)(match.rm_eo - match.rm_so),
                     remaining + match.rm_so);
              remaining = remaining + match.rm_eo;
            }
          }
        }

        else {
          if (count_f - 1 > 1) {
            printf("%s:%.*s\n", filename, (int)(match.rm_eo - match.rm_so),
                   line + match.rm_so);
            char *remaining = line + match.rm_eo;
            while (!regexec(preg, remaining, 1, &match, 0)) {
              printf("%s:%.*s\n", filename, (int)(match.rm_eo - match.rm_so),
                     remaining + match.rm_so);
              remaining = remaining + match.rm_eo;
            }
          } else {
            printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                   line + match.rm_so);
            char *remaining = line + match.rm_eo;
            while (!regexec(preg, remaining, 1, &match, 0)) {
              printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                     remaining + match.rm_so);
              remaining = remaining + match.rm_eo;
            }
          }
        }
      }
    }
  }
  free(line);
}

void Grep(int argc, char *argv[], Flags flags) {
  char **end = &argv[argc];
  regex_t preg_storage;
  regex_t *preg = &preg_storage;
  if (flags.size == 0) {
    if (regcomp(preg, argv[0], flags.regex_flag)) {
      fprintf(stderr, "failed to compile regex\n");
      exit(1);
    }
  } else {
    if (regcomp(preg, flags.pattern + 2, flags.regex_flag)) {
      fprintf(stderr, "failed to compile regex\n");
      exit(1);
    }
  }
  free(flags.pattern);
  if (argc == (flags.size ? 2 : 1)) {
    if (flags.quantity_lines) {
      GrepCount(stdin, "", flags, preg, 1);
    } else {
      GrepFile(stdin, "", flags, preg, 1);
    }
  }
  for (char **filename = argv + (flags.size ? 0 : 1); filename != end;
       ++filename) {
    FILE *file = fopen(*filename, "rb");
    if (errno) {
      if (flags.no_error) {
        continue;
      } else {
        fprintf(stderr, "%s", argv[0]);
        perror(*filename);
        continue;
      }
    }
    if (flags.quantity_lines) {
      GrepCount(file, *filename, flags, preg, argc);
    }
    if (flags.invert_search) {
      GrepInvert(file, *filename, flags, preg, argc);
    }
    if (flags.name_file) {
      GrepNameFile(file, *filename, flags, preg);
    }

    if (flags.only_coincidence) {
      GrepTemplate(file, *filename, flags, preg, argc);
    }
    if (flags.number_lines) {
      GrepNumberLine(file, *filename, flags, preg, argc);
    } else {
      GrepFile(file, *filename, flags, preg, argc);
    }
    fclose(file);
  }
  regfree(preg);
}

int main(int argc, char *argv[]) {
  Flags flags = ReadFlags(argc, argv);
  argv += optind;
  argc -= optind;
  if (argc == 0) {
    fprintf(stderr, "no pattern\n");
    exit(1);
  }
  Grep(argc, argv, flags);
}