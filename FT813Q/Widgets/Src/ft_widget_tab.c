#include "util_vector.h"
#include "util_dlist.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"
#include "ft_screen.h"
#include "ft_widget.h"
#include "ft_widget_tab.h"

#define MARGIN      4
#define PLUS_W      4
#define CROSS_W     2
#define TAB_MIN_W   100
#define TAB_MAX_W   200
#define TAB_BAR_H   42

struct _Tab;

typedef struct
{
  FT_WIDGET_HEADER
  int16_t       font;
  FT_TextCut    cut;
  uint8_t       maxTabs;
  Bool          closeableTabs;
  FT_Dimensions tabDimensions;
  struct
  {
    FT_Color    active;
    FT_Color    inactive;
    struct
    {
      FT_Color  active;
      FT_Color  inactive;
    } name;
  } tabColor;
  struct _Tab   *selected;
} TabBar;

static TabBar* tab_bar_create(FT_Widget *owner)
{
  TabBar *widget = (TabBar*)ft_widget_create(owner, sizeof(TabBar));
  widget->parent.dimensions.h = TAB_BAR_H;
  widget->font = FT_WIDGET_DEFAULT_FONT;
  widget->cut = ftTextCutFront;
  widget->maxTabs = 1;
  widget->closeableTabs = bTrue;
  widget->tabDimensions.w = TAB_MAX_W;
  widget->tabDimensions.h = TAB_BAR_H - 2;
  widget->tabColor.active.value = 0xFF;
  widget->tabColor.inactive.value = 0x808080FF;
  widget->tabColor.name.active.value = 0xFFFFFFFF;
  widget->tabColor.name.inactive.value = 0xFFFFFFA0;
  widget->selected = NULL;
  return widget;
}

typedef struct _Tab
{
  FT_WIDGET_HEADER
  FT_Color    textColor;
  String      text;
  FT_Widget   *capture;
  uint8_t     touched;
} Tab;

static void select_tab(FT_WidgetTab *window, Tab *tab)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&window->parent.widgets));
  if(tabBar->selected == tab)
    return;
  if(tabBar->selected) {
    tabBar->selected->textColor = tabBar->tabColor.name.inactive;
    ft_widget_set_color(&tabBar->selected->parent, tabBar->tabColor.inactive.red,
        tabBar->tabColor.inactive.green, tabBar->tabColor.inactive.blue, bFalse);
    dlist_pop_back(&window->parent.widgets, NULL);
  }
  if(tab) {
    tab->textColor = tabBar->tabColor.name.active;
    ft_widget_set_color(&tab->parent, tabBar->tabColor.active.red,
        tabBar->tabColor.active.green, tabBar->tabColor.active.blue, bFalse);
    dlist_push_back(&window->parent.widgets, &tab->capture);
  }
  tabBar->selected = tab;
}

static void tab_press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Tab *tab = (Tab*)widget;
  const Bool redraw = tab->touched == 0 ? bTrue : bFalse;
  ++tab->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void tab_release(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Tab *tab = (Tab*)widget;
  --tab->touched;
  if(tab->touched == 0) {
    FT_WidgetTab *window = (FT_WidgetTab*)tab->owner->owner;
    select_tab(window, tab);
    ft_screen_draw(widget->screen);
  }
}

