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

void DisplayControl::init(uint16_t rotation)
{
    m_lcd.Init_LCD(); //initialize lcd
    m_lcd.Fill_Screen(BLACK);
    setRotation(rotation);
}

void DisplayControl::setRotation(uint16_t rotation)
{
    m_lcd.Set_Rotation(rotation);
    m_maxLines = 48;
    if (rotation == 1 || rotation == 3)
    {
        m_maxLines = 32;
    }
}

void DisplayControl::fillScreen(uint16_t color)
{
    m_lcd.Fill_Screen(color);
}

void DisplayControl::drawBitMap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale)
{
    m_lcd.Draw_Bit_Map(x, y, sx, sy, data, scale);
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

void DisplayControl::print(String str, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert)
{
    int charLength = str.length();
    int x = 4+(m_currentIndex*m_charWidth);
    if(x+(charLength*m_charWidth) > m_lcd.Get_Display_Width())
    {
        m_currentIndex = 0;
        x = 4;
        printLine();
    }

    int y = m_currentLine*m_lineHeight;
    drawString(str, x, y, TEXT_LEFT, 1, foregroudColor, backgroundColor, invert);
    // hacky way of doing it...but m_lcd.Get_Text_Y_Cousur 
    // doesn't return the end of the string after a print
    m_currentIndex += charLength;
}

void DisplayControl::printLine()
{
    m_currentLine++;
}

void DisplayControl::printLine(String str, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert)
{
    int y = m_currentLine*m_lineHeight;
    int x = 4;
    if (m_currentLine > m_maxLines)
    {
        m_lcd.Vert_Scroll(0, m_lcd.Get_Display_Height(), -m_lineHeight*(m_currentLine%m_maxLines));
        y = ((m_currentLine%m_maxLines)-1)*m_lineHeight;
    }

    drawString(str, x, y, TEXT_LEFT, 1, foregroudColor, backgroundColor, invert);
    m_currentLine++;
}

void DisplayControl::drawString(String str, int16_t x, int16_t y, TextAlignment align, uint8_t textSize, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert, boolean mode)
{
    m_lcd.Set_Text_Mode(mode);
    m_lcd.Set_Text_Size(textSize);
    if (invert)
    {
        m_lcd.Set_Text_colour(backgroundColor);
        m_lcd.Set_Text_Back_colour(foregroudColor);
    }
    else
    {
        m_lcd.Set_Text_colour(foregroudColor);
        m_lcd.Set_Text_Back_colour(backgroundColor);
    }

    
    if (align == TEXT_CENTER)
    {
        x -= (str.length() * textSize * m_charWidth * 0.5);
        y -= textSize * m_charHeight * 0.5;
    }
    else if (align == TEXT_RIGHT)
    {
        x -= (str.length() * textSize * m_charWidth);
        //y += textSize * m_charHeight;
    }
    
    m_lcd.Print_String(str, x, y);
}

void DisplayControl::drawProgress(int16_t x, int16_t y, int16_t sx, int16_t sy, int16_t p, String str, uint8_t textSize, uint16_t foregroudColor, uint16_t backgroundColor)
{
    int16_t strl = str.length();
    int16_t minX = 6 + strl * textSize * m_charWidth;
    int16_t minY = 6 + textSize * m_charHeight;
    if (sx < minX) sx = minX;
    if (sy < minY) sy = minY;
    int16_t px = x+(sx*(p*0.01))-2;
    int16_t strx = x+(sx*0.5)-(strl*0.5*m_charWidth*textSize);
    int16_t stry = y+(sy*0.5)-(0.5*m_charHeight*textSize);

    m_lcd.Set_Draw_color(backgroundColor);
    m_lcd.Fill_Round_Rectangle(x, y, x+sx, y+sy, 2);
    m_lcd.Set_Draw_color(foregroudColor);
    m_lcd.Draw_Round_Rectangle(x, y, x+sx, y+sy, 2);
    m_lcd.Fill_Round_Rectangle(x+2, y+2, px, y+sy-2, 2);

    for (int i = 0; i < strl; i++)
    {
        if (strx > px)
        {
            m_lcd.Draw_Char(strx, stry, str[i], foregroudColor, backgroundColor, textSize, true);
        }
        else
        {
            m_lcd.Draw_Char(strx, stry, str[i], backgroundColor, foregroudColor, textSize, true);
        }
        strx += m_charWidth * textSize;
    }
}

void DisplayControl::drawFrame()
{

}

void DisplayControl::drawOverlays()
{

}

void DisplayControl::tick()
{

}

void DisplayControl::resetState()
{
    m_state.lastUpdate = 0;
    m_state.ticksSinceLastStateSwitch = 0;
    m_state.frameState = FIXED;
    m_state.currentFrame = 0;
    m_state.isIndicatorDrawen = true;
}

// -/----- Frame settings -----\-
void DisplayControl::setFrameAnimation(AnimationDirection dir)
{
    m_frameAnimationDirection = dir;
}

void DisplayControl::setFrames(FrameCallback* frameFunctions, uint8_t frameCount)
{
    m_frameFunctions = frameFunctions;
    m_frameCount     = frameCount;
    resetState();
}

//m_-- Overlays ------\-
void DisplayControl::setOverlays(OverlayCallback* overlayFunctions, uint8_t overlayCount)
{
    m_overlayFunctions = overlayFunctions;
    m_overlayCount     = overlayCount;
}

DisplayControlState* DisplayControl::getUiState()
{
    return &m_state;
}

int8_t DisplayControl::update()
{
  unsigned long frameStart = millis();
  int8_t timeBudget = m_updateInterval - (frameStart - m_state.lastUpdate);
  if ( timeBudget <= 0) 
  {
    // Implement frame skipping to ensure time budget is keept
    if (m_autoTransition && m_state.lastUpdate != 0) 
    {
        m_state.ticksSinceLastStateSwitch += ceil(-timeBudget / m_updateInterval);
    }

    m_state.lastUpdate = frameStart;
    this->tick();
  }
  return m_updateInterval - (millis() - frameStart);
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

/*
void DisplayControl::showMainMenu()
{
    m_lcd.Set_Draw_color(YELLOW);
    m_lcd.Fill_Round_Rectangle(5, 0, (m_lcd.Get_Display_Width()-20)/3+5,COLORBOXSIZE/2+20, 5);
    m_lcd.Fill_Round_Rectangle((m_lcd.Get_Display_Width()-20)/3*2+15, 0, (m_lcd.Get_Display_Width()-20)/3*3+15,COLORBOXSIZE/2+20, 5);
    m_lcd.Set_Draw_color(MAGENTA);
    m_lcd.Fill_Round_Rectangle((m_lcd.Get_Display_Width()-20)/3+10, 0, (m_lcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
    drawString("COLOUR",5+((m_lcd.Get_Display_Width()-20)/3-72)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
    drawString("CLEAR",(m_lcd.Get_Display_Width()-20)/3+10+((m_lcd.Get_Display_Width()-20)/3-60)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,WHITE, BLACK,1);
    drawString("PENSIZE",(m_lcd.Get_Display_Width()-20)/3*2+15+((m_lcd.Get_Display_Width()-20)/3-84)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
}
*/