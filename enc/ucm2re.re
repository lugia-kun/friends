/* -*- mode: c -*- */
/* vim: set ft=c: */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

typedef unsigned char mbcsChar;

struct charInfoT;
typedef struct charInfoT charInfo;

struct charInfoT
{
  uint_least32_t u32char;
  mbcsChar *mbcs;
  enum fallback_indicator {
    FB_EXACT = 0,          /* exact 1-1 roundtrip mapping */
    FB_BEST_FALL_BACK = 1, /* for the best fallback codepage byte sequence */
    FB_SUBSTITUTE = 2,     /* substitution charactor */
    FB_BEST_REVERSE_FALL_BACK = 3, /* best reverse fallback Unicode value */
  } fb_indicator;
  charInfo *next;
  charInfo *prev;
};

struct allocDataT;
typedef struct allocDataT allocData;

struct allocDataT
{
  void *p;
  allocData *next;
};

typedef struct
{
  const char *funcname;
  const char *headname;
  const char *iana_name;
  mbcsChar *code_set_name;
  int mb_cur_max;
  int mb_cur_min;
  mbcsChar *uconv_class;
  mbcsChar *subchar;
  mbcsChar *charsetFamily;
  charInfo *charmaps;
  allocData *allocs;
} tblset;

struct parserT
{
  FILE *fp;    /* source    */
  mbcsChar *b; /* Buffer    */
  mbcsChar *p; /* Cursor    */
  mbcsChar *m; /* Marker    */
  mbcsChar *c; /* CtxMarker */
  mbcsChar *t; /* Token     */
  mbcsChar *l; /* Limit     */
  size_t bufsize;
};
typedef struct parserT parser;

void *add_alloc(tblset *set, void *p)
{
  allocData *a;

  if (!set) return NULL;
  if (!p) return NULL;

  a = (allocData *)calloc(sizeof(allocData), 1);
  if (!a) return NULL;

  a->p = p;
  a->next = set->allocs;
  set->allocs = a;
  return p;
}

void *parser_init(parser **p)
{
  parser *tp;

  if (!p) return NULL;

  *p = NULL;
  tp = (parser*)calloc(sizeof(parser), 1);
  if (!tp) return NULL;

  tp->bufsize = 1024;
  tp->b = (mbcsChar*)calloc(sizeof(mbcsChar), tp->bufsize);
  if (tp->b) {
    tp->p = tp->b;
    tp->l = tp->b - 1;
    tp->m = tp->b;
    tp->c = tp->b;
    tp->t = tp->b;
  } else {
    tp->bufsize = 0;
  }

  *p = tp;
  return tp;
}

void parser_free(parser *p)
{
  if (!p) return;

  free(p->b);
  free(p);
}

int parser_fill(parser *p, int count)
{
  mbcsChar *t;
  size_t ts;

  if (p->bufsize == 0) return 1;

  if (!p->b) {
    p->b = (mbcsChar*)calloc(sizeof(mbcsChar), p->bufsize);
    p->p = p->b;
    p->l = p->b - 1;
    p->m = p->b;
    p->c = p->b;
    p->t = p->b;
  } else {
    ts = p->bufsize - (p->l - p->b) + count;
    if (ts > p->bufsize) {
      t = realloc(p->b, ts);
      if (!t) return 2;
      if (t != p->b) {
        p->p = t + (p->p - p->b);
        p->l = t + (p->l - p->b);
        p->m = t + (p->m - p->b);
        p->c = t + (p->c - p->b);
        p->t = t + (p->t - p->b);
        p->b = t;
      }
      p->bufsize = ts;
    }
  }
  if (p->l >= p->t) {
    memmove(p->b, p->t, p->l - p->t + 1);
    p->p = p->b + (p->p - p->t);
    p->l = p->b + (p->l - p->t);
    p->m = p->b + (p->m - p->t);
    p->c = p->b + (p->c - p->t);
    p->t = p->b;
  }
  ts = p->bufsize - (p->l - p->b + 1);
  ts = fread(p->l + 1, sizeof(mbcsChar), ts, p->fp);
  p->l = p->l + ts;
  ts = p->bufsize - (p->l - p->b + 1);
  if (ts > 0) {
    memset(p->l + 1, 0, ts);
    p->l = p->l + ts;
  }
  return 0;
}