static void tab_reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Tab *tab = (Tab*)widget;
  --tab->touched;
  if(tab->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void tab_compile(FT_Widget *widget, Vector *cmds)
{
  Tab *tab = (Tab*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  ft_shape_rectangle(cmds, widget->absoluteCoord.x, widget->absoluteCoord.y,
      widget->dimensions.w, widget->dimensions.h);
  if(str_cdata(&tab->text)) {
    TabBar const *tabBar = (TabBar const*)tab->owner;
    FT_Font const *fontTable = ft_widget_font_table(widget);
    const uint8_t height = fontTable->height[tabBar->font-16];
    const int16_t dy = (widget->dimensions.h - height) / 2;
    const int16_t crossW = widget->dimensions.h / 2 + MARGIN * 2;
    const uint16_t cutLimitW = tabBar->closeableTabs == bTrue ? widget->dimensions.w - crossW - MARGIN :
        widget->dimensions.w - MARGIN;
    ft_cmd_dl_set(cmds, ft_dl_color_a(tab->textColor.alpha));
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(tab->textColor.red, tab->textColor.green, tab->textColor.blue));
    char *textCut = ft_util_text_cut(fontTable, tabBar->font, &tab->text, tabBar->cut,
        cutLimitW, widget->dimensions.h);
    ft_cmd_text(cmds, widget->absoluteCoord.x+MARGIN, widget->absoluteCoord.y+dy, tabBar->font, 0,
        tabBar->cut == ftTextNoCut ? str_cdata(&tab->text) : textCut);
    ft_util_free(textCut);
  }
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static void tab_clear(FT_Widget *widget)
{
  Tab *context = (Tab*)widget;
  TabBar const *tabBar = (TabBar const*)context->owner;
  if(context != tabBar->selected)
    ft_widget_destroy(context->capture);
  str_clear(&context->text);
}

struct _Cross;
static struct _Cross* cross_create(FT_Widget *owner);

static Tab* tab_create(FT_Widget *owner, const char *name, FT_Widget *body)
{
  TabBar const *tabBar = (TabBar const*)owner;
  Tab *widget = (Tab*)ft_widget_create(owner, sizeof(Tab));
  widget->parent.touch.slot.common.press = tab_press;
  widget->parent.touch.slot.common.release = tab_release;
  widget->parent.touch.slot.common.reset = tab_reset;
  widget->parent.touch.type = ftWidgetTouchUsual;
  widget->parent.action.compile = tab_compile;
  widget->parent.action.clear = tab_clear;
  widget->parent.defaultColor = bFalse;
  widget->parent.color = tabBar->tabColor.inactive;
  widget->parent.coord.x += (dlist_size(&owner->widgets) - 1) * tabBar->tabDimensions.w;
  widget->parent.dimensions.w = tabBar->tabDimensions.w;
  widget->parent.dimensions.h = tabBar->tabDimensions.h;
  widget->textColor = tabBar->tabColor.name.inactive;
  if(name && name[0] != '\0')
    widget->text = make_str(name);
  else
    widget->text = make_str_null();
  widget->capture = body;
  widget->touched = 0;
  ft_widget_calculate_absolute_coord(&widget->parent);
  if(tabBar->closeableTabs == bTrue)
    cross_create(&widget->parent);
  return widget;
}

typedef struct _Cross
{
  FT_WIDGET_HEADER
  uint8_t touched;
} Cross;

static void cross_press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Cross *cross = (Cross*)widget;
  const Bool redraw = cross->touched == 0 ? bTrue : bFalse;
  ++cross->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void cross_release(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Cross *cross = (Cross*)widget;
  --cross->touched;
  if(cross->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void cross_reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  Cross *cross = (Cross*)widget;
  --cross->touched;
  if(cross->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void cross_compile(FT_Widget *widget, Vector *cmds)
{
  Tab const *tab = (Tab const*)widget->owner;
  const int16_t w = tab->parent.dimensions.h / 2;
  widget->coord.x = tab->parent.dimensions.w - MARGIN - w;
  widget->coord.y = (tab->parent.dimensions.h - w) / 2;
  ft_widget_calculate_absolute_coord(widget);
  ft_widget_calculate_dimensions(widget);

  Cross const *cross = (Cross const*)widget;
  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_touch_color_cmds(widget, cmds);
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimLines));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+w)*16, (widget->absoluteCoord.y+w)*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+w)*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, (widget->absoluteCoord.y+w)*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
  ft_cmd_dl_set(cmds, ft_dl_color_a(0));
  ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f(widget->absoluteCoord.x*16, widget->absoluteCoord.y*16));
  ft_cmd_dl_set(cmds, ft_dl_vertex2f((widget->absoluteCoord.x+w)*16, (widget->absoluteCoord.y+w)*16));
  ft_cmd_dl_set(cmds, ft_dl_end());
  if(cross->touched > 0) {
    const FT_Coord center = {
        .x = widget->absoluteCoord.x + w / 2,
        .y = widget->absoluteCoord.y + w / 2
    };
    const uint16_t radius = 0.7 * w;   //< 2^(1/2) / 2 * w
    ft_cmd_dl_set(cmds, ft_dl_color_a(80));
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(0x7E, 0, 0));
    ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimPoints));
    ft_cmd_dl_set(cmds, ft_dl_point_size(radius*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(center.x*16, center.y*16));
    ft_cmd_dl_set(cmds, ft_dl_end());
  }
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

