#ifndef _DISPLAY_CONTROL_
#define _DISPLAY_CONTROL_

#define SUPPORT_B509_7793

#include <UTFTGLUE.h>
#include <gfxfont.h>

#define BITS_PER_PIXEL 2 // 2^2 =  4 colors
#define BIT_MASK ((1 << BITS_PER_PIXEL) - 1)
#define PIXELS_PER_BYTE (8 / BITS_PER_PIXEL)
#define CUSTOM_BITMAP_DATA_START 6

// Using window system colors and converter
// https://learn.microsoft.com/en-us/dotnet/api/system.windows.media.colors?view=windowsdesktop-7.0&viewFallbackFrom=netcore-3.1
// http://www.rinkydinkelectronics.com/calc_rgb565.php

#define CYAN 0x07FF
#define DEEPSKYBLUE 0x05FF
#define BLACK 0x0000
#define BLUE 0x001F
#define GOLD 0xFEA0
#define GREEN 0x07E0
#define GREENYELLOW 0xAFE5
#define DARKCYAN 0x03EF
#define DARKGREEN 0x03E0
#define DARKGREY 0x7BEF
#define LIGHTGREY 0xC618
#define MAGENTA 0xF81F
#define MAROON 0x7800
#define NAVY 0x000F
#define ORANGE 0xFD20
#define OLIVE 0x7BE0
#define PINK 0xF81F
#define PURPLE 0x780F
#define RED 0xF800
#define WHITE 0xFFFF
#define YELLOW 0xFFE0

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

struct DisplayContolProgress
{
  int16_t x = 0;
  int16_t y = 0;
  int16_t width = 100;
  int16_t height = 30;
  int16_t padding = 0;
  int16_t corner = 2;
  int16_t progress = 0; 
  String message = "";
  uint16_t foregroundColor = WHITE;
  uint16_t backgroundColor = BLACK;
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
        uint8_t m_lineHeight = 10;
        uint8_t m_maxLines = 0;
        const GFXfont *m_gfxFont;
        const GFXfont *m_gfxFontTemp;
        uint8_t m_updateInterval = 33;
        // Values for the Frames
        AnimationDirection  m_frameAnimationDirection   = SLIDE_RIGHT;
        int8_t              m_lastTransitionDirection   = 1;
        uint16_t            m_ticksPerFrame             = 15 * 30; // ~ 5000ms at 30 FPS
        uint16_t            m_ticksPerTransition        = 1;  // ~  100ms at 30 FPS
        bool                m_autoTransition = true;
        FrameCallback*      m_frameFunctions;
        uint8_t             m_frameCount                = 0;
        // Internally used to transition to a specific frame
        int8_t              m_nextFrameNumber           = -1;
        int8_t              m_currentFrameNumber        = -1;
        // Values for Overlays
        OverlayCallback*    m_overlayFunctions;
        uint8_t             m_overlayCount              = 0;

        DisplayControlState m_state;
        DisplayContolProgress m_progress;

        uint8_t getNextFrameNumber();
        void drawFrame();
        void drawOverlays();
        void tick();
        void resetState();

    public:

        DisplayControl();
        /// @brief 
        /// @param rotation 0,1,2,3 = (0,90,180,270)
        void init(uint16_t rotation, const GFXfont *gfxFont);

        MCUFRIEND_kbv* getDisplay();

        void setFont(const GFXfont *font);

        void setRotation(uint16_t rotation);

        void fillScreen(uint16_t color);

        void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t foregroudColor = WHITE);

        void drawString(String str, int16_t x, int16_t y, TextAlignment align = TEXT_LEFT, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false, boolean mode = false);

        void print(String str, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);

        void printLine();

        void printLine(String str, uint16_t foregroudColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);
        
        void drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, bool center = true, int16_t scale = 1);

        void drawPaletteBitmap(int16_t x, int16_t y, uint16_t *palette, const unsigned char *data);

        void setProgress(DisplayContolProgress progress);

        void drawProgress(int16_t progress = 0, String message = "");

        void enableIndicator();
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

        void testDisplay();
};

#endif