int parser_copyToken(mbcsChar **dst, parser *p, int remove_quote)
{
  size_t sz;
  mbcsChar *t;

  if (!dst) return 1;

  sz = p->p - p->t + 1;
  t = (mbcsChar *)malloc(sizeof(mbcsChar) * sz);
  if (!t) return 1;

  if (remove_quote) {
    memmove(t, p->t + 1, sizeof(mbcsChar) * (sz - 3));
    *(t + sz - 3) = '\0';
  } else {
    memmove(t, p->t, sizeof(mbcsChar) * (sz - 1));
    *(t + sz - 1) = '\0';
  }

  *dst = t;
  return 0;
}

int parse_ucm(tblset *tset, FILE *fp)
{
  parser *p;
  int start_charmap = 0;
  int intval;
  charInfo *ci;

  if (!parser_init(&p)) return 1;
  p->fp = fp;
  ci = NULL;

  for(;;) {
    p->t = p->p;
    if (!start_charmap) goto metadata;
    /*!re2c
      re2c:define:YYCTYPE = "mbcsChar";
      re2c:define:YYCURSOR = "p->p";
      re2c:define:YYMARKER = "p->m";
      re2c:define:YYLIMIT = "p->l";
      re2c:define:YYFILL:naked = 1;
      re2c:define:YYFILL = "{ parser_fill(p, @@); }";

      *         { goto mbcswords; }
      "\x00"    { goto error; }
      [ \r\n\t] { continue; }
      "#"   { goto comment; }
      "END"" "+"CHARMAP" { start_charmap = 0; continue; }
      "<U"  { goto unichar; }
      "|"   { goto mode_parse; }
    */

  unichar:
    ci = (charInfo*)calloc(sizeof(charInfo), 1);
    if (!ci) goto error;
    if (tset->charmaps) {
      tset->charmaps->prev->next = ci;
      ci->prev = tset->charmaps->prev;
      tset->charmaps->prev = ci;
    } else {
      tset->charmaps = ci;
      ci->prev = ci;
    }
    if (!add_alloc(tset, ci)) goto error;
    for (;;) {
      p->t = p->p;
      /*!re2c
        *      { goto error; }
        "\x00" { goto error; }
        [0-9]  { ci->u32char = (ci->u32char << 4) | (*p->t - '0'); continue; }
        [a-f]  { ci->u32char = (ci->u32char << 4) | (*p->t - 'a' + 10); continue; }
        [A-F]  { ci->u32char = (ci->u32char << 4) | (*p->t - 'A' + 10); continue; }
        ">"    { break; }
       */
    }
    continue;

  mbcswords:
    if (!ci) goto error;
    p->p = p->t;
    for (;;) {
      /*!re2c
        *         { continue; }
        "\x00"    { break; }
        [ \r\n\t] { break; }
       */
    }
    p->p--;
    if (parser_copyToken(&ci->mbcs, p, 0) != 0) goto error;
    if (!add_alloc(tset, ci->mbcs)) goto error;
    continue;

  mode_parse:
    if (!ci) goto error;
    p->t = p->p;
    /*!re2c
      *   { goto error; }
      "0" { ci->fb_indicator = FB_EXACT; continue; }
      "1" { ci->fb_indicator = FB_BEST_FALL_BACK; continue; }
      "2" { ci->fb_indicator = FB_SUBSTITUTE; continue; }
      "3" { ci->fb_indicator = FB_BEST_REVERSE_FALL_BACK; continue; }
     */
    continue;

  metadata:
    /*!re2c
      *      { continue; }
      "\x00" { break; }
      "#"    { goto comment; }
      "CHARMAP" { start_charmap = 1; continue; }
      "<code_set_name>"  { goto code_set_name; }
      "<mb_cur_max>"     { goto mb_cur_max; }
      "<mb_cur_min>"     { goto mb_cur_min; }
      "<uconv_class>"    { goto uconv_class; }
     */

  code_set_name:
    p->t = p->p;
    /*!re2c
      *      { goto code_set_name; }
      "\x00" { goto error; }
      "\n"   { continue; }
      "\"".*"\"" {
          if (parser_copyToken(&tset->code_set_name, p, 1) != 0) goto error;
          if (!add_alloc(tset, tset->code_set_name)) goto error;
          continue;
      }
     */

  uconv_class:
    p->t = p->p;
    /*!re2c
     *      { goto uconv_class; }
     "\x00" { goto error; }
     "\n"   { continue; }
     "\"".*"\"" {
         if (parser_copyToken(&tset->uconv_class, p, 1) != 0) goto error;
         if (!add_alloc(tset, tset->uconv_class)) goto error;
         continue;
     }
    */

  mb_cur_max:
    intval = 0;
    for (;;) {
      p->t = p->p;
      /*!re2c
        *      { if (intval > 0) break; continue; }
        "\x00" { goto error; }
        [0-9]  { intval = intval * 10 + (*p->t - '0'); continue; }
       */
    }
    tset->mb_cur_max = intval;
    continue;

  mb_cur_min:
    intval = 0;
    for (;;) {
      p->t = p->p;
      /*!re2c
       *      { if (intval > 0) break; continue; }
       "\x00" { goto error; }
       [0-9]  { intval = intval * 10 + (*p->t - '0'); continue; }
      */
    }
    tset->mb_cur_min = intval;
    continue;

  comment:
    for (;;) {
      p->t = p->p;
      /*!re2c
        *    { continue; }
        "\n" { break; }
      */
    }
    continue;
  }

  parser_free(p);
  return 0;

 error:
  parser_free(p);
  return 2;
}

