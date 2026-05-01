/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

#define MAX_ENEMIES 6

typedef struct {
  int x;
  int y;
  int w;
  int h;
  int vel_x;
  int vel_y;
  int health;
  Sprite* sprite;
  char name[6];
} Entity;

Entity enemies[MAX_ENEMIES];
u16 enemies_left = 0;

void init_background(void) {
  VDP_loadTileSet(background.tileset, 1, DMA);
  PAL_setPalette(PAL1, background.palette->data, DMA);

  int i;
  int the_x;
  int the_y;
  int value;

  SYS_disableInts();
  for (i = 0; i < 1280; i++) {
    the_x = i % 40;
    the_y = i / 40;
    value = (random() % 10) + 1;
    if (value > 3) {
      value = 1;
    }
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, value), the_x, the_y);
  }

  VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
  SYS_enableInts();
}

void kill_entity(Entity* entity) {
  entity->health = 0;
  SPR_setVisibility(entity->sprite, HIDDEN);
}

void revive_entity(Entity* entity) {
  entity->health = 1;
  SPR_setVisibility(entity->sprite, VISIBLE);
}

int main(void) {
  init_background();
  SPR_init();

  Entity player_entity = {
      .x = 152,
      .y = 192,
      .w = 16,
      .h = 16,
      .vel_x = 0,
      .vel_y = 0,
      .health = 1,
      .sprite = NULL,
      .name = "PLAYER",
  };

  player_entity.sprite = SPR_addSprite(&ship, player_entity.x, player_entity.y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
  // SPR_setAnim(player_entity.sprite, 0);
  SPR_update();

  int offset = 0;
  while (1) {
    offset -= 2;
    VDP_setVerticalScroll(BG_B, offset);
    if (offset <= -256) {
      offset = 0;
    }

    SPR_update();
    SYS_doVBlankProcess();
  }

  return 0;
}
