
//
// Bounces the dvd logo around the screen
//
#include <stdint.h>
#include <stdlib.h>
#include "neslib.h"

// Palette data
unsigned char dvd2_pal[] = {
  // Background palette data..all black
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,

  // Sprite palette data
  0x0d, 0x23, 0x0d, 0x0d,
  0x0d, 0xd, 0x0d, 0x0d,
  0x0d, 0xd, 0x0d, 0x0d,
  0x0d, 0xd, 0x0d, 0x0d
};

unsigned int dvd2_pal_len = 32;

// Initialize the NES to display our sprites
void setup_graphics()
{
  // Clear sprite memory
  oam_clear();

  // Set the palette data 
  pal_all(dvd2_pal);

  // Turn the picture unit on
  ppu_on_all();
}

// Metasprite definition.  A four byte array of which sprite number each part
// of the metasprite is to make it easier to pass to the put_metasprite
// function
unsigned char metasprite[] = {
  0, // upper left hand corner
  1, // lower left hand corner
  2, // upper right hand corner
  3, // lower right hand corner
};


// Current x screen position
unsigned int x = 8;

// Current y screen position
unsigned int y = 8;

// Change is x position between each frame
int dx = 1;

// Change in y position between each frame
int dy = 1;

// Puts a metasprite at a specific location on the screen
//
// oam_id - Input oam_id
//
// Returns oam_id from the last oam_spr() call
unsigned int put_metasprite(unsigned int oam_id)
{
  oam_id = oam_spr(x, y, metasprite[0], 0, oam_id);
  oam_id = oam_spr(x + 8, y, metasprite[1], 0, oam_id);
  oam_id = oam_spr(x, y + 8, metasprite[2], 0, oam_id);
  oam_id = oam_spr(x + 8, y + 8, metasprite[3], 0, oam_id);

  return oam_id;
}

// main function, run after console reset
void main(void) {
  char oam_id;

  setup_graphics();

  while (1)
  {
    // Set initial OAM sprite offset to 0
    oam_id = 0;

    oam_id = put_metasprite(oam_id);

    // Set up x and y coordinates for next frame
    x += dx;
    y += dy;

    // Check if the x value has hit a boundary
    if (x >= (255 - 16))
    {
      dx = -1;
    }
    else if (x <= 0)
    {
      dx = 1;
    }

    // Check if the y value has hit a boundary
    if (y >= (240 - 20))
    {
      dy = -1;
    }
    else if (y <= 8)
    {
      dy = 1;
    }

    // Wait for next frame to be displayed
    ppu_wait_frame();
  }
}