int cs_printf(FILE *output,
              int top, const mbcsChar *indent, const char *stmt, ...)
{
  va_list ap;
  int i;

  for (i = 0; i < top; i++) {
    fprintf(output, "%s", indent ? indent : (mbcsChar *)" ");
  }
  va_start(ap, stmt);
  vfprintf(output, stmt, ap);
  va_end(ap);

  return 0;
}

typedef struct {
  int dir;
  mbcsChar *from;
  mbcsChar *to;
  mbcsChar *oks;
  mbcsChar *ngs;
  mbcsChar *ends;
  mbcsChar *indent;
  int top;
  int utfbits;
} EmbedData;

void free_embed_data(EmbedData *e)
{
  free(e->from);
  free(e->to);
  free(e->oks);
  free(e->ngs);
  free(e->ends);
}

int embed_code(tblset *set, FILE *input, FILE *output)
{
  parser *p;
  charInfo *ci;
  size_t st;
  size_t sr;
  int i;
  enum EmbedderState {
    Normal, MktbleComment
  } state;
  EmbedData data;
  mbcsChar **dest;
  int *idest;
  int neg;

  memset(&data, 0, sizeof(EmbedData));

  if (!parser_init(&p)) return 1;
  p->fp = input;
  state = Normal;

  for(;;) {
    switch(state) {
    case Normal: break;
    case MktbleComment: goto mktble;
    default: goto error;
    }

    p->t = p->p;
    /*!re2c
      * { goto copy; }
      "\x00" { break; }
      "/""*!mktble" { goto mktble_start; }
      "@" { goto at_embed; }
     */

  copy:
    st = p->p - p->t;
    sr = fwrite(p->t, sizeof(char), st, output);
    if (st != sr) {
      goto error;
    }
    continue;

  mktble_start:
    state = MktbleComment;
    /* Allocated data will be freed very later. */
    data.oks = NULL;
    data.ngs = NULL;
    data.ends = NULL;

  mktble:
    for(;;) {
      p->t = p->p;
      /*!re2c
        * { goto error; }
        [ \t\r\n] { continue; }
        "OK"   { dest = &data.oks;  goto statement; }
        "NG"   { dest = &data.ngs;  goto statement; }
        "END"  { dest = &data.ends; goto statement; }
        "mktble:" { goto mktbleconf; }
        "//"   { goto mktblecomment; }
        "*""/" { break; }
       */

    mktblecomment:
      for(;;) {
        /*!re2c
          * { continue; }
          "\x00" { goto error; }
          ("\r\n"|"\r"|"\n") { break; }
        */
      }
      continue;

    mktbleconf:
      /*!re2c
        * { goto error; }
        "dir"     { idest = &data.dir; goto intvalparse; }
        "from"    { dest = &data.from; goto strvalparse; }
        "to"      { dest = &data.to; goto strvalparse; }
        "utfbits" { idest = &data.utfbits; goto intvalparse; }
        "indent:string" { dest = &data.indent; goto strvalparse; }
        "indent:top"    { idest = &data.top; goto intvalparse; }
       */
      continue;

    strvalparse:
      for (;;) {
        /*!re2c
         * { goto error; }
         "\x00" { goto error; }
         [ \t\r\n]*"="[ \t\r\n]* { break; }
        */
      }
      p->t = p->p;
      for (;;) {
        /*!re2c
          * { goto error; }
          "\x00" { goto error; }
          "\"" { break; }
        */
      }
      for (;;) {
        /*!re2c
          * { continue; }
          "\x00" { goto error; }
          "\\\"" { continue; }
          "\""   { break; }
        */
      }
      if (parser_copyToken(dest, p, 1) != 0) goto error;
      if (!add_alloc(set, *dest)) goto error;
      for (;;) {
        /*!re2c
          * { goto error; }
          "\x00" { goto error; }
          [ \t\r\n]*";" { break; }
         */
      }
      continue;

    intvalparse:
      for (;;) {
        /*!re2c
         * { goto error; }
         "\x00" { goto error; }
         [ \t\r\n]*"="[ \t\r\n]* { break; }
        */
      }
      *idest = 0;
      neg = 0;
      for (;;) {
        p->t = p->p;
        /*!re2c
          *   { p->p = p->t; break; }
          "\x00" { goto error; }
          "-" { neg = 1; break; }
         */
      }
      for (;;) {
        p->t = p->p;
        /*!re2c
          * { goto error; }
          "\x00" { goto error; }
          [ \t\r\n]*";" { break; }
          [0-9] { *idest = *idest * 10 + (*p->t - '0'); continue; }
         */
      }
      if (neg) {
        *idest = -(*idest);
      }
      continue;

    statement:
      for(;;) {
        p->t = p->p;
        /*!re2c
          * { continue; }
          "\x00" { goto error; }
          "*""/" { goto error; }
          "{"    { break; }
        */
      }
      i = 0;
      for(;;) {
        /*!re2c
          * { continue; }
          "\x00" { goto error; }
          "/""*" { goto incomment; }
          "{"    { i++; continue; }
          "}"    { if (i == 0) break; i--; continue; }
        */

      incomment:
        for(;;) {
          /*!re2c
           * { continue; }
           "\x00" { goto error; }
           "*""/" { break; }
          */
        }
      }
      if (parser_copyToken(dest, p, 1) != 0) goto error;
      if (!add_alloc(set, *dest)) goto error;
      continue;
    }
    if (data.dir != 0) {
      fprintf(output, "\n");
      cs_printf(output, data.top, data.indent, "{\n");

      cs_printf(output, data.top + 1, data.indent, "/""*!re2c\n");
      cs_printf(output, data.top + 1, data.indent, "  re2c:define:YYCURSOR = \"%s\";\n",
                data.from);
      cs_printf(output, data.top + 1, data.indent, "  re2c:indent:top = %d;\n",
                data.top + 1);
      cs_printf(output, data.top + 1, data.indent,
                "  re2c:indent:string = \"%s\";\n\n", data.indent);
      cs_printf(output, data.top + 1, data.indent,
                "  * { goto mktble_ng; }\n", data.indent);
      cs_printf(output, data.top + 1, data.indent,
                "  \"\\x00\" { goto mktble_end; }\n", data.indent);
      if (data.dir > 0) { // Unicode to MBCS.
        ci = set->charmaps;
        for (; ci; ci = ci->next) {
          if (ci->fb_indicator == FB_BEST_REVERSE_FALL_BACK) continue;
          if (ci->u32char == 0) continue;
          if (ci->u32char <= 0xffff) {
            cs_printf(output, data.top + 1, data.indent,
                      "  \"\\u%04x\" { (%s) = \"%s\"; goto mktble_ok; }\n",
                      ci->u32char, data.to, ci->mbcs);
          } else {
            cs_printf(output, data.top + 1, data.indent,
                      "  \"\\U%08x\" { (%s) = \"%s\"; goto mktble_ok; }\n",
                      ci->u32char, data.to, ci->mbcs);
          }
        }
      } else if (data.dir < 0) {
        ci = set->charmaps;
        for (; ci; ci = ci->next) {
          if (ci->fb_indicator == FB_BEST_FALL_BACK) continue;
          if (ci->u32char == 0) continue;
          if (ci->u32char <= 0xffff) {
            if (data.utfbits > 16) {
              cs_printf(output, data.top + 1, data.indent,
                        "  \"%s\" { (%s) = %#06x; goto mktble_ok; }\n",
                        ci->mbcs, data.to, ci->u32char);
            } else if (data.utfbits > 8) {
              cs_printf(output, data.top + 1, data.indent,
                        "  \"%s\" { (%s)[0] = %#06x; (%s)[1] = 0x0000; goto mktble_ok; }\n",
                        ci->mbcs, data.to, ci->u32char, data.to);
            } else {
              if (ci->u32char >= 0x0800) {
                cs_printf(output, data.top + 1, data.indent,
                          "  \"%s\" { "
                          "(%s)[0] = %#04x; (%s)[1] = %#04x; "
                          "(%s)[2] = %#04x; (%s)[3] = %#04x; "
                          "goto mktble_ok; }\n",
                          ci->mbcs,
                          data.to, (0xe0 | ((ci->u32char >> 12) & 0x0f)),
                          data.to, (0x80 | ((ci->u32char >>  6) & 0x3f)),
                          data.to, (0x80 | ( ci->u32char        & 0x3f)),
                          data.to, 0);
              } else if (ci->u32char >= 0x0080) {
                cs_printf(output, data.top + 1, data.indent,
                          "  \"%s\" { "
                          "(%s)[0] = %#04x; (%s)[1] = %#04x; "
                          "(%s)[2] = %#04x; (%s)[3] = %#04x; "
                          "goto mktble_ok; }\n",
                          ci->mbcs,
                          data.to, (0xc0 | ((ci->u32char >>  6) & 0x1f)),
                          data.to, (0x80 | ( ci->u32char        & 0x3f)),
                          data.to, 0, data.to, 0);
              } else {
                cs_printf(output, data.top + 1, data.indent,
                          "  \"%s\" { "
                          "(%s)[0] = %#04x; (%s)[1] = %#04x; "
                          "(%s)[2] = %#04x; (%s)[3] = %#04x; "
                          "goto mktble_ok; }\n",
                          ci->mbcs,
                          data.to, ci->u32char,
                          data.to, 0, data.to, 0, data.to, 0);
              }
            }
          } else {
            if (data.utfbits > 16) {
              cs_printf(output, data.top + 1, data.indent,
                        "  \"%s\" { (%s) = %#10x; goto mktble_ok; }\n",
                        ci->mbcs, data.to, ci->u32char);
            } else if (data.utfbits > 8) {
              ci->u32char -= 0x10000;
              cs_printf(output, data.top + 1, data.indent,
                        "  \"%s\" { (%s)[0] = %#06x; (%s)[1] = %#06x; goto mktble_ok; }\n",
                        ci->mbcs,
                        data.to, (0xd800 | (ci->u32char >> 10)),
                        data.to, (0xdc00 | (ci->u32char & 0x3ff)));
            } else {
              cs_printf(output, data.top + 1, data.indent,
                        "  \"%s\" { "
                        "(%s)[0] = %#04x; (%s)[1] = %#04x; "
                        "(%s)[2] = %#04x; (%s)[3] = %#04x; "
                        "goto mktble_ok; }\n",
                        ci->mbcs,
                        data.to, (0xf0 | ((ci->u32char >> 18) & 0x07)),
                        data.to, (0x80 | ((ci->u32char >> 12) & 0x3f)),
                        data.to, (0x80 | ((ci->u32char >>  6) & 0x3f)),
                        data.to, (0x80 | ( ci->u32char        & 0x3f)));
            }
          }
        }
      }
      cs_printf(output, data.top + 1, data.indent, " *""/\n\n");

      cs_printf(output, data.top + 1, data.indent, "mktble_ok:\n");
      cs_printf(output, data.top + 1, data.indent, "{%s}\n\n", data.oks);

      cs_printf(output, data.top + 1, data.indent, "mktble_ng:\n");
      cs_printf(output, data.top + 1, data.indent, "{%s}\n\n", data.ngs);

      cs_printf(output, data.top + 1, data.indent, "mktble_end:\n");
      cs_printf(output, data.top + 1, data.indent, "{%s}\n\n", data.ends);

      cs_printf(output, data.top, data.indent, "}\n");
    }
    state = Normal;
    data.dir = 0;
    continue;

  at_embed:
    p->t = p->p;
    for(;;) {
      /*!re2c
        *      { goto error; }
        "\n"   { goto error; }
        "\x00" { goto error; }
        "NAME@"        { goto name_embed; }
        "HEADER_NAME@" { goto headname_embed; }
        "INAME@"       { goto iname_embed; }
        "IANA_NAME@"   { goto iana_name_embed; }
        "MAX_CHAR@"    { goto maxchar_embed; }
        "MIN_CHAR@"    { goto minchar_embed; }
       */
      continue;

    iname_embed:
      fprintf(output, "%s", set->code_set_name);
      break;

    iana_name_embed:
      fprintf(output, "%s", set->iana_name);
      break;

    name_embed:
      fprintf(output, "%s", set->funcname);
      break;

    headname_embed:
      fprintf(output, "%s", set->headname);
      break;

    maxchar_embed:
      fprintf(output, "%d", set->mb_cur_max);
      break;

    minchar_embed:
      fprintf(output, "%d", set->mb_cur_min);
      break;
    }
    continue;
  }

  parser_free(p);
  return 0;

 error:
  for(;;) {
    /*!re2c
      * { continue; }
      "\x00" { break; }
      "\n" { break; }
     */
  }
  p->p--;
  p->c = p->t;
  while(p->c >= p->b && *p->c != '\n') p->c--;
  p->c++;
  fflush(stdout);
  fprintf(stderr, "\n");
  fprintf(stderr, "Parse Error near: %.*s\n", (int)(p->p - p->c), p->c);
  fprintf(stderr, "                  %*s^\n", (int)(p->t - p->c), "");
  parser_free(p);
  return 2;
}

