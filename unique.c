#include <stdint.h>
#include <math.h>
#include <locale.h>
#define __USE_XOPEN
#include <wchar.h>
#include <stdio.h>
#include <unistd.h> // for sleep()
#define TB_OPT_TRUECOLOR 1
#define TB_IMPL
#include "termbox.h"
#include "hsluv.h"

// Canvas height and width
#define CANVAS_H 25
#define CANVAS_W 80
// With border included
#define CANVAS_H_B CANVAS_H + 1
#define CANVAS_W_B CANVAS_W + 1

// Top Toolbar height
#define TTOOL_H 3

// Window size
int WINDOW_W;
int WINDOW_H;

int STOOL_W;

// fg and bg pen colors in HSL
double g_fg_h, g_fg_s, g_fg_l;
double g_bg_h, g_bg_s, g_bg_l;

// pen char
uint32_t g_ch;

// pen char index
int g_ch_x, g_ch_y;

// Unicode page starting code
uint32_t unicode_page_start;

void draw_6x3(int x, int y, uintattr_t color)
{
  for (int sx = x; sx < x + 6; sx++) {
    for (int sy = y; sy < y + 3; sy++) {
      tb_set_cell(sx, sy, U' ', 0, color);
    }
  }
}

uintattr_t rgb2uintattr(double r, double g, double b)
{
  uint32_t i_r = r * 255.0;
  uint32_t i_g = g * 255.0;
  uint32_t i_b = b * 255.0;

  i_r = i_r << 16;
  i_g = i_g << 8;

  return i_r | i_g | i_b;
}

uintattr_t hsl2rgb(double h, double s, double l) {
  double pr = 0;
  double pg = 0;
  double pb = 0;
  hsluv2rgb(h, s, l, &pr, &pg, &pb);
  return rgb2uintattr(pr, pg, pb);
}

