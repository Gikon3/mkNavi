#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_hal.h"
#include "ft_heap.h"
#include "ft_core_impl.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_image.h"

void ft_image_init(FT_Image *image, FT_Widget *widget)
{
  image->parent = widget;
  image->picture = NULL;
  image->addr = 0;
  image->handle = -1;
  image->state = ftImageNone;
  image->filter = ftFilterNearest;
  image->wrap.x = ftWrapBorder;
  image->wrap.y = ftWrapBorder;
  image->color.value = 0xFFFFFFFF;
  image->coord.x = widget->coord.x;
  image->coord.y = widget->coord.y;
}

void ft_image_append(FT_Image *image, Vector *cmds)
{
  const FT_Coord absoluteCoord = {
      .x = image->parent->absoluteCoord.x + image->coord.x,
      .y = image->parent->absoluteCoord.y + image->coord.y
  };
  FT_Host const *dev = &image->parent->screen->core->host;
  FT_Heap *heap = &image->parent->screen->core->heap;
  switch(image->state) {
    case ftImageNone: {
      const size_t needSpace = image->picture->stride * image->picture->resolution.h;
      Vector loadImage = make_vect(128, sizeof(uint32_t));
      ft_heap_clip(heap, needSpace, &image->addr);
      ft_cmd_inflane(&loadImage, image->addr, image->picture->bin, image->picture->size);
      ft_hal_wr_into_cmd(dev, &loadImage);
      while(ft_hal_fifo_empty(dev) != bTrue);
      image->state = ftImageLoad;
    }
    case ftImageLoad: {
      FT_Picture const *pic = image->picture;
      image->handle = ft_screen_bitmap_handle(image->parent->screen);
      if(image->handle < 0)
        break;
      ft_cmd_dl_set(cmds, ft_dl_bitmap_handle(image->handle));
      ft_cmd_dl_set(cmds, ft_dl_bitmap_source(image->addr));
      ft_cmd_dl_set(cmds, ft_dl_bitmap_layout(pic->format, pic->stride, pic->resolution.h));
      ft_cmd_dl_set(cmds, ft_dl_bitmap_size(image->filter, image->wrap.x, image->wrap.y,
                                                  pic->resolution.w, pic->resolution.h));
      image->state = ftImageTake;
    }
    case ftImageTake:
      ft_cmd_dl_set(cmds, ft_dl_save_context());
      ft_cmd_dl_set(cmds, ft_dl_color_a(image->color.alpha));
      ft_cmd_dl_set(cmds, ft_dl_color_rgb(image->color.red, image->color.green, image->color.blue));
      ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimBitmaps));
      ft_cmd_dl_set(cmds, ft_dl_bitmap_handle(image->handle));
      ft_cmd_dl_set(cmds, ft_dl_vertex2f(absoluteCoord.x*16, absoluteCoord.y*16));
      ft_cmd_dl_set(cmds, ft_dl_end());
      ft_cmd_dl_set(cmds, ft_dl_restore_context());
  }
}

void ft_image_destroy(FT_Image *image)
{
  if(image->addr != 0)
    ft_heap_remove(&image->parent->screen->core->heap, image->addr);
}

void ft_image_set_picture(FT_Image *image, FT_Picture const *picture)
{
  if(image->picture != picture) {
    ft_widget_semphr_take(image->parent);
    image->picture = picture;
    image->state = ftImageNone;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_filter(FT_Image *image, FT_Filter filter)
{
  if(image->filter != filter) {
    ft_widget_semphr_take(image->parent);
    image->filter = filter;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_wrapx(FT_Image *image, FT_Wrap x)
{
  if(image->wrap.x != x) {
    ft_widget_semphr_take(image->parent);
    image->wrap.x = x;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_wrapy(FT_Image *image, FT_Wrap y)
{
  if(image->wrap.y != y) {
    ft_widget_semphr_take(image->parent);
    image->wrap.y = y;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_x(FT_Image *image, int16_t x)
{
  if(image->coord.x != x) {
    ft_widget_semphr_take(image->parent);
    image->coord.x = x;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_y(FT_Image *image, int16_t y)
{
  if(image->coord.y != y) {
    ft_widget_semphr_take(image->parent);
    image->coord.y = y;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_color(FT_Image *image, FT_Color color)
{
  if(image->color.value != color.value) {
    ft_widget_semphr_take(image->parent);
    image->color.value = color.value;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}

void ft_image_set_coord(FT_Image *image, int16_t x, int16_t y)
{
  if(image->coord.x != x || image->coord.y != y) {
    ft_widget_semphr_take(image->parent);
    image->coord.x = x;
    image->coord.y = y;
    image->state = image->state == ftImageTake ? ftImageLoad : image->state;
    ft_screen_bitmap_handle_clear(image->parent->screen, image->handle);
    image->handle = -1;
    ft_widget_set_modified(image->parent);
    ft_widget_semphr_give(image->parent);
  }
}
