#include "DisplayControl.h"

DisplayControl::DisplayControl()
{
}

void DisplayControl::init(uint16_t rotation, const GFXfont *gfxFont)
{
    m_displayWrapper->begin();
    m_displayWrapper->setRotation(rotation);
    m_gfxFontDefault = gfxFont;
    setFont(gfxFont);
    m_displayWrapper->fillScreen(BLACK);
    m_displayWrapper->setTextColor(WHITE);
}

uint16_t DisplayControl::colorLerp(uint16_t fg, uint16_t bg, int8_t alpha) 
{
    uint8_t fg_r = (fg >> 8) & 0b11111000;
    uint8_t fg_g = (fg >> 3) & 0b11111100;
    uint8_t fg_b = (fg << 3) & 0b11111000;

    uint8_t bg_r = (bg >> 8) & 0b11111000;
    uint8_t bg_g = (bg >> 3) & 0b11111100;
    uint8_t bg_b = (bg << 3) & 0b11111000;

    uint8_t r = (fg_r * alpha + bg_r * (255-alpha)) / 255;
    uint8_t g = (fg_g * alpha + bg_g * (255-alpha)) / 255;
    uint8_t b = (fg_b * alpha + bg_b * (255-alpha)) / 255;

    //Serial.println(String(r) + ", " + String(g) + ", " + String(b));
            
    return m_displayWrapper->color565(r, g, b);
}

void DisplayControl::setMaxLines()
{
    switch (m_displayWrapper->getRotation())
    {
        case 1:
        case 3:
            m_maxLines = floor(480/m_lineHeight);
            break;
        default:
            m_maxLines = floor(800/m_lineHeight);
            break;
    } 
}

void DisplayControl::setFont(const GFXfont *gfxFont)
{
    m_gfxFont = gfxFont;
    m_displayWrapper->setFont(m_gfxFont);
    m_lineHeight = pgm_read_byte(&m_gfxFont->yAdvance);
    setMaxLines();
}

void DisplayControl::setRotation(uint16_t rotation)
{
    m_displayWrapper->setRotation(rotation);
    setMaxLines();
}

void DisplayControl::fillScreen(uint16_t color)
{
    m_displayWrapper->fillScreen(color);
}

void DisplayControl::drawBitmap(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, bool center, int16_t scale)
{
    if(center)
    {
        x -= sx * scale * 0.5;
        y -= sy * scale * 0.5;
    }

    uint16_t color = BLACK;
    for (int16_t row = 0; row < sy; row++) 
    {
        for (int16_t col = 0; col < sx; col++) 
        {
            color = *(data + (row*sx + col)*1);//pgm_read_word(data + (row*sx + col)*1);
            if (scale > 1)
            {
                m_displayWrapper->fillRect(x+col*scale, y+row*scale, scale, scale, color);
            }
            else
            {
                m_displayWrapper->drawPixel(x + col, y + row, color);
            }
        }
    }
}

void DisplayControl::drawMaskBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t foregroundColor, uint16_t backgroundColor, bool center, int16_t scale)
{
    if(center)
    {
        x -= w * scale * 0.5;
        y -= h * scale * 0.5;
    }

    uint16_t color = BLACK;
    uint8_t alpha = 0;
    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            alpha = (uint8_t)pgm_read_word(&bitmap[j * w + i]);
            // skip pure black, should this be an option?
            if (alpha == 0) continue;
            color = colorLerp(foregroundColor, backgroundColor, alpha);
            if (scale > 1)
            {
                m_displayWrapper->fillRect(x + i * scale, y + j * scale, scale, scale, color);
            }
            else
            {
                m_displayWrapper->drawPixel(x + i, y + j, color);
            }
        }
    }
}

