
#include <Adafruit_NeoPixel.h>
#include <IRremote.h>

#include "resources/lib/Log.h"        // used in debug mode
#include "resources/lib/Colors.h"     // COLOR.name
#include "resources/lib/XBIRRemote.h" // xKEY.name
#include "Configs.h" // SETTINGS.name and NUM_PIXELS


// Constants
#define OFF         LOW
#define ON          HIGH
#define TX_RX_ON    LOW  // RX led or TX led on
#define TX_RX_OFF   HIGH // RX led or TX led off

#define PIN_INT1    2    // Interrupt button
#define PIN_LED1    6    // led to indicate standby power. see psON()
#define PIN_LED2    9    // led to indicate a pressed key on remote
#define PIN_PIXELS  10   // Digital IO pin connected to the NeoPixels.

#define PIN_RELAY   7
#define RELAY_ON()  (digitalWrite(PIN_RELAY, HIGH))
#define RELAY_OFF() (digitalWrite(PIN_RELAY, LOW))

// Resistor (10k / 10k + 10k)) = 1.65 vout of 3.3v
#define SENSOR_3V() (analogRead(A0)) 


// Declare our NeoPixel ring object:
Adafruit_NeoPixel ring(NUM_PIXELS, PIN_PIXELS, PIXEL_TYPE+NEO_KHZ800);

// Declare our IRrecv object:
IRrecv irrecv(PIN_A2);


// Variables
uint8_t BRT_STEP   = 5; // Step of brightness

int MAX_EFFECT     = 6; //
int MODE_EFFECT    = 0; //
bool EFFECT_CHANGE = 0; // Used to stop current effect

uint32_t KEY_PRESSED;   // Current key pressed on remote


//********************************************************************************************************************************
// Declare functions with default arguments, before they are used.
// For Arduino IDE compile without errors.
//********************************************************************************************************************************
void flashLED(int pin, int repeat=3) {
  int state = digitalRead(pin);
  for (int i=0; i<repeat; i++) {
    digitalWrite(pin, ON);
    delay(250);
    digitalWrite(pin, OFF);
    delay(250);
  }
  if (state == 1) {
    digitalWrite(pin, ON);
  }
}

//********************************************************************************************************************************
// Leds Builtin Functions
// https://github.com/arduino/ArduinoCore-avr/blob/master/variants/leonardo/pins_arduino.h
// https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-1-blinkies
//********************************************************************************************************************************
void ledsBuiltinOFF() {
#ifndef DEBUG_MODE
  // set the RX and TX LEDs OFF
  pinMode(LED_BUILTIN_RX, OUTPUT);
  if (digitalRead(LED_BUILTIN_RX) == TX_RX_ON) {
    digitalWrite(LED_BUILTIN_RX, TX_RX_OFF);
  }
  pinMode(LED_BUILTIN_TX, OUTPUT);
  if (digitalRead(LED_BUILTIN_TX) == TX_RX_ON) {
    digitalWrite(LED_BUILTIN_TX, TX_RX_OFF);
  }
#endif
}

//********************************************************************************************************************************
// Interrupt Functions button pressed
//********************************************************************************************************************************
void stopEffect() {
  if (digitalRead(PIN_INT1) == ON) {
    restart();
  }
}
void restart() {
  EFFECT_CHANGE = true;
  asm volatile ("jmp 0");
}

//*****************************THE POWER*****************************
// send press power button on xbox (EXPERIMENTAL)
void xPowerButton() {
  LOG_SEP();
  if (!psON()) {
    LOG_END(F("Turned xbox ON!"));
  } else {
    LOG_END(F("Turned xbox OFF!"));
  }
  RELAY_ON();
  LOG_END(F("Relay ON!"));
  delay(100);
  RELAY_OFF();
  LOG_END(F("Relay OFF!"));
  delay(100);
}

bool psON() {
  bool state = (SENSOR_3V() > 0);
  switchLedSTB(state);
  return state;
}

void switchLedSTB(bool change) {
  if (!change && digitalRead(PIN_LED1) == OFF) {
    digitalWrite(PIN_LED1, ON);
    return;
  }
  if (change && digitalRead(PIN_LED1) == ON) {
    digitalWrite(PIN_LED1, OFF);
  }
}

