#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>


/* Strip away trailing newline, if any, of string s. */
void chomp(char *s)
{
  size_t last;
  if (!s || s[0]=='\0') return;
  last = strlen(s) - 1;
  if (s[last] == '\n') s[last] = '\0';
}


/* Repeatedly read text to match, report if it matches regex pattern p */
void try_matches(regex_t *compiled_pattern)
{
  int error;
  char test_pattern[1024];
  do {
    puts("");
    puts("Text to match:");
    fgets(test_pattern, sizeof(test_pattern), stdin);
    chomp(test_pattern);

    if (strlen(test_pattern) == 0) break; /* Loop-and-a-half */

    error = regexec(compiled_pattern, test_pattern, (size_t) 0, NULL, 0);
    if (error == 0) {
      puts("  MATCH");
    } else if (error == REG_NOMATCH) {
      puts("  NO MATCH");
    } else {
      puts("  REGEX FAILED!");
    }
  } while (1);
}



int main()
{
  int error;
  char pattern[1024];
  regex_t compiled_pattern;

  puts("regex - demonstrate regular expressions");
  do {
    puts("");
    puts("Enter regular expression (or empty string to stop):");
    fgets(pattern, sizeof(pattern), stdin); /* Guarantees \0 termination. */
    chomp(pattern);

    if (strlen(pattern) == 0) break; /* Loop-and-a-half */

    error = regcomp(&compiled_pattern, pattern, REG_EXTENDED | REG_NOSUB);
    if (error) {
      puts("Regex compilation failed");
      exit(error);
    }
    try_matches(&compiled_pattern);
    regfree(&compiled_pattern);
  } while (1);
  return 0;
}