void DisplayControl::drawPaletteBitmap(int16_t x, int16_t y, uint16_t *palette, const unsigned char *palBmp)
{
    //uint8_t version = pgm_read_byte(palBmp);
    uint8_t bmpBitDepth = pgm_read_byte(palBmp + 1);
    if (bmpBitDepth != BITS_PER_PIXEL) {
        //Serial.println("Bmp has wrong bit depth");
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
            m_displayWrapper->drawPixel(x + px, y + py, palette[paletteIndex]);
            bitCounter++;
        }
    //pointer++;
    bitCounter = 0;
    } 
}

void DisplayControl::drawPolygon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color)
{
    m_displayWrapper->drawLine(x0, y0, x1, y1, color);
    m_displayWrapper->drawLine(x1, y1, x2, y2, color);
    m_displayWrapper->drawLine(x2, y2, x3, y3, color);
    m_displayWrapper->drawLine(x3, y3, x0, y0, color);
}

void DisplayControl::fillPolygon(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color)
{
    m_displayWrapper->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    m_displayWrapper->fillTriangle(x0, y0, x3, y3, x2, y2, color);
}

/// @brief Note if rx and ry are the same then an arc of a circle is drawn
/// @param x = coords of centre of arc
/// @param y = coords of centre of arc
/// @param start_angle = 0 - 359
/// @param seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
/// @param rx = x axis radius
/// @param ry = y axis radius
/// @param w = width (thickness) of arc in pixels
/// @param color = 16 bit colour value
void DisplayControl::fillArc(int16_t x, int16_t y, int16_t start_angle, int16_t seg_count, int16_t r, int16_t w, uint16_t color)
{
#ifdef LCDWIKI
    start_angle = 0;
    seg_count = 60;
    byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
    byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

    // Calculate first pair of coordinates for segment start
    float sx = cos((start_angle - 90) * DEG_TO_RAD);
    float sy = sin((start_angle - 90) * DEG_TO_RAD);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Draw color blocks every inc degrees
    for (int i = start_angle; i < start_angle + seg * seg_count; i += inc)
    {
        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * DEG_TO_RAD);
        float sy2 = sin((i + seg - 90) * DEG_TO_RAD);
        int x2 = sx2 * (r - w) + x;
        int y2 = sy2 * (r - w) + y;
        int x3 = sx2 * r + x;
        int y3 = sy2 * r + y;

        m_displayWrapper->fillTriangle(x0, y0, x1, y1, x2, y2, color);
        m_displayWrapper->fillTriangle(x1, y1, x2, y2, x3, y3, color);

        // Copy segment end to sgement start for next segment
        x0 = x2;
        y0 = y2;
        x1 = x3;
        y1 = y3;
    }
#else
    start_angle -= 90;// to match start of LCDWIKI function
    int16_t end_angle = start_angle + (seg_count * 3);
    m_displayWrapper->fillArc(x, y, r, r - w, start_angle, end_angle, color);
#endif
}

void DisplayControl::drawFatLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t wd, uint16_t color)
{
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1; 
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

    if (wd > 2)
    {
        m_displayWrapper->fillCircle(x0, y0, (wd+1)/2, color);
        m_displayWrapper->fillCircle(x1, y1, (wd+1)/2, color);
    }

    for (wd = (wd+1)/2; ; ) {                                   /* pixel loop */
        m_displayWrapper->drawPixel(x0,y0,color);//max(0,255*(abs(err-dx+dy)/ed-wd+1)));
        e2 = err; x2 = x0;
        if (2*e2 >= -dx) {                                           /* x step */
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx) {
                m_displayWrapper->drawPixel(x0, y2 += sy,color);// max(0,255*(abs(e2)/ed-wd+1)));
            }
            if (x0 == x1) {
                break;
            }
            e2 = err; err -= dy; x0 += sx; 
        } 
        if (2*e2 <= dy) {                                            /* y step */
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy) {
                m_displayWrapper->drawPixel(x2 += sx, y0,color);// max(0,255*(abs(e2)/ed-wd+1)));
            }
            if (y0 == y1) {
                break;
            }
            err += dx; y0 += sy; 
        }
    }
}

