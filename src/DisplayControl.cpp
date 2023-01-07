#include "DisplayControl.h"

//MCUFRIEND_kbv m_lcd;
UTFTGLUE m_lcd(0x7796,38,39,40,41,43);
//LCDWIKI_KBV m_lcd(ILI9488, 40, 38, 39, 43, 41); //model,cs,cd,wr,rd,reset
//LCDWIKI_TOUCH m_touch(53, 52, 50, 51, 44); //tcs,tclk,tdout,tdin,tirq
//LCDWIKI_KBV m_lcd(ST7796S,A3,A2,A1,A0,A4);
//LCDWIKI_KBV m_lcd(ST7796S,40,38,39,44,41);
//LCDWIKI_KBV m_lcd(320,480,A3,A2,A1,A0,A4);
//LCDWIKI_KBV m_lcd(320,480,40,38,39,44,41);

#define COLORBOXSIZE m_lcd.Get_Display_Width()/6
#define PENBOXSIZE m_lcd.Get_Display_Width()/4

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
#ifdef __AVR__
  return &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[c]);
#else
  // expression in __AVR__ section may generate "dereferencing type-punned
  // pointer will break strict-aliasing rules" warning In fact, on other
  // platforms (such as STM32) there is no need to do this pointer magic as
  // program memory may be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
#endif //__AVR__
}

DisplayControl::DisplayControl()
{
}

void DisplayControl::init(uint16_t rotation)
{
    m_lcd.begin(0x7796); //initialize lcd
    m_lcd.fillScreen(BLACK);
    setRotation(rotation);
}

MCUFRIEND_kbv* DisplayControl::getDisplay()
{
    return &m_lcd;
}

void DisplayControl::setFont(const GFXfont *font) {
  m_gfxFont = (GFXfont *)font;
  m_lcd.setFont(m_gfxFont);
  m_charHeight = pgm_read_byte(&m_gfxFont->yAdvance);
  m_lineHeight = m_charHeight;
  m_charWidth = m_charHeight * 0.5 + 1;// hack for now
}

void DisplayControl::setRotation(uint16_t rotation)
{
    m_lcd.setRotation(rotation);
    m_maxLines = 48;
    if (rotation == 1 || rotation == 3)
    {
        m_maxLines = 32;
    }
}

void DisplayControl::fillScreen(uint16_t color)
{
    m_lcd.fillScreen(color);
}

