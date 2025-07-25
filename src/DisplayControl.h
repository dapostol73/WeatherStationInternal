#ifndef _DISPLAY_CONTROL_H_
#define _DISPLAY_CONTROL_H_

// ILI9488 max TFT width - 480
// ILI9488 max TFT height - 320
// NT35510 max TFT width - 800
// NT35510 max TFT height - 480

//#define DISABLE_COLOR_DEFINES
#include <Arduino_GFX_Library.h>
#include <gfxfont.h>

#define BITS_PER_PIXEL 2 // 2^2 =  4 colors
#define BIT_MASK ((1 << BITS_PER_PIXEL) - 1)
#define PIXELS_PER_BYTE (8 / BITS_PER_PIXEL)
#define CUSTOM_BITMAP_DATA_START 6

// Using window system colors and converter
// https://learn.microsoft.com/en-us/dotnet/api/system.windows.media.colors?view=windowsdesktop-7.0&viewFallbackFrom=netcore-3.1
// https://learn.microsoft.com/en-us/dotnet/media/art-color-table.png?view=windowsdesktop-7.0
// http://www.rinkydinkelectronics.com/calc_rgb565.php

//#define BLACK 0x0000
//#define GREY 0x8410
//#define WHITE 0xFFFF
//#define BLUE 0x001F
//#define GREEN 0x07E0
//#define RED 0xF800
//#define CYAN 0x07FF
//#define YELLOW 0xFFE0
//#define PURPLE 0x780F

enum FrameState
{
	IN_TRANSITION,
	FIXED
};

enum TextAlignment
{
	TEXT_LEFT_TOP,
	TEXT_LEFT_MIDDLE,
	TEXT_LEFT_BOTTOM,
	TEXT_CENTER_TOP,
	TEXT_CENTER_MIDDLE,
	TEXT_CENTER_BOTTOM,
	TEXT_RIGHT_TOP,
	TEXT_RIGHT_MIDDLE,
	TEXT_RIGHT_BOTTOM
};

struct RGBColor
{
	uint8_t R = 0;
	uint8_t G = 0;
	uint8_t B = 0;
};

struct HSVColor
{
	float H = 0;
	float S = 0;
	float V = 0;
};

struct DisplayContolProgress
{
	int16_t x = 0;
	int16_t y = 0;
	int16_t width = 100;
	int16_t height = 30;
	int16_t padding = 0;
	int16_t border = 2;
	int16_t corner = 2;
	int16_t progress = 0; 
	String message = "";
	const GFXfont *gfxFont;
	uint16_t foregroundColor = WHITE;
	uint16_t backgroundColor = BLACK;
};

// Structure of the UiState
struct DisplayControlState
{
	uint64_t   lastUpdate                = 0;
	uint16_t   ticksSinceLastFrameSwitch = 0;
	uint16_t   ticksSinceLastOverlaySwitch = 0;

	FrameState frameState                = FIXED;
	uint8_t    currentFrame              = 0;

	bool       isIndicatorDrawen         = true;

	// Normal = 1, Inverse = -1;
	int8_t     frameTransitionDirection  = 1;

	bool       manuelControl             = false;

	// Custom data that can be used by the user
	void*      userData                  = NULL;
};

typedef void (*FrameCallback)(DisplayControlState* state, int16_t x, int16_t y);
typedef void (*OverlayCallback)(DisplayControlState* state);

class DisplayControl
{
	private:
		uint8_t m_currentLine = 0;
		uint8_t m_lineHeight  = 10;
		uint8_t m_maxLines    = 0;
		// Values for the Frames
		uint8_t          m_updateInterval          = 33;
		int8_t           m_lastTransitionDirection = 1;
		uint16_t         m_ticksPerOverlay         = 10 * 30; // ~ 300ms
		uint16_t         m_ticksPerFrame           = 30 * 30; // ~ 900ms
		uint16_t         m_ticksPerTransition      = 1;  // ~  1ms
		bool             m_autoTransition          = true;
		FrameCallback*   m_frameFunctions;
		uint8_t          m_frameCount              = 0;
		// Internally used to transition to a specific frame
		int8_t           m_nextFrameNumber         = -1;
		int8_t           m_currentFrameNumber      = -1;
		// Values for Overlays
		OverlayCallback* m_overlayFunctions;
		uint8_t          m_overlayCount            = 0;