static Cross* cross_create(FT_Widget *owner)
{
  Cross *widget = (Cross*)ft_widget_create(owner, sizeof(Cross));
  widget->parent.touch.slot.common.press = cross_press;
  widget->parent.touch.slot.common.release = cross_release;
  widget->parent.touch.slot.common.reset = cross_reset;
  widget->parent.touch.type = ftWidgetTouchUsual;
  widget->parent.action.compile = cross_compile;
  widget->parent.defaultColor = bFalse;
  widget->parent.color.value = 0xFF0000FF;
  widget->parent.dimensions.w = 0;
  widget->parent.dimensions.h = 0;
  widget->touched = 0;
  return widget;
}

static void press(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetTab *tab = (FT_WidgetTab*)widget;
  const Bool redraw = tab->touched == 0 ? bTrue : bFalse;
  ++tab->touched;
  if(redraw == bTrue) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void release(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetTab *tab = (FT_WidgetTab*)widget;
  ft_widget_semphr_take(widget);
  --tab->touched;
  if(tab->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
  ft_widget_semphr_give(widget);
}

static void reset(FT_Widget *widget, FT_WidgetSlotParam *arg)
{
  FT_WidgetTab *tab = (FT_WidgetTab*)widget;
  --tab->touched;
  if(tab->touched == 0) {
    ft_widget_set_modified(widget);
    ft_screen_draw(widget->screen);
  }
}

static void compile(FT_Widget *widget, Vector *cmds)
{
  FT_WidgetTab *window = (FT_WidgetTab*)widget;
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->widgets));
  const uint8_t numberTabs = dlist_size(&tabBar->parent.widgets);

  ft_cmd_dl_set(cmds, ft_dl_save_context());
  ft_widget_insert_color_cmds(widget, cmds);
  const int16_t lineY = widget->absoluteCoord.y + tabBar->parent.dimensions.h;
  ft_shape_line(cmds, widget->absoluteCoord.x, lineY, widget->absoluteCoord.x+widget->dimensions.w, lineY);

  if(numberTabs < tabBar->maxTabs) {
    const int16_t allTabsW = widget->absoluteCoord.x + numberTabs * tabBar->tabDimensions.w;
    const FT_Coord v0 = {
        .x = allTabsW + (tabBar->tabDimensions.h - PLUS_W) / 2,
        .y = widget->absoluteCoord.y + MARGIN
    };
    const FT_Coord v1 = {
        .x = v0.x + PLUS_W,
        .y = widget->absoluteCoord.y + tabBar->tabDimensions.h - MARGIN
    };
    const FT_Coord h0 = {
        .x = allTabsW + MARGIN,
        .y = widget->absoluteCoord.y + (tabBar->tabDimensions.h - PLUS_W) / 2
    };
    const FT_Coord h1 = {
        .x = allTabsW + tabBar->tabDimensions.h - MARGIN,
        .y = h0.y + PLUS_W
    };

    ft_widget_insert_touch_cmds(widget, cmds);
    ft_cmd_dl_set(cmds, ft_dl_begin(ftPrimRect));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(v0.x*16, v0.y*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(v1.x*16, v1.y*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(h0.x*16, h0.y*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(h1.x*16, h1.y*16));
    ft_cmd_dl_set(cmds, ft_dl_color_a(0));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(h0.x*16, v0.y*16));
    ft_cmd_dl_set(cmds, ft_dl_vertex2f(h1.x*16, v1.y*16));
    ft_cmd_dl_set(cmds, ft_dl_end());
    if(window->touched > 0) {
      const FT_Coord center = {
          .x = allTabsW + tabBar->tabDimensions.h / 2,
          .y = widget->absoluteCoord.y + tabBar->tabDimensions.h / 2
      };
      const uint16_t radius = tabBar->tabDimensions.h / 2;
      ft_cmd_dl_set(cmds, ft_dl_color_a(80));
      ft_cmd_dl_set(cmds, ft_dl_color_rgb(0x7E, 0x7E, 0x7E));
      ft_cmd_dl_set(cmds, ft_dl_point_size(radius*16));
      ft_shape_point(cmds, center.x, center.y);
    }
  }
  ft_cmd_dl_set(cmds, ft_dl_restore_context());
}

FT_WidgetTab* ft_widget_tab_create(FT_Widget *owner)
{
  FT_WidgetTab *widget = (FT_WidgetTab*)ft_widget_create(owner, sizeof(FT_WidgetTab));
  widget->parent.touch.slot.common.press = press;
  widget->parent.touch.slot.common.release = release;
  widget->parent.touch.slot.common.reset = reset;
  widget->parent.touch.type = ftWidgetTouchUsual;
  widget->parent.action.compile = compile;
  tab_bar_create(&widget->parent);
  widget->touched = 0;
  return widget;
}

void ft_widget_tab_delete_slot_connect(FT_WidgetTab *widget, FT_Widget const *body, FT_WidgetUserSlot slot, void *arg)
{
  ft_widget_semphr_take(&widget->parent);
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  DListContainer *container = dlist_front(&tabBar->parent.widgets);
  while(container) {
    Tab *tab = *(Tab**)dlist_item(container);
    if(tab->capture == body) {
      Cross *cross = *(Cross**)dlist_item(dlist_front(&tab->parent.widgets));
      cross->parent.touch.slot.user.release = slot;
      cross->parent.touch.slot.user.arg = arg;
      break;
    }
    container = dlist_next(container);
  }
  ft_widget_semphr_give(&widget->parent);
}

void ft_widget_tab_set_font(FT_WidgetTab *widget, int16_t font)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->font != font) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->font = font;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_cut(FT_WidgetTab *widget, FT_TextCut cut)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->cut != cut) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->cut = cut;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_color_active_tab(FT_WidgetTab *widget, FT_Color color)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->tabColor.active.value != color.value) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->tabColor.active = color;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_color_inactive_tab(FT_WidgetTab *widget, FT_Color color)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->tabColor.inactive.value != color.value) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->tabColor.inactive = color;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_color_active_tab_name(FT_WidgetTab *widget, FT_Color color)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->tabColor.name.active.value != color.value) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->tabColor.name.active = color;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_color_inactive_tab_name(FT_WidgetTab *widget, FT_Color color)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->tabColor.name.inactive.value != color.value) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->tabColor.name.inactive = color;
    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_max_tabs(FT_WidgetTab *widget, uint8_t maxTabs)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->maxTabs != maxTabs) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->maxTabs = maxTabs;
    if(dlist_size(&tabBar->parent.widgets) >= maxTabs)
      ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_closeable_tabs(FT_WidgetTab *widget, Bool closeableTabs)
{
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(tabBar->closeableTabs != closeableTabs) {
    ft_widget_semphr_take(&tabBar->parent);
    tabBar->closeableTabs = closeableTabs;

    DListContainer *container = dlist_front(&tabBar->parent.widgets);
    while(container) {
      Tab *tab = *(Tab**)dlist_item(container);
      if(closeableTabs == bFalse) {
        Cross *cross = NULL;
        dlist_pop_front(&tab->parent.widgets, &cross);
        ft_widget_destroy(&cross->parent);
      }
      else
        cross_create(&tab->parent);
      ft_widget_set_modified(&tab->parent);
      container = dlist_next(container);
    }

    ft_widget_set_modified(&tabBar->parent);
    ft_widget_semphr_give(&tabBar->parent);
  }
}

