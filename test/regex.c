#include <sys/types.h>
#include <regex.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int r;
  regex_t reg;

  ++argv;  /* Danger! */

  if (r = regcomp(&reg, *argv, REG_NOSUB|REG_EXTENDED)) {
    char errbuf[1024];
    regerror(r, &reg, errbuf, sizeof(errbuf));
    printf("error: %s\n", errbuf);
    return 1;
  }

  for (++argv; *argv; ++argv) {
    if (regexec(&reg, *argv, 0, NULL, 0) == REG_NOMATCH)
      continue;
    printf("matched: %s\n", *argv);
  }

  return 0;
}