void DisplayControl::drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, uint16_t *data, int16_t scale)
{
	int16_t color;
    //int16_t byteWidth = (sx + 3) / 4; // Bitmap scanline pad = whole byte
	m_lcd.setAddrWindow(x, y, x + sx*scale - 1, y + sy*scale - 1); 
	if(scale > 1)
	{
		for (int16_t row = 0; row < sy; row++) 
		{
			for (int16_t col = 0; col < sx; col++) 
			{
				color = pgm_read_word(&data[row * sx + col]);
                m_lcd.setColor(color);
				m_lcd.fillRect(x+col*scale, y+row*scale, scale, scale);
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
                m_lcd.setColor(color);
				m_lcd.drawPixel(x+col, y+row);
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
            m_lcd.setColor(palette[paletteIndex]);
            m_lcd.drawPixel(x + px, y + py);
            bitCounter++;
        }
    //pointer++;
    bitCounter = 0;
    } 
}

Position DisplayControl::drawChar(int16_t x, int16_t y, unsigned char c, uint8_t size, uint16_t foregroundColor, uint16_t backgroundColor, boolean mode)
{	
    Position pos;
    pos.x = x;
    pos.y = y;
    unsigned char tc = c - (uint8_t)pgm_read_byte(&m_gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(m_gfxFont, tc);
    uint8_t w = pgm_read_byte(&glyph->width), 
            h = pgm_read_byte(&glyph->height),
            xa = pgm_read_byte(&glyph->xAdvance);

    if((x >= m_lcd.width()) || (y >= m_lcd.height()) || ((x + w * size - 1) < 0) || ((y + h * size - 1) < 0))
	{
    	return pos;
	}

    m_lcd.drawChar(x, y, c, foregroundColor, backgroundColor, size);
    
    pos.x += (xa + 1) * size;
    return pos;
}

void DisplayControl::print(String str, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert)
{
    int charLength = str.length();
    int x = 4+(m_currentIndex*m_charWidth);
    if(x+(charLength*m_charWidth) > m_lcd.width())
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
        m_lcd.vertScroll(0, m_lcd.height(), -m_lineHeight*(m_currentLine%m_maxLines));
        y = ((m_currentLine%m_maxLines)-1)*m_lineHeight;
    }

    drawString(str, x, y, TEXT_LEFT, 1, foregroudColor, backgroundColor, invert);
    m_currentLine++;
}

void DisplayControl::printString(String str, int16_t x, int16_t y, uint8_t textSize, uint16_t foregroudColor, uint16_t backgroundColor)
{
	Position pos;
    pos.x = x;
    pos.y = y;
    for (uint16_t i=0; i < str.length(); i++)
    {
        Position npos = drawChar(pos.x, pos.y, str[i], textSize, foregroudColor, backgroundColor);
        pos.x = npos.x;
        pos.y = npos.y;
    }
}

void DisplayControl::drawString(String str, int16_t x, int16_t y, TextAlignment align, uint8_t textSize, uint16_t foregroudColor, uint16_t backgroundColor, boolean invert, boolean mode)
{    
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

    printString(str, x, y, textSize, foregroudColor, backgroundColor);
}

void DisplayControl::setProgress(DisplayContolProgress progress)
{
    m_progress = progress;
}

void DisplayControl::drawProgress(int16_t percent, String message)
{
    m_progress.progress = percent;
    m_progress.message = message;
    int16_t strl = m_progress.message.length();
    int16_t minX = 6 + strl * m_progress.textSize * m_charWidth;
    int16_t minY = 6 + m_progress.textSize * m_charHeight;
    int16_t x = m_progress.x + m_progress.padding;
    int16_t y = m_progress.y + m_progress.padding;
    int16_t sx = x + m_progress.width - 2*m_progress.padding;
    int16_t sy = y + m_progress.height - 2*m_progress.padding;
    int16_t corner = min(m_progress.corner, min(sx*0.5, sy*0.5));
    
    if (sx < minX) sx = minX;
    if (sy < minY) sy = minY;
    int16_t px = max((sx*(m_progress.progress*0.01))-2, 0);
    Position pos;
    pos.x = (x+sx)*0.5-(strl*0.5*m_charWidth*m_progress.textSize);
    pos.y = (y+sy)*0.5-(0.5*m_charHeight*m_progress.textSize);

    m_lcd.setColor(m_progress.backgroundColor);
    m_lcd.fillRoundRect(x, y, sx, sy, corner);
    m_lcd.setColor(m_progress.foregroundColor);
    m_lcd.drawRoundRect(x, y, sx, sy, corner);
    m_lcd.fillRoundRect(x+2, y+2, px, sy-2, max(corner-2, 0));

    //Serial.println(String(x) + ", " + String(y) + ": Size:" + String(sx) + ", " + String(sy) + ", String:" + String(strx) + ", " + String(stry) + ", Corner:" + String(corner));
    for (int i = 0; i < strl; i++)
    {
        if (pos.x > px)
        {
            pos = drawChar(pos.x, pos.y, m_progress.message[i], m_progress.foregroundColor, m_progress.backgroundColor, m_progress.textSize, true);
        }
        else
        {
            pos = drawChar(pos.x, pos.y, m_progress.message[i], m_progress.backgroundColor, m_progress.foregroundColor, m_progress.textSize, true);
        }
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
            m_lcd.fillScreen(BLACK);
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
    m_lcd.fillScreen(BLACK);  
    m_lcd.fillScreen(WHITE); 
    m_lcd.fillScreen(RED); 
    m_lcd.fillScreen(GREEN);
    m_lcd.fillScreen(BLUE);
}
