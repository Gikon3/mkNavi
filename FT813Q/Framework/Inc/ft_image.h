#ifndef FRAMEWORK_INC_FT_IMAGE_H_
#define FRAMEWORK_INC_FT_IMAGE_H_

#include "util_vector.h"
#include "ft_widget_impl.h"
#include "ft_image_impl.h"

void ft_image_init(FT_Image *image, FT_Widget *widget);
void ft_image_append(FT_Image *image, Vector *cmds);
void ft_image_destroy(FT_Image *image);
void ft_image_set_picture(FT_Image *image, FT_Picture const *picture);
void ft_image_set_filter(FT_Image *image, FT_Filter filter);
void ft_image_set_wrapx(FT_Image *image, FT_Wrap x);
void ft_image_set_wrapy(FT_Image *image, FT_Wrap y);
void ft_image_set_x(FT_Image *image, int16_t x);
void ft_image_set_y(FT_Image *image, int16_t y);
void ft_image_set_color(FT_Image *image, FT_Color color);
void ft_image_set_coord(FT_Image *image, int16_t x, int16_t y);

#endif /* FRAMEWORK_INC_FT_IMAGE_H_ */
