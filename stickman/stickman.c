
//
// Move a stick figure around the screen
//
#include <stdint.h>
#include <stdlib.h>
#include "neslib.h"

// Palette data
unsigned char dvd2_pal[] =
{
  // Background palette data..all black
  0xd, 0x23, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,

  // Sprite palette data
  0xd, 0x21, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd,
  0xd, 0xd, 0xd, 0xd
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

// Still metasprite
unsigned char walk1[] = {
  16, // upper left hand corner
  17, // upper right hand corner
  32, // lower left hand corner
  33, // lower right hander corner
};

// Moving metasprite
unsigned char walk2[] = {
  18, // upper left hand corner
  19, // upper right hand corner
  34, // lower left hand corner
  35, // lower right hand corner
};

// Globals for keeping track of where the metasprite should go.  Start in the
// middle of the screen
uint8_t x = 128;
uint8_t y = 120;

// Puts a metasprite at a specific location on the screen
//
// oam_id - Input oam_id
//
// Returns oam_id from the last oam_spr() call
unsigned int put_metasprite(unsigned char *sprite, unsigned int oam_id)
{
  oam_id = oam_spr(x, y, sprite[0], 0, oam_id); // Upper left hand corner
  oam_id = oam_spr(x + 8, y, sprite[1], 0, oam_id); // Upper right hand corner
  oam_id = oam_spr(x, y + 8, sprite[2], 0, oam_id); // Lower left hand corner
  oam_id = oam_spr(x + 8, y + 8, sprite[3], 0, oam_id); // Lower right hand corner

  return oam_id;
}

// Frames to wait inbetween each animation
#define ANIMATION_COUNTER   4

// Does the sprite animaion when the directional pad has been pressed
void animate()
{
  char oam_id;
  unsigned char counter;

  // Change the metasprite to create the animation effect
  oam_id = 0;
  oam_id = put_metasprite((unsigned char *)&walk2, oam_id);

  counter = ANIMATION_COUNTER;
  while (counter > 0)
  {
    ppu_wait_frame();
    counter--;
  }

  // Change the metasprite back to the original
  oam_id = 0;
  oam_id = put_metasprite((unsigned char *)&walk1, oam_id);
}

// Number of pixels to move when the d-pad is pressed
#define NUM_PIXELS_TO_MOVE    8

// main function, run after console reset
void main(void) {
  uint8_t pad;
  uint8_t moved;
  char oam_id = 0;

  setup_graphics();

  // Put the initial sprite
  oam_id = put_metasprite((unsigned char *)&walk1, oam_id);

  // Main game loop
  while (1)
  {
    moved = 0;

    // Sample the d-pad
    pad = pad_poll(0);

    // Check each bit individually to account for multiple directions
    if (pad & PAD_RIGHT)
    {
      if (x < (256 - NUM_PIXELS_TO_MOVE))
      {
        x += NUM_PIXELS_TO_MOVE;
        moved = 1;
      }
    }
    if (pad & PAD_LEFT)
    {
      if (x > 0)
      {
        x -= NUM_PIXELS_TO_MOVE;
        moved = 1;
      }
    }
    if (pad & PAD_UP)
    {
      if (y > 0)
      {
        y -= NUM_PIXELS_TO_MOVE;
        moved = 1;
      }
    }
    if (pad & PAD_DOWN)
    {
      if (y < (240 - NUM_PIXELS_TO_MOVE))
      {
        y += NUM_PIXELS_TO_MOVE;
        moved = 1;
      }
    }

    // If the d-pad had been pressed then animate the stickman moving  
    if (moved == 1)
    {
      animate();
      moved = 0;
    }
  }
}