void DisplayControl::drawFatCircle(int16_t x, int16_t y, int16_t r, int16_t wd, uint16_t color)
{
    int16_t rad = 0;
    for (int16_t w = 0; w < wd; w++)
    {   rad = r-w;
        if (rad <= 0) break;
        m_displayWrapper->drawCircle(x, y, rad, color);
    }
}

void DisplayControl::drawChar(int16_t x, int16_t y, unsigned char c, TextAlignment verticalAlign, uint16_t foregroundColor)
{
    switch (verticalAlign)
    {
        case TEXT_LEFT_TOP:
        case TEXT_CENTER_TOP:
        case TEXT_RIGHT_TOP:
            y += m_lineHeight;
            break;
        case TEXT_LEFT_MIDDLE:
        case TEXT_CENTER_MIDDLE:
        case TEXT_RIGHT_MIDDLE:
            y += m_lineHeight * 0.5;
            break;
        case TEXT_LEFT_BOTTOM:
        case TEXT_CENTER_BOTTOM:
        case TEXT_RIGHT_BOTTOM:
        default:
            break;
    }
    //char str[2];
    //str[0] = c;
    //str[1] = '\0';
    m_displayWrapper->setTextColor(foregroundColor);
    //For gfx_font text draw from bottom left
    m_displayWrapper->setCursor(x, y);
    m_displayWrapper->write(c);
}

void DisplayControl::drawString(String str, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor, uint16_t backgroundColor, boolean invert, boolean mode)
{
    int16_t x1, y1 = 0;
    uint16_t w, h, hIng = 0;
    str.trim();
    // Hack, we want to ignore , and lowercase letters that go below like "y"
    // So we ingore height and use hard coded "0" for full height.
    m_displayWrapper->getTextBounds("0", 0, y, &x1, &y1, &w, &h);
    m_displayWrapper->getTextBounds(str, 0, y, &x1, &y1, &w, &hIng);
    switch (align)
    {
        case TEXT_LEFT_TOP:
            break;
        case TEXT_LEFT_MIDDLE:
            y -= h * 0.5;
            break;
        case TEXT_LEFT_BOTTOM:
            y -= h;
            break;
        case TEXT_CENTER_TOP:
            x -= w * 0.5;
            break;
        case TEXT_CENTER_MIDDLE:
            x -= w * 0.5;
            y -= h * 0.5;
            break;
        case TEXT_CENTER_BOTTOM:
            x -= w * 0.5;
            y -= h;
            break;
        case TEXT_RIGHT_TOP:
            x -= w + 1;
            break;
        case TEXT_RIGHT_MIDDLE:
            x -= w + 1;
            y -= h * 0.5;
            break;
        case TEXT_RIGHT_BOTTOM:
            x -= w + 1;
            y -= h;
            break;
        default:
            break;
    }

    // for gfx_font, they draw from bottom left, so we need to offset y by the height.
    if (invert)
    {
        if (mode)
        {
            m_displayWrapper->fillRect(x, y, x + w, y + h, foregroundColor);
        }
        m_displayWrapper->setTextColor(backgroundColor);
        if (m_gfxFont != nullptr) y += h;
        print(str, x, y);
    }
    else
    {
        if (mode)
        {
            m_displayWrapper->fillRect(x, y, x + w, y + h, backgroundColor);
        }
        m_displayWrapper->setTextColor(foregroundColor);
        if (m_gfxFont != nullptr) y += h;
        print(str, x, y);
    }
}

void DisplayControl::print(const char *st, int16_t x, int16_t y)
{
    m_displayWrapper->setCursor(x, y);
    m_displayWrapper->print(st);
}
void DisplayControl::print(char *st, int16_t x, int16_t y)
{
    m_displayWrapper->setCursor(x, y);
    m_displayWrapper->print(st);
}
void DisplayControl::print(String st, int16_t x, int16_t y)
{
    m_displayWrapper->setCursor(x, y);
    m_displayWrapper->print(st);
}

