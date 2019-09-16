#include <string.h> // for memset, etc.
#include <stdio.h>  // for printf

#include "bu.h"

// NOTE: In this code "word" always refers to a uint32_t

// Copy dest = src
void bu_cpy(bigunsigned *dest, bigunsigned *src) {
  uint16_t cnt = src->used; //used is the number of digit indices occupied
  dest->used = cnt; //set the same amt of digit indices for dest
  dest->base = 0; //dunno yet

  // reset upper 0s in dest
  memset(dest->digit, 0, sizeof(uint32_t)*BU_DIGITS-cnt); //replace all unused uppder digit indices to 0

  uint8_t i_dest = 0; // TODO: This is wrong. Fix it.  
  uint8_t i_src = src->base; 

  while (cnt-- > 0) {
    dest->digit[i_dest--] = src->digit[i_src--]; //inf loop
  }
}

// Set to 0
void bu_clear(bigunsigned *a_ptr) {
  memset(a_ptr->digit, 0, sizeof(uint32_t)*BU_DIGITS);
  a_ptr->used = 0;
  a_ptr->base = 0;
}

/*
 * in order to shift a bigunsigned struct cnt bits to the left digit, used and base must all be updated
  If no carry propagates in the little end, only internal calculation needed. just left shift by cnt.
 * 
 */
// Shift in place a bigunsigned by cnt bits to the left
// Example: beef shifted by 4 results in beef0
void bu_shl_ip(bigunsigned* a_ptr, uint16_t cnt) { 
  uint16_t wrds = cnt >> 5; // # of whole words to shift 
 // uint16_t wrds2 = cnt >> 5;
  uint16_t bits = cnt &0x1f;// number of bits in a word to shift 00..100 == cnt
  uint32_t mask = 0xffffffff << bits; //255 1s shifted left by number of bits to shift 11..1000000
  
  uint8_t i = 0;
  uint32_t usedbits = a_ptr->digit[a_ptr->used-1];
  printf("usedbits: %x\n", usedbits);
  while (usedbits > 0) {
    usedbits = usedbits >> 1;
    printf("usedbits after shift: %x\n",usedbits);
    i++;
  }
  uint8_t j = bits + i;
  printf("i: %d\n", i);
  if (bits+i-32 > 0) {
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << (32-i);
    a_ptr->used++;
  }
  else {
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << bits;    
  }
  
  while (j > 32) {
    printf("shifting by wordsize\n");
    a_ptr->digit[a_ptr->used] = 0x00000000;
    a_ptr->used++;
    j-=32;
  }
  if (bits+i-32 > 0) {
    printf("shifting the remaining bits\n");
    a_ptr->digit[a_ptr->used-1] = a_ptr->digit[a_ptr->used-1] << (bits+i-32);
  }
  
  /*
  
  
  printf("wrds2: %d\ntarget index: %x\n",wrds2, a_ptr->digit[a_ptr->used-1-wrds2]);
  if ((a_ptr->digit[a_ptr->used-1-wrds2] >> (32 - bits)) > 0) { //check for carry, 
     //   a_ptr->digit[a_ptr->used-1-wrds2] = a_ptr->digit[a_ptr->used-1-wrds2] << cnt;
  }
  else { //only need to shift the used-1-wrds index
      a_ptr->digit[a_ptr->used-1-wrds2] = a_ptr->digit[a_ptr->used-1-wrds2] << bits;
  }

  */
 // printf("words do something : %x\n",a_ptr->digit[a_ptr->used-1]);
  //a_ptr->digit[a_ptr->used] = a_ptr->digit[a_ptr->used] << cnt;
  //a_ptr->digit[a_ptr->used-1] << cnt;
  //a_ptr->digit[pos>>3] =  a_ptr->digit[pos>>3] << 4
  
}