void ft_widget_tab_set_tab_name(FT_WidgetTab *window, FT_Widget *body, char const *newName)
{
  ft_widget_semphr_take(&window->parent);
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&window->parent.widgets));
  DListContainer *container = dlist_front(&tabBar->parent.widgets);
  while(container) {
    Tab *tab = *(Tab**)dlist_item(container);
    if(tab->capture == body) {
      str_clear(&tab->text);
      tab->text = make_str(newName);
      ft_widget_set_modified(&tab->parent);
      break;
    }
    container = dlist_next(container);
  }
  ft_widget_set_modified(&window->parent);
  ft_widget_semphr_give(&window->parent);
}

static Bool correct_tab_width(TabBar *tabBar)
{
  const uint8_t numberTabs = dlist_size(&tabBar->parent.widgets) + 1;
  int16_t tabWidgth = (tabBar->parent.dimensions.w - tabBar->parent.dimensions.h) / numberTabs;
  if(tabWidgth < TAB_MIN_W)
    return bFalse;
  tabWidgth = tabWidgth > TAB_MAX_W ? TAB_MAX_W : tabWidgth;
  tabBar->tabDimensions.w = tabWidgth;
  DListContainer *container = dlist_front(&tabBar->parent.widgets);
  uint8_t count = 0;
  while(container) {
    FT_Widget *nested = *(FT_Widget**)dlist_item(container);
    ft_widget_set_x(nested, count++*tabWidgth);
    ft_widget_set_w(nested, tabWidgth);
    // Изменение состояние креста на NotCompile
    if(tabBar->closeableTabs == bTrue)
      ft_widget_set_modified(*(FT_Widget**)dlist_item(dlist_front(&nested->widgets)));
    container = dlist_next(container);
  }
  return bTrue;
}

