#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "houdini.h"

#if !defined(__has_builtin)
# define __has_builtin(b) 0
#endif

#if !__has_builtin(__builtin_expect)
# define __builtin_expect(e, v) (e)
#endif

#define likely(e) __builtin_expect((e), 1)
#define unlikely(e) __builtin_expect((e), 0)

static const char HREF_SAFE[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int houdini_escape_href(cmark_strbuf *ob, const uint8_t *src, bufsize_t size) {
  static const uint8_t hex_chars[] = "0123456789ABCDEF";
  bufsize_t i = 0, org;
  uint8_t hex_str[3];

  hex_str[0] = '%';

  while (i < size) {
    org = i;
    while (i < size && HREF_SAFE[src[i]] != 0)
      i++;

    if (likely(i > org))
      cmark_strbuf_put(ob, src + org, i - org);

    if (i >= size)
      break;

    switch (src[i]) {

    case '&':
      cmark_strbuf_puts(ob, "&amp;");
      break;

    case '\'':
      cmark_strbuf_puts(ob, "&#x27;");
      break;

    default:
      hex_str[1] = hex_chars[(src[i] >> 4) & 0xF];
      hex_str[2] = hex_chars[src[i] & 0xF];
      cmark_strbuf_put(ob, hex_str, 3);
    }

    i++;
  }

  return 1;
}
