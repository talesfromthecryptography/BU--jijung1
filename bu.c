#include <string.h> // for memset, etc.
#include <stdio.h>  // for printf

#include "bu.h"

// NOTE: In this code "word" always refers to a uint32_t

// Copy dest = src
void bu_cpy(bigunsigned *dest, bigunsigned *src) {
  uint16_t cnt = src->used; //used is the number of digit indices occupied
  dest->used = cnt; //set the same amt of digit indices for dest
  dest->base = 0; 

  // reset upper 0s in dest
  memset(dest->digit, 0, sizeof(uint32_t)*BU_DIGITS-cnt); //replace all unused upper digit indices to 0

  uint8_t i_dest = dest->base; 
  uint8_t i_src = src->base; 

  while (cnt-- > 0) {
    dest->digit[i_dest++] = src->digit[i_src++]; 
  }
}

// Set to 0
void bu_clear(bigunsigned *a_ptr) {
  memset(a_ptr->digit, 0, sizeof(uint32_t)*BU_DIGITS);
  a_ptr->used = 0;
  a_ptr->base = 0;
}

// Shift in place a bigunsigned by cnt bits to the left
// Example: beef shifted by 4 results in beef0
void bu_shl_ip(bigunsigned* a_ptr, uint16_t cnt) {  //sometimes works...
  uint16_t wrds = cnt >> 5; // # of whole words to shift 
  uint16_t bits = cnt &0x1f;// number of bits in a word to shift 00..100 == cnt
  uint32_t mask = 0xffffffff << bits; //255 1s shifted left by number of bits to shift 11..1000000
  uint32_t i = 0;
  uint32_t usedbits = a_ptr->digit[a_ptr->used-1];
  while (usedbits > 0) { //count number of initial bits in a_ptr->digit[a_ptr->used-1] aka least significant index
    usedbits = usedbits >> 1;
    i++;
  }
  uint32_t j = bits + i; //set j = bits to shift + initial bits in lsi
  if (j > 32) { 
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << (32-i); //a_ptr->digit[used-1] will be fully populated 
    a_ptr->used++;
    j = j - (32 - i); 
    while (j > 32) { //iteratively shift words
      a_ptr->digit[a_ptr->used-1] = 0x00000000;
      a_ptr->used++;
      j-=32;
    }
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << j;
  }
  else {
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << bits;  
    j-=bits;
  }
  
}

void bu_shr_ip(bigunsigned* a_ptr, uint16_t cnt) { //kind of works..
    uint32_t i = 0;
    uint32_t usedbits = a_ptr->digit[a_ptr->used-1];
    while (usedbits > 0) { //count number of initial bits in a_ptr->digit[a_ptr->used-1] aka least significant index
        usedbits = usedbits >> 1;
        i++;
    } 
    //if cnt <= i, then only need to shift the least significant index
    if (cnt <= i) {
        a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] >> cnt;
        i -= cnt;
        
        if (i <= 0) {
            a_ptr->used--;
        }
        
    }
    else { //set cnt = cnt - i and shr the lsi by i, then 
        a_ptr->digit[a_ptr->used-1] = ((a_ptr->digit[a_ptr->used-1]) >> i);
        cnt = cnt - i;
        if (i <= cnt) {
            a_ptr->used--;
        }
        
        while (cnt >= 32) {
            a_ptr->digit[a_ptr->used-1] = 0x00000000;
            a_ptr->used--;
            cnt-=32;
        }
        while (cnt > 0) {
            i=0;
            usedbits = a_ptr->digit[a_ptr->used-1];
            while (usedbits > 0) { //reset i
                usedbits = usedbits >> 1;
                i++;
            }
            if (i < cnt && i > 0) {
                a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << i;
                a_ptr->used--;
                cnt -= i;
            }
            else {
                a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << cnt;
                cnt = 0;
            }
        }
    }
}

// Produce a = b + c
void bu_add(bigunsigned *a_ptr, bigunsigned *b_ptr, bigunsigned *c_ptr) {
  uint8_t carry = 0;
  uint64_t nxt;
  uint16_t cnt = 0;
  uint16_t min_used = b_ptr->used <= c_ptr->used 
                      ? b_ptr->used : c_ptr->used;
  uint8_t  b_dig = b_ptr->base;
  uint8_t  c_dig = c_ptr->base;
  uint8_t  a_dig = 0;

  while (cnt < min_used) {
    nxt = ((uint64_t)b_ptr->digit[b_dig++]) 
          + (uint64_t)(c_ptr->digit[c_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < b_ptr->used && carry) {
    nxt = ((uint64_t)b_ptr->digit[b_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < b_ptr->used) {
    a_ptr->digit[a_dig++] = b_ptr->digit[b_dig++];
    cnt++;
  }  

  while (cnt < c_ptr->used && carry) {
    nxt = ((uint64_t)c_ptr->digit[c_dig++]) + carry;
    carry = 0 != (nxt&0x100000000);
    a_ptr->digit[a_dig++] = (uint32_t)nxt;
    cnt++;
  }

  while (cnt < c_ptr->used) {
    a_ptr->digit[a_dig++] = c_ptr->digit[c_dig++];
    cnt++;
  }

  while (cnt < BU_DIGITS && carry) {
    a_ptr->digit[a_dig++] = 1;
    carry = 0;
    cnt++;
  }

  a_ptr->base = 0;
  a_ptr->used = cnt;
}

// return the length in bits (should always be less or equal to 32*a->used)
uint16_t bu_len(bigunsigned *a_ptr) {
  uint16_t res = a_ptr->used<<5;
  uint32_t bit_mask = 0x80000000;
  uint32_t last_wrd = a_ptr->digit[a_ptr->base+a_ptr->used-1];

  while (bit_mask && !(last_wrd&bit_mask)) {
    bit_mask >>= 1;
    res--;
  }
  return res;
}

// Read from a string of hex digits
//
// TODO: This is wrong. See the test main.c
//       Modify to resolve 'endian' conflict.
//       Also modify to permit strings to include whitespace
//        that will be ignored. For example, "DEAD BEEF" should
//        be legal input resulting in the value 0xDEADBEEF.


void bu_readhex(bigunsigned * a_ptr, char *s) { //first call - char *s is the hex data to be read
  bu_clear(a_ptr);
  unsigned pos = 0;
  char *s_ptr = s;
  while (*s_ptr && pos < BU_MAX_HEX) {
      if (*s_ptr == ' ') {
        s_ptr++;
      }
      else {
        a_ptr->digit[pos>>3] |= ((uint32_t)hex2bin(*s_ptr)); //left of |= equates to a_ptr->digit[0] while pos < 9, so the values currently in                         //a_ptr->digit[0] is | with the value of *s_ptr 
        s_ptr++;          
        if ((pos+1)%8 != 0 && *(s_ptr) != '\0') { //shift as long as current index is not full and s_ptr != null
          a_ptr->digit[pos>>3] =  a_ptr->digit[pos>>3] << 4; //shl          
        }
        pos++;
      }
  }
    a_ptr->used = (pos>>3) + ((pos&0x7)!=0); //counts how many 8 hex blocks have been populated
}

void bu_dbg_printf(bigunsigned *a_ptr) { //modified to print in big endian order, may be problematic later..
    
  printf("Used %x\n", a_ptr->used); //%x specifies hexadecimal output format
  printf("Base %x\n", a_ptr->base);
  uint16_t i = 0;
  printf("Digits: ");
  while (i < a_ptr->used) {
        printf("%8x ", a_ptr->digit[a_ptr->base+i]);
        i++;      
    }

  printf("Length: %x\n", bu_len(a_ptr));
}