//********************************************************************************************************************************
// Default Functions required setup and loop
//********************************************************************************************************************************
void setup() {
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_INT1, INPUT_PULLUP);  // internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(PIN_INT1), stopEffect, CHANGE); // pressed

  ring.begin(); // Initialize NeoPixel ring object (REQUIRED)
  ring.setBrightness(max(SETTINGS.brightness, 1));
  ring.show(); //delay(25); // Initialize all pixels to 'off' and wait for apply brightness

  irrecv.enableIRIn(); // Start the receiver

  LOG_BEGIN(); // Start log output if enabled

  ledsBuiltinOFF();
  digitalWrite(PIN_LED2, OFF);
  // flashLED(PIN_LED2);
  // psON();

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // set first effect on startup
  MODE_EFFECT = SETTINGS.startup_mode_effect;
  if (MODE_EFFECT == -1) MODE_EFFECT = random(MAX_EFFECT);
  translateKey(SETTINGS.startup_sendkey);
}

void loop() {
  onRemote();
  if (!psON()) {
    delay(1000);
    return;
  }
  switch (KEY_PRESSED) {
    // Mode effects predefined, changed by REVERSE/FORWARD
    case xKEY.reverse:
    case xKEY.forward:
    case xKEY.play:
      EFFECT_CHANGE = false;
      switch (MODE_EFFECT) {
        case 0:
          knightRider();
          break;
        case 1:
          meteorRain();
          break;
        case 2:
          random2ColorWipe();
          break;
        case 3:
          altColor(COLOR.blue, COLOR.midnightblue);
          break;
        case 4:
          theaterChase();
          break;
        case 5:
          randomColorFill();
          delay(100);
          break;
        case 6:
          snake();
          break;
      }
      break;

    // Mode effects predefined
    case xKEY.skip_minus:
      EFFECT_CHANGE = false;
      theaterChaseRainbow();
      break;

    case xKEY.stop:
      EFFECT_CHANGE = false;
      rainbowCycle();
      break;

    case xKEY.pause:
      EFFECT_CHANGE = false;
      rainbow();
      // rainbowReverse();
      break;

    case xKEY.skip_plus:
      EFFECT_CHANGE = false;
      rainbowWheel();
      break;

    case xKEY.info:
      EFFECT_CHANGE = false;
      pacman();
      break;

    default:
      delay(50);

    // case xKEY.display: // ON/OFF power
    // case xKEY.title:
    // COLOR PREDEFINED
    // case xKEY.menu:
    // case xKEY.select:
    // case xKEY.back:
    // case xKEY.one:
    // case xKEY.two:
    // case xKEY.three:
    // case xKEY.four:
    // case xKEY.five:
    // case xKEY.six:
    // case xKEY.seven:
    // case xKEY.eight:
    // case xKEY.nine:
    // case xKEY.zero:
      // EFFECT_CHANGE = false;
      // break;

  }
  // delay(50);
}

//********************************************************************************************************************************
// IRremote Functions
//********************************************************************************************************************************
void onRemote() {
  decode_results results;        // Somewhere to store the results
  if (irrecv.decode(&results)) { // Grab an IR code
#ifdef DEBUG_MODE
    digitalWrite(PIN_LED2, ON);
    logRemote(&results);
#endif
    if (results.decode_type == xKEY.type) {
      if (!digitalRead(PIN_LED2)) digitalWrite(PIN_LED2, ON);
      translateKey(results.value);
    }
    delay(50);
    for (int z=0; z<2; z++) { // ignore the 2nd and 3rd repeated codes
      irrecv.resume();        // receive the next value
      delay(25);
      }
    if (digitalRead(PIN_LED2))
      digitalWrite(PIN_LED2, OFF);
  }
}

