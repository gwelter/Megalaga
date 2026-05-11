/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <resources.h>

#define MAX_BULLETS 6
#define MAX_PLAYER_BULLETS 3
#define MAX_ENEMIES 6
#define LEFT_EDGE 0
#define RIGHT_EDGE 320
#define BOTTOM_EDGE 224
#define ANIM_STRAIGHT 0
#define ANIM_MOVE 1
#define SHOT_INTERVAL 120

u16 shot_by_player = 0;
u16 shot_ticker = 0;
int score = 0;
char hud_string[40] = "\0";

typedef struct {
  int x;
  int y;
  int w;
  int h;
  int vel_x;
  int vel_y;
  int health;
  Sprite *sprite;
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
Entity bullets[MAX_BULLETS];
Entity enemies[MAX_ENEMIES];
u16 enemies_left = 0;
u16 bullets_on_screen = 0;

int i;

void shoot_bullet(Entity shooter);

void update_score_display() {
  sprintf(hud_string, "SCORE: %d - LEFT: %d", score, enemies_left);
  VDP_clearText(0, 0, 40);
  VDP_drawText(hud_string, 0, 0);
}

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
    // random() % (max - min + 1) + min
    value = (random() % (10 - 1 + 1)) + 1;
    if (value > 3) {
      value = 1;
    }
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, value), the_x, the_y);
  }

  VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
  SYS_enableInts();
}

void kill_entity(Entity *entity) {
  entity->health = 0;
  SPR_setVisibility(entity->sprite, HIDDEN);
}

void revive_entity(Entity *entity) {
  entity->health = 1;
  SPR_setVisibility(entity->sprite, VISIBLE);
}

int collide_entities(Entity *a, Entity *b) {
  return (a->x < b->x + b->w && a->x + a->w > b->x && a->y < b->y + b->h && a->y + a->h >= b->y);
}

void handle_collisions() {
  Entity *b;
  Entity *e;
  int i = 0;
  int j = 0;
  for (i = 0; i < MAX_BULLETS; i++) {
    b = &bullets[i];
    if (b->health > 0) {
      if (b->vel_y < 0) {  // Shoot by player
        for (j = 0; j < MAX_ENEMIES; j++) {
          e = &enemies[j];
          if (e->health > 0) {
            if (collide_entities(b, e)) {
              kill_entity(b);
              kill_entity(e);
              enemies_left--;
              bullets_on_screen--;
              shot_by_player--;
              score += 10;
              update_score_display();
              break;
            }
          }
        }
      } else {  // Shoot by enemy
        if (collide_entities(b, &player_entity)) {
          kill_entity(&player_entity);
        }
      }
    }
  }
}

void create_player() {
  player_entity.sprite = SPR_addSprite(&ship, player_entity.x, player_entity.y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
  // SPR_setAnim(player_entity.sprite, 0);
}

void create_bullets() {
  Entity *b = bullets;
  for (i = 0; i < MAX_BULLETS; i++) {
    b->x = 0;
    b->y = -10;
    b->w = 8;
    b->h = 8;
    b->sprite = SPR_addSprite(&bullet, bullets[0].x, bullets[0].y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
    sprintf(b->name, "Bu%i", i);
    b++;
  }
}

void create_enemies() {
  PAL_setPalette(PAL2, background.palette->data, DMA);
  Entity *e = enemies;
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
  shot_ticker++;
  u16 i = 0;
  for (i = 0; i < MAX_ENEMIES; i++) {
    Entity *e = &enemies[i];
    if (e->health > 0) {
      e->x += e->vel_x;
      SPR_setPosition(e->sprite, e->x, e->y);

      // Shooting
      if (shot_ticker >= SHOT_INTERVAL) {
        // random() % (max - min + 1) + min
        if ((random() % (10 - 1 + 1) + 1) > 4) {
          shoot_bullet(*e);
          shot_ticker = 0;
        }
      }

      if ((e->x + e->w) > RIGHT_EDGE) {
        e->vel_x = -1;
      } else if (e->x < LEFT_EDGE) {
        e->vel_x = 1;
      }
    }
  }
}

void position_bullets() {
  u16 i = 0;
  for (i = 0; i < MAX_BULLETS; i++) {
    Entity *b = &bullets[i];
    if (b->health > 0) {
      b->y += b->vel_y;
      if ((b->y + b->h) < 0) {  // Shoot by player
        kill_entity(b);
        bullets_on_screen--;
        shot_by_player--;
      } else if (b->y > BOTTOM_EDGE) {  // Shoot by enemy
        kill_entity(b);
        bullets_on_screen--;
      } else {
        SPR_setPosition(b->sprite, b->x, b->y);
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

void shoot_bullet(Entity shooter) {
  bool fromPlayer = (shooter.y > 100);

  if (bullets_on_screen < MAX_BULLETS) {
    if (fromPlayer && shot_by_player >= MAX_PLAYER_BULLETS) {
      return;
    }
    Entity *b;
    u16 i = 0;
    for (i = 0; i < MAX_BULLETS; i++) {
      b = &bullets[i];
      if (b->health == 0) {  // Available
        b->x = shooter.x + 4;
        b->y = shooter.y;
        revive_entity(b);
        if (fromPlayer) {
          b->vel_y = -3;
          shot_by_player++;
        } else {
          b->vel_y = 3;
        }

        SPR_setPosition(b->sprite, b->x, b->y);
        bullets_on_screen++;
        break;
      }
    }
  }
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
    if (state & BUTTON_B & changed) {
      shoot_bullet(player_entity);
    }
  }
}

int main() {
  SPR_init();
  init_background();
  update_score_display();

  JOY_init();
  JOY_setEventHandler(&myJoyHandler);

  create_player();
  create_bullets();
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
    position_bullets();
    handle_collisions();
    SPR_update();
    SYS_doVBlankProcess();
  }

  return 0;
}
