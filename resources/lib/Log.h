
// In debug mode the hex codes of buttons pressed on your remote
// will be printed to the serial console
// Uncomment below to enable debug output.
// #define DEBUG_MODE

// These defines setup log output if enabled above (otherwise it
// turns into no-ops that compile out). Status message will be sent to PC at 9600 baud.
// And Wait for serial to be connected. Required on Leonardo with integrated USB.
#ifdef DEBUG_MODE
  char effectsname[16][24] = { "Knight Rider", "Meteor Rain", "Random 2 Color Wipe",
    "Alternate Color", "Theater Chase", "Random Color Fill", "Snake", "", "", "",
    "Theater Chase Rainbow", "Rainbow Cycle", "Rainbow", "Rainbow Wheel",
    "Side Fill", "Pac Man" };

  #define LOG_BEGIN(...) { Serial.begin(9600); while (!Serial); \
                           Serial.println(F("Start " __DATE__ ", " __TIME__)); \
                           Serial.println(F("Beginning Debug Mode...")); }
  #define LOG_SEP(...)     Serial.println(F("--------------------------------------------------"))
  #define LOG_END(...)     Serial.println(__VA_ARGS__)
  #define LOG(...)         Serial.print(__VA_ARGS__)

  #define LOG_KEY(...) { LOG(F("Key pressed: 0x")); LOG_END(__VA_ARGS__, HEX); }
  #define LOG_COLOR(...) LOG_END(F("*** Starting: All leds " __VA_ARGS__ " (Fill strip pixels one after another) ***"))

  #define LOG_EFFECT(...) { LOG(F("*** Starting animation: ")); \
                            LOG(effectsname[__VA_ARGS__]); \
                            LOG_END(F(" ***")); }
#else
  #define LOG_BEGIN(...)
  #define LOG_SEP(...)
  #define LOG_END(...)
  #define LOG(...)
  #define LOG_KEY(...)
  #define LOG_COLOR(...)
  #define LOG_EFFECT(...)
#endif

void logRemote(decode_results * results) {
  // print the HEX code on the serial monitor
  LOG_SEP();
  LOG(F("Remote Type:  0x")); LOG_END(results->decode_type, HEX);
  LOG(F("Key Code: 0x")); LOG_END(results->value, HEX);
  // if (results->decode_type == 0xFFFF) { LOG(F("Type: UNKNOWN or Xbox")); }
}
