#include <stdio.h>
#include <string.h>

#include "bu.h"


int main() {
  bigunsigned a,b,c,d;
  //char s[BU_MAX_HEX+1];

  bu_readhex(&a,"deadbeef12345678");
   // bigunsigned* ptr = &a;

  bu_readhex(&b,"111111110000");
  
    bu_shl_ip(&a,1);
    printf("shl by 1...\n");
    bu_dbg_printf(&a);

    bu_shl_ip(&a,4);
    printf("shl by 4...\n");
    bu_dbg_printf(&a);

    bu_shl_ip(&a,8);
    printf("shl by 8...\n");
    bu_dbg_printf(&a);
        bu_shl_ip(&a,16);
    printf("shl by 16...\n");
    bu_dbg_printf(&a);
    
         bu_shl_ip(&a,32);
    printf("shl by 32...\n");
    bu_dbg_printf(&a);
  
    bu_dbg_printf(&a);

    printf("shr by 1...\n");

    bu_shr_ip(&a,1);
    bu_dbg_printf(&a);
    printf("shr by 4...\n");

    bu_shr_ip(&a,4);
    bu_dbg_printf(&a);
    printf("shr by 8...\n");

    bu_shr_ip(&a,8);
    bu_dbg_printf(&a);
    printf("shr by 16...\n");

    bu_shr_ip(&a,16);
    bu_dbg_printf(&a);
    printf("shr by 33...\n");
    bu_shr_ip(&a,32);
    bu_dbg_printf(&a);
    

    
  printf("a:\n");
  bu_dbg_printf(&a);

 
  
    


  
  
  bu_dbg_printf(&a);

  
  bu_dbg_printf(&b);

  bu_add(&c, &a, &b);
 
  bu_dbg_printf(&c);
  
  return 0;
}
