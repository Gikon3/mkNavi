#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"

void ft_shape_point(Vector *cmds, int16_t x, int16_t y)
{
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimPoints));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x*16, y*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
}

void ft_shape_line(Vector *cmds, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x0*16, y0*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x1*16, y1*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
}

void ft_shape_rectangle(Vector *cmds, int16_t x, int16_t y, int16_t w, int16_t h)
{
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x*16, y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+w)*16, (y+h)*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
}

void ft_shape_rectangle_outline(Vector *cmds, int16_t x, int16_t y, int16_t w, int16_t h)
{
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimLineStrip));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x*16, y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+w)*16, y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((x+w)*16, (y+h)*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x*16, (y+h)*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(x*16, y*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
}
