#include <stdio.h>
#include <string.h>

#include "bu.h"


int main() {
  bigunsigned a,b,c;
  //char s[BU_MAX_HEX+1];

  bu_readhex(&a,"12341234aaaa1"); //currently appending to the right
   // bigunsigned* ptr = &a;

  bu_readhex(&b,"1111111111111111111111111111111111110001");
//printf("numerical value of digit[used-1]: %d\n", ptr->digit[ptr->used-1]);

  bu_dbg_printf(&a);
  
  bu_shl_ip(&a,4);
  printf("after SHL: \n");
      bu_dbg_printf(&a);

    bu_shl_ip(&a,28);
  printf("after SHL: \n");
    bu_dbg_printf(&a);
    

    


  
  
  bu_dbg_printf(&a);

  
  bu_dbg_printf(&b);

  bu_add(&c, &a, &b);
 
  bu_dbg_printf(&c);
  
  return 0;
}
