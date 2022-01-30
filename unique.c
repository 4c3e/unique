#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // for sleep()
#define TB_OPT_TRUECOLOR 1
#define TB_IMPL
#include "termbox.h"

// Canvas height and width
#define CANVAS_H 25
#define CANVAS_W 80

// Top Toolbar height and width
#define TTOOL_H 4 
#define TTOOL_W CANVAS_W

// Side Toolbar height and width
#define STOOL_H CANVAS_H + TTOOL_H
#define STOOL_W 18

void draw_color_pallet()
{
  int x0 = 1;
  int y0 = 1;
  int x1 = CANVAS_W + 1;
  int y1 = TTOOL_H;
  for (int x = x0; x < x1; x++) {
    for (int y = y0; y < y1; y++) {
      tb_set_cell(x, y, U'▒', 0x5efdea, 0x000000);  
    }
  }
  tb_present();
}
void draw_unicode_pallet(uint32_t start)
{
  int x0 = CANVAS_W + 2;
  int y0 = TTOOL_H + 1;
  int x1 = CANVAS_W + STOOL_W;
  int y1 = CANVAS_H + TTOOL_H;
  for (int y = y0; y < y1; y++) {
    for (int x = x0; x < x1; x++) {
      tb_set_cell_ex(x, y, &start, sizeof(start), 0xffffff, 0x000000);  
      start++;
    }
  }
  tb_present();
}
void draw_ui_border()
{
  int canvas_border_w = CANVAS_W + 1;
  int canvas_border_h = CANVAS_H + 1;
  int outer_w = CANVAS_W + STOOL_W;
  int outer_h = CANVAS_H + TTOOL_H;

  uintattr_t w = 0xffffff;
  uintattr_t b = 0x000000;

  // Draw all corner characters
  tb_set_cell(0, 0, U'┏', w, b);
  tb_set_cell(canvas_border_w, 0, U'┳', w, b);
  tb_set_cell(outer_w, 0, U'┓', w, b);
  tb_set_cell(0, TTOOL_H, U'┣', w, b);;
  tb_set_cell(canvas_border_w, TTOOL_H, U'┫', w, b);
  tb_set_cell(0, outer_h, U'┗', w, b); 
  tb_set_cell(canvas_border_w, outer_h, U'┻', w, b); 
  tb_set_cell(outer_w, outer_h, U'┛', w, b);

  // Draw all horizontal lines
  for (int x = 1; x < canvas_border_w; x++) {
      tb_set_cell(x, 0, U'─', w, b);
      tb_set_cell(x, outer_h, U'─', w, b);
      tb_set_cell(x, TTOOL_H, U'─', w, b);
  }
  for (int x = canvas_border_w + 1; x < outer_w; x++) {
      tb_set_cell(x, 0, U'─', w, b);
      tb_set_cell(x, outer_h, U'─', w, b);
  }

  // Draw all vertical lines
  for (int y = 1; y < outer_h; y++) {
    tb_set_cell(outer_w, y, U'│', w, b);
    if (y == TTOOL_H)
      continue;
    tb_set_cell(0, y, U'│', w, b);
    tb_set_cell(canvas_border_w, y, U'│', w, b);
  }
  tb_present();
}

void handle_mouse_ev(struct tb_event *ev)
{
  tb_printf(ev->x, ev->y, 0x565656, 0x1f0155, " ");
}

void handle_keypress_ev(struct tb_event *ev)
{
  return;
}

int main(int argc, char **argv) 
{
  if (tb_init() != 0)
    return 1;

  tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
  tb_set_output_mode(TB_OUTPUT_TRUECOLOR);

  struct tb_event ev;

  tb_clear();

  int w = tb_width();
  int h = tb_height();

  draw_ui_border();
  draw_unicode_pallet(0x02500);
  draw_color_pallet();

  while (tb_poll_event(&ev) == TB_OK) {
    switch (ev.type) {
      case TB_EVENT_KEY:
        if (ev.key == TB_KEY_CTRL_C) {
          tb_shutdown();
          return 0;
        }
      case TB_EVENT_MOUSE:
        handle_mouse_ev(&ev);
        tb_present();
      default:
        break; 
    } 
  }
  sleep(7);
  tb_shutdown();
  return 0;
}
