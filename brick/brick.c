
//
// Background bricks scroll up and down.  Yes, that's all this really does..
//
#include "neslib.h"


#define ATTRIBUTE_TABLE_A_ADDR    (NAMETABLE_A + 0x3c0)
#define ATTRIBUTE_TABLE_C_ADDR    (NAMETABLE_C + 0x3c0)

// We're only using the first palette for the brick color
const char PALETTE[16] = {
  0x0D, 0x16, 0x10, 0x20,
  0x0D, 0x0D, 0x0D, 0x0D,
  0x0D, 0x0D, 0x0D, 0x0D,
  0x0D, 0x0D, 0x0D, 0x0D
};

// Each background sprite only uses the first palette
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

int y;
int dy = 1;
unsigned short do_scroll = 0;
int row, col;

// main function, run after console reset
void main(void) {
  // Load background pallette
  pal_bg(PALETTE);

  // We have three brick sprites. We alternate the three sprites in each row so
  // do that dynamically.  This is probably not the most performant way to do this
  // but does the job
  for (row = 0; row < 30; row++) {
    for (col = 0; col < 32; col++) {
      vram_adr(NTADR_A(col, row));
      if ((row % 3) == 0) {
        vram_put(0);
      } else if ((row % 3) == 1) {
        vram_put(1);
      } else {
        vram_put(2);
      }    }
  }

  for (row = 0; row < 30; row++) {
    for (col = 0; col < 32; col++) {
      vram_adr(NTADR_C(col, row));
      if ((row % 3) == 0) {
        vram_put(0);
      } else if ((row % 3) == 1) {
        vram_put(1);
      } else {
        vram_put(2);
      }
    }
  }

  // Load attribute table to define which palette each background sprite group uses
  vram_adr(ATTRIBUTE_TABLE_A_ADDR);
  vram_write(ATTRIBUTE_TABLE, sizeof(ATTRIBUTE_TABLE));

  // Load attribute table to define which palette each sprite group uses
  vram_adr(ATTRIBUTE_TABLE_C_ADDR);
  vram_write(ATTRIBUTE_TABLE, sizeof(ATTRIBUTE_TABLE));
  
  ppu_on_all();

  // infinite loop
  while (1) {
    // Only scroll the screen mod 512 so that the scrolling is not too fast
    // but not too slow
    if ((do_scroll % 512) == 0) {
      ppu_wait_frame();
  
      if (y == 479) {
        dy = -1;
      } else if (y == 0) {
        dy = 1;
      }
  
      y = y + dy;

      scroll(0, y);
    }
    do_scroll++;
  }
}
