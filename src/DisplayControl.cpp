#include "DisplayControl.h"

LCDWIKI_KBV m_lcd(ILI9488, 40, 38, 39, 43, 41); //model,cs,cd,wr,rd,reset
LCDWIKI_TOUCH m_touch(53, 52, 50, 51, 44); //tcs,tclk,tdout,tdin,tirq
//LCDWIKI_KBV m_lcd(ST7796S,A3,A2,A1,A0,A4);
//LCDWIKI_KBV m_lcd(ST7796S,40,38,39,44,41);
//LCDWIKI_KBV m_lcd(320,480,A3,A2,A1,A0,A4);
//LCDWIKI_KBV m_lcd(320,480,40,38,39,44,41);

#define COLORBOXSIZE m_lcd.Get_Display_Width()/6
#define PENBOXSIZE m_lcd.Get_Display_Width()/4

DisplayControl::DisplayControl()
{
}

void DisplayControl::initialize(int rotation)
{
    m_lcd.Init_LCD(); //initialize lcd
    m_maxLines = 46;
    m_lcd.Set_Rotation(rotation);
    if (rotation == 1 || rotation == 3)
    {
        m_maxLines = 30;
    }
    m_lcd.Fill_Screen(BLACK);
}

void DisplayControl::fillScreen(uint16_t color)
{
    m_lcd.Fill_Screen(color);
}

void DisplayControl::windowScroll(int16_t x, int16_t y, int16_t wid, int16_t ht, int16_t dx, int16_t dy, uint16_t *buf)
{
    if (dx)
    { 
      for (int16_t row = 0; row < ht; row++) 
      {
            m_lcd.Read_GRAM(x, y + row, buf,wid, 1);
            m_lcd.Set_Addr_Window(x, y + row, x + wid - 1, y + row);
            m_lcd.Push_Any_Color(buf + dx, wid - dx, 1,0);
            m_lcd.Push_Any_Color(buf + 0, dx, 0,0);
        }
    }
    if (dy) 
    {
      for (int16_t col = 0; col < wid; col++) 
      {
            m_lcd.Read_GRAM(x + col, y, buf,1, ht);
            m_lcd.Set_Addr_Window(x + col, y, x + col, y + ht - 1);
            m_lcd.Push_Any_Color(buf + dy, ht - dy, 1,0);
            m_lcd.Push_Any_Color(buf + 0, dy, 0,0);
      }
    }
}

void DisplayControl::printLine(char *str, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert)
{
    if (m_currentLine >= m_maxLines)
    {
        m_currentLine = m_maxLines;
    }

    int y = m_currentLine*10+2;
    int x = 4;
    if (invert)
    {
        printString(str, x, y, 1, backgroundColor, foregroudColor);
    }
    else
    {
        printString(str, x, y, 1, foregroudColor, backgroundColor);
    }
    m_currentLine++;
}

void DisplayControl::printString(char *str, int16_t x, int16_t y, uint8_t csize, uint16_t foregroudColor, uint16_t backgroundColor, boolean mode)
{
    m_lcd.Set_Text_Mode(mode);
    m_lcd.Set_Text_Size(csize);
    m_lcd.Set_Text_colour(foregroudColor);
    m_lcd.Set_Text_Back_colour(backgroundColor);
    m_lcd.Print_String(str,x,y);
}

void DisplayControl::showMainMenu()
{
    m_lcd.Set_Draw_color(YELLOW);
    m_lcd.Fill_Round_Rectangle(5, 0, (m_lcd.Get_Display_Width()-20)/3+5,COLORBOXSIZE/2+20, 5);
    m_lcd.Fill_Round_Rectangle((m_lcd.Get_Display_Width()-20)/3*2+15, 0, (m_lcd.Get_Display_Width()-20)/3*3+15,COLORBOXSIZE/2+20, 5);
    m_lcd.Set_Draw_color(MAGENTA);
    m_lcd.Fill_Round_Rectangle((m_lcd.Get_Display_Width()-20)/3+10, 0, (m_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
    printString("COLOUR",5+((m_lcd.Get_Display_Width()-20)/3-72)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
    printString("CLEAR",(m_lcd.Get_Display_Width()-20)/3+10+((m_lcd.Get_Display_Width()-20)/3-60)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,WHITE, BLACK,1);
    printString("PENSIZE",(m_lcd.Get_Display_Width()-20)/3*2+15+((m_lcd.Get_Display_Width()-20)/3-84)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
}

void DisplayControl::testDisplay()
{
    //Sequential display black,white,red,green,blue
    m_lcd.Fill_Screen(0,0,0);  
    m_lcd.Fill_Screen(255,255,255); 
    m_lcd.Fill_Screen(255,0,0); 
    m_lcd.Fill_Screen(0,255,0);
    m_lcd.Fill_Screen(0,0,255);
}
