/* basic macros */
#include <unistd.h>
#define RGB16(r,g,b) ((r)+(g<<5)+(b<<10))
#include <keypad.h>
#define GAMEPAK_RAM ((u8*)0x0E000000)
#define xyToNum(x,y) ((x)+(y)*240)
#define keydown(k) !((*KEYS) & k)

/* probably bad to do this as a macro */
#define paintChar(x, y, _char) {\
	int i = 0;\
	int j = 0;\
	while(i < 8 && j < 8) {\
		dot(x + i, y + j, _char[i+j*8]);\
		i++;\
		if(i > 7) {\
			j++;\
			i = 0;\
		}\
	}\
}

/* colors */
unsigned short white = RGB16(31, 31, 31);
unsigned short black = RGB16(0, 0, 0);
unsigned short red = RGB16(31, 0, 0);
unsigned short darkred = RGB16(15, 0, 0);
unsigned short lightgray = RGB16(23, 23, 23);

int chr_SK[] = {
	0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x7FFF, 0x3DEF, 0x3DEF, 0x3DEF, 0x0000, 0x0000, 0x0000, 0x0000,
	0x7FFF, 0x7FFF, 0x7FFF, 0x3DEF, 0x7FFF, 0x0000, 0x7FFF, 0x0000,
	0x0000, 0x3DEF, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF,
	0x7FFF, 0x7FFF, 0x7FFF, 0x3DEF, 0x7FFF, 0x7FFF, 0x0000, 0x3DEF,
	0x0000, 0x3DEF, 0x3DEF, 0x3DEF, 0x7FFF, 0x3DEF, 0x7FFF, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3DEF, 0x0000, 0x3DEF,
};

int chr_CH[] = {
	0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x7FFF, 0x3DEF, 0x3DEF, 0x3DEF, 0x0000, 0x0000, 0x0000, 0x0000,
	0x7FFF, 0x3DEF, 0x0000, 0x0000, 0x7FFF, 0x0000, 0x7FFF, 0x0000,
	0x7FFF, 0x3DEF, 0x0000, 0x0000, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF,
	0x7FFF, 0x7FFF, 0x7FFF, 0x0000, 0x7FFF, 0x7FFF, 0x7FFF, 0x3DEF,
	0x0000, 0x3DEF, 0x3DEF, 0x3DEF, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF,
	0x0000, 0x0000, 0x0000, 0x0000, 0x7FFF, 0x3DEF, 0x7FFF, 0x3DEF,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3DEF, 0x0000, 0x3DEF,
};

int chr_load[] = {
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

/* other variables (screen, image) */
u8 *pSaveMemory = GAMEPAK_RAM;
unsigned short* Screen = (unsigned short*)0x6000000;
unsigned short* Image = (unsigned short*)0x2000000;

/* grid size
 * TODO add some way to change grid size, maybe using L and R buttons?
 * we also need a way to efficiently redraw the grid */
u8 gridSize = 8;

/* just set a pixel to the color c */
void dot(int x,int y, unsigned short int c) {
	Screen[y * 240 + x] = c;
}

/* same as dot, but do it with the IMAGE */
void setImagePixel(int x, int y, unsigned short int c) {
	Image[y * 240 + x] = c;
}

void clear() {
	int x, y;
	for(x = 0; x < 240; x++) {
		for(y = 0; y < 160; y++) {
			setImagePixel(x, y, white);
		}
	}
	bg();
}

/* clear the screen */
void bg() {
	int x, y;
	for(x = 0; x < 240; x++) {
		for(y = 0; y < 160; y++) {
			drawPixel(x, y);
		}
	}
}

int getPixel(int x, int y) {
	return Screen[(y) *240 + (x)];
}

int getImagePixel(int x, int y) {
	return Image[(y) *240 + (x)];
}

void drawPixel(int x, int y) {
	if(getImagePixel(x, y) == black) {
		dot(x, y, black);
		return;
	}
	if(gridSize == 0) {
		dot(x, y, white);
		return;
	}
	/* TODO find a better way to find out if pixel is on grid */
	//for(k = 0; k < 240; k += gridSize) if(x == k || y == k) onGrid = 1;
	if(x % gridSize == 0 || y % gridSize == 0) {
		dot(x, y, lightgray);
	} else {
		dot(x, y, white);
	}
}

int main() {
	if(pSaveMemory[65535] == 0xFF) pSaveMemory[65535] = 8;
	gridSize = pSaveMemory[65535];
	char x, y;
	int i, j;
	int sketchx = 0;
	int sketchy = 0;
	paintChar(112, 76, chr_SK);
	paintChar(120, 76, chr_CH);
	*(unsigned long*)0x4000000 = 0x403; /* mode 3: +bg2 */
	for(y = 0; y < 160; y++) {
		for(x = 0; x < 240; x++) {
			if(pSaveMemory[241 * y + x] == 0x23) {
				setImagePixel(x, y, black);
			} else {
				setImagePixel(x, y, white);
			}
		}
	}
	bg();
	int draw = 0;
	while(1) {
		draw = 0;
		if(keydown(KEY_UP)) if(sketchy > 0) {
			drawPixel(sketchx, sketchy);
			sketchy--;
			draw = 1;
		}
		if(keydown(KEY_DOWN)) if(sketchy < 159) {
			drawPixel(sketchx, sketchy);
			sketchy++;
			draw = 1;
		}
		if(keydown(KEY_LEFT)) if(sketchx > 0) {
			drawPixel(sketchx, sketchy);
			sketchx--;
			draw = 1;
		}
		if(keydown(KEY_RIGHT)) if(sketchx < 239) {
			drawPixel(sketchx, sketchy);
			sketchx++;
			draw = 1;
		}
		if(keydown(KEY_A)) {
			setImagePixel(sketchx, sketchy, black);
			drawPixel(sketchx, sketchy);
		}
		if(keydown(KEY_B)) {
			setImagePixel(sketchx, sketchy, white);
			drawPixel(sketchx, sketchy);
		}
		if(keydown(KEY_SELECT)) clear();
		if(getImagePixel(sketchx, sketchy) == black) dot(sketchx, sketchy, red);
		else dot(sketchx, sketchy, darkred);
		if(keydown(KEY_START)) {
			for(y = 0; y < 160; y++) {
				for(x = 0; x < 241; x++) {
					if(x == 240) pSaveMemory[241 * y + x] = 0x0a; else pSaveMemory[241 * y + x] = getImagePixel(x, y) == black ? 0x23 : 0x20;
				}
			}
			char endstr[] = "Saved with Sketch beta 1\n";
			for(x = 0; x < strlen(endstr); x++) {
				pSaveMemory[241 * 160 + x] = endstr[x];
			}
			char endstr2[] = "\nGrid size (0-255, ascii): ";
			for(x = 0; x < strlen(endstr2); x++) {
				pSaveMemory[65535 - (strlen(endstr2) - x)] = endstr2[x];
			}
		}
		if(draw == 1) usleep(25000);
	}
}