void draw_color_pallet()
{
  tb_printf(1, 0, 0xffffff, 0, "f_h");
  tb_printf(1, 1, 0xffffff, 0, "f_s");
  tb_printf(1, 2, 0xffffff, 0, "f_l");
  int x00 = 4;
  int y00 = 0;
  int x01 = (CANVAS_W/2)-5;
  int y01 = TTOOL_H;
  for (int x = x00; x < x01; x++) {
    for (int y = y00; y < y01; y++) {
      double norm_x00 = x - x00;
      double norm_x01 = x01 - x00;
      if (y == 0) {
        double h0 = 360*(norm_x00/norm_x01);
        double next_h0 = 360*((norm_x00 + 1.0)/norm_x01);
        uint32_t t_ch = U' ';
        if (g_fg_h >= h0 && g_fg_h < next_h0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(h0, g_fg_s, g_fg_l);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      if (y == 1) {
        double s0 = 100*(norm_x00/norm_x01);
        double next_s0 = 100*((norm_x00+1)/norm_x01);
        uint32_t t_ch = U' ';
        if (g_fg_s >= s0 && g_fg_s < next_s0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(g_fg_h, s0, g_fg_l);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      if (y == 2) {
        double l0 = 100*(norm_x00/norm_x01);
        double next_l0 = 100*((norm_x00+1)/norm_x01);
        uint32_t t_ch = U' ';
        if (g_fg_l >= l0 && g_fg_l < next_l0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(g_fg_h, g_fg_s, l0);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      tb_present();
    }
  }
  draw_6x3(CANVAS_W/2 - 5, 0, hsl2rgb(g_fg_h, g_fg_s, g_fg_l));

  // bg pallet
  tb_printf(CANVAS_W/2+1, 0, 0xffffff, 0, "b_h");
  tb_printf(CANVAS_W/2+1, 1, 0xffffff, 0, "b_s");
  tb_printf(CANVAS_W/2+1, 2, 0xffffff, 0, "b_l");
  int x10 = CANVAS_W/2 + 4;
  int y10 = 0;
  int x11 = CANVAS_W-5;
  int y11 = TTOOL_H;
  for (int x = x10; x < x11; x++) {
    for (int y = y10; y < y11; y++) {
      double norm_x10 = x - x10;
      double norm_x11 = x11 - x10;
      if (y == 0) {
        double h0 = 360*(norm_x10/norm_x11);
        double next_h0 = 360*((norm_x10 + 1.0)/norm_x11);
        uint32_t t_ch = U' ';
        if (g_bg_h >= h0 && g_bg_h < next_h0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(h0, g_bg_s, g_bg_l);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      if (y == 1) {
        double s0 = 100*(norm_x10/norm_x11);
        double next_s0 = 100*((norm_x10 + 1.0)/norm_x11);
        uint32_t t_ch = U' ';
        if (g_bg_s >= s0 && g_bg_s < next_s0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(g_bg_h, s0, g_bg_l);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      if (y == 2) {
        double l0 = 100*(norm_x10/norm_x11);
        double next_l0 = 100*((norm_x10 + 1.0)/norm_x11);
        uint32_t t_ch = U' ';
        if (g_bg_l >= l0 && g_bg_l < next_l0) {
            t_ch = U'•';
        } else {
          t_ch = U' ';
        }
        uintattr_t bg = hsl2rgb(g_bg_h, g_bg_s, l0);
        tb_set_cell(x, y, t_ch, 0, bg);  
      }
      tb_present();
    }
  }
  draw_6x3(CANVAS_W - 5, 0, hsl2rgb(g_bg_h, g_bg_s, g_bg_l));
}

void draw_unicode_pallet(uint32_t start)
{
  int x0 = CANVAS_W + 2;
  int y0 = TTOOL_H + 1;
  int x1 = WINDOW_W;
  int y1 = WINDOW_H;
  for (int y = y0; y < y1; y++) {
    for (int x = x0; x < x1; x++) {
      uint32_t s = start;
      if (wcwidth(s) < 2) {
        if(start == g_ch) {
          tb_set_cell(x, y, U'•', 0, 0xffffff);  
        } else {
          tb_set_cell(x, y, start, 0xffffff, 0x000000);  
        }
      }
      start++;
    }
  }
}
void draw_ui_border()
{
  int outer_w = CANVAS_W + STOOL_W;
  int outer_h = CANVAS_H + TTOOL_H;

  uintattr_t w = 0xffffff;
  uintattr_t b = 0;

  // Draw all corner characters
  tb_set_cell(0, TTOOL_H, U'┌', w, b);;
  tb_set_cell(CANVAS_W_B, TTOOL_H, U'┐', w, b);
  tb_set_cell(0, outer_h, U'└', w, b); 
  tb_set_cell(CANVAS_W_B, outer_h, U'┘', w, b); 

  // Draw all horizontal lines
  for (int x = 1; x < CANVAS_W_B; x++) {
      tb_set_cell(x, TTOOL_H, U'─', w, b);
      tb_set_cell(x, outer_h, U'─', w, b);
  }
  // Draw all vertical lines
  for (int y = TTOOL_H+1; y < outer_h; y++) {
    tb_set_cell(0, y, U'│', w, b);
    tb_set_cell(CANVAS_W_B, y, U'│', w, b);
  }
}

// Returns 0 if click is in canvas, 
// 1 if top toolbar
// 2 if side toolbar
// 3 if top right section
// -1 if none of these
int segment_mouse_click(struct tb_event *ev)
{
  int x = ev->x;
  int y = ev->y;
  if (x > 0 && y > TTOOL_H && x < CANVAS_W_B && y < CANVAS_H + TTOOL_H) {
    return 0;
  }
  if (x > 0 && y >= 0 && x < CANVAS_W_B + 1 && y < TTOOL_H) {
    return 1;
  }
  if (x > CANVAS_W + 1 && y > TTOOL_H && x < CANVAS_W + STOOL_W + 1 && y < WINDOW_H) {
    return 2;
  }
  tb_present();
  return -1;
}

void canvas_handle_click(struct tb_event *ev)
{
  uintattr_t fg = hsl2rgb(g_fg_h, g_fg_s, g_fg_l);
  uintattr_t bg = hsl2rgb(g_bg_h, g_bg_s, g_bg_l);
  tb_set_cell(ev->x, ev->y, g_ch, fg, bg);
  return;
}

uintattr_t ttool_handle_click(struct tb_event *ev)
{
  int x = ev->x;
  int y = ev->y;
  if (x > 3 && x < CANVAS_W/2.0 - 5) {
    int x00 = 4;
    int x01 = (CANVAS_W/2)-5;
    double norm_x0 = x - x00;
    double norm_x1 = x01 - x00; 
    if (y == 0) {
      g_fg_h = 360*((x-4)/((CANVAS_W/2.0 - 5) -4));
      draw_color_pallet();
      tb_present();
    }
    if (y == 1) {
      g_fg_s = 100*((x-4)/((CANVAS_W/2.0 - 5) -4));
      draw_color_pallet();
      tb_present();
    }
    if (y == 2) {
      g_fg_l = 100*((x-4)/((CANVAS_W/2.0 - 5) -4));
      draw_color_pallet();
      tb_present();
    }
  }
  if (x >= CANVAS_W/2.0 + 4 && x < CANVAS_W - 5) {
    int x10 = CANVAS_W/2 + 4;
    int x11 = CANVAS_W-5;
    double norm_x0 = x - x10;
    double norm_x1 = x11 - x10; 
    if (y == 0) {
      g_bg_h = 360*(norm_x0/norm_x1);
      draw_color_pallet();
      tb_present();
    }
    if (y == 1) {
      g_bg_s = 100*(norm_x0/norm_x1);
      draw_color_pallet();
      tb_present();
    }
    if (y == 2) {
      g_bg_l = 100*(norm_x0/norm_x1);
      draw_color_pallet();
      tb_present();
    }
  }
  return 0;
}

void stool_handle_click(struct tb_event *ev)
{
  int x = ev->x;
  int y = ev->y;
  int x0 = CANVAS_W + 2;
  int y0 = TTOOL_H + 1;
  int x1 = WINDOW_W;
  int y1 = WINDOW_H;
  if (x >= x0 && x < x1 && y >= y0 && y <= y1) {
    int norm_x = x - x0;
    int norm_y = y - y0; 
    int width = x1 - x0;
    int offset = norm_y * width + norm_x;

    g_ch = unicode_page_start + offset;
    draw_unicode_pallet(unicode_page_start);
    tb_present();
  }
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
  setlocale(LC_ALL, "");
  if (tb_init() != 0)
    return 1;

  tb_set_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
  tb_set_output_mode(TB_OUTPUT_TRUECOLOR);

  g_fg_h = 180;
  g_fg_s = 50;
  g_fg_l = 50;
  g_bg_h = 180;
  g_bg_s = 50;
  g_bg_l = 50;
  g_ch = U' ';
  unicode_page_start = 0x1fb00;
  int curr_segment = -1;

  struct tb_event ev;

  tb_clear();

  WINDOW_H = tb_height();
  WINDOW_W = tb_width();

  STOOL_W = WINDOW_W - (CANVAS_W + 1);

  draw_ui_border();
  draw_unicode_pallet(unicode_page_start);
  draw_color_pallet();
  tb_printf(CANVAS_W_B+1, TTOOL_H, 0xffffff, 0, "U+%X", unicode_page_start);
  tb_present();

  while (tb_poll_event(&ev) == TB_OK) {
    switch (ev.type) {
      case TB_EVENT_KEY:
        if (ev.key == TB_KEY_CTRL_C) {
          tb_shutdown();
          return 0;
        }
      case TB_EVENT_MOUSE:
        curr_segment = segment_mouse_click(&ev);
        switch (curr_segment) {
          case 0:
            canvas_handle_click(&ev);
          case 1:
            ttool_handle_click(&ev);
          case 2:
            stool_handle_click(&ev);
          case 3:

          default:
            break;
        }
        tb_present();
      default:
        break; 
    } 
  }
  tb_shutdown();
  return 0;
}