		DisplayControlState m_state;
		DisplayContolProgress* m_progress;

		void setMaxLines();
		uint8_t getNextFrameNumber();
		void drawFrame();
		void drawOverlays();
		void tick();
		void resetState();
		DisplayControlState* getUiState();

    protected:
		// Arduino_DataBus *bus = new Arduino_SWPAR16(38, 40, 39, 43, 37, 36, 35, 34, 33, 32, 31, 30, 22, 23, 24, 25, 26, 27, 28, 29);
	#ifdef DISPLAY_ILI9488
		Arduino_DataBus *bus = new Arduino_AVRPAR8(38 /* DC */, 40 /* CS */, 39 /* WR */, 43 /* RD */, 3 /* PORT LOW */);
	#else
		Arduino_DataBus *bus = new Arduino_AVRPAR16(38 /* DC */, 40 /* CS */, 39 /* WR */, 43 /* RD */, 3 /* PORT LOW */, 1 /* PORT HIGH */);
	#endif
		const GFXfont *m_gfxFont;
		const GFXfont *m_gfxFontDefault;
		const GFXfont *m_gfxFontTemp;

	public:
	#ifdef DISPLAY_ILI9488
		Arduino_GFX *DisplayGFX = new Arduino_ILI9488(bus, 41);
	#else
		Arduino_GFX *DisplayGFX = new Arduino_NT35510(bus, 41);
	#endif

		DisplayControl();
		/// @brief 
		/// @param rotation 0,1,2,3 = (0,90,180,270)
		void init(uint16_t rotation, const GFXfont *gfxFont);

		uint16_t colorLerp(uint16_t fg, uint16_t bg, int8_t alpha);

		uint16_t color565(RGBColor color);

		uint16_t color565(HSVColor color);

		RGBColor colorRGB(uint16_t color);

		HSVColor colorHSV(uint16_t color);

		void setFont(const GFXfont *font);

		void setRotation(uint16_t rotation);

		void fillScreen(uint16_t color);

		void drawChar(int16_t x, int16_t y, char c, TextAlignment verticalAlign = TEXT_LEFT_TOP, uint16_t foregroundColor = WHITE);

		void drawString(String str, int16_t x, int16_t y, TextAlignment align = TEXT_LEFT_TOP, uint16_t foregroundColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false, boolean mode = false);
		
		void print(const char *st, int16_t x, int16_t y);
		
		void print(char *st, int16_t x, int16_t y);
		
		void print(String st, int16_t x, int16_t y);
		
		void print(String str, uint16_t foregroundColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);

		void printLine();

		void printLine(String str, uint16_t foregroundColor = WHITE, uint16_t backgroundColor = BLACK, boolean invert = false);

		void drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, bool center = true, int16_t scale = 1);

		void drawMaskBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint8_t *data, uint16_t foregroundColor = WHITE, uint16_t backgroundColor = BLACK, bool center = true, int16_t scale = 1);

		void drawPaletteBitmap(int16_t x, int16_t y, uint16_t *palette, const unsigned char *data);

		void drawPolygon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);

		void fillPolygon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);

		void fillArc(int16_t x, int16_t y, int16_t start_angle, int16_t seg_count, int16_t r, int16_t w, uint16_t color);

		void drawFatLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t wd, uint16_t color);

		void drawFatCircle(int16_t x, int16_t y, int16_t r, int16_t wd, uint16_t color);

		void setProgress(DisplayContolProgress *progress);

		void drawProgress(int8_t progress = 0, String message = "");

		void enableIndicator();

		void setFrames(FrameCallback* frameFunctions, uint8_t frameCount);

		void setOverlays(OverlayCallback* overlayFunctions, uint8_t overlayCount);

		void navigateForwardFrame();

		void navigateBackwardFrame();

		int8_t update();

		void testDisplay();
};

#endif