FT_Widget* ft_widget_tab_new_tab(FT_WidgetTab *widget, char const *tabName)
{
  FT_Widget *body = NULL;
  ft_widget_semphr_take(&widget->parent);
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  if(correct_tab_width(tabBar) == bTrue) {
    // В качестве владельца NULL, чтобы виджет не добавился в список
    body = ft_widget_create(NULL, 0);
    body->coord.y = tabBar->parent.dimensions.h;
    ft_widget_set_dependent_param(body, &widget->parent);
    ft_widget_calculate_absolute_coord_recursive(body);

    Tab *tab = tab_create(&tabBar->parent, tabName, body);
    select_tab(widget, tab);
    ft_widget_set_modified(&widget->parent);
  }
  ft_widget_semphr_give(&widget->parent);
  return body;
}

static void tab_erase(FT_WidgetTab *window, TabBar *tabBar, Tab *tab, DListContainer *container)
{
  if(tab == tabBar->selected) {
    DListContainer *containerNewSelect = dlist_prev(container);
    if(containerNewSelect)
      select_tab(window, *(Tab**)dlist_item(containerNewSelect));
    else {
      containerNewSelect = dlist_next(container);
      Tab *newSelectTab = containerNewSelect ? *(Tab**)dlist_item(containerNewSelect) : NULL;
      select_tab(window, newSelectTab);
    }
  }

  dlist_erase(&tabBar->parent.widgets, container, NULL);
  ft_widget_destroy(&tab->parent);
  correct_tab_width(tabBar);
}

void ft_widget_tab_delete_tab(FT_WidgetTab *widget, FT_Widget *body)
{
  ft_widget_semphr_take(&widget->parent);
  TabBar *tabBar = *(TabBar**)dlist_item(dlist_front(&widget->parent.widgets));
  DListContainer *container = dlist_front(&tabBar->parent.widgets);
  while(container) {
    Tab *tab = *(Tab**)dlist_item(container);
    if(tab->capture == body) {
      tab_erase(widget, tabBar, tab, container);
      break;
    }
    container = dlist_next(container);
  }
  ft_widget_set_modified(&widget->parent);
  ft_widget_semphr_give(&widget->parent);
}