void DisplayControl::print(String str, uint16_t foregroundColor, uint16_t backgroundColor, boolean invert)
{
    m_gfxFontTemp = m_gfxFont;
    setFont(m_gfxFontDefault);
    int x = m_displayWrapper->getCursorX();
    int y = m_currentLine*m_lineHeight;
    drawString(str, x, y, TEXT_LEFT_TOP, foregroundColor, backgroundColor, invert);
    // check if the text cursor over flowed and by how many lines
    while (m_displayWrapper->getCursorY() < m_currentLine*m_lineHeight - 2)
    {
        m_currentLine++;
    }
    setFont(m_gfxFontTemp);
}

void DisplayControl::printLine()
{
    m_currentLine++;
}

void DisplayControl::printLine(String str, uint16_t foregroundColor, uint16_t backgroundColor, boolean invert)
{
    int16_t y = m_currentLine*m_lineHeight;
    int16_t x = 4;
    if (m_currentLine > m_maxLines)
    {
        //m_displayWrapper->vertScroll(0, m_displayWrapper->height(), -m_lineHeight*(m_currentLine%m_maxLines));
        y = ((m_currentLine%m_maxLines)-1)*m_lineHeight;
    }

    m_displayWrapper->setCursor(x, y);
    print(str, foregroundColor, backgroundColor, invert);
    printLine();
}

void DisplayControl::setProgress(DisplayContolProgress *progress)
{
    m_progress = progress;
}

void DisplayControl::drawProgress(int16_t percent, String message)
{
    int16_t x1, y1 = 0;
    uint16_t w1, h1, hIng = 0;
    // Hack, we want to ignore , and lowercase letters that go below like "y"
    // So we ingore height and use hard coded "0" for full height.
    m_displayWrapper->getTextBounds("0", 0, 0, &x1, &y1, &w1, &h1);
    m_displayWrapper->getTextBounds(message, 0, 0, &x1, &y1, &w1, &hIng);
    setFont(m_progress->gfxFont);
    m_progress->progress = percent;
    m_progress->message = message;
    int16_t strl = m_progress->message.length();
    int16_t minX = x1 - 2 * m_progress->padding;
    int16_t minY = y1 - 2 * m_progress->padding;
    int16_t x = m_progress->x + m_progress->padding;
    int16_t y = m_progress->y + m_progress->padding;
    int16_t sx = m_progress->width - 2 * m_progress->padding;
    int16_t sy = m_progress->height - 2 * m_progress->padding;
    int16_t corner = min(m_progress->corner, min(sx*0.5, sy*0.5));
    
    if (sx < minX) sx = minX;
    if (sy < minY) sy = minY;
    int16_t px = max((sx*(m_progress->progress*0.01))-2, 0);
    int16_t cx = m_progress->x+(m_progress->width*0.5)-(0.5*w1);
    int16_t cy = m_progress->y+(m_progress->height*0.5)-(0.5*h1);

    m_displayWrapper->fillRoundRect(x, y, sx, sy, corner, m_progress->backgroundColor);
    m_displayWrapper->drawRoundRect(x, y, sx, sy, corner, m_progress->foregroundColor);
    m_displayWrapper->fillRoundRect(x+m_progress->border, y+m_progress->border, px, sy-m_progress->border, max(corner-m_progress->border, 0), m_progress->foregroundColor);

    //Serial.println("Pos: " + String(x) + ", " + String(y) + " Size: " + String(sx) + ", " + String(sy) + " Start: " + String(cx) + ", " + String(cy) + ", Corner:" + String(corner));
    for (int16_t i = 0; i < strl; i++)
    {
        if (cx > px)
        {
            drawChar(cx, cy, m_progress->message[i], TEXT_LEFT_MIDDLE, m_progress->foregroundColor);
        }
        else
        {
            drawChar(cx, cy, m_progress->message[i], TEXT_LEFT_MIDDLE, m_progress->backgroundColor);
        }
        cx = m_displayWrapper->getCursorX();
        //Serial.println("Cursor: " + String(cx) + ", " + String(cy) + " Px: " + String(px)); 
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
            //m_displayWrapper->fillScreen(BLACK);
            break;
        }
        case FIXED:
            // draw the frame if it hasn't been drawn.
            if (m_currentFrameNumber != m_state.currentFrame)
            {
                enableIndicator();
                (m_frameFunctions[m_state.currentFrame])(&m_state, 0, 0);
                m_currentFrameNumber = m_state.currentFrame;
                // If we draw a frame, make sure we redraw the overlay
                m_state.ticksSinceLastOverlaySwitch = 0;
            }
        break;
    }
}

