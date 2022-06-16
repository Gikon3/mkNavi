#include <string.h>
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"
#include "ft_shape.h"
#include "ft_hal.h"
#include "ft_heap.h"
#include "ft_touch.h"
#include "ft_screen.h"
#include "ft_keyboard.h"
#include "ft_widget_line_edit.h"

#define BACKSPACE_TAG 0x08  /* [BAKCSPACE] */
#define RETURN_TAG    0x0A  /* [LF] */
#define NUMERIC_TAG   0x11  /* [DC1] */
#define SYMBOLIC_TAG  0x12  /* [DC2] */
#define SHIFT_TAG     0x7F  /* [DEL] */
#define KEY_MARGIN    3

typedef struct
{
  union
  {
    struct
    {
      struct
      {
        char const    *lower[3];
        char const    *upper[3];
      } main;
      char const      *numeric[3];
      char const      *symbolic[2];
      struct
      {
        struct
        {
          char const  *lower;
          char const  *upper;
          char const  *alwaysUpper;
        } shift;
        char const    *backspace;
        struct
        {
          char const  *symbolic;
          char const  *numeric;
          char const  *main;
        } swap;
        char const    *enter;
      } special;
    };
    char const        *key[19];
  };
} KeyRows;

static const KeyRows keyRows = {
    .main.lower = {
        "qwertyuiop",
        "asdfghjkl",
        "zxcvbnm"
    },
    .main.upper = {
        "QWERTYUIOP",
        "ASDFGHJKL",
        "ZXCVBNM"
    },
    .numeric = {
        "1234567890",
        "-/:;()$&@\"",
        ".,?!'"
    },
    .symbolic = {
        "[]{}#%^*+=",
        "_\\|~<>$"
    },
    .special.shift = {
        .lower = "shift",
        .upper = "SHIFT",
        .alwaysUpper = "CapsLock"
    },
    .special.backspace = "<--",
    .special.swap = {
        .symbolic = "#+=",
        .numeric = "123",
        .main = "abc"
    },
    .special.enter = "return"
};

void set_modified(FT_Keyboard *keyboard)
{
  if(keyboard->state == ftContextHeapLoad) {
    ft_heap_remove(&keyboard->screen->core->heap, keyboard->heap.property.addr);
    keyboard->heap.property.addr = 0;
    keyboard->heap.property.size = 0;
  }
  keyboard->state = ftContextNotCompile;
  ft_screen_draw(keyboard->screen);
}

void ft_keyboard_press(FT_Keyboard *keyboard, uint16_t tag)
{
  if(tag == SHIFT_TAG)
    switch(keyboard->caseManagement.confirmed) {
        case ftKeyboardCaseLower:
          keyboard->caseManagement.pending = ftKeyboardCaseUpper;
          break;
        case ftKeyboardCaseUpper:
        case ftKeyboardCaseAlwaysUpper:
          keyboard->caseManagement.pending = ftKeyboardCaseLower;
          break;
    }
  keyboard->pressKey = tag;
  set_modified(keyboard);
}

void ft_keyboard_long_press(FT_Keyboard *keyboard, uint16_t tag)
{
  if(tag == SHIFT_TAG) {
    switch(keyboard->caseManagement.confirmed) {
        case ftKeyboardCaseLower:
        case ftKeyboardCaseUpper:
          keyboard->caseManagement.pending = ftKeyboardCaseAlwaysUpper;
          break;
        case ftKeyboardCaseAlwaysUpper:
          keyboard->caseManagement.pending = ftKeyboardCaseLower;
          break;
    }
    keyboard->pressKey = tag;
    set_modified(keyboard);
  }
  else if(tag == BACKSPACE_TAG)
    xTimerStart(keyboard->timers.repeatBackspace, portMAX_DELAY);
}

