#include <unistd.h> // for sleep()
#define TB_OPT_TRUECOLOR 1
#define TB_IMPL
#include "termbox.h"


int main(int argc, char **argv) {
  if (tb_init() != 0)
    return 1; // couldn't initialize our screen

  tb_set_output_mode(TB_OUTPUT_TRUECOLOR);
  // set up our colors
  uint32_t bg = 0x88B8A0;
  uint32_t fg = 0xa281c6;

  // get the screen resolution
  int w = tb_width();
  int h = tb_height();

  // calculate the coordinates for the center of the screen
  int x = (w/2);
  int y = (h/2);

  // put some text into those coordinates
  tb_printf(x-4, y, fg, bg, "hello world");

  // flush the output to the screen
  tb_present();

  // wait a few secs
  sleep(3);

  // and finally, shutdown the screen so we exit cleanly
  tb_shutdown();

  return 0;
}
