/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

int main() {
  VDP_loadTileSet(background.tileset, 1, DMA);
  PAL_setPalette(PAL1, background.palette->data, DMA);

  int i = 0;
  int thex = 0;
  int they = 0;
  int val = 1;

  SYS_disableInts();
  for (i = 0; i < 1280; i++) {
    thex = i % 40;
    they = i / 40;
    val = (random() % (10 - 1 + 1)) + 1;
    if (val > 3) val = 1;
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, val), thex, they);
  }

  int offset = 0;
  VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
  SYS_enableInts();

  while (1) {
    VDP_setVerticalScroll(BG_B, offset -= 2);
    if (offset <= -256) offset = 0;

    SYS_doVBlankProcess();
  }
  return (0);
}