void ft_keyboard_release(FT_Keyboard *keyboard, uint16_t tag)
{
  if(tag == NUMERIC_TAG)
    switch(keyboard->layout) {
      case ftKeyboardLayoutMain: keyboard->layout = ftKeyboardLayoutNumeric; break;
      case ftKeyboardLayoutNumeric:
      case ftKeyboardLayoutSymbolic: keyboard->layout = ftKeyboardLayoutMain; break;
  }
  else if(tag == SYMBOLIC_TAG)
    switch(keyboard->layout) {
      case ftKeyboardLayoutMain:
      case ftKeyboardLayoutNumeric: keyboard->layout = ftKeyboardLayoutSymbolic; break;
      case ftKeyboardLayoutSymbolic: keyboard->layout = ftKeyboardLayoutNumeric; break;
    }
  if(tag >= ' ' && tag <= '~' && keyboard->capture)
    ft_widget_line_edit_inser_char(keyboard->capture, tag);
  else if(tag == BACKSPACE_TAG) {
    if(xTimerIsTimerActive(keyboard->timers.repeatBackspace) == pdTRUE)
      xTimerStop(keyboard->timers.repeatBackspace, portMAX_DELAY);
    else
      ft_widget_line_edit_erase_char(keyboard->capture);
  }
  else if(tag == RETURN_TAG)
    ft_keyboard_detach(keyboard);
  keyboard->pressKey = '\0';
  if(tag == SHIFT_TAG)
    keyboard->caseManagement.confirmed = keyboard->caseManagement.pending;
  else if(keyboard->caseManagement.confirmed == ftKeyboardCaseUpper)
    keyboard->caseManagement.confirmed = keyboard->caseManagement.pending = ftKeyboardCaseLower;
  set_modified(keyboard);
}

void ft_keyboard_reset(FT_Keyboard *keyboard, uint16_t tag)
{
  keyboard->pressKey = '\0';
  keyboard->caseManagement.pending = keyboard->caseManagement.confirmed;
  set_modified(keyboard);
}