void translateKey(uint32_t value) {
  switch (value) {
    case xKEY.display: // ON/OFF Power
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.display;
      LOG_KEY(KEY_PRESSED);
      // restart();
      xPowerButton();
      break;

    case xKEY.reverse:
      EFFECT_CHANGE = true;
      MODE_EFFECT--;
      if (MODE_EFFECT < 0) MODE_EFFECT = MAX_EFFECT;
      KEY_PRESSED = xKEY.reverse;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(MODE_EFFECT);
      break;

    case xKEY.play:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.play;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(MODE_EFFECT);
      break;

    case xKEY.forward:
      EFFECT_CHANGE = true;
      MODE_EFFECT++;
      if (MODE_EFFECT > MAX_EFFECT) MODE_EFFECT = 0;
      KEY_PRESSED = xKEY.forward;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(MODE_EFFECT);
      break;

    case xKEY.skip_minus:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.skip_minus;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(10);
      break;

    case xKEY.stop:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.stop;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(11);
      break;

    case xKEY.pause:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.pause;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(12);
      break;

    case xKEY.skip_plus:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.skip_plus;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(13);
      break;

    case xKEY.title:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.title;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(14);
      sideFill(getRandomColor());
      break;

    case xKEY.info:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.info;
      LOG_KEY(KEY_PRESSED);
      LOG_EFFECT(15);
      break;

    // Color predefined
    case xKEY.menu:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.menu;
      colorWipe(COLOR.red);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("RED");
      break;

    case xKEY.select:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.select;
      colorWipe(COLOR.lime);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("LIME");
      break;

    case xKEY.back:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.back;
      colorWipe(COLOR.blue);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("BLUE");
      break;

    case xKEY.one:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.one;
      colorWipe(COLOR.magenta);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("MAGENTA");
      break;

    case xKEY.two:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.two;
      colorWipe(COLOR.green);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("GREEN");
      break;

    case xKEY.three:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.three;
      colorWipe(COLOR.blueconfluence);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("BLUE CONFLUENCE");
      break;

    case xKEY.four:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.four;
      colorWipe(COLOR.orange);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("ORANGE");
      break;

    case xKEY.five:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.five;
      colorWipe(COLOR.darkgreen);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("DARK GREEN");
      break;

    case xKEY.six:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.six;
      colorWipe(COLOR.midnightblue);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("MIDNIGHT BLUE");
      break;

    case xKEY.seven:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.seven;
      colorWipe(COLOR.gold);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("GOLD");
      break;

    case xKEY.eight:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.eight;
      colorWipe(COLOR.yellow);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("YELLOW");
      break;

    case xKEY.nine:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.nine;
      colorWipe(COLOR.black);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("BLACK/OFF");
      break;

    case xKEY.zero:
      EFFECT_CHANGE = true;
      KEY_PRESSED = xKEY.zero;
      colorWipe(COLOR.white);
      LOG_KEY(KEY_PRESSED);
      LOG_COLOR("WHITE");
      break;

    // Ajust brightness
    case xKEY.up:
      LOG_KEY(xKEY.up);
      SETTINGS.brightness += (BRT_STEP * BRT_STEP);
      ajustBrightness(HIGH);
      break;
    case xKEY.down:
      LOG_KEY(xKEY.down);
      SETTINGS.brightness -= (BRT_STEP * BRT_STEP);
      ajustBrightness(LOW);
      break;
    case xKEY.right:
      LOG_KEY(xKEY.right);
      SETTINGS.brightness += BRT_STEP;
      ajustBrightness(HIGH);
      break;
    case xKEY.left:
      LOG_KEY(xKEY.left);
      SETTINGS.brightness -= BRT_STEP;
      ajustBrightness(LOW);
      break;

    case 0xFFFFFFFF: break; // repeat
    default: break;
  }
}

void ajustBrightness(bool up) {
  if (up) {
    if (SETTINGS.brightness > 255) SETTINGS.brightness = 255; // max
    ring.setBrightness(SETTINGS.brightness);
    ring.show();
    delay(250); // wait for apply brightness
    LOG(F("*** Ajust Brightness (+): "));
  } else {
    if (SETTINGS.brightness <= 0) SETTINGS.brightness = 1; // don't turn off
    ring.setBrightness(SETTINGS.brightness);
    ring.show();
    delay(250); // wait for apply brightness
    LOG(F("*** Ajust Brightness (-): "));
  }
  LOG(SETTINGS.brightness); LOG_END(F(" ***"));
}

//********************************************************************************************************************************
// Misc Functions
//********************************************************************************************************************************
// used by randomColorFill
int inArray(uint32_t val, uint32_t arr[]) {
  for (uint8_t i=0; i<(sizeof(arr)/sizeof(uint32_t)); i++) {
    if (arr[i] == val)
      return 1;
  }
  return 0;
}

