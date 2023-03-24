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
            setRotation(orientation);
        }
        void setRotation(uint8_t orientation)
        {
            m_lcdwiki_kbv->Set_Rotation(m_orientation = orientation);
            Adafruit_GFX::setRotation(orientation);
        }
        void drawPixel(int16_t x, int16_t y, uint16_t color) 
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Pixel(x, y);
        }
        void writePixel(int16_t x, int16_t y, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Pixe(x, y, color);
        }
        void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Fill_Rectangle(x, y, x+w, y+h);
        }
        void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Fast_VLine(x, y, h);
        }
        void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Fast_HLine(x, y, w);
        }
        void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
        {
            m_lcdwiki_kbv->Set_Draw_color(m_foregroundColor = color);
            m_lcdwiki_kbv->Draw_Line(x0, y0, x1, y1);
        }
        void endWrite(void)
        {
            // not implemented
        }
        // Custom code
        uint16_t color565(byte r, byte g, byte b) { return ((r&0xF8) << 8) | ((g&0xFC) << 3) | (b>>3); }
        
        void vertScroll(int16_t top, int16_t scrollines, int16_t offset)
        {
            m_lcdwiki_kbv->Vert_Scroll(top, scrollines, offset);
        }
        void print(const char *st, int16_t x, int16_t y, int16_t deg=0)
        {
            setCursor(x, y);
            Adafruit_GFX::print(st);
        }
        void print(char *st, int16_t x, int16_t y, int16_t deg=0)
        {
            setCursor(x, y);
            Adafruit_GFX::print(st);
        }
        void print(String st, int16_t x, int16_t y, int16_t deg=0)
        {
            setCursor(x, y);
            Adafruit_GFX::print(st);
        }
        void drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale=1)
        {
            m_lcdwiki_kbv->Draw_Bit_Map(x, y, sx, sy, data, scale);
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