static void compile(FT_Keyboard *keyboard, Vector *cmds)
{
  ft_cmd_dl_set(cmds, ft_dl_save_context());

  if(keyboard->defaultColor == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_rgb(keyboard->color.red, keyboard->color.green, keyboard->color.blue));
  if(keyboard->defaultAlpha == bFalse)
    ft_cmd_dl_set(cmds, ft_dl_color_a(keyboard->color.alpha));

  const int16_t x = keyboard->coord.x;
  const int16_t y = keyboard->coord.y + KEY_MARGIN;
  const int16_t w = keyboard->dimensions.w;
  const int16_t keyH = keyboard->dimensions.h / 4 - KEY_MARGIN;
  const int16_t keyW = keyboard->dimensions.w / 10;

  ft_shape_rectangle(cmds, x, keyboard->coord.y, w, keyboard->dimensions.h);

  const int16_t font = keyboard->font;
  const uint16_t keyOptions = keyboard->pressKey;
  switch(keyboard->layout) {
    case ftKeyboardLayoutMain: {
      const uint8_t index = keyboard->caseManagement.confirmed == ftKeyboardCaseUpper ||
          keyboard->caseManagement.confirmed == ftKeyboardCaseAlwaysUpper ? 3 : 0;
      ft_cmd_keys(cmds, x, y, w, keyH, font, keyOptions, keyRows.key[index]);
      ft_cmd_keys(cmds, x+keyW/2, y+keyH+KEY_MARGIN, w-keyW, keyH, font, keyOptions, keyRows.key[index+1]);
      ft_cmd_keys(cmds, x+keyW+keyW/2, y+(keyH+KEY_MARGIN)*2, w-keyW*3, keyH, font, keyOptions, keyRows.key[index+2]);

      char const *shiftText = NULL;
      if(keyboard->caseManagement.confirmed == ftKeyboardCaseLower)
        shiftText = keyRows.special.shift.lower;
      else if(keyboard->caseManagement.confirmed == ftKeyboardCaseUpper)
        shiftText = keyRows.special.shift.upper;
      else
        shiftText = keyRows.special.shift.alwaysUpper;
      ft_cmd_dl_set(cmds, ft_dl_tag(SHIFT_TAG));
      ft_cmd_button(cmds, x, y+(keyH+KEY_MARGIN)*2, keyW+keyW/4-KEY_MARGIN, keyH, font,
          keyboard->pressKey == SHIFT_TAG ? FT_OPT_FLAT : FT_OPT_3D, shiftText);
      ft_cmd_keys(cmds, x+keyW*1.25, y+(keyH+KEY_MARGIN)*3, keyW-KEY_MARGIN, keyH, font, keyOptions, ".");
      break;
    }
    case ftKeyboardLayoutNumeric:
      ft_cmd_keys(cmds, x, y, w, keyH, font, keyOptions, keyRows.numeric[0]);
      ft_cmd_keys(cmds, x, y+keyH+KEY_MARGIN, w, keyH, font, keyOptions, keyRows.numeric[1]);
      ft_cmd_keys(cmds, x+keyW*1.5, y+(keyH+KEY_MARGIN)*2, keyW*7, keyH, font, keyOptions, keyRows.numeric[2]);

      ft_cmd_dl_set(cmds, ft_dl_tag(SYMBOLIC_TAG));
      ft_cmd_button(cmds, x+keyW*1.25, y+(keyH+KEY_MARGIN)*3, keyW-KEY_MARGIN, keyH, font,
          keyboard->pressKey == SYMBOLIC_TAG ? FT_OPT_FLAT : FT_OPT_3D, keyRows.special.swap.symbolic);
      break;
    case ftKeyboardLayoutSymbolic:
      ft_cmd_keys(cmds, x, y, w, keyH, font, keyOptions, keyRows.symbolic[0]);
      ft_cmd_keys(cmds, x+keyW/2, y+keyH+KEY_MARGIN, keyW*9, keyH, font, keyOptions, keyRows.symbolic[1]);
      ft_cmd_keys(cmds, x+keyW*1.5, y+(keyH+KEY_MARGIN)*2, keyW*7, keyH, font, keyOptions, keyRows.numeric[2]);

      ft_cmd_dl_set(cmds, ft_dl_tag(SYMBOLIC_TAG));
      ft_cmd_button(cmds, x+keyW*1.25, y+(keyH+KEY_MARGIN)*3, keyW-KEY_MARGIN, keyH, font,
          keyboard->pressKey == SYMBOLIC_TAG ? FT_OPT_FLAT : FT_OPT_3D, keyRows.special.swap.numeric);
      break;
  }

  ft_cmd_dl_set(cmds, ft_dl_tag(BACKSPACE_TAG));
  ft_cmd_button(cmds, x+keyW*8.75+KEY_MARGIN, y+(keyH+KEY_MARGIN)*2, keyW+keyW/4-KEY_MARGIN, keyH, font,
      keyboard->pressKey == BACKSPACE_TAG ? FT_OPT_FLAT : FT_OPT_3D, keyRows.special.backspace);

  ft_cmd_dl_set(cmds, ft_dl_tag(NUMERIC_TAG));
  ft_cmd_button(cmds, x, y+(keyH+KEY_MARGIN)*3, keyW-KEY_MARGIN, keyH, font,
      keyboard->pressKey == NUMERIC_TAG ? FT_OPT_FLAT : FT_OPT_3D,
      keyboard->layout == ftKeyboardLayoutMain ? keyRows.special.swap.numeric : keyRows.special.swap.main);

  ft_cmd_keys(cmds, x+keyW*2.5, y+(keyH+KEY_MARGIN)*3, keyW*5, keyH, font, keyOptions, " ");

  ft_cmd_dl_set(cmds, ft_dl_tag(RETURN_TAG));
  ft_cmd_button(cmds, x+keyW*7.75+KEY_MARGIN, y+(keyH+KEY_MARGIN)*3, keyW*2.25-KEY_MARGIN, keyH, font,
      keyboard->pressKey == RETURN_TAG ? FT_OPT_FLAT : FT_OPT_3D, keyRows.special.enter);

  ft_cmd_dl_set(cmds, ft_dl_restore_context());

  keyboard->nextChar = '\0';
}

void ft_keyboard_compile(FT_Keyboard *keyboard, Vector *cmds)
{
  if(keyboard->visible == bFalse)
    return;

  switch(keyboard->state) {
    case ftContextNotCompile: {
      if(keyboard->heap.enable == bFalse) {
        compile(keyboard, cmds);
        break;
      }
      Vector heapCmds = make_vect(62, sizeof(uint32_t));
      compile(keyboard, &heapCmds);
      if(vect_size(&heapCmds) == 0) {
        vect_clear(&heapCmds);
        break;
      }

      // Выделение памяти и запись в диспелей
      FT_Heap *heap = &keyboard->screen->core->heap;
      if(keyboard->heap.property.addr != 0)
        ft_heap_remove(heap, keyboard->heap.property.addr);
      ft_heap_append_own(heap, &heapCmds, &keyboard->heap.property);
    }
    case ftContextHeapLoad:
      // Добавление в общий вектор команды append
      ft_cmd_append(cmds, keyboard->heap.property.addr, keyboard->heap.property.size);
      keyboard->state = ftContextHeapLoad;
      break;
  }
}

static void tim_repeat_backspace(TimerHandle_t tim)
{
  FT_Keyboard *keyboard = pvTimerGetTimerID(tim);
  ft_widget_line_edit_erase_char(keyboard->capture);
  set_modified(keyboard);
}

void ft_keyboard_init(FT_Keyboard *keyboard, FT_Screen *screen)
{
  keyboard->screen = screen;
  keyboard->capture = NULL;
  keyboard->timers.repeatBackspace = xTimerCreate("ftRepeatBackspace", pdMS_TO_TICKS(150),
      pdTRUE, keyboard, tim_repeat_backspace);
  keyboard->heap.enable = bTrue;
  keyboard->heap.property.addr = 0;
  keyboard->heap.property.size = 0;
  keyboard->state = ftContextNotCompile;
  keyboard->visible = bFalse;
  keyboard->defaultColor = bTrue;
  keyboard->defaultAlpha = bTrue;
  keyboard->color.value = 0xFF;
  keyboard->coord.x = 0;
  keyboard->coord.y = 300;
  keyboard->dimensions.w = 800;
  keyboard->dimensions.h = 180;
  keyboard->font = 28;
  keyboard->layout = ftKeyboardLayoutMain;
  keyboard->caseManagement.pending = ftKeyboardCaseLower;
  keyboard->caseManagement.confirmed = ftKeyboardCaseLower;
  keyboard->pressKey = '\0';
  keyboard->nextChar = '\0';
}

void ft_keyboard_destroy(FT_Keyboard *keyboard)
{
  xTimerDelete(keyboard->timers.repeatBackspace, portMAX_DELAY);
  if(keyboard->heap.property.addr != 0)
    ft_heap_remove(&keyboard->screen->core->heap, keyboard->heap.property.addr);
}

void ft_keyboard_attach(FT_Keyboard *keyboard, FT_WidgetLineEdit *lineEdit)
{
  if(lineEdit != keyboard->capture) {
    ft_keyboard_detach(keyboard);
    ft_widget_line_edit_set_enable(lineEdit, bTrue);
    keyboard->capture = lineEdit;
    keyboard->visible = bTrue;
    set_modified(keyboard);
  }
}

void ft_keyboard_detach(FT_Keyboard *keyboard)
{
  if(keyboard->capture) {
    ft_widget_line_edit_set_enable(keyboard->capture, bFalse);
    keyboard->capture = NULL;
    keyboard->visible = bFalse;
    set_modified(keyboard);
  }
}