// used by knightRider
uint32_t dimColor(uint32_t color, uint8_t width) {
   return (((color & COLOR.red)  / width) & COLOR.red) +
          (((color & COLOR.lime) / width) & COLOR.lime) +
          (((color & COLOR.blue) / width) & COLOR.blue);
}

// used by rainbowCycle and rainbowWheel
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return ring.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return ring.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return ring.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

uint32_t getRandomColor() {
  return ring.Color(random(255), random(255), random(255));
}

void clearRing() {
  ring.clear();
  ring.show();
}

void sleep(long ms) {
  unsigned long time = millis();
  while (millis() - time <= ms) {
    onRemote();
    if (EFFECT_CHANGE) return;
    delay(50);
  }
}

//********************************************************************************************************************************
// Examples LEDs Effect Functions in:
// https://github.com/adafruit/Adafruit_NeoPixel/tree/master/examples
//********************************************************************************************************************************
// Fill strip pixels one after another with a color.
void colorWipe(uint32_t color) {
  for (int i=0; i<NUM_PIXELS; i++) { // For each pixel in strip...
    ring.setPixelColor(i, color);    // Set pixel's color (in RAM)
    ring.show();                     // Update strip to match
    delay(SETTINGS.colorW_speed);    // Pause for a moment
  }
}

// Rainbow cycle along whole strip.
void rainbow() {
  unsigned long time = millis();
  for (long firstPixelHue=0; firstPixelHue<(3*65536); (firstPixelHue+=256)) {
    for (int i=0; i<NUM_PIXELS; i++) {
      if (EFFECT_CHANGE) return;
      int pixelHue = firstPixelHue + (i * 65536L / NUM_PIXELS);
      ring.setPixelColor(i, ring.gamma32(ring.ColorHSV(pixelHue)));
    }
    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show(); // Update strip with new contents
    delay(SETTINGS.rainbow_speed); // Pause for a moment
  }
}

