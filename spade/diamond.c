
//
// Make the screen filled with spade background sprites and then keep
// randomly assigning palette values
//
#include "neslib.h"

#define ATTRIBUTE_TABLE_A_ADDR   (NAMETABLE_A + 0x3c0)

// There is 4 possible palettes for bacground sprites
const char PALETTE[16] = {
 0xf, 0x14, 0x20, 0x30,
 0xf, 0x29, 0x20, 0x30,
 0xf, 0x21, 0x20, 0x30,
 0xf, 0x28, 0x20, 0x30
};

// Default attribute which we'll fill in with random data later
unsigned char ATTRIBUTE_TABLE[64] = {
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

// Creates an entry in the attribute table where a, b, c and d are quadrants
// in the entry:
//
//  _______
// | a | b |
// |---|---|
// | c | d |
//  -------
char create_attribute_byte(char a, char b, char c, char d)
{
  return d << 6 | c << 4 | b << 2 | a;
}

// Global counter that we key off of to change the attribute table
unsigned int change_background = 0;

// main function, run after console reset
void main(void) {
  int i;
  char a, b, c, d, rand_byte;

  // Load background pallette
  pal_bg(PALETTE);

  // The nametable contains a 32x30 grid of the background sprites
  // for the whole screen.  Fill the background with a spade character
  //mfrom chr_generis.s (20)
  vram_adr(NAMETABLE_A);		// set address
  vram_fill(20, (32 * 30));	// write bytes to video RAM

  // Fill the attribute table with random data
  for (i = 0; i < 64; i++) {
    a = rand8() % 4;
    b = rand8() % 4;
    c = rand8() % 4;
    d = rand8() % 4;
    rand_byte = create_attribute_byte(a, b, c, d);
    ATTRIBUTE_TABLE[i] = rand_byte;
  }

  // Load attribute table to define which palette each sprite group uses
  vram_adr(ATTRIBUTE_TABLE_A_ADDR);
  vram_write(ATTRIBUTE_TABLE, sizeof(ATTRIBUTE_TABLE));

  // enable PPU rendering (turn on screen)
  ppu_on_all();

  // infinite loop
  while (1) {
    // Only change the attribute table every so often so the screen doesn't induce seizures.
    // A value of 0xff is from experimenting
    if ((change_background % 0xff) == 0) {
      for (i = 0; i < 64; i++) {
        a = rand8() % 4;
        b = rand8() % 4;
        c = rand8() % 4;
        d = rand8() % 4;
        rand_byte = create_attribute_byte(a, b, c, d);
        ATTRIBUTE_TABLE[i] = rand_byte;
      }

      // Load attribute table to define which palette each sprite group uses
      vram_adr(ATTRIBUTE_TABLE_A_ADDR);
      vram_write(ATTRIBUTE_TABLE, sizeof(ATTRIBUTE_TABLE));
    }

    change_background++;
  }
}
