#include "NanoBERT.h"

// =============== RPC Handlers ================ //

void copyStream(StreamBuff& outstream, StreamBuff& instream)
{
  while (instream.available()) {
    outstream.write(instream.read());
  }
}



/* ============ ATOMS ============= */

// #define BASE 36

#define atom_t uint32_t


uint32_t ipow(uint32_t base, uint32_t exponent)
{
    uint32_t result = 1;
    while (exponent)
    {
        if (exponent & 1)
            result *= base;
        exponent >>= 1;
        base *= base;
    }

    return result;
}

atom_t atom_parse(char *atomStr)
{
  unsigned int i;
  unsigned int c;
  unsigned int x,y;

  int n = strnlen(atomStr,NANO_BERT_RPC_MAX_ATOM_LEN);

  y = 0;
  for (i = 0; i < n; ++i) {
    x = (atomStr[i]-'a') % BASE;
    y += x * ipow(BASE,i);
  }

  return y;
}

void atom_print(StreamBuff *ostream, atom_t atom) {


  size_t i;
  char c;

  for (i = 0; i < 6 && atom; ++i) {
    c = atom % BASE + 'a';
    ostream->write(c);
    atom = atom / BASE;
  }
}
