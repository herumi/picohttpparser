#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <x86intrin.h>

extern const char* find_str_range(const char* buf, const char* buf_end, const char* range, size_t rangeLen);

const char* find_str_rangeC2(const char* buf, const char* buf_end)
{
  static const char ranges[] __attribute__((aligned(16))) =
      "\011\011" /* allow HT */
      "\040\176" /* allow printable ascii */
      "\200\377" /* allow chars with MSB set */
      ;
  __m128i ranges16 = _mm_load_si128((const __m128i*)ranges);
  __m128i v;
  size_t left = buf_end - buf;
  for (;;) {
    size_t addr = (size_t)buf;
    size_t addr2 = addr & 0xfff;
    if (addr2 > 0xff0 && addr2 + left <= 0x1000) {
      static const unsigned char shiftPtn[32] __attribute__((aligned(16))) = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
      };
      addr2 = addr & -16;
      v = _mm_load_si128((const __m128i*)addr2);
      v = _mm_shuffle_epi8(v, *(const __m128i*)(shiftPtn + (addr & 15)));
    } else {
      v = _mm_loadu_si128((const __m128i*)buf);
    }
    if (!_mm_cmpestra(ranges16, sizeof(ranges) - 1, v, left, 0x14)) break;
    buf += 16;
    left -= 16;
  }
  if (_mm_cmpestrc(ranges16, sizeof(ranges) - 1, v, left, 0x14)) {
    buf += _mm_cmpestri(ranges16, sizeof(ranges) - 1, v, left, 0x14);
  }
  return buf;
}


const char* find_str_rangeA(const char* buf, const char* buf_end)
{
  static const char ranges[] __attribute__((aligned(16))) =
      "\011\011" /* allow HT */
      "\040\176" /* allow printable ascii */
      "\200\377" /* allow chars with MSB set */;
  return find_str_range(buf, buf_end, ranges, 6);
}

const char* find_str_rangeC(const char* buf, const char* buf_end)
{
  while (buf != buf_end) {
    char c = *buf;
    if (!(c == '\011' || ('\040' <= c && c <= '\176') || ('\200' <= c && c <= '\377'))) break;
    buf++;
  }
  return buf;
}

void test(const char* p, size_t len)
{
  const char *a = find_str_rangeA(p, p + len);
  const char *b = find_str_rangeC(p, p + len);
  if (a != b) {
    printf("ERR p=%p, len=%zd a=%p, b=%p\n", p, len, a, b);
    exit(1);
  }
}

void test_boundary()
{
  static char buf[4096] __attribute__((aligned(4096)));
  char *const p = &buf[4096 - 32];
  int begin;
  for (begin = 0; begin < 32; begin++) {
    int pos;
    for (pos = begin; pos < 32; pos++) {
      int len;
      memset(p, 'a', 32);
      p[pos] = '\001';
      for (len = 1; len < 32 - begin; len++) {
        test(p + begin, len);
      }
    }
  }
}
int main()
{
#if 0
  const struct {
    const char *str;
    size_t len;
  } tbl[] = {
    { "abc", 1 },
    { "abc", 2 },
    { "abc", 3 },
    { "\001\001\001a", 1 },
    { "\001\001\001a", 2 },
    { "\001\001\001a", 3 },
    { "\001\001\001a", 4 },
  };
  size_t i;
  for (i = 0; i < sizeof(tbl) / sizeof(*tbl); i++) {
    test(tbl[i].str, tbl[i].len);
  }
#endif
  test_boundary();
  puts("ok");
  return 0;
}
