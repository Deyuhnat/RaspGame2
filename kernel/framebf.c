// ----------------------------------- framebf.c -------------------------------------
#include "mbox.h"
#include "../uart/uart1.h"
#include "font.h"
// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32
// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

#define SCR_WIDTH 1024
#define SCR_HEIGHT 768
// Screen info
unsigned int width, height, pitch;
/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;
/**
 * Set screen resolution to 1024x768
 */
void framebf_init()
{
    mBuf[0] = 35 * 4; // Length of message in bytes
    mBuf[1] = MBOX_REQUEST;
    mBuf[2] = MBOX_TAG_SETPHYWH;  // Set physical width-height
    mBuf[3] = 8;                  // Value size in bytes
    mBuf[4] = 0;                  // REQUEST CODE = 0
    mBuf[5] = 1024;               // Value(width)
    mBuf[6] = 768;                // Value(height)
    mBuf[7] = MBOX_TAG_SETVIRTWH; // Set virtual width-height

    mBuf[8] = 8;
    mBuf[9] = 0;
    mBuf[10] = 1024;
    mBuf[11] = 768;
    mBuf[12] = MBOX_TAG_SETVIRTOFF; // Set virtual offset
    mBuf[13] = 8;
    mBuf[14] = 0;
    mBuf[15] = 0;                 // x offset
    mBuf[16] = 0;                 // y offset
    mBuf[17] = MBOX_TAG_SETDEPTH; // Set color depth
    mBuf[18] = 4;
    mBuf[19] = 0;
    mBuf[20] = COLOR_DEPTH;         // Bits per pixel
    mBuf[21] = MBOX_TAG_SETPXLORDR; // Set pixel order
    mBuf[22] = 4;
    mBuf[23] = 0;
    mBuf[24] = PIXEL_ORDER;
    mBuf[25] = MBOX_TAG_GETFB; // Get frame buffer
    mBuf[26] = 8;
    mBuf[27] = 0;
    mBuf[28] = 16;                // alignment in 16 bytes
    mBuf[29] = 0;                 // will return Frame Buffer size in bytes
    mBuf[30] = MBOX_TAG_GETPITCH; // Get pitch
    mBuf[31] = 4;
    mBuf[32] = 0;
    mBuf[33] = 0; // Will get pitch value here
    mBuf[34] = MBOX_TAG_LAST;
    // Call Mailbox
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) // mailbox call is successful ?
        && mBuf[20] == COLOR_DEPTH          // got correct color depth ?
        && mBuf[24] == PIXEL_ORDER          // got correct pixel order ?
        && mBuf[28] != 0                    // got a valid address for frame buffer ?
    )
    {
        /* Convert GPU address to ARM address (clear higher address bits)
         * Frame Buffer is located in RAM memory, which VideoCore MMU
         * maps it to bus address space starting at 0xC0000000.
         * Software accessing RAM directly use physical addresses
         * (based at 0x00000000)
         */
        mBuf[28] &= 0x3FFFFFFF;
        // Access frame buffer as 1 byte per each address
        fb = (unsigned char *)((unsigned long)mBuf[28]);
        uart_puts("Got allocated Frame Buffer at RAM physical address: ");
        uart_hex(mBuf[28]);
        uart_puts("\n");
        uart_puts("Frame Buffer Size (bytes): ");
        uart_dec(mBuf[29]);
        uart_puts("\n");
        width = mBuf[5];  // Actual physical width
        height = mBuf[6]; // Actual physical height
        pitch = mBuf[33]; // Number of bytes per line
    }
    else
    {
        uart_puts("Unable to get a frame buffer with provided setting\n");
    }
}
void drawPixelARGB32(int x, int y, unsigned int attr)
{
    int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);
    // Access and assign each byte
    *(fb + offs) = (attr >> 0) & 0xFF;      // BLUE
    *(fb + offs + 1) = (attr >> 8) & 0xFF;  // GREEN
    *(fb + offs + 2) = (attr >> 16) & 0xFF; // RED
    *(fb + offs + 3) = (attr >> 24) & 0xFF; // ALPHA

    // Access 32-bit together
    *((unsigned int *)(fb + offs)) = attr;
}
void drawPixel(int x, int y, unsigned char attr)
{
	int offs = (y * pitch) + (x * 4);
	*((unsigned int *)(fb + offs)) = vgapal[attr & 0x0f];
}
void drawRect(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
	for (int y = y1; y <= y2; y++)
		for (int x = x1; x <= x2; x++)
		{
			if ((x == x1 || x == x2) || (y == y1 || y == y2))
				drawPixel(x, y, attr);
			else if (fill)
				drawPixel(x, y, attr);
		}
}
void drawALineARG(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
}

