#include <stdio.h>
#include <string.h>

#include "bu.h"


int main() {
  bigunsigned a,b,c;
  //char s[BU_MAX_HEX+1];

  bu_readhex(&a,"CAB51AFFBEEE"); //currently appending to the right
    printf("\n\n\nfinished with a\n\n\n");

  bu_readhex(&b,"a23456787765");
      printf("\n\n\nfinished with b\n\n\n");

  
  printf("need to print correct output here: %x\n", a.digit[0]);
    printf("need to print correct output here: %x\n", a.digit[1]);

  bu_dbg_printf(&a);
  bu_dbg_printf(&b);

  bu_add(&c, &a, &b);
 
  bu_dbg_printf(&c);
  return 0;
}
