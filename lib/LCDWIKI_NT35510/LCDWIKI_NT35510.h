#ifndef _LCDWIKI_NT35510_
#define _LCDWIKI_NT35510_

#include <Adafruit_GFX.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

class LCDWIKI_NT35510 : public Adafruit_GFX
{
    public:
        LCDWIKI_NT35510(int16_t modelID, int16_t RS, int16_t WR, int16_t CS, int16_t RST, int16_t RD = A0) : Adafruit_GFX(480, 800)
        {
            m_modelID = modelID;
            m_lcdwiki_kbv = new LCDWIKI_KBV(modelID, CS, RS, WR, RD, RST); 
        }
        void begin(uint8_t orientation=0) 
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
        void vertScroll(int16_t top, int16_t scrollines, int16_t offset)
        {
            m_lcdwiki_kbv->Vert_Scroll(top, scrollines, offset);
        }
    protected:
        uint16_t m_foregroundColor;
        uint16_t m_backgroundColor;
        uint16_t m_modelID;
        uint8_t m_orientation;
        LCDWIKI_KBV *m_lcdwiki_kbv;
};

#endif