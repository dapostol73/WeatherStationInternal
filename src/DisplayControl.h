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

class DisplayControl
{
    private:
        int m_currentLine = 0;
        int m_maxLines = 0;

    public:

        DisplayControl();
        /// @brief 
        /// @param rotation 0,1,2,3 = (0,90,180,270)
        void initialize(int rotation = 0);

        void fillScreen(uint16_t color);

        void printLine(char *str, uint16_t foregroudColor, uint16_t backgroundColor = BLACK, boolean invert = false);

        void printString(char *str, int16_t x, int16_t y, uint8_t csize, uint16_t foregroudColor, uint16_t backgroundColor, boolean mode = false);

        void showMainMenu();

        void testDisplay();

        void windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf);
};

#endif