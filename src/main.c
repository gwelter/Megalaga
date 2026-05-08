/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

#define MAX_ENEMIES 6
#define LEFT_EDGE 0
#define RIGHT_EDGE 320
#define ANIM_STRAIGHT 0
#define ANIM_MOVE 1

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
Entity enemies[MAX_ENEMIES];
u16 enemies_left = 0;

int i;

void init_background(void) {
  VDP_loadTileSet(background.tileset, 1, DMA);
  PAL_setPalette(PAL1, background.palette->data, DMA);

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

void create_player() {
  player_entity.sprite = SPR_addSprite(&ship, player_entity.x, player_entity.y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
  // SPR_setAnim(player_entity.sprite, 0);
}

void create_enemies() {
  PAL_setPalette(PAL2, background.palette->data, DMA);
  Entity* e = enemies;
  for (i = 0; i < MAX_ENEMIES; i++) {
    e->x = i * 32;
    e->y = 32;
    // Enemey prite is 2 by 2 meaning 8+8 by 8+8 (default sprite size)
    e->w = 16;
    e->h = 16;
    e->vel_x = 1;
    e->health = 1;
    e->sprite = SPR_addSprite(&ship, e->x, e->y, TILE_ATTR(PAL2, 0, TRUE, FALSE));
    sprintf(e->name, "En%d", i);
    enemies_left++;
    e++;
  }
  PAL_setColor(34, RGB24_TO_VDPCOLOR(0x0078f8));
}

void position_enemies() {
  u16 i = 0;
  for (i = 0; i < MAX_ENEMIES; i++) {
    Entity* e = &enemies[i];
    if (e->health > 0) {
      e->x += e->vel_x;
      SPR_setPosition(e->sprite, e->x, e->y);

      if ((e->x + e->w) > RIGHT_EDGE) {
        e->vel_x = -1;
      } else if (e->x < LEFT_EDGE) {
        e->vel_x = 1;
      }
    }
  }
}

void position_player() {
  player_entity.x += player_entity.vel_x;

  if (player_entity.x < LEFT_EDGE) {
    player_entity.x = LEFT_EDGE;
  }
  if ((player_entity.x + player_entity.w) > RIGHT_EDGE) {
    player_entity.x = RIGHT_EDGE - player_entity.w;
  }
  SPR_setPosition(player_entity.sprite, player_entity.x, player_entity.y);
}

void myJoyHandler(u16 joy, u16 changed, u16 state) {
  if (joy == JOY_1) {
    if (state & BUTTON_RIGHT) {
      player_entity.vel_x = 2;
      SPR_setAnim(player_entity.sprite, ANIM_MOVE);
      SPR_setHFlip(player_entity.sprite, TRUE);
    } else if (state & BUTTON_LEFT) {
      player_entity.vel_x = -2;
      SPR_setAnim(player_entity.sprite, ANIM_MOVE);
      SPR_setHFlip(player_entity.sprite, FALSE);
    } else {
      if ((changed & BUTTON_RIGHT) || (changed & BUTTON_LEFT)) {
        player_entity.vel_x = 0;
        SPR_setAnim(player_entity.sprite, ANIM_STRAIGHT);
      }
    }
  }
}

int main() {
  SPR_init();
  init_background();

  JOY_init();
  JOY_setEventHandler(&myJoyHandler);

  create_player();
  create_enemies();
  SPR_update();

  int offset = 0;
  while (1) {
    offset -= 2;
    VDP_setVerticalScroll(BG_B, offset);
    if (offset <= -256) {
      offset = 0;
    }

    position_enemies();
    position_player();
    SPR_update();
    SYS_doVBlankProcess();
  }

  return 0;
}