void DisplayControl::drawOverlays()
{
    if (m_state.ticksSinceLastOverlaySwitch == 0)
    {
        for (uint8_t i=0; i < m_overlayCount; i++)
        {
            (m_overlayFunctions[i])(&m_state);
        }
    }
}

void DisplayControl::tick()
{
    m_state.ticksSinceLastFrameSwitch++;
    m_state.ticksSinceLastOverlaySwitch++;

    if (m_state.ticksSinceLastOverlaySwitch >= m_ticksPerOverlay)
    {
        m_state.ticksSinceLastOverlaySwitch = 0;
    }

    switch (m_state.frameState)
    {
        case IN_TRANSITION:
            if (m_state.ticksSinceLastFrameSwitch >= m_ticksPerTransition)
            {
                m_state.frameState = FIXED;
                m_state.currentFrame = getNextFrameNumber();
                m_state.ticksSinceLastFrameSwitch = 0;
                m_nextFrameNumber = -1;
            }
            break;
        case FIXED:
            // Revert manuelControl
            if (m_state.manuelControl)
            {
                m_state.frameTransitionDirection = m_lastTransitionDirection;
                m_state.manuelControl = false;
            }
            if (m_state.ticksSinceLastFrameSwitch >= m_ticksPerFrame)
            {
                if (m_autoTransition)
                {
                    m_state.frameState = IN_TRANSITION;
                }
                m_state.ticksSinceLastFrameSwitch = 0;
            }
            break;
    }

    drawFrame();
    drawOverlays();
}

void DisplayControl::resetState()
{
    m_state.lastUpdate = 0;
    m_state.ticksSinceLastFrameSwitch = 0;
    m_state.ticksSinceLastOverlaySwitch = 0;
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

void DisplayControl::navigateForwardFrame()
{
    m_displayWrapper->fillTriangle(775, 200, 750, 150, 750, 250, DARKGRAY);
    delay(250);
    if (m_state.frameState != IN_TRANSITION) {
        m_state.manuelControl = true;
        m_state.frameState = IN_TRANSITION;
        m_state.ticksSinceLastFrameSwitch = 0;
        m_lastTransitionDirection = m_state.frameTransitionDirection;
        m_state.frameTransitionDirection = 1;
    }
}

void DisplayControl::navigateBackwardFrame()
{
    m_displayWrapper->fillTriangle(25, 200, 50, 150, 50, 250, DARKGRAY);
    delay(250);
    if (m_state.frameState != IN_TRANSITION) {
        m_state.manuelControl = true;
        m_state.frameState = IN_TRANSITION;
        m_state.ticksSinceLastFrameSwitch = 0;
        m_lastTransitionDirection = m_state.frameTransitionDirection;
        m_state.frameTransitionDirection = -1;
    }
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
        m_state.ticksSinceLastFrameSwitch += ceil(-timeBudget / m_updateInterval);
    }

    m_state.lastUpdate = frameStart;
    tick();
  }
  return m_updateInterval - (millis() - frameStart);
}

void DisplayControl::testDisplay()
{
    //Sequential display black,white,red,green,blue
    m_displayWrapper->fillScreen(BLACK);  
    m_displayWrapper->fillScreen(WHITE); 
    m_displayWrapper->fillScreen(RED); 
    m_displayWrapper->fillScreen(GREEN);
    m_displayWrapper->fillScreen(BLUE);
}
