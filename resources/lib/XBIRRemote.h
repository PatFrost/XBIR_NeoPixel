
// xKEYs

struct CUSTOM
{
  // Change the decode type to your remote type
  uint32_t type       = 0xFFFFFFFF;

  // Hex keys for custom remote
  uint32_t display    = 0x344961BE; // Used for Xbox Power ON/OFF
  uint32_t reverse    = 0x1235D5B6;
  uint32_t play       = 0x9EC0A8FA;
  uint32_t forward    = 0xE3CEA9AB;
  uint32_t skip_minus = 0xDEA77A82;
  uint32_t stop       = 0xE3047EEF;
  uint32_t pause      = 0x50F22CDB;
  uint32_t skip_plus  = 0xE0051F36;
  uint32_t title      = 0xC340503E;
  uint32_t info       = 0x4FCB2382;

  uint32_t up         = 0xC0D43502;
  uint32_t down       = 0x7305B8E3;
  uint32_t left       = 0x5E235B3;
  uint32_t right      = 0x5180AF7;

  uint32_t select     = 0xBB068A;
  uint32_t menu       = 0x94D92433;
  uint32_t back       = 0x8FB1F50A;

  uint32_t one        = 0x1F1E0BB6;
  uint32_t two        = 0xABA8DEFA;
  uint32_t three      = 0xF0B6DFAB;
  uint32_t four       = 0xEB8FB082;
  uint32_t five       = 0xEFECB4EF;
  uint32_t six        = 0x5DDA62DB;
  uint32_t seven      = 0xECED5536;
  uint32_t eight      = 0xDFD854FA;
  uint32_t nine       = 0xE4355967;
  uint32_t zero       = 0x6CC0265F;
};

struct ORIGINAL
{
  // Change the decode type to your remote type
  uint32_t type       = 0xFFFFFFFF;

  // Hex keys based on xbox original dvd remotes with 27 keys
  // https://www.amazon.com/Xbox-DVD-Movie-Playback-Kit/dp/B00005O0I9
  uint32_t display    = 0x8F3A25F9; // Used for Xbox Power ON/OFF
  uint32_t reverse    = 0xE8572B7A;
  uint32_t play       = 0x5A03C83E;
  uint32_t forward    = 0x89DEE1B9;
  uint32_t skip_minus = 0x94DF0175;
  uint32_t stop       = 0x32A6223E;
  uint32_t pause      = 0xBAD4D9DE;
  uint32_t skip_plus  = 0xC5EF9F9;
  uint32_t title      = 0x7A56B2D9;
  uint32_t info       = 0xBD0B58F5;

  uint32_t up         = 0xEEB21A42;
  uint32_t down       = 0xA003A639;
  uint32_t left       = 0xDAF86EDD;
  uint32_t right      = 0xBFA9B6DE;

  uint32_t select     = 0xCEBCDBD9;
  uint32_t menu       = 0xA11E6F9;
  uint32_t back       = 0x2EF93F1A;

  uint32_t one        = 0x49641316;
  uint32_t two        = 0x1CF35919;
  uint32_t three      = 0xF14FF65A;
  uint32_t four       = 0x3E81CB71;
  uint32_t five       = 0x8D303F7A;
  uint32_t six        = 0x39297135;
  uint32_t seven      = 0xD860E76;
  uint32_t eight      = 0x8F890759;
  uint32_t nine       = 0xEE01511A;
  uint32_t zero       = 0xEAEBC955;
};

struct GENERIC
{
  // Change the decode type to your remote type
  uint32_t type       = 0xFFFFFFFF;

  // Hex keys based on xbox generic dvd remotes with 27 keys
  // https://www.amazon.com/DVD-Remote-Control-Xbox-Buttons-Sensor/dp/B000YQSKZC
  uint32_t display    = 0x344961BE; // Used for Xbox Power ON/OFF
  uint32_t reverse    = 0x1235D5B6;
  uint32_t play       = 0x9EC0A8FA;
  uint32_t forward    = 0xE3CEA9AB;
  uint32_t skip_minus = 0xDEA77A82;
  uint32_t stop       = 0xE3047EEF;
  uint32_t pause      = 0x50F22CDB;
  uint32_t skip_plus  = 0xE0051F36;
  uint32_t title      = 0xC340503E;
  uint32_t info       = 0x4FCB2382;

  uint32_t up         = 0xC0D43502;
  uint32_t down       = 0x7305B8E3;
  uint32_t left       = 0x5E235B3;
  uint32_t right      = 0x5180AF7;

  uint32_t select     = 0xBB068A;
  uint32_t menu       = 0x94D92433;
  uint32_t back       = 0x8FB1F50A;

  uint32_t one        = 0x1F1E0BB6;
  uint32_t two        = 0xABA8DEFA;
  uint32_t three      = 0xF0B6DFAB;
  uint32_t four       = 0xEB8FB082;
  uint32_t five       = 0xEFECB4EF;
  uint32_t six        = 0x5DDA62DB;
  uint32_t seven      = 0xECED5536;
  uint32_t eight      = 0xDFD854FA;
  uint32_t nine       = 0xE4355967;
  uint32_t zero       = 0x6CC0265F;
};
// constexpr xKEY; // used with case, value must be a constant.
