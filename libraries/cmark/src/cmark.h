#ifndef CMARK_H
#define CMARK_H

#include <stdio.h>
#include <cmark_export.h>
#include <cmark_version.h>

#ifdef __cplusplus
extern "C" {
#endif

CMARK_EXPORT
char *cmark_markdown_to_html(const char *text, size_t len, int options);

typedef enum {

  CMARK_NODE_NONE,

  CMARK_NODE_DOCUMENT,
  CMARK_NODE_BLOCK_QUOTE,
  CMARK_NODE_LIST,
  CMARK_NODE_ITEM,
  CMARK_NODE_CODE_BLOCK,
  CMARK_NODE_HTML_BLOCK,
  CMARK_NODE_CUSTOM_BLOCK,
  CMARK_NODE_PARAGRAPH,
  CMARK_NODE_HEADING,
  CMARK_NODE_THEMATIC_BREAK,

  CMARK_NODE_FIRST_BLOCK = CMARK_NODE_DOCUMENT,
  CMARK_NODE_LAST_BLOCK = CMARK_NODE_THEMATIC_BREAK,

  CMARK_NODE_TEXT,
  CMARK_NODE_SOFTBREAK,
  CMARK_NODE_LINEBREAK,
  CMARK_NODE_CODE,
  CMARK_NODE_HTML_INLINE,
  CMARK_NODE_CUSTOM_INLINE,
  CMARK_NODE_EMPH,
  CMARK_NODE_STRONG,
  CMARK_NODE_LINK,
  CMARK_NODE_IMAGE,

  CMARK_NODE_FIRST_INLINE = CMARK_NODE_TEXT,
  CMARK_NODE_LAST_INLINE = CMARK_NODE_IMAGE
} cmark_node_type;

#define CMARK_NODE_HEADER CMARK_NODE_HEADING
#define CMARK_NODE_HRULE CMARK_NODE_THEMATIC_BREAK
#define CMARK_NODE_HTML CMARK_NODE_HTML_BLOCK
#define CMARK_NODE_INLINE_HTML CMARK_NODE_HTML_INLINE

typedef enum {
  CMARK_NO_LIST,
  CMARK_BULLET_LIST,
  CMARK_ORDERED_LIST
} cmark_list_type;

typedef enum {
  CMARK_NO_DELIM,
  CMARK_PERIOD_DELIM,
  CMARK_PAREN_DELIM
} cmark_delim_type;

typedef struct cmark_node cmark_node;
typedef struct cmark_parser cmark_parser;
typedef struct cmark_iter cmark_iter;

typedef struct cmark_mem {
  void *(*calloc)(size_t, size_t);
  void *(*realloc)(void *, size_t);
  void (*free)(void *);
} cmark_mem;

CMARK_EXPORT cmark_mem *cmark_get_default_mem_allocator(void);

CMARK_EXPORT cmark_node *cmark_node_new(cmark_node_type type);

CMARK_EXPORT cmark_node *cmark_node_new_with_mem(cmark_node_type type,
                                                 cmark_mem *mem);

CMARK_EXPORT void cmark_node_free(cmark_node *node);

CMARK_EXPORT cmark_node *cmark_node_next(cmark_node *node);

CMARK_EXPORT cmark_node *cmark_node_previous(cmark_node *node);

CMARK_EXPORT cmark_node *cmark_node_parent(cmark_node *node);

CMARK_EXPORT cmark_node *cmark_node_first_child(cmark_node *node);

CMARK_EXPORT cmark_node *cmark_node_last_child(cmark_node *node);

typedef enum {
  CMARK_EVENT_NONE,
  CMARK_EVENT_DONE,
  CMARK_EVENT_ENTER,
  CMARK_EVENT_EXIT
} cmark_event_type;

CMARK_EXPORT
cmark_iter *cmark_iter_new(cmark_node *root);

CMARK_EXPORT
void cmark_iter_free(cmark_iter *iter);

CMARK_EXPORT
cmark_event_type cmark_iter_next(cmark_iter *iter);

CMARK_EXPORT
cmark_node *cmark_iter_get_node(cmark_iter *iter);

CMARK_EXPORT
cmark_event_type cmark_iter_get_event_type(cmark_iter *iter);

CMARK_EXPORT
cmark_node *cmark_iter_get_root(cmark_iter *iter);

CMARK_EXPORT
void cmark_iter_reset(cmark_iter *iter, cmark_node *current,
                      cmark_event_type event_type);

CMARK_EXPORT void *cmark_node_get_user_data(cmark_node *node);

CMARK_EXPORT int cmark_node_set_user_data(cmark_node *node, void *user_data);

CMARK_EXPORT cmark_node_type cmark_node_get_type(cmark_node *node);

CMARK_EXPORT
const char *cmark_node_get_type_string(cmark_node *node);

CMARK_EXPORT const char *cmark_node_get_literal(cmark_node *node);

CMARK_EXPORT int cmark_node_set_literal(cmark_node *node, const char *content);

CMARK_EXPORT int cmark_node_get_heading_level(cmark_node *node);

#define cmark_node_get_header_level cmark_node_get_heading_level
#define cmark_node_set_header_level cmark_node_set_heading_level

CMARK_EXPORT int cmark_node_set_heading_level(cmark_node *node, int level);

CMARK_EXPORT cmark_list_type cmark_node_get_list_type(cmark_node *node);

CMARK_EXPORT int cmark_node_set_list_type(cmark_node *node,
                                          cmark_list_type type);

CMARK_EXPORT cmark_delim_type cmark_node_get_list_delim(cmark_node *node);

CMARK_EXPORT int cmark_node_set_list_delim(cmark_node *node,
                                           cmark_delim_type delim);

CMARK_EXPORT int cmark_node_get_list_start(cmark_node *node);

CMARK_EXPORT int cmark_node_set_list_start(cmark_node *node, int start);

CMARK_EXPORT int cmark_node_get_list_tight(cmark_node *node);

CMARK_EXPORT int cmark_node_set_list_tight(cmark_node *node, int tight);

CMARK_EXPORT const char *cmark_node_get_fence_info(cmark_node *node);

CMARK_EXPORT int cmark_node_set_fence_info(cmark_node *node, const char *info);

CMARK_EXPORT const char *cmark_node_get_url(cmark_node *node);

CMARK_EXPORT int cmark_node_set_url(cmark_node *node, const char *url);

CMARK_EXPORT const char *cmark_node_get_title(cmark_node *node);

CMARK_EXPORT int cmark_node_set_title(cmark_node *node, const char *title);

CMARK_EXPORT const char *cmark_node_get_on_enter(cmark_node *node);

CMARK_EXPORT int cmark_node_set_on_enter(cmark_node *node,
                                         const char *on_enter);

CMARK_EXPORT const char *cmark_node_get_on_exit(cmark_node *node);

CMARK_EXPORT int cmark_node_set_on_exit(cmark_node *node, const char *on_exit);

CMARK_EXPORT int cmark_node_get_start_line(cmark_node *node);

CMARK_EXPORT int cmark_node_get_start_column(cmark_node *node);

CMARK_EXPORT int cmark_node_get_end_line(cmark_node *node);

CMARK_EXPORT int cmark_node_get_end_column(cmark_node *node);

CMARK_EXPORT void cmark_node_unlink(cmark_node *node);

CMARK_EXPORT int cmark_node_insert_before(cmark_node *node,
                                          cmark_node *sibling);

CMARK_EXPORT int cmark_node_insert_after(cmark_node *node, cmark_node *sibling);

CMARK_EXPORT int cmark_node_replace(cmark_node *oldnode, cmark_node *newnode);

CMARK_EXPORT int cmark_node_prepend_child(cmark_node *node, cmark_node *child);

CMARK_EXPORT int cmark_node_append_child(cmark_node *node, cmark_node *child);

CMARK_EXPORT void cmark_consolidate_text_nodes(cmark_node *root);

CMARK_EXPORT
cmark_parser *cmark_parser_new(int options);

CMARK_EXPORT
cmark_parser *cmark_parser_new_with_mem(int options, cmark_mem *mem);

CMARK_EXPORT
cmark_parser *cmark_parser_new_with_mem_into_root(
    int options, cmark_mem *mem, cmark_node *root);

CMARK_EXPORT
void cmark_parser_free(cmark_parser *parser);

CMARK_EXPORT
void cmark_parser_feed(cmark_parser *parser, const char *buffer, size_t len);

CMARK_EXPORT
cmark_node *cmark_parser_finish(cmark_parser *parser);

CMARK_EXPORT
cmark_node *cmark_parse_document(const char *buffer, size_t len, int options);

CMARK_EXPORT
cmark_node *cmark_parse_file(FILE *f, int options);

CMARK_EXPORT
char *cmark_render_xml(cmark_node *root, int options);

CMARK_EXPORT
char *cmark_render_html(cmark_node *root, int options);

CMARK_EXPORT
char *cmark_render_man(cmark_node *root, int options, int width);

CMARK_EXPORT
char *cmark_render_commonmark(cmark_node *root, int options, int width);

CMARK_EXPORT
char *cmark_render_latex(cmark_node *root, int options, int width);

#define CMARK_OPT_DEFAULT 0

#define CMARK_OPT_SOURCEPOS (1 << 1)

#define CMARK_OPT_HARDBREAKS (1 << 2)

#define CMARK_OPT_SAFE (1 << 3)

#define CMARK_OPT_UNSAFE (1 << 17)

#define CMARK_OPT_NOBREAKS (1 << 4)

#define CMARK_OPT_NORMALIZE (1 << 8)

#define CMARK_OPT_VALIDATE_UTF8 (1 << 9)

#define CMARK_OPT_SMART (1 << 10)

CMARK_EXPORT
int cmark_version(void);

CMARK_EXPORT
const char *cmark_version_string(void);

#ifndef CMARK_NO_SHORT_NAMES
#define NODE_DOCUMENT CMARK_NODE_DOCUMENT
#define NODE_BLOCK_QUOTE CMARK_NODE_BLOCK_QUOTE
#define NODE_LIST CMARK_NODE_LIST
#define NODE_ITEM CMARK_NODE_ITEM
#define NODE_CODE_BLOCK CMARK_NODE_CODE_BLOCK
#define NODE_HTML_BLOCK CMARK_NODE_HTML_BLOCK
#define NODE_CUSTOM_BLOCK CMARK_NODE_CUSTOM_BLOCK
#define NODE_PARAGRAPH CMARK_NODE_PARAGRAPH
#define NODE_HEADING CMARK_NODE_HEADING
#define NODE_HEADER CMARK_NODE_HEADER
#define NODE_THEMATIC_BREAK CMARK_NODE_THEMATIC_BREAK
#define NODE_HRULE CMARK_NODE_HRULE
#define NODE_TEXT CMARK_NODE_TEXT
#define NODE_SOFTBREAK CMARK_NODE_SOFTBREAK
#define NODE_LINEBREAK CMARK_NODE_LINEBREAK
#define NODE_CODE CMARK_NODE_CODE
#define NODE_HTML_INLINE CMARK_NODE_HTML_INLINE
#define NODE_CUSTOM_INLINE CMARK_NODE_CUSTOM_INLINE
#define NODE_EMPH CMARK_NODE_EMPH
#define NODE_STRONG CMARK_NODE_STRONG
#define NODE_LINK CMARK_NODE_LINK
#define NODE_IMAGE CMARK_NODE_IMAGE
#define BULLET_LIST CMARK_BULLET_LIST
#define ORDERED_LIST CMARK_ORDERED_LIST
#define PERIOD_DELIM CMARK_PERIOD_DELIM
#define PAREN_DELIM CMARK_PAREN_DELIM
#endif

#ifdef __cplusplus
}
#endif

#endif