void free_charinfo(charInfo *cip) {
  free(cip);
}

void free_tblset(tblset *set)
{
  allocData *a, *q;

  if (!set) return;

  a = set->allocs;
  while (a) {
    q = a->next;
    free(a->p);
    free(a);
    a = q;
  }
}

int main(int argc, char **argv)
{
  FILE *fp, *ofp;
  tblset tset;
  int ret;

  memset(&tset, 0, sizeof(tblset));

  if (argc <= 6) {
    fprintf(stderr, "Usage: %s UCM_DATA TEMPLATE OUTPUT CNAME HNAME IANA_NAME\n",
            argv[0]);
    return 2;
  }

  fp = fopen(argv[1], "rb");
  if (!fp) { perror(argv[1]); return 1; }

  ret = parse_ucm(&tset, fp);
  fclose(fp);

  tset.funcname = argv[4];
  tset.headname = argv[5];
  tset.iana_name = argv[6];

  if (ret != 0) {
    fprintf(stderr, "failed to parse ucm\n");
    free_tblset(&tset);
    return 1;
  }

  /*
  for(cip = tset.charmaps; cip; cip = cip->next) {
    fprintf(stderr, "U:%08x MB:%s FLG:%d\n",
            cip->u32char, cip->mbcs, cip->fb_indicator);
  }
  */

  fp = fopen(argv[2], "rb");
  if (!fp) { perror(argv[2]); return 1; }

  ofp = fopen(argv[3], "wb");
  if (!ofp) { perror(argv[3]); fclose(fp); return 1; }

  ret = embed_code(&tset, fp, ofp);
  fclose(fp);
  fclose(ofp);

  free_tblset(&tset);
  return ret;
}
