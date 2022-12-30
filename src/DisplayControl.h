#ifndef _DISPLAY_CONTROL_
#define _DISPLAY_CONTROL_

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library

#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

enum AnimationDirection {
  SLIDE_UP,
  SLIDE_DOWN,
  SLIDE_LEFT,
  SLIDE_RIGHT
};

enum FrameState
{
  IN_TRANSITION,
  FIXED
};

enum TextAlignment
{
  TEXT_LEFT,
  TEXT_CENTER,
  TEXT_RIGHT
};

// Structure of the UiState
struct DisplayControlState
{
  uint64_t   lastUpdate                = 0;
  uint16_t   ticksSinceLastStateSwitch = 0;

  FrameState frameState                = FIXED;
  uint8_t    currentFrame              = 0;

  bool       isIndicatorDrawen         = true;

  // Normal = 1, Inverse = -1;
  int8_t     frameTransitionDirection  = 1;

  bool       manuelControll            = false;

  // Custom data that can be used by the user
  void*      userData                  = NULL;
};

typedef void (*FrameCallback)(DisplayControlState* state, int16_t x, int16_t y);
typedef void (*OverlayCallback)(DisplayControlState* state);

class DisplayControl
{
    private:
        uint8_t m_currentLine = 0;
        uint8_t m_currentIndex = 0;
        uint8_t m_maxLines = 0;
        uint8_t m_lineHeight = 10;
        uint8_t m_charWidth = 6;
        uint8_t m_charHeight = 8;
        uint8_t m_updateInterval = 33;
        // Values for the Frames
        AnimationDirection  m_frameAnimationDirection   = SLIDE_RIGHT;
        int8_t              m_lastTransitionDirection   = 1;
        uint16_t            m_ticksPerFrame             = 151; // ~ 5000ms at 30 FPS
        uint16_t            m_ticksPerTransition        = 15;  // ~  500ms at 30 FPS
        bool                m_autoTransition = true;
        FrameCallback*      m_frameFunctions;
        uint8_t             m_frameCount                = 0;
        // Internally used to transition to a specific frame
        int8_t              m_nextFrameNumber           = -1;
        // Values for Overlays
        OverlayCallback*    m_overlayFunctions;
        uint8_t             m_overlayCount              = 0;

        DisplayControlState m_state;

        void drawFrame();
        void drawOverlays();
        void tick();
        void resetState();

    public:

        DisplayControl();
        /// @brief 
        /// @param rotation 0,1,2,3 = (0,90,180,270)
        void init(uint16_t rotation = 0);

        void setRotation(uint16_t rotation);

        void fillScreen(uint16_t color);

        void print(String str, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);

        void printLine();

        void printLine(String str, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);

        void drawString(String str, int16_t x, int16_t y, TextAlignment align = TEXT_LEFT, uint8_t textSize = 1, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false, boolean mode = false);

        void drawBitMap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale);

        void drawProgress(int16_t x, int16_t y, int16_t sx, int16_t sy, int16_t p, String str = "", uint8_t textSize = 1, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK);

        /**
         * Configure what animation is used to transition from one frame to another
         */
        void setFrameAnimation(AnimationDirection dir);
        /**
         * Add frame drawing functions
         */
        void setFrames(FrameCallback* frameFunctions, uint8_t frameCount);
        /**
         * Add overlays drawing functions that are draw independent of the Frames
         */
        void setOverlays(OverlayCallback* overlayFunctions, uint8_t overlayCount);

        DisplayControlState* getUiState();

        int8_t update();

        void windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf);

        void testDisplay();
};

#endif