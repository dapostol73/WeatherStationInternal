/*
 * DisplayWrapper.h   @@@@@@@@@@@@@@@@@@@@@@@@ LIBRARY @@@@@@@@@@@@@@@@@@@@@@@@@
 *
 * Created: 12/05/2019 14:25:06
 *  Author: David Prentice
 *  
 *  1. replace "UTFT.h" with "UTFTGLUE.h"
 *  2. replace UTFT constructor with UTFTGLUE(model_ID, 0,0,0,0,0)
 *  3. remove smallFont, BigFont, SeveSegNumFont declarations
 *  4. UTFTGLUE uses FreeFonts or NULL (System 5x7)
 *
 *  5. knows FreeSmallFont, FreeBigFont and FreeSevenSegNumFont
 *  6. any other Fonts just need to #include "FreeOtherXXX.h"
 *  7. #define OtherXXX &FreeOtherXXX
 */ 

#warning   @@@@@@@@@@@@@@@@@@@@@@@@@ <DisplayWrapper.h> @@@@@@@@@@@@@@@@@@@@@@@@@@

#ifndef _Display_Wrapper_
#define _Display_Wrapper_

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <Adafruit_GFX.h>

class DisplayWrapper : public Adafruit_GFX
{
    public:
        DisplayWrapper(int16_t modelID, int16_t RS, int16_t WR, int16_t CS, int16_t RST, int16_t RD = A0) : Adafruit_GFX(480, 800)
        {
            m_modelID = modelID;
            m_lcdwiki_kbv = new LCDWIKI_KBV(modelID, CS, RS, WR, RD, RST); 
        }
        void InitLCD(uint8_t orientation=0) 
        {
            m_lcdwiki_kbv->reset();
            //uint16_t ID = m_lcdwiki_kbv->Read_ID();
            // if (ID == 0) ID = 0x9341;        //DealExtreme with EXTC=0
            // if (ID == 0x8989) ID = 0x1289;
            // if (ID == 0xD3D3) ID = 0x9481;   //write-only controller
            // if (ID == 0xD3D3) ID = 0x9486;   //write-only controller
            // if (ID == 0xD3D3) ID = 0x9090;   //write-only controller HX8357-D
            // if (ID == 0x9327 && orientation == LANDSCAPE) orientation = 3;
            m_lcdwiki_kbv->Init_LCD();
            m_lcdwiki_kbv->Set_Rotation(m_orientation = orientation);
        }
        void setRotation(uint16_t orientation) { m_lcdwiki_kbv->Set_Rotation(m_orientation = orientation); }
        uint16_t getDisplayXSize() { return m_lcdwiki_kbv->Get_Width(); }
        uint16_t getDisplayYSize() { return m_lcdwiki_kbv->Get_Height(); }
        uint16_t color565(byte r, byte g, byte b) { return ((r&0xF8) << 8) | ((g&0xFC) << 3) | (b>>3); }
        void vertScroll(int16_t top, int16_t scrollines, int16_t offset)
        {
            m_lcdwiki_kbv->Vert_Scroll(top, scrollines, offset);
        }
        void clearScreen() { m_lcdwiki_kbv->Fill_Screen(0x0000); }
        void fillScreen(uint16_t color) { m_lcdwiki_kbv->Fill_Screen(color);}
        void fillScreen(byte r, byte g, byte b) { m_lcdwiki_kbv->Fill_Screen(color565(r, g, b));}
        void writePixel(int16_t x, int16_t y, uint16_t color)
        {
            m_foregroundColor = color;
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Pixe(x, y, color);
        }
        void drawPixel(int16_t x, int16_t y)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Pixel(x, y);
        }
        void drawPixel(int16_t x, int16_t y, uint16_t color) 
        {
            m_foregroundColor = color;
            drawPixel(x, y);
        }
        void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Line(x1, y1, x2, y2);
        }
        void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Fast_HLine(x, y, w);
        }
        void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Fast_VLine(x, y, h);
        }
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h) 
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Rectangle(x, y, x+w, y+h);
        }
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
        {
            m_foregroundColor = color;
            drawRect(x, y, w, h);
        }
        void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius) 
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Round_Rectangle(x, y, x+w, y+h, radius);
        }
        void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color) 
        {
            m_foregroundColor = color;
            drawRoundRect(x, y, w, h, radius);
        }
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Fill_Rectangle(x, y, x+w, y+h);
        }
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
        {
            m_foregroundColor = color;
            fillRect(x, y, w, h);
        }
        void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Fill_Round_Rectangle(x, y, x+w, y+h, radius);
        }
        void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, uint16_t color) 
        {
            m_foregroundColor = color;
            fillRoundRect(x, y, w, h, radius);
        }
        void drawCircle(int16_t x, int16_t y, int16_t radius)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Circle(x, y, radius);
        }
        void drawCircle(int16_t x, int16_t y, int16_t radius, uint16_t color)
        {
            m_foregroundColor = color;
            drawCircle(x, y, radius);
        }
        void fillCircle(int16_t x, int16_t y, int16_t radius)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Fill_Circle(x, y, radius);
        }
        void fillCircle(int16_t x, int16_t y, int16_t radius, uint16_t color)
        {
            m_foregroundColor = color;
            fillCircle(x, y, radius);
        }
        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Draw_Triangle(x0, y0, x1, y1,x2, y2);
        }
        void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color)
        {
            m_foregroundColor = color;
            drawTriangle(x0, y0, x1, y1, x2, y2);
        }
        void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
            m_lcdwiki_kbv->Fill_Triangle(x0, y0, x1, y1,x2, y2);
        }
        void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color)
        {
            m_foregroundColor = color;
            fillTriangle(x0, y0, x1, y1,x2, y2);
        }
        void setTextColor(uint16_t color) { m_lcdwiki_kbv->Set_Text_colour(m_foregroundColor = color); }
        void setColor(byte r, byte g, byte b)  { setColor(color565(r, g, b)); }
        void setColor(uint16_t c)
        {
            m_foregroundColor = c;
            m_lcdwiki_kbv->Set_Text_colour(m_foregroundColor);
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor);
        }
        void setBackColor(byte r, byte g, byte b)  { setBackColor(color565(r, g, b)); }
        void setBackColor(uint32_t c)
        { 
            if (c == 0xFFFFFFFF) m_backgroundColor = m_foregroundColor;
            else m_backgroundColor = c;
            m_lcdwiki_kbv->Set_Text_Back_colour(m_backgroundColor);
        }
        void print(const char *st, int16_t x, int16_t y, int16_t deg=0) { m_lcdwiki_kbv->Print_String(st, x, y); }
        void print(char *st, int16_t x, int16_t y, int16_t deg=0) { m_lcdwiki_kbv->Print_String(st, x, y); }
        void print(String st, int16_t x, int16_t y, int16_t deg=0) { m_lcdwiki_kbv->Print_String(st, x, y); }
        void printNumI(long num, int16_t x, int16_t y, int16_t length=0, char filler=' ')
        {
            m_lcdwiki_kbv->Print_Number_Int(num, x, y, length, filler, 0);
        }
        void printNumF(double num, byte dec, int16_t x, int16_t y, char divider='.', int16_t length=0, char filler=' ')
        {
            m_lcdwiki_kbv->Print_Number_Float(num, dec, x, y, divider, length, filler);
        }
        void setTextSize(int16_t sz) { m_lcdwiki_kbv->Set_Text_Size(m_gfxFont == NULL ? sz : 1); } // ####### GFX ########
        void setFont(const GFXfont* font) 
        {
            if (font) {          // Font struct pointer passed in?
                if (!m_gfxFont) { // And no current font struct?
                // Switching from classic to new font behavior.
                // Move cursor pos down 6 pixels so it's on baseline.
                //cursor_y += 6;
                }
            } else if (m_gfxFont) { // NULL passed.  Current font struct defined?
                // Switching from new to classic font behavior.
                // Move cursor pos up 6 pixels so it's at top-left of char.
                //cursor_y -= 6;
            }
            m_gfxFont = (GFXfont *)font;
        }
        void drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale=1)
        {
            m_lcdwiki_kbv->Draw_Bit_Map(x, y, sx, sy, data, scale);
        }
        void setCursor(int16_t x, int16_t y) { m_lcdwiki_kbv->Set_Text_Cousur(x, y); }
        uint16_t getCursorX() { return m_lcdwiki_kbv->Get_Text_X_Cousur(); }
        uint16_t getCursorY() { return m_lcdwiki_kbv->Get_Text_Y_Cousur(); }
        void getTextBounds(const char *str, int16_t x, int16_t y,
                                int16_t *x1, int16_t *y1,
                                uint16_t *w, uint16_t *h) {};
        void getTextBounds(const String &str, int16_t x, int16_t y,
                                int16_t *x1, int16_t *y1,
                                uint16_t *w, uint16_t *h) 
        {
            if (str.length() != 0) getTextBounds(const_cast<char *>(str.c_str()), x, y, x1, y1, w, h);
        }

    protected:
        uint16_t m_foregroundColor;
        uint16_t m_backgroundColor;
        uint16_t m_modelID;
        uint8_t m_orientation;
        const GFXfont *m_gfxFont;
        LCDWIKI_KBV *m_lcdwiki_kbv;
};

#endif /* _Display_Wrapper_ */