void drawRectARGB32(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
        {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixelARGB32(x, y, attr);
            else if (fill)
                drawPixelARGB32(x, y, attr);
        }
}
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr)
{
	int dx, dy, p, x, y;

	dx = x2 - x1;
	dy = y2 - y1;
	x = x1;
	y = y1;
	p = 2 * dy - dx;

	while (x < x2)
	{
		if (p >= 0)
		{
			drawPixel(x, y, attr);
			y++;
			p = p + 2 * dy - 2 * dx;
		}
		else
		{
			drawPixel(x, y, attr);
			p = p + 2 * dy;
		}
		x++;
	}
}

void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill)
{
	int x = radius;
	int y = 0;
	int err = 0;

	while (x >= y)
	{
		if (fill)
		{
			drawLine(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
			drawLine(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
			drawLine(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
			drawLine(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
		}
		drawPixel(x0 - y, y0 + x, attr);
		drawPixel(x0 + y, y0 + x, attr);
		drawPixel(x0 - x, y0 + y, attr);
		drawPixel(x0 + x, y0 + y, attr);
		drawPixel(x0 - x, y0 - y, attr);
		drawPixel(x0 + x, y0 - y, attr);
		drawPixel(x0 - y, y0 - x, attr);
		drawPixel(x0 + y, y0 - x, attr);

		if (err <= 0)
		{
			y += 1;
			err += 2 * y + 1;
		}

		if (err > 0)
		{
			x -= 1;
			err -= 2 * x + 1;
		}
	}
}

void clearScreen(int color)
{
	drawRect(0, 0, 1024, 768, color, 1);
}

void drawImage(unsigned int image[], int x, int y, int w, int h)
{
	int count = 0;

	while (y < h)
	{
		while (x < w)
		{
			int offs = (y * pitch) + (x * 4); //print array
			*((unsigned int *)(fb + offs)) = image[count];

			x++;
			count++;
		}
		y++;
		x = 0;
	}
}

void drawChar(unsigned char ch, int x, int y, unsigned char attr)
{
	unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

	for (int i = 0; i < FONT_HEIGHT; i++)
	{
		for (int j = 0; j < FONT_WIDTH; j++)
		{
			unsigned char mask = 1 << j;
			unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

			drawPixel(x + j, y + i, col);
		}
		glyph += FONT_BPL;
	}
}

void drawString(int x, int y, char *s, unsigned char attr)
{
	while (*s)
	{
		if (*s == '\r')
		{
			x = 0;
		}
		else if (*s == '\n')
		{
			x = 0;
			y += FONT_HEIGHT;
		}
		else
		{
			drawChar(*s, x, y, attr);
			x += FONT_WIDTH;
		}
		s++;
	}
}
// Centering strings on the screen
    int centerPosition(int strLength)
    {
        // Assuming a character takes up about 10 pixels in width (modify this if different)
        int estimatedStrWidth = strLength * 10;
        return (SCR_WIDTH - estimatedStrWidth) / 2;
    }
void drawOnScreen()
{
    // Constants to make positioning clearer and more adaptable
    const int TITLE_Y = 200;
    const int LINE_Y = TITLE_Y + 20;
    const int START_Y = LINE_Y + 30;
    const int LINE_SPACING = 25;
    const int CIRCLE_RADIUS_LARGE = 250;
    const int CIRCLE_RADIUS_SMALL = 30;

    

    // Draw title and underline
    drawString(centerPosition(strlen("PROJECT OF GROUP 26")), TITLE_Y, "PROJECT OF GROUP 26", 0xd);
    drawLine(centerPosition(strlen("PROJECT OF GROUP 26")) - 10, LINE_Y, centerPosition(strlen("PROJECT OF GROUP 26")) + strlen("PROJECT OF GROUP 26") * 10 + 10, LINE_Y, 0x0f);

    // Draw group members with role
    drawString(centerPosition(strlen("Programming:               ")), START_Y, "Programming:", 0x7);
    drawString(centerPosition(strlen("Programming:               ")) + strlen("Programming") * 10 + 10, START_Y, "Doan Hoang Anh", 0xa);

    drawString(centerPosition(strlen("Programming:               ")) + strlen("Programming") * 10 + 10, START_Y + LINE_SPACING, "Nguyen Tan Huy", 0x6);

    drawString(centerPosition(strlen("Programming:               ")) + strlen("Programming") * 10 + 10, START_Y + 2 * LINE_SPACING, "Vu Viet Minh", 0x2);

    drawString(centerPosition(strlen("Programming:               ")) + strlen("Programming") * 10 + 10, START_Y + 3 * LINE_SPACING, "Nguyen Nam Vinh", 0x6);

    // Draw Circles
    drawCircle(SCR_WIDTH / 2, SCR_HEIGHT / 2, CIRCLE_RADIUS_LARGE, 0x09, 0);  // Centered large circle
}