void bu_shr_ip(bigunsigned* a_ptr, uint16_t cnt) {
    uint16_t shift = 0;
    uint32_t carry = 0;
    if (cnt%32 != 0) {
        shift = cnt%32;
        if (cnt/32 > 0) {
            //need to iteratively shift a_ptr->digit here. Can't just do what i have on the next line.
            //a_ptr->digit >> (cnt/32) * 32; //truncate cnt
        }        
        int i = cnt/32;
        while (i < a_ptr->used-1) {
           
        }

        
    }
    else {
        int j = cnt/32;
        while (--j > 0) {
           // printf ("j: %d\n",j);
            a_ptr->digit[a_ptr->used-1 + j] = a_ptr->digit[a_ptr->used-1]; 
           // printf("used-1+j: %x\n", a_ptr->digit[a_ptr->used-1+j]);
        }
    }
   
    /*
     *starting with the 0th index, store cnt%32 msbs
     * if cnt / 32 > 0, shift a_ptr to the right by cnt/32 indices (>>1 = 1 hexbit, so if cnt / 32 = 2, you would >> 64
     * afterwards, starting with index cnt/32, you would shr by cnt%32 and insert the saved carry (do this iteratively until least sig index)
     * then for digit[used-1], if (4*(cnt%32) + digit[used-1]) / 32 > 0, save (4*(cnt%32) + digit[used-1]) % 32 as carry.
     *then shr by cnt%32 and insert the saved carry. insert the next saved carry in the following index.
     * 
     * 
     */
    
    
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
  while (*s_ptr && pos < BU_MAX_HEX) { //while <2048
      if (*s_ptr == ' ') {
        s_ptr++;
      }
      else {
        a_ptr->digit[pos>>3] |= ((uint32_t)hex2bin(*s_ptr)); /*left of |= equates to a_ptr->digit[0] while pos < 9, so the values currently in                         a_ptr->digit[0] is | with the value of *s_ptr shifted left by 4n bits. */

        //printf("digit1: %x\n", a_ptr->digit[pos>>3]);
        s_ptr++;          
        if ((pos+1)%8 != 0 && *(s_ptr) != '\0') { //if current index is full or there are no more hexbits to read in 
          a_ptr->digit[pos>>3] =  a_ptr->digit[pos>>3] << 4; //shl          
        }
        pos++;
      }
  }
  a_ptr->used = (pos>>3) + ((pos&0x7)!=0); //counts how many 8 hex blocks have been populated
}



/*
void bu_readhex(bigunsigned * a_ptr, char *s) { //first call - char *s is the hex data to be read
void bu_readhex(bigunsigned * a_ptr, char *s) { //first call - char *s is the hex data to be read
  bu_clear(a_ptr); //set all 32 bits of a to 0

  unsigned pos = 0;
  char *s_ptr = s;
  while (*s_ptr && pos < BU_MAX_HEX) { //while <2048
      if (*s_ptr == ' ') {
        s_ptr++;
      }
      else {
        a_ptr->digit[pos>>3] |= (((uint32_t)hex2bin(*s_ptr)) << ((pos & 0x7)<<2)); //left of |= equates to a_ptr->digit[0] while pos < 9, so the values currently in a_ptr->digit[0] is | with the value of *s_ptr shifted left by 4n bits. 0x7 kind of mods up to 7 bits so that the index of digit increments and *s_ptr shifts left a maximum of 7 hexbits.
        pos++;
        s_ptr++;          
      }

  }
  a_ptr->used = (pos>>3) + ((pos&0x7)!=0); //counts how many 8 hex blocks have been populated
  printf("a_ptr used: %x\n",a_ptr->used);
}
*/
void bu_dbg_printf(bigunsigned *a_ptr) {
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

/*
void bu_dbg_printf(bigunsigned *a_ptr) {
  printf("Used %x\n", a_ptr->used); //%x specifies hexadecimal output format
  printf("Base %x\n", a_ptr->base);
  uint16_t i = a_ptr->used;
  printf("Digits: ");
  while (i-- > 0)
    printf("%8x ", a_ptr->digit[a_ptr->base+i]);
  printf("Length: %x\n", bu_len(a_ptr));
}
*/
