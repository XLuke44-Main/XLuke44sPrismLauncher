#include <stdlib.h>
#include "chunk.h"
#include "scanners.h"

bufsize_t _scan_at(bufsize_t (*scanner)(const unsigned char *), cmark_chunk *c, bufsize_t offset)
{
	bufsize_t res;
	unsigned char *ptr = (unsigned char *)c->data;

        if (ptr == NULL || offset > c->len) {
          return 0;
        } else {
	  unsigned char lim = ptr[c->len];

	  ptr[c->len] = '\0';
	  res = scanner(ptr + offset);
	  ptr[c->len] = lim;
        }

	return res;
}

/*!re2c
  re2c:define:YYCTYPE  = "unsigned char";
  re2c:define:YYCURSOR = p;
  re2c:define:YYMARKER = marker;
  re2c:define:YYCTXMARKER = marker;
  re2c:yyfill:enable = 0;

  wordchar = [^\x00-\x20];

  spacechar = [ \t\v\f\r\n];

  reg_char     = [^\\()\x00-\x20];

  escaped_char = [\\][!"#$%&'()*+,./:;<=>?@[\\\]^_`{|}~-];

  tagname = [A-Za-z][A-Za-z0-9-]*;

  blocktagname = 'address'|'article'|'aside'|'base'|'basefont'|'blockquote'|'body'|'caption'|'center'|'col'|'colgroup'|'dd'|'details'|'dialog'|'dir'|'div'|'dl'|'dt'|'fieldset'|'figcaption'|'figure'|'footer'|'form'|'frame'|'frameset'|'h1'|'h2'|'h3'|'h4'|'h5'|'h6'|'head'|'header'|'hr'|'html'|'iframe'|'legend'|'li'|'link'|'main'|'menu'|'menuitem'|'nav'|'noframes'|'ol'|'optgroup'|'option'|'p'|'param'|'section'|'source'|'title'|'summary'|'table'|'tbody'|'td'|'tfoot'|'th'|'thead'|'title'|'tr'|'track'|'ul';

  attributename = [a-zA-Z_:][a-zA-Z0-9:._-]*;

  unquotedvalue = [^ \t\r\n\v\f"'=<>`\x00]+;
  singlequotedvalue = ['][^'\x00]*['];
  doublequotedvalue = ["][^"\x00]*["];

  attributevalue = unquotedvalue | singlequotedvalue | doublequotedvalue;

  attributevaluespec = spacechar* [=] spacechar* attributevalue;

  attribute = spacechar+ attributename attributevaluespec?;

  opentag = tagname attribute* spacechar* [/]? [>];
  closetag = [/] tagname spacechar* [>];

  htmlcomment = "--" ([^\x00-]+ | "-" [^\x00-] | "--" [^\x00>])* "-->";

  processinginstruction = ([^?>\x00]+ | [?][^>\x00] | [>])+;

  declaration = [A-Z]+ [^>\x00]*;

  cdata = "CDATA[" ([^\]\x00]+ | "]" [^\]\x00] | "]]" [^>\x00])*;

  htmltag = opentag | closetag;

  in_parens_nosp   = [(] (reg_char|escaped_char|[\\])* [)];

  in_double_quotes = ["] (escaped_char|[^"\x00])* ["];
  in_single_quotes = ['] (escaped_char|[^'\x00])* ['];
  in_parens        = [(] (escaped_char|[^)\x00])* [)];

  scheme           = [A-Za-z][A-Za-z0-9.+-]{1,31};
*/


bufsize_t _scan_scheme(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  scheme [:] { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_autolink_uri(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  scheme [:][^\x00-\x20<>]*[>]  { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_autolink_email(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+
    [@]
    [a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?
    ([.][a-zA-Z0-9]([a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*
    [>] { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_html_tag(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  htmltag { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}

bufsize_t _scan_html_comment(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  htmlcomment { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}

bufsize_t _scan_html_pi(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  processinginstruction { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}

bufsize_t _scan_html_declaration(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  declaration { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}

bufsize_t _scan_html_cdata(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  cdata { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}




bufsize_t _scan_html_block_start(const unsigned char *p)
{
  const unsigned char *marker = NULL;
/*!re2c
  [<] ('script'|'pre'|'textarea'|'style') (spacechar | [>]) { return 1; }
  '<!--' { return 2; }
  '<?' { return 3; }
  '<!' [A-Za-z] { return 4; }
  '<![CDATA[' { return 5; }
  [<] [/]? blocktagname (spacechar | [/]? [>])  { return 6; }
  * { return 0; }
*/
}



bufsize_t _scan_html_block_start_7(const unsigned char *p)
{
  const unsigned char *marker = NULL;
/*!re2c
  [<] (opentag | closetag) [\t\n\f ]* [\r\n] { return 7; }
  * { return 0; }
*/
}


bufsize_t _scan_html_block_end_1(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [^\n\x00]* [<] [/] ('script'|'pre'|'textarea'|'style') [>] { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_html_block_end_2(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [^\n\x00]* '-->' { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_html_block_end_3(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [^\n\x00]* '?>' { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_html_block_end_4(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [^\n\x00]* '>' { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_html_block_end_5(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [^\n\x00]* ']]>' { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}




bufsize_t _scan_link_title(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  ["] (escaped_char|[^"\x00])* ["]   { return (bufsize_t)(p - start); }
  ['] (escaped_char|[^'\x00])* ['] { return (bufsize_t)(p - start); }
  [(] (escaped_char|[^()\x00])* [)]  { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_spacechars(const unsigned char *p)
{
  const unsigned char *start = p; \
/*!re2c
  [ \t\v\f\r\n]+ { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_atx_heading_start(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [#]{1,6} ([ \t]+|[\r\n])  { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}



bufsize_t _scan_setext_heading_line(const unsigned char *p)
{
  const unsigned char *marker = NULL;
/*!re2c
  [=]+ [ \t]* [\r\n] { return 1; }
  [-]+ [ \t]* [\r\n] { return 2; }
  * { return 0; }
*/
}


bufsize_t _scan_open_code_fence(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [`]{3,} / [^`\r\n\x00]*[\r\n] { return (bufsize_t)(p - start); }
  [~]{3,} / [^\r\n\x00]*[\r\n] { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}


bufsize_t _scan_close_code_fence(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [`]{3,} / [ \t]*[\r\n] { return (bufsize_t)(p - start); }
  [~]{3,} / [ \t]*[\r\n] { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}



bufsize_t _scan_entity(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  [&] ([#] ([Xx][A-Fa-f0-9]{1,6}|[0-9]{1,7}) |[A-Za-z][A-Za-z0-9]{1,31} ) [;]
     { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}



bufsize_t _scan_dangerous_url(const unsigned char *p)
{
  const unsigned char *marker = NULL;
  const unsigned char *start = p;
/*!re2c
  'data:image/' ('png'|'gif'|'jpeg'|'webp') { return 0; }
  'javascript:' | 'vbscript:' | 'file:' | 'data:' { return (bufsize_t)(p - start); }
  * { return 0; }
*/
}