// Rainbow cycle along whole strip.
void rainbowReverse() {
  unsigned long time = millis();
  for (long firstPixelHue=0; firstPixelHue<(3*65536); (firstPixelHue+=256)) {
    for (int i=0; i<NUM_PIXELS+1; i++) {
      if (EFFECT_CHANGE) return;
      int pixelHue = firstPixelHue + (i * 65536L / NUM_PIXELS);
      ring.setPixelColor(NUM_PIXELS-i, ring.gamma32(ring.ColorHSV(pixelHue)));
    }
    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show(); // Update strip with new contents
    delay(SETTINGS.rainbow_speed); // Pause for a moment
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  unsigned long time = millis();
  for (uint16_t j=0; j<(256*5); j++) { // 5 cycles of all colors on wheel
    for (int i=0; i<NUM_PIXELS; i++) {
      if (EFFECT_CHANGE) return;
      ring.setPixelColor(i, Wheel(((i * 256 / NUM_PIXELS) + j) & 255));
    }
    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show();
    delay(SETTINGS.rbCycle_speed);
  }
}

void rainbowWheel() {
  unsigned long time = millis();
  for (uint16_t j=0; j<256; j++) {
    for (int i=0; i<NUM_PIXELS; i++) {
      if (EFFECT_CHANGE) return;
      ring.setPixelColor(i, Wheel((i+j) & 255));
    }
    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show();
    delay(SETTINGS.rbWheel_speed);
  }
}

// Theater-marquee-style chasing lights.
void theaterChase() {
  // int step = (SETTINGS.thChase_step <= 0) ? 1 : SETTINGS.thChase_step;
  int step = max(SETTINGS.thChase_step, 1);
  uint32_t color = getRandomColor();
  for (int a=0; a<10; a++) {     // Repeat 10 times...
    for (int b=0; b<step; b++) { // 'b' counts from 0 to 2...
      ring.clear();              // Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of step...
      for (int c=b; c<NUM_PIXELS; c += step) {
        ring.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      onRemote();
      if (EFFECT_CHANGE) return;

      ring.show(); // Update strip with new contents
      delay(SETTINGS.thChase_speed); // Pause for a moment
    }
  }
}

// Rainbow-enhanced theater marquee.
void theaterChaseRainbow() {
  // int step = (SETTINGS.thChaseRB_step <= 0) ? 1 : SETTINGS.thChaseRB_step;
  int step = max(SETTINGS.thChaseRB_step, 1);
  int firstPixelHue = 0;      // First pixel starts at red (hue 0)
  for (int a=0; a<30; a++) {  // Repeat 30 times...
    for (int b=0; b<step; b++) { // 'b' counts from 0 to 2...
      ring.clear();           // Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of step...
      for (int c=b; c<NUM_PIXELS; (c+=step)) {
        int hue = firstPixelHue + (c * 65536L / NUM_PIXELS);
        ring.setPixelColor(c, ring.gamma32(ring.ColorHSV(hue)));
      }
      onRemote();
      if (EFFECT_CHANGE) return;

      ring.show();
      delay(SETTINGS.thChaseRB_speed);
      firstPixelHue += (65536/90); // One cycle of color wheel over 90 frames
    }
  }
}

//********************************************************************************************************************************
// LEDs Effect Functions
//********************************************************************************************************************************
// Me Frost
void snake() {
  int pos = 0;
  for (int size=1; size<NUM_PIXELS+1; size++) { // tour
    for (int i=0; i<NUM_PIXELS; i++) {
      // draw snake
      bool head = false;
      ring.clear();
      for (int j=0; j<size; j++) {
        if (head) {
          ring.setPixelColor((i-j + pos) % NUM_PIXELS, SETTINGS.snake_bodycolor);
        } else {
          ring.setPixelColor((i-j + pos) % NUM_PIXELS, SETTINGS.snake_headcolor);
          head = true;
        }
      }
      ring.show();
      delay(SETTINGS.snake_speed);
    }
    onRemote();
    if (EFFECT_CHANGE) return;

    pos++;
    pos %= NUM_PIXELS;
  }
}

// Me Frost
// Fill 2 random colors: c1 right to left and c2 left to right.
void random2ColorWipe() {
  uint32_t c1 = getRandomColor();
  uint32_t c2 = ~c1; //getRandomColor();
  for (int i=0; i<NUM_PIXELS; i++) {
    ring.setPixelColor(i, c1);
    ring.setPixelColor((NUM_PIXELS-1)-i, c2);
    ring.show();
    delay(SETTINGS.random2Wipe_speed);

    onRemote();
    if (EFFECT_CHANGE) return;
  }
}

// Credit: Frederic Kvandenbosch https://www.element14.com/community/community/arduino/arduino-projects/blog/2014/12/20/christmas-tree-internet-of-holiday-lights--getting-started
// Light up the strip starting from the sides
void sideFill(uint32_t color) {
  clearRing();

  int half = (NUM_PIXELS/2);
  for (int i=0; i<half; i++) { // fill strip from sides to middle
    ring.setPixelColor(i, color);
    ring.setPixelColor(NUM_PIXELS - i, color);
    ring.show();
    delay(SETTINGS.sideFill_speed);
  }
  ring.setPixelColor(half, color);
  ring.show();
  delay(SETTINGS.sideFill_speed);

  if (SETTINGS.sideFill_reverse) {
    for (int i=0; i<half; i++) {
      ring.setPixelColor(half + i, COLOR.black);
      ring.setPixelColor(half - i, COLOR.black);
      ring.show();
      delay(SETTINGS.sideFill_speed);
    }
    ring.setPixelColor(0, COLOR.black);
    ring.show();
    delay(SETTINGS.sideFill_speed);
  }
}

// Credit: Frederic Kvandenbosch
// Gradually fill up the strip with random colors
void randomColorFill() {
  clearRing();

  uint32_t colors[NUM_PIXELS]; // up to 256 lights!
  uint32_t color;              // current random color

  for (int i=0; i<NUM_PIXELS; i++) { // iterate over every LED of the strip
    if (EFFECT_CHANGE) return;
    // generate a random color
    while (!EFFECT_CHANGE) {
      color = getRandomColor();
      if (!inArray(color, colors)) {
        colors[i] = color;
        break;
      } else if (!inArray(~color, colors)) {
        colors[i] = ~color;
        break;
      }
    }
    if (EFFECT_CHANGE) return;

    for (int j=0; j<NUM_PIXELS-i; j++) {    // iterate over every LED of the strip, that hasn't lit up yet
      ring.setPixelColor(j-1, COLOR.black); // turn previous LED off
      ring.setPixelColor(j, color);         // turn current LED on
      ring.show();                          // apply the colors
      delay(SETTINGS.randomFill_speed);

      onRemote();
      if (EFFECT_CHANGE) return;
    }
  }
}

// Credit: Frederic Kvandenbosch
// Alternate between two colors on even and odd LEDs
void altColor(uint32_t c1, uint32_t c2) {
  unsigned long time = millis();
  for (uint8_t i=0; i<SETTINGS.altColor_cycles; i++) {
    for (int i=0; i<NUM_PIXELS; i++) {
      if (i%2 == 0) { // set even LED to color 1
        ring.setPixelColor(i, c1);
      } else { // set odd LED to color 2
        ring.setPixelColor(i, c2);
      }
    }

    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show(); // apply the colors
    delay(SETTINGS.altColor_speed);

    for (int i=0; i<NUM_PIXELS; i++) {
      if(i%2 == 0) { // set even LED to color 2
        ring.setPixelColor(i, c2);
      } else { // set odd LED to color 1
        ring.setPixelColor(i, c1);
      }
    }

    if (millis() - time > 500) {
      delay(50);
      onRemote();
      time = millis();
    }
    if (EFFECT_CHANGE) return;

    ring.show(); // apply the colors
    delay(SETTINGS.altColor_speed);
  }
}

// Credit: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#LEDStripEffectMeteorRain
void meteorRain() {
  uint32_t color = SETTINGS.meteor_color;
  if (color == COLOR.black) {
    color = getRandomColor();
  }
  byte fadeValue = SETTINGS.meteor_trailDecay;
  byte size = max(SETTINGS.meteor_size, 1);
  for (uint8_t i=0; i<(NUM_PIXELS*4); i++) {
    // fade brightness all LEDs one step
    for (int j=0; j<NUM_PIXELS; j++) {
      if ((!SETTINGS.meteor_randomDecay) || (random(10)>5)) {
        uint32_t c = ring.getPixelColor(j);
        uint8_t r = (c & 0x00ff0000UL) >> 16;
        uint8_t g = (c & 0x0000ff00UL) >> 8;
        uint8_t b = (c & 0x000000ffUL);

        ring.setPixelColor(j, (r <= 10) ? 0 : (int)r-(r*fadeValue/256),
                              (g <= 10) ? 0 : (int)g-(g*fadeValue/256),
                              (b <= 10) ? 0 : (int)b-(b*fadeValue/256));
      }
    }

    // draw meteor
    for (int j=0; j<size; j++) {
      if ((i-j<NUM_PIXELS) && (i-j>=0)) {
        ring.setPixelColor(i-j, color);
      }
    }
    onRemote();
    if (EFFECT_CHANGE) return;

    ring.show();
    delay(SETTINGS.meteor_speed);
  }
}

// Credit: https://github.com/technobly/NeoPixel-KnightRider
void knightRider() {
  uint8_t width = max(SETTINGS.kitt_width, 2);
  uint32_t old_val[NUM_PIXELS]; // up to 256 lights!
  for (uint8_t i=0; i<SETTINGS.kitt_cycles; i++) {
    for (int count=1; count<NUM_PIXELS; count++) {
      ring.setPixelColor(count, SETTINGS.kitt_color);
      old_val[count] = SETTINGS.kitt_color;
      for (int x=count; x>0; x--) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        ring.setPixelColor(x-1, old_val[x-1]);
      }
      onRemote();
      if (EFFECT_CHANGE) return;

      ring.show();
      delay(SETTINGS.kitt_speed);
    }
    for (int count=NUM_PIXELS-1; count>=0; count--) {
      ring.setPixelColor(count, SETTINGS.kitt_color);
      old_val[count] = SETTINGS.kitt_color;
      for (int x=count; x<=NUM_PIXELS; x++) {
        old_val[x-1] = dimColor(old_val[x-1], width);
        ring.setPixelColor(x+1, old_val[x+1]);
      }
      onRemote();
      if (EFFECT_CHANGE) return;

      ring.show();
      delay(SETTINGS.kitt_speed);
    }
  }
  uint8_t cur_speed = SETTINGS.colorW_speed;
  SETTINGS.colorW_speed = SETTINGS.kitt_speed;
  colorWipe(SETTINGS.kitt_color);
  SETTINGS.colorW_speed = cur_speed;
}

//********************************************************************************************************************************
// Pac Man Functions Credit: https://github.com/KokoDoko/PacmanLamp
//********************************************************************************************************************************
// check if this position is within the led strip, if yes then show the color
void setPac(uint32_t c, int pos) {
  if (pos < NUM_PIXELS && pos > -1) {
     ring.setPixelColor(pos, c);
  }
  // switch off the pixel before and after this one (depends on direction)
  if (pos+1 < NUM_PIXELS && pos+1 > -1) {
     ring.setPixelColor(pos+1, COLOR.black);
  }
  if (pos-1 < NUM_PIXELS && pos-1 > -1) {
    ring.setPixelColor(pos-1, COLOR.black);
  }
}

void pacman() {
  clearRing();
  // ring.setBrightness(120);

  // white food pixels
  // int step = (SETTINGS.pac_foodstep <= 0) ? 1 : SETTINGS.pac_foodstep;
  int step = max(SETTINGS.pac_foodstep, 1);
  for (int br=0; br<128; br++) {
    for (int i=0; i<NUM_PIXELS; i+=step) {
      ring.setPixelColor(i, br, br, br);
    }
    ring.show();
    delay(5);
  }
  sleep(SETTINGS.pac_wait);
  if (EFFECT_CHANGE) return;

  // pacman eats the food
  for (int i=0; i<NUM_PIXELS+1; i++) {
    ring.setPixelColor(i, COLOR.pacman);
    ring.setPixelColor(i-1, COLOR.black);

    onRemote();
    if (EFFECT_CHANGE) return;

    ring.show();
    delay(SETTINGS.pac_speed);
  }
  sleep(SETTINGS.pac_wait);
  if (EFFECT_CHANGE) return;

  // pacman is chased by ghosts
  int ipill = 0;
  bool pill = false;
  for (int c=0; c<2; c++) {
    pill = (c>0);
    if (pill) {
      for (int br=0; br<128; br++) {
        ring.setPixelColor(0, br, br, br);
        ring.show(); delay(5);
      }
      onRemote();
      if (EFFECT_CHANGE) return;
      ipill = -1;
    }
    for (int i=NUM_PIXELS+ipill; i>-NUM_PIXELS+ipill; i--) {
      // check: color, position, space between pacman and ghost
      if (pill && i < 1) {
        if (i == 0) setPac(COLOR.black, -i + 11);
        setPac(COLOR.ghost,  -i + 9);
        setPac(COLOR.ghost,  -i + 7);
        setPac(COLOR.ghost,  -i + 5);
        setPac(COLOR.ghost,  -i + 3);
        setPac(COLOR.pacman, -i);
      } else {
        setPac(COLOR.pacman, i);
        setPac(COLOR.blinky, i + 5);
        setPac(COLOR.inky,   i + 7);
        setPac(COLOR.clyde,  i + 9);
        setPac(COLOR.pinky,  i + 11);
      }

      onRemote();
      if (EFFECT_CHANGE) return;

      ring.show();
      delay(SETTINGS.pac_speed);
    }
    if (!pill) sleep(SETTINGS.pac_wpill);
    if (EFFECT_CHANGE) return;
  }

  // now pacman is chasing the ghosts
  for (int j=-NUM_PIXELS; j<NUM_PIXELS+1; j++) {
    setPac(COLOR.ghost,  j + 8);
    setPac(COLOR.ghost,  j + 6);
    setPac(COLOR.ghost,  j + 4);
    setPac(COLOR.ghost,  j + 2);
    setPac(COLOR.pacman, j);

    onRemote();
    if (EFFECT_CHANGE) return;

    ring.show();
    delay(SETTINGS.pac_speed);
  }
  sleep(SETTINGS.pac_wait);
  if (EFFECT_CHANGE) return;
}
