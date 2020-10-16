// future ini file on sd card
// https://www.adafruit.com/product/254 or any clone

// Number of NeoPixels on Ring or Strip. Change count for match your setup.
// You can use multiple ring and strip in "Series" (strip(24)+ring(16)+strip(24) = 64 pixels)
// #define NUM_PIXELS 16
#define NUM_PIXELS 24

// if using the WS2811 with Integrated Drivers,
// #define PIXEL_TYPE NEO_GRB otherwise #define PIXEL_TYPE NEO_RGB
// more https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h#L52
#define PIXEL_TYPE NEO_GRB

// Uncomment below USE_ORIGINAL, if you want to use xbox original dvd remote.
// #define USE_ORIGINAL
// Original https://www.amazon.com/Xbox-DVD-Movie-Playback-Kit/dp/B00005O0I9
// Generic https://www.amazon.com/DVD-Remote-Control-Xbox-Buttons-Sensor/dp/B000YQSKZC

// Uncomment below USE_CUSTOM, if you want to use a custom remote control.
// #define USE_CUSTOM


// DON'T TOUCH THIS
#ifdef USE_ORIGINAL
  constexpr ORIGINAL xKEY; // used with case, value must be a constant.
#elif defined USE_CUSTOM
  constexpr CUSTOM xKEY;
#else
  constexpr GENERIC xKEY;
#endif


// SETTINGS
struct
{
  // startup: see (XBIR_NeoPixel/resources/images/xbox generic dvd remote.jpg) for key and effects
  // sendkey - key to start your effect (default xKEY.back for all pixels blue)
  // mode_effect - 0 to 6 (default -1 for random effect)
  //               effect 6 is snake and not indicated in image remote.jpg
  //               if xKEY is play
  uint32_t startup_sendkey = xKEY.back;
  int startup_mode_effect  = -1;

  // will hold the current brightness level 0-255 default 64 = 25%
  uint8_t brightness = 64;

  // colorWipe: speed - time between pixel in milliseconde
  uint8_t colorW_speed = 50;

  // rainbow: speed time (in ms) between frames.
  uint8_t rainbow_speed = 10;

  // rainbowCycle: speed - time (in ms) between frames.
  uint8_t rbCycle_speed = 10;

  // rainbowWheel: speed - time (in ms) between frames.
  uint8_t rbWheel_speed = 32;

  // theaterChase: speed - time (in ms) between frames.
  //               step - between pixels. (minimum is 1)
  uint8_t thChase_speed = 50;
  int thChase_step      = 2;

  // theaterChaseRainbow: speed - time (in ms) between frames.
  //               step - between pixels. (minimum is 1)
  uint8_t thChaseRB_speed = 50;
  int thChaseRB_step      = 3;

  // altColor: speed - time (in ms) between frames.
  //           cycles - one cycle is scanning through all pixels.
  uint8_t altColor_cycles = 7;
  uint8_t altColor_speed  = 128;

  // knightRider
  // Cycles - one cycle is scanning through all pixels left then right (or right then left)
  // Speed - how fast one cycle is (32 with 16 pixels is default knightRider speed)
  // Width - how wide the trail effect is on the fading out LEDs.  The original display used
  //         light bulbs, so they have a persistance when turning off.  This creates a trail.
  //         Effective range is 2 - 8, 4 is default for 16 pixels.  (minimum is 1)
  // Color - 32-bit packed RGB color value.  All pixels will be this color.
  uint8_t kitt_cycles = 7;
  uint8_t kitt_speed  = 32;
  uint8_t kitt_width  = 2;
  uint32_t kitt_color = COLOR.kitt;

  // size – the number of LEDs that represent the meteor,
  //        not counting the tail of the meteor. (minimum is 1)
  // trailDecay - A larger number makes the tail short and/or disappear faster.
  //              Theoretically a value of 64 should reduce the brightness by 25%
  //              for each time the meteor gets drawn.
  // randomDecay - If this value is set to “true” then some randomness is added to the rail.
  //               If you set the value to “false” then the tail will be very smooth.
  // Speed - how much the drawing speed has to be delayed.
  //         A value of zero (0) means maximum speed.
  //         Any value above zero indicates how many milliseconds
  //         (1000 ms in a second) the drawing will be delayed.
  // Color - All pixels of meteor will be this color.
  //         Black color indicate to use random color.
  byte meteor_size        = 3;
  byte meteor_trailDecay  = 64;
  bool meteor_randomDecay = false;
  uint8_t meteor_speed    = 32;
  uint32_t meteor_color   = COLOR.black;

  // randomColorFill: Speed - time (in ms) for each color to step next pixel.
  uint8_t randomFill_speed = 50;

  // random2ColorWipe: Speed - time (in ms) for each colors to step next pixel in same time.
  uint8_t random2Wipe_speed = 128;

  // sideFill: speed - time (in ms) for each pixels move from sides to middle
  //           reverse - “true” move from middle to sides after move from sides to middle
  uint8_t sideFill_speed = 100;
  bool sideFill_reverse  = false;
  uint32_t sideFill_color;

  // pacman: speed - time (in ms) for pacman/ghost move to next pixels
  //         wait - time (in ms) between frames.
  //         wpill - wait time (in ms) between chasing ghosts again after eating power pill.
  //         foodstep - step between food. (minimum is 1)
  uint8_t pac_speed = 100;
  long pac_wait     = 1000;
  long pac_wpill    = 500;
  int pac_foodstep  = 2;

  // snake: speed - time between pixel in milliseconde
  uint8_t snake_speed      = 64;
  uint32_t snake_headcolor = COLOR.darkred;
  uint32_t snake_bodycolor = COLOR.darkgreen;

}
SETTINGS;
