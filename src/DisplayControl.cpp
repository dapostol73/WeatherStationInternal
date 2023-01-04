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

LCDWIKI_KBV* DisplayControl::getDisplay()
{
    return &m_lcd;
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

void DisplayControl::drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, uint16_t *data, int16_t scale)
{
	int16_t color;
    //int16_t byteWidth = (sx + 3) / 4; // Bitmap scanline pad = whole byte
	m_lcd.Set_Addr_Window(x, y, x + sx*scale - 1, y + sy*scale - 1); 
	if(scale > 1)
	{
		for (int16_t row = 0; row < sy; row++) 
		{
			for (int16_t col = 0; col < sx; col++) 
			{
				color = pgm_read_word(&data[row * sx + col]);
				m_lcd.Fill_Rect(x+col*scale, y+row*scale, scale, scale, color);
			}
		}
	}
	else 
	{
		for (int16_t row = 0; row < sy; row++) 
		{
			for (int16_t col = 0; col < sx; col++) 
			{
                color = pgm_read_word(&data[row * sx + col]);
                Serial.println(color);
				m_lcd.Draw_Pixe(x+col, y+row, color);
			}
		}
	}
}

void DisplayControl::drawPaletteBitmap(int16_t x, int16_t y, uint16_t *palette, const unsigned char *palBmp)
{
    //uint8_t version = pgm_read_byte(palBmp);
    uint8_t bmpBitDepth = pgm_read_byte(palBmp + 1);
    if (bmpBitDepth != BITS_PER_PIXEL) {
        Serial.println("Bmp has wrong bit depth");
        return;
    }
    uint16_t width = pgm_read_byte(palBmp + 2) << 8 | pgm_read_byte(palBmp + 3);
    uint16_t height = pgm_read_byte(palBmp + 4) << 8 | pgm_read_byte(palBmp + 5);

    //int16_t widthRoundedUp = (width + 7) & ~7;
    uint8_t data;
    uint8_t paletteIndex = 0;
    uint32_t pointer = CUSTOM_BITMAP_DATA_START;
    // bitdepth = 8, initialShift = 0
    // bitdepth = 4, initialShift = 4
    // bitdepth = 2, initialShift = 6
    // bitdepth = 1, initialShift = 7
    uint8_t shift = 8 - BITS_PER_PIXEL;
    data = pgm_read_byte(palBmp + pointer);
    uint8_t bitCounter = 0;
    for(uint16_t py = 0; py < height; py++) 
    {
        for(uint16_t px = 0; px < width; px++ ) 
        {
            if (bitCounter == PIXELS_PER_BYTE || bitCounter == 0) 
            {
                //Serial.println("Reading new data");
                data = pgm_read_byte(palBmp + pointer);
                pointer++;
                //shift = bitsPerPixel;
                bitCounter = 0;
            }
            shift = 8 - (bitCounter + 1) * BITS_PER_PIXEL;
            paletteIndex = (data >> shift) & BIT_MASK;

            //Serial.println(String(x) + ", " + String(y) + ": Pointer:" + String(pointer) + ", data:" + String(data) + ", Bit:" + String(bitCounter) + ", Shift:" + String(shift) + ", IDX:" + String(paletteIndex));
            //Serial.println(paletteIndex);
            // if there is a bit draw it

            m_lcd.Set_Draw_color(palette[paletteIndex]);
            m_lcd.Draw_Pixel(x + px, y + py);
            bitCounter++;
        }
    //pointer++;
    bitCounter = 0;
    } 
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

void DisplayControl::enableIndicator()
{
    m_state.isIndicatorDrawen = true;
}

void DisplayControl::drawFrame()
{
    switch (m_state.frameState)
    {
        case IN_TRANSITION: 
        {
            m_lcd.Fill_Screen(BLACK);
            break;
        }
        case FIXED:
            // draw the frame if it hasn't been drawn.
            if (m_currentFrameNumber != m_state.currentFrame)
            {
                enableIndicator();
                (m_frameFunctions[m_state.currentFrame])(&m_state, 0, 0);
                m_currentFrameNumber = m_state.currentFrame;
            }
        break;
    }
}

void DisplayControl::drawOverlays()
{
    for (uint8_t i=0; i<m_overlayCount; i++)
    {
        (m_overlayFunctions[i])(&m_state);
    }
}

void DisplayControl::tick()
{
    m_state.ticksSinceLastStateSwitch++;

    switch (m_state.frameState)
    {
        case IN_TRANSITION:
            if (m_state.ticksSinceLastStateSwitch >= m_ticksPerTransition)
            {
                m_state.frameState = FIXED;
                m_state.currentFrame = getNextFrameNumber();
                m_state.ticksSinceLastStateSwitch = 0;
                m_nextFrameNumber = -1;
            }
            break;
        case FIXED:
            // Revert manuelControll
            if (m_state.manuelControll)
            {
                m_state.frameTransitionDirection = m_lastTransitionDirection;
                m_state.manuelControll = false;
            }
            if (m_state.ticksSinceLastStateSwitch >= m_ticksPerFrame)
            {
                if (m_autoTransition)
                {
                    m_state.frameState = IN_TRANSITION;
                }
                m_state.ticksSinceLastStateSwitch = 0;
            }
        break;
    }

    drawFrame();
    drawOverlays();
}

void DisplayControl::resetState()
{
    m_state.lastUpdate = 0;
    m_state.ticksSinceLastStateSwitch = 0;
    m_state.frameState = FIXED;
    m_state.currentFrame = 0;
    m_state.isIndicatorDrawen = true;
}

uint8_t DisplayControl::getNextFrameNumber()
{
    if (m_nextFrameNumber != -1)
    {
        return m_nextFrameNumber;
    }

    return (m_state.currentFrame + m_frameCount + m_state.frameTransitionDirection) % m_frameCount;
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
    tick();
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
