#ifndef RXVT_H_                /* include once only */
#define RXVT_H_

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_STRREDIR_H
#include <sys/strredir.h>
#endif

#if HAVE_WCHAR_H
# include <wchar.h>
#else
// stdlib.h might provide it
#endif

#include <libptytty.h>

// we assume that Xlib.h defines XPointer, and it does since at least 1994...

extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
}

#if UNICODE_3
typedef uint32_t text_t;
#else
typedef uint16_t text_t; // saves lots of memory
#endif
typedef uint32_t rend_t;
typedef  int32_t tlen_t;  // was int16_t, but this results in smaller code and memory use
typedef  int32_t tlen_t_; // specifically for use in the line_t structure

#include "feature.h"

#if defined (ISO_14755) || defined (ENABLE_PERL)
# define ENABLE_OVERLAY 1
#endif

#if ENABLE_PERL
# define ENABLE_FRILLS    1
# define ENABLE_COMBINING 1
#endif

#if ENABLE_FRILLS
# define ENABLE_XEMBED        1
# define ENABLE_EWMH          1
# define ENABLE_XIM_ONTHESPOT 1
# define CURSOR_BLINK         1
# define OPTION_HC            1
# define BUILTIN_GLYPHS       1
#else
# define ENABLE_MINIMAL 1
#endif

#include <limits.h>

#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>

#if HAVE_PIXBUF
# include <gdk-pixbuf/gdk-pixbuf.h>
#endif

#if XRENDER && (HAVE_PIXBUF || ENABLE_TRANSPARENCY)
# define HAVE_IMG 1
#endif

#define ECB_NO_THREADS 1
#include "ecb.h"

#include "encoding.h"
#include "rxvtutil.h"
#include "rxvtfont.h"
#include "rxvttoolkit.h"
#include "rxvtimg.h"
#include "scrollbar.h"
#include "ev_cpp.h"

#include "rxvtperl.h"

// try to avoid some macros to decrease code size, on some systems
#if ENABLE_MINIMAL
# define strcmp(a,b)   (strcmp)(a,b)
# define strlen(a)     (strlen)(a)
# define strcpy(a,b)   (strcpy)(a,b)
# define memset(a,c,l) (memset)(a,c,l)
# define memcpy(a,b,l) (memcpy)(a,b,l)
#endif

/*
 *****************************************************************************
 * SYSTEM HACKS
 *****************************************************************************
 */

#include <termios.h>

#ifndef STDIN_FILENO
# define STDIN_FILENO   0
# define STDOUT_FILENO  1
# define STDERR_FILENO  2
#endif

#ifndef EXIT_SUCCESS            /* missing from <stdlib.h> */
# define EXIT_SUCCESS           0       /* exit function success */
# define EXIT_FAILURE           1       /* exit function failure */
#endif

/****************************************************************************/

// exception thrown on fatal (per-instance) errors
class rxvt_failure_exception : public std::exception
{
public:
  const char *
  what () const noexcept override
  {
    return "";
  }
};

// exception thrown when the command parser runs out of input data
class out_of_input { };

/*
 *****************************************************************************
 * PROTOTYPES
 *****************************************************************************
 */
// main.C
#define SET_LOCALE(locale) rxvt_set_locale (locale)
extern bool rxvt_set_locale (const char *locale) noexcept;
extern void rxvt_push_locale (const char *locale) noexcept;
extern void rxvt_pop_locale () noexcept;
void rxvt_init ();

// misc.C
char *           rxvt_wcstombs                    (const wchar_t *str, int len = -1);
wchar_t *        rxvt_mbstowcs                    (const char *str, int len = -1);
char *           rxvt_wcstoutf8                   (const wchar_t *str, int len = -1);
wchar_t *        rxvt_utf8towcs                   (const char *str, int len = -1);

const char *     rxvt_basename                    (const char *str) noexcept;
void             rxvt_vlog                        (const char *fmt, va_list arg_ptr) noexcept;
void             rxvt_log                         (const char *fmt,...) noexcept;
void             rxvt_warn                        (const char *fmt,...) noexcept;
ecb_noreturn ecb_cold
void             rxvt_fatal                       (const char *fmt, ...);
ecb_noreturn ecb_cold
void             rxvt_exit_failure                ();

void *           rxvt_malloc                      (size_t size);
void *           rxvt_calloc                      (size_t number, size_t size);
void *           rxvt_realloc                     (void *ptr, size_t size);

char *           rxvt_strtrim                     (char *str) noexcept;
char **          rxvt_strsplit                    (char delim, const char *str) noexcept;

static inline void
rxvt_free_strsplit (char **ptr) noexcept
{
  free (ptr[0]);
  free (ptr);
}

KeySym rxvt_XKeycodeToKeysym (Display *dpy, KeyCode keycode, int index);

/////////////////////////////////////////////////////////////////////////////

// temporarily replace the process environment
extern char **environ;
extern char **rxvt_environ; // the original environ pointer

static inline void
set_environ (char **envv)
{
#if ENABLE_PERL
  assert (envv);
#else
  if (envv)
#endif
    environ = envv;
}

struct localise_env
{
  char **orig_env;

  localise_env (char **new_env)
  {
    orig_env = environ;
    environ = new_env;
  }

  ~localise_env ()
  {
    environ = orig_env;
  }
};

/*
 *****************************************************************************
 * STRUCTURES AND TYPEDEFS
 *****************************************************************************
 */

/*
 * the 'essential' information for reporting Mouse Events
 * pared down from XButtonEvent
 */
struct mouse_event
{
  int clicks;
  Time time;             /* milliseconds */
  unsigned int state;    /* key or button mask */
  unsigned int button;   /* detail */
};

#if ENABLE_XEMBED
// XEMBED messages
# define XEMBED_EMBEDDED_NOTIFY          0
# define XEMBED_WINDOW_ACTIVATE          1
# define XEMBED_WINDOW_DEACTIVATE        2
# define XEMBED_REQUEST_FOCUS            3
# define XEMBED_FOCUS_IN                 4
# define XEMBED_FOCUS_OUT                5
# define XEMBED_FOCUS_NEXT               6
# define XEMBED_FOCUS_PREV               7

# define XEMBED_MODALITY_ON              10
# define XEMBED_MODALITY_OFF             11
# define XEMBED_REGISTER_ACCELERATOR     12
# define XEMBED_UNREGISTER_ACCELERATOR   13
# define XEMBED_ACTIVATE_ACCELERATOR     14

// XEMBED detail code
# define XEMBED_FOCUS_CURRENT            0
# define XEMBED_FOCUS_FIRST              1
# define XEMBED_FOCUS_LAST               2

# define XEMBED_MAPPED			(1 << 0)
#endif

/*
 *****************************************************************************
 * NORMAL DEFINES
 *****************************************************************************
 */

/* COLORTERM, TERM environment variables */
#define COLORTERMENV    "rxvt"
#if HAVE_IMG
# define COLORTERMENVFULL COLORTERMENV "-xpm"
#else
# define COLORTERMENVFULL COLORTERMENV
#endif
#ifndef TERMENV
# if USE_256_COLORS
#  define TERMENV        "rxvt-unicode-256color"
# else
#  define TERMENV        "rxvt-unicode"
# endif
#endif

// Hidden color cube for indexed 24-bit colors. There are fewer blue levels
// because normal human eye is less sensitive to the blue component than to
// the red or green. (https://en.m.wikipedia.org/wiki/Color_depth#8-bit_color)
#if USE_256_COLORS
// 7x7x5=245 < 254 unused color indices
# define Red_levels      7
# define Green_levels    7
# define Blue_levels     5
#else
// 6x6x4=144 < 166 unused color indices
# define Red_levels      6
# define Green_levels    6
# define Blue_levels     4
#endif

#define RGB24_CUBE_SIZE (Red_levels * Green_levels * Blue_levels)

#if defined (NO_MOUSE_REPORT) && !defined (NO_MOUSE_REPORT_SCROLLBAR)
# define NO_MOUSE_REPORT_SCROLLBAR 1
#endif

#define scrollBar_esc           30

#if !defined (RXVT_SCROLLBAR) && !defined (NEXT_SCROLLBAR)
# define NO_SCROLLBAR_BUTTON_CONTINUAL_SCROLLING 1
#endif

enum {
  NO_REFRESH       = 0,  /* Window not visible at all!        */
  FAST_REFRESH     = 1,  /* Fully exposed window              */
  SLOW_REFRESH     = 2,  /* Partially exposed window          */
};

#ifdef NO_SECONDARY_SCREEN
# define NSCREENS               0
#else
# define NSCREENS               1
#endif

/* flags for rxvt_term::scr_gotorc () */
enum {
  C_RELATIVE = 1,       /* col movement is relative */
  R_RELATIVE = 2,       /* row movement is relative */
  RELATIVE   = C_RELATIVE | R_RELATIVE,
};

/* modes for rxvt_term::scr_insdel_chars (), rxvt_term::scr_insdel_lines () */
enum {
  INSERT = -1,				/* don't change these values */
  DELETE = +1,
  ERASE  = +2,
};

/* modes for rxvt_term::scr_page () - scroll page. used by scrollbar window */
enum page_dirn {
  DN     = -1,
  NO_DIR =  0,
  UP     =  1,
};

/* arguments for rxvt_term::scr_change_screen () */
enum {
  PRIMARY = 0,
  SECONDARY,
};

// define various rendition bits and masks. the rendition word
// is 32 bits in size, and we should use it as efficiently as possible

#define RS_None                 0

// GET_BGATTR depends on RS_fgShift > RS_bgShift
#define RS_colorMask		((1UL << Color_Bits) - 1UL)
#define RS_bgShift		0
#define RS_fgShift		(RS_bgShift + Color_Bits)
#define RS_bgMask               (RS_colorMask << RS_bgShift)
#define RS_fgMask               (RS_colorMask << RS_fgShift)

// must have space for rxvt_fontset::fontCount * 2 + 2 values
#define RS_fontShift            (RS_fgShift + Color_Bits)
#define RS_Careful		(1UL << RS_fontShift)	/* be careful when drawing these */
#define RS_fontCount		rxvt_fontset::fontCount
#define RS_fontMask             ((RS_fontCount << (RS_fontShift + 1)) | RS_Careful)   // includes RS_Careful

// toggle this to force redraw, must be != RS_Careful and otherwise "pretty neutral"
#define RS_redraw		(2UL << RS_fontShift)

#define RS_Sel                  (1UL << 22)

// 4 custom bits for extensions
#define RS_customCount          16UL
#define RS_customShift          23
#define RS_customMask           ((RS_customCount - 1UL) << RS_customShift)

// font styles
#define RS_Bold                 (1UL << RS_styleShift)
#define RS_Italic		(2UL << RS_styleShift)

#define RS_styleCount		4
#define RS_styleShift		27
#define RS_styleMask		(RS_Bold | RS_Italic)

// fake styles
#define RS_Blink                (1UL << 29)
#define RS_RVid                 (1UL << 30)    // reverse video
#define RS_Uline                (1UL << 31)    // underline

#define RS_baseattrMask         (RS_Italic | RS_Bold | RS_Blink | RS_RVid | RS_Uline)
#define RS_attrMask             (RS_baseattrMask | RS_fontMask)

#define DEFAULT_RSTYLE  (RS_None | (Color_fg    << RS_fgShift) | (Color_bg     << RS_bgShift))
#define OVERLAY_RSTYLE  (RS_None | (Color_Black << RS_fgShift) | (Color_Yellow << RS_bgShift))

enum {
  C0_NUL = 0x00,
          C0_SOH, C0_STX, C0_ETX, C0_EOT, C0_ENQ, C0_ACK, C0_BEL,
  C0_BS , C0_HT , C0_LF , C0_VT , C0_FF , C0_CR , C0_SO , C0_SI ,
  C0_DLE, C0_DC1, C0_DC2, D0_DC3, C0_DC4, C0_NAK, C0_SYN, C0_ETB,
  C0_CAN, C0_EM , C0_SUB, C0_ESC, C0_IS4, C0_IS3, C0_IS2, C0_IS1,
};

struct string_term
{
  unsigned char v[3];
};

#define CHAR_ST                 0x9c    /* 0234 */

/*
 * XTerm Operating System Commands: ESC ] Ps;Pt (ST|BEL)
 * colour extensions by Christian W. Zuckschwerdt <zany@triq.net>
 */
enum {
  XTerm_name             =  0,
  XTerm_iconName         =  1,
  XTerm_title            =  2,
  XTerm_property         =  3,      // change X property
  XTerm_Color            =  4,      // change colors
  XTerm_Color00          = 10,      // change fg color
  XTerm_Color01          = 11,      // change bg color
  XTerm_Color_cursor     = 12,      // change actual 'Cursor' color
  XTerm_Color_pointer_fg = 13,      // change actual 'Pointer' fg color
  XTerm_Color_pointer_bg = 14,      // change actual 'Pointer' bg color
  XTerm_Color05          = 15,      // not implemented (tektronix fg)
  XTerm_Color06          = 16,      // not implemented (tektronix bg)
  XTerm_Color_HC         = 17,      // change actual 'Highlight' bg color
  XTerm_Color_HTC        = 19,      // change actual 'Highlight' fg color
  XTerm_logfile          = 46,      // not implemented
  XTerm_font             = 50,
  XTerm_Color00_reset    = 110,     // reset fg color
  XTerm_Color01_reset    = 111,     // reset bg color
  XTerm_Color_cursor_reset = 112,   // reset actual 'Cursor' color

  XTerm_konsole30        = 30,      // reserved for konsole
  XTerm_konsole31        = 31,      // reserved for konsole
  XTerm_emacs51          = 51,      // reserved for emacs shell
  /*
   * rxvt extensions of XTerm OSCs: ESC ] Ps;Pt (ST|BEL)
   */

  // deprecated
  Rxvt_restoreFG         = 39,
  Rxvt_restoreBG         = 49,

  Rxvt_dumpscreen        = 55,      // dump scrollback and all of screen

  URxvt_locale           = 701,     // change locale
  URxvt_version          = 702,     // request version

  URxvt_Color_IT         = 704,     // change actual 'Italic' colour
  URxvt_Color_BD         = 706,     // change actual 'Bold' color
  URxvt_Color_UL         = 707,     // change actual 'Underline' color
  URxvt_Color_border     = 708,

  URxvt_font             = 710,
  URxvt_boldFont         = 711,
  URxvt_italicFont       = 712,
  URxvt_boldItalicFont   = 713,

  URxvt_view_up          = 720,
  URxvt_view_down        = 721,

  URxvt_cellinfo         = 776,     // returns font cell width, height etc.
  URxvt_perl             = 777,     // for use by perl extensions, starts with "extension-name;"
};

/* Words starting with `Color_' are colours.  Others are counts */
/*
 * The PixColor and rendition colour usage should probably be decoupled
 * on the unnecessary items, e.g. Color_pointer, but won't bother
 * until we need to.  Also, be aware of usage in pixcolor_set
 */

enum colour_list {
  Color_none = -2,
  Color_transparent = -1,
  Color_fg = 0,
  Color_bg,
  minCOLOR,                   /* 2 */
  Color_Black = minCOLOR,
  Color_Red3,
  Color_Green3,
  Color_Yellow3,
  Color_Blue3,
  Color_Magenta3,
  Color_Cyan3,
  maxCOLOR,                   /* minCOLOR + 7 */
#ifndef NO_BRIGHTCOLOR
  Color_AntiqueWhite = maxCOLOR,
  minBrightCOLOR,             /* maxCOLOR + 1 */
  Color_Grey25 = minBrightCOLOR,
  Color_Red,
  Color_Green,
  Color_Yellow,
  Color_Blue,
  Color_Magenta,
  Color_Cyan,
  maxBrightCOLOR,             /* minBrightCOLOR + 7 */
  Color_White = maxBrightCOLOR,
#else
  Color_White = maxCOLOR,
#endif
  minTermCOLOR = Color_White + 1,
#if USE_256_COLORS
  maxTermCOLOR = Color_White + 240,
#else
  maxTermCOLOR = Color_White + 72,
#endif
  minTermCOLOR24,
  maxTermCOLOR24 = minTermCOLOR24 +
                   RGB24_CUBE_SIZE - 1,
#ifndef NO_CURSORCOLOR
  Color_cursor,
  Color_cursor_orig,
  Color_cursor2,
#endif
  Color_pointer_fg,
  Color_pointer_bg,
  Color_border,
#ifndef NO_BOLD_UNDERLINE_REVERSE
  Color_BD,
  Color_IT,
  Color_UL,
  Color_RV,
#endif
#if ENABLE_FRILLS
  Color_underline,
#endif
#ifdef OPTION_HC
  Color_HC,
  Color_HTC,
#endif
  Color_scroll,
#ifdef RXVT_SCROLLBAR
  Color_trough,
#endif
#if OFF_FOCUS_FADING
  Color_fade,
#endif
  Color_fg_orig,
  Color_bg_orig,
  NRS_COLORS,                 /* */
#ifdef RXVT_SCROLLBAR
  Color_topShadow = NRS_COLORS,
  Color_bottomShadow,
  TOTAL_COLORS
#else
  TOTAL_COLORS = NRS_COLORS
#endif
};

#if USE_256_COLORS
# define Color_Bits      9 // 0 .. maxTermCOLOR24
#else
# define Color_Bits      8 // 0 .. maxTermCOLOR24
#endif

#if maxTermCOLOR24 >= (1 << Color_Bits)
# error color index overflow
#endif

/*
 * Resource list
 */
enum {
# define def(name) Rs_ ## name,
# define reserve(name,count) Rs_ ## name ## _ = Rs_ ## name + (count) - 1,
# include "rsinc.h"
# undef def
# undef reserve
  NUM_RESOURCES
};

/* DEC private modes */
#define PrivMode_132            (1UL<<0)
#define PrivMode_132OK          (1UL<<1)
#define PrivMode_rVideo         (1UL<<2)
#define PrivMode_relOrigin      (1UL<<3)
#define PrivMode_Screen         (1UL<<4)
#define PrivMode_Autowrap       (1UL<<5)
#define PrivMode_aplCUR         (1UL<<6)
#define PrivMode_aplKP          (1UL<<7)
#define PrivMode_HaveBackSpace  (1UL<<8)
#define PrivMode_BackSpace      (1UL<<9)
#define PrivMode_ShiftKeys      (1UL<<10)
#define PrivMode_VisibleCursor  (1UL<<11)
#define PrivMode_MouseX10       (1UL<<12)
#define PrivMode_MouseX11       (1UL<<13)
#define PrivMode_scrollBar      (1UL<<14)
#define PrivMode_TtyOutputInh   (1UL<<15)
#define PrivMode_Keypress       (1UL<<16)
#define PrivMode_smoothScroll   (1UL<<17)
#define PrivMode_vt52           (1UL<<18)
#define PrivMode_LFNL		(1UL<<19)
#define PrivMode_MouseBtnEvent  (1UL<<20)
#define PrivMode_MouseAnyEvent  (1UL<<21)
#define PrivMode_BracketPaste   (1UL<<22)
#define PrivMode_ExtMouseUTF8   (1UL<<23) // xterm pseudo-utf-8 hack
#define PrivMode_ExtMouseUrxvt  (1UL<<24) // xterm pseudo-utf-8, but works in non-utf-8-locales
#define PrivMode_BlinkingCursor (1UL<<25)
#define PrivMode_FocusEvent     (1UL<<26)
#define PrivMode_ExtMouseSGR    (1UL<<27)

#define PrivMode_mouse_report   (PrivMode_MouseX10|PrivMode_MouseX11|PrivMode_MouseBtnEvent|PrivMode_MouseAnyEvent)

#ifdef ALLOW_132_MODE
# define PrivMode_Default (PrivMode_Autowrap|PrivMode_ShiftKeys|PrivMode_VisibleCursor|PrivMode_132OK)
#else
# define PrivMode_Default (PrivMode_Autowrap|PrivMode_ShiftKeys|PrivMode_VisibleCursor)
#endif

// do not change these constants lightly, there are many interdependencies
#define IMBUFSIZ               128     // input modifier buffer sizes
#define KBUFSZ                 512     // size of keyboard mapping buffer
#define CBUFSIZ                32768   // size of command buffer (longest command sequence possible)
#define CBUFCNT                8       // never call pty_fill/cmd_parse more than this often in a row
#define UBUFSIZ                2048    // character buffer

#if ENABLE_FRILLS
# include <X11/Xmd.h>
typedef struct _mwmhints
{
  CARD32 flags;
  CARD32 functions;
  CARD32 decorations;
  INT32  input_mode;
  CARD32 status;
} MWMHints;
#endif

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS     (1L << 0)
#define MWM_HINTS_DECORATIONS   (1L << 1)
#define MWM_HINTS_INPUT_MODE    (1L << 2)
#define MWM_HINTS_STATUS        (1L << 3)
/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)
/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL           (1L << 0)
#define MWM_DECOR_BORDER        (1L << 1)
#define MWM_DECOR_RESIZEH       (1L << 2)
#define MWM_DECOR_TITLE         (1L << 3)
#define MWM_DECOR_MENU          (1L << 4)
#define MWM_DECOR_MINIMIZE      (1L << 5)
#define MWM_DECOR_MAXIMIZE      (1L << 6)
/* bit definitions for MwmHints.inputMode */
#define MWM_INPUT_MODELESS                  0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL              2
#define MWM_INPUT_FULL_APPLICATION_MODAL    3
#define PROP_MWM_HINTS_ELEMENTS             5

/*
 *****************************************************************************
 * MACRO DEFINES
 *****************************************************************************
 */

// speed hack, copy some member variable into a local variable of the same name
#define dLocal(type,name)       type const name = this->name

// for speed reasons, we assume that all codepoints 32 to 126 are
// single-width.
#define WCWIDTH(c)		(IN_RANGE_INC (c, 0x20, 0x7e) ? 1 : wcwidth (c))

/* convert pixel dimensions to row/column values.  Everything as int32_t */
#define Pixel2Col(x)            Pixel2Width((int32_t)(x))
#define Pixel2Row(y)            Pixel2Height((int32_t)(y))
#define Pixel2Width(x)          ((int32_t)(x) / (int32_t)fwidth)
#define Pixel2Height(y)         ((int32_t)(y) / (int32_t)fheight)
#define Col2Pixel(col)          ((int32_t)Width2Pixel(col))
#define Row2Pixel(row)          ((int32_t)Height2Pixel(row))
#define Width2Pixel(n)          ((int32_t)(n) * (int32_t)fwidth)
#define Height2Pixel(n)         ((int32_t)(n) * (int32_t)fheight)

#define LINENO_of(t,n) MOD ((t)->term_start + int(n), (t)->total_rows)
#define ROW_of(t,n) (t)->row_buf [LINENO_of ((t), n)]

#define LINENO(n) LINENO_of (this, n)
#define ROW(n) ROW_of (this, n)

/* how to build & extract colors and attributes */
#define GET_BASEFG(x)           (((x) & RS_fgMask) >> RS_fgShift)
#define GET_BASEBG(x)           (((x) & RS_bgMask) >> RS_bgShift)

#define GET_FONT(x)             (((x) & RS_fontMask) >> RS_fontShift)
#define SET_FONT(x,fid)         (((x) & ~RS_fontMask) | ((fid) << RS_fontShift))

#define GET_STYLE(x)		(((x) & RS_styleMask) >> RS_styleShift)
#define SET_STYLE(x,style)	(((x) & ~RS_styleMask) | ((style) << RS_styleShift))

#define GET_ATTR(x)             (((x) & RS_attrMask))
#define SET_FGCOLOR(x,fg)       (((x) & ~RS_fgMask)   | ((fg) << RS_fgShift))
#define SET_BGCOLOR(x,bg)       (((x) & ~RS_bgMask)   | ((bg) << RS_bgShift))
#define SET_ATTR(x,a)           (((x) & ~RS_attrMask) | (a))

#define RS_SAME(a,b)		(!(((a) ^ (b)) & ~RS_Careful))

#define PIXCOLOR_NAME(idx)      rs[Rs_color + (idx)]
#define ISSET_PIXCOLOR(idx)     (!!rs[Rs_color + (idx)])

#if ENABLE_STYLES
# define FONTSET_of(t,style) (t)->fontset[GET_STYLE (style)]
#else
# define FONTSET_of(t,style) (t)->fontset[0]
#endif

#define FONTSET(style) FONTSET_of (this, style)

typedef callback<void (const char *)> log_callback;
typedef callback<int (int)> getfd_callback;

/****************************************************************************/

#define LINE_LONGER     0x0001 // line is continued on the next row
#define LINE_FILTERED   0x0002 // line has been filtered
#define LINE_COMPRESSED 0x0004 // line has been compressed (NYI)
#define LINE_FILTER     0x0008 // line needs to be filtered before display (NYI)
#define LINE_BIDI       0x0010 // line needs bidi (NYI)

struct line_t
{
   text_t *t; // terminal the text
   rend_t *r; // rendition, uses RS_ flags
   tlen_t_ l; // length of each text line
   uint32_t f; // flags

   bool valid ()
   {
     return l >= 0;
   }

   void alloc ()
   {
     l = 0;
   }

   bool is_longer ()
   {
     return f & LINE_LONGER;
   }

   void is_longer (int set)
   {
     if (set)
       f |= LINE_LONGER;
     else
       f &= ~LINE_LONGER;
   }

   void clear ()
   {
     t = 0;
     r = 0;
     l = 0;
     f = 0;
   }

   void touch () // call whenever a line is changed/touched/updated
   {
#if ENABLE_PERL
     f &= ~LINE_FILTERED;
#endif
   }

   void touch (int col)
   {
     max_it (l, col);
     touch ();
   }
};

/****************************************************************************/

// primitive wrapper around mbstate_t to ensure initialisation
struct mbstate
{
  mbstate_t mbs;

  operator mbstate_t *() { return &mbs; }
  void reset () { memset (&mbs, 0, sizeof (mbs)); }
  mbstate () { reset (); }
};

/****************************************************************************/

#define UNICODE_MASK 0x1fffffUL

#if UNICODE_3
# define COMPOSE_LO 0x110000UL
# define COMPOSE_HI 0x1fffffUL
# define IS_COMPOSE(n) ((int32_t)(n) >= COMPOSE_LO)
#else
# if ENABLE_PERL
#  define COMPOSE_LO 0xe000UL // our _own_ functions don't like (illegal) surrogates
#  define COMPOSE_HI 0xf8ffUL // in utf-8, so use private use area only
# else
#  define COMPOSE_LO 0xd800UL
#  define COMPOSE_HI 0xf8ffUL
# endif
# define IS_COMPOSE(n) IN_RANGE_INC ((n), COMPOSE_LO, COMPOSE_HI)
#endif

#if ENABLE_COMBINING
// compose chars are used to represent composite characters
// that are not representable in unicode, as well as characters
// not fitting in the BMP.
struct compose_char
{
  // c1 can be any character != NOCHAR, including another compose character
  // c2 must always be a zero-width character or NOCHAR, in case
  // this just extends beyond the BMP.
  unicode_t c1, c2;

  compose_char (unicode_t c1, unicode_t c2)
  : c1(c1), c2(c2)
  { }
};

struct rxvt_composite_vec
{
  vector<compose_char> v;

  text_t compose (unicode_t c1, unicode_t c2 = NOCHAR);
  template<typename T> int expand (unicode_t c, T *r);
  int expand (unicode_t c) { return expand (c, (text_t *)0); }
  compose_char *operator [](text_t c)
  {
    return c >= COMPOSE_LO && c < COMPOSE_LO + v.size ()
           ? &v[c - COMPOSE_LO]
           : 0;
  }
};

extern class rxvt_composite_vec rxvt_composite;
#endif

#if 0
// expand the sequence into a static array
// works even without ENABLE_COMBINING
template<typename T, int max_size_ = 48>
struct rxvt_compose_expand_static
{
#if ENABLE_COMBINING
  enum { max_size = max_size_ };

  T chrs[max_size];

  // expand sequence and return start ptr
  // guarantees at least one output
  // get the length with length () on that ptr
  T *operator ()(unicode_t c)
  {
    T *cur = chrs + max_size;

    while (ecb_expect_false (IS_COMPOSE (c)))
      if (ecb_expect_true (c - COMPOSE_LO < rxvt_composite.v.size ()))
        {
          compose_char *cc = &rxvt_composite.v [c - COMPOSE_LO];

          if (cc->c2 != NOCHAR)
            {
              cur -= cur > chrs; *cur = cc->c2;
            }

          c = cc->c1;
        }
       else
         c = NOCHAR;

    cur -= cur > chrs; *cur = c;

    return cur;
  }

  int length (T *first)
  {
    return chrs + max_size - first;
  }
#else
  T chr;

  T *operator ()(text_t c);
  {
    chr = c;
    return &chr;
  }

  int length (T *first)
  {
    return 1;
  }
#endif
};
#endif

/****************************************************************************/

#ifdef KEYSYM_RESOURCE
class keyboard_manager;
#endif

typedef struct rxvt_term *rxvt_t;

extern rxvt_t rxvt_current_term;

#define SET_R(r) rxvt_current_term = const_cast<rxvt_term *>(r)
#define GET_R rxvt_current_term

/* ------------------------------------------------------------------------- */
struct overlay_base
{
  int x, y, w, h; // overlay dimensions
  text_t **text;
  rend_t **rend;

  // while tempting to add swap() etc. here, it effectively only increases code size
};

/* ------------------------------------------------------------------------- */

typedef struct
{
  int row;
  int col;
} row_col_t;

/*
 * terminal limits:
 *
 *  width      : 1 <= width
 *  height     : 1 <= height
 *  ncol       : 1 <= ncol       <= MAX(tlen_t)
 *  nrow       : 1 <= nrow       <= MAX(int)
 *  saveLines  : 0 <= saveLines  <= MAX(int)
 *  term_start : 0 <= term_start < saveLines
 *  total_rows : nrow + saveLines
 *
 *  top_row    : -saveLines <= top_row    <= 0
 *  view_start : top_row    <= view_start <= 0
 *
 *          | most coordinates are stored relative to term_start,
 *  ROW_BUF | which is the first line of the terminal screen
 *  |························= row_buf[0]
 *  |························= row_buf[1]
 *  |························= row_buf[2] etc.
 *  |
 *  +------------+···········= term_start + top_row
 *  | scrollback |
 *  | scrollback +---------+·= term_start + view_start
 *  | scrollback | display |
 *  | scrollback | display |
 *  +------------+·display·+·= term_start
 *  |  terminal  | display |
 *  |  terminal  +---------+
 *  |  terminal  |
 *  |  terminal  |
 *  +------------+···········= term_start + nrow - 1
 *  |
 *  |
 *  END······················= total_rows
 */

struct TermWin_t
{
  int            vt_width;      /* actual window width             [pixels] */
  int            vt_height;     /* actual window height            [pixels] */
  int            width;         /* window width                    [pixels] */
  int            height;        /* window height                   [pixels] */
  int            fwidth;        /* font width                      [pixels] */
  int            fheight;       /* font height                     [pixels] */
  int            fbase;         /* font ascent (baseline)          [pixels] */
  int            ncol;          /* window columns              [characters] */
  int            nrow;          /* window rows                 [characters] */
  int            focus;         /* window has focus                         */
  int            mapped;        /* window state mapped?                     */
  int            int_bwidth;    /* internal border width                    */
  int            ext_bwidth;    /* external border width                    */
  int            lineSpace;     /* number of extra pixels between rows      */
  int            letterSpace;   /* number of extra pixels between columns   */
  int            saveLines;     /* number of lines that fit in scrollback   */
  int            total_rows;    /* total number of rows in this terminal    */
  int            term_start;    /* term lines start here                    */
  int            view_start;    /* scrollback view starts here              */
  int            top_row;       /* topmost row index of scrollback          */
  Window         parent;        /* parent identifier                        */
  Window         vt;            /* vt100 window                             */
  GC             gc;            /* GC for drawing                           */
  rxvt_drawable *drawable;
  rxvt_fontset  *fontset[4];
};

/*
 * screen accounting:
 * screen_t elements
 *   row:       Cursor row position                   : 0 <= row < nrow
 *   col:       Cursor column position                : 0 <= col < ncol
 *   tscroll:   Scrolling region top row inclusive    : 0 <= row < nrow
 *   bscroll:   Scrolling region bottom row inclusive : 0 <= row < nrow
 *
 * selection_t elements
 *   clicks:    1, 2 or 3 clicks - 4 indicates a special condition of 1 where
 *              nothing is selected
 *   beg:       row/column of beginning of selection  : never past mark
 *   mark:      row/column of initial click           : never past end
 *   end:       row/column of one character past end of selection
 * * Note: top_row <= beg.row <= mark.row <= end.row < nrow
 * * Note: col == -1 ==> we're left of screen
 *
 */
struct screen_t
{
  row_col_t       cur;          /* cursor position on the screen             */
  int             tscroll;      /* top of settable scroll region             */
  int             bscroll;      /* bottom of settable scroll region          */
  unsigned int    charset;      /* character set number [0..3]               */
  unsigned int    flags;        /* see below                                 */
  row_col_t       s_cur;        /* saved cursor position                     */
  unsigned int    s_charset;    /* saved character set number [0..3]         */
  char            s_charset_char;
  rend_t          s_rstyle;     /* saved rendition style                     */
};

enum selection_op_t
{
  SELECTION_CLEAR = 0,  /* nothing selected                          */
  SELECTION_INIT,       /* marked a point                            */
  SELECTION_BEGIN,      /* started a selection                       */
  SELECTION_CONT,       /* continued selection                       */
  SELECTION_DONE        /* selection put in CUT_BUFFER0              */
};

struct selection_t
{
  wchar_t          *text;       /* selected text                             */
  unsigned int      len;        /* length of selected text                   */
  unsigned int      screen;     /* screen being used                         */
  unsigned int      clicks;     /* number of clicks                          */
  selection_op_t    op;         /* current operation                         */
  bool              rect;       /* rectangular selection?                    */
  row_col_t         beg;        /* beginning of selection   <= mark          */
  row_col_t         mark;       /* point of initial click   <= end           */
  row_col_t         end;        /* one character past end point              */
  wchar_t          *clip_text;  /* text copied to the clipboard              */
  unsigned int      clip_len;   /* length of clipboard text                  */
};

/* ------------------------------------------------------------------------- */

/* screen_t flags */
#define Screen_Relative          (1<<0)  /* relative origin mode flag         */
#define Screen_VisibleCursor     (1<<1)  /* cursor visible?                   */
#define Screen_Autowrap          (1<<2)  /* auto-wrap flag                    */
#define Screen_Insert            (1<<3)  /* insert mode (vs. overstrike)      */
#define Screen_WrapNext          (1<<4)  /* need to wrap for next char?       */
#define Screen_DefaultFlags      (Screen_VisibleCursor | Screen_Autowrap)

/* rxvt_vars.options */
enum {
# define def(name)   Opt_ ## name,
# define nodef(name) Opt_prev_ ## name, Opt_ ## name = 0, Opt_next_ ## name = Opt_prev_ ## name - 1,
  Opt_0,
# include "optinc.h"
# undef nodef
# undef def
  Opt_count
};

/* ------------------------------------------------------------------------- */

struct rxvt_vars : TermWin_t
{
  scrollBar_t     scrollBar;
  uint8_t         options[(Opt_count + 7) >> 3];
  XSizeHints      szHint;
  rxvt_color     *pix_colors;
  Cursor          TermWin_cursor;       /* cursor for vt window */

  line_t         *row_buf;      // all lines, scrollback + terminal, circular
  line_t         *drawn_buf;    // text on screen
  line_t         *swap_buf;     // lines for swap buffer
  char           *tabs;         /* per location: 1 == tab-stop               */
  screen_t        screen;
  screen_t        swap;
  selection_t     selection;
  rxvt_color      pix_colors_focused[TOTAL_COLORS];
#ifdef OFF_FOCUS_FADING
  rxvt_color      pix_colors_unfocused[TOTAL_COLORS];
#endif
};

struct rxvt_term : zero_initialized, rxvt_vars, rxvt_screen
{

  // special markers with magic addresses
  static const char resval_undef [];    // options specifically unset
  static const char resval_on [];       // boolean options switched on
  static const char resval_off [];      // or off

  log_callback   *log_hook;             // log error messages through this hook, if != 0
  getfd_callback *getfd_hook;           // convert remote to local fd, if != 0
#if ENABLE_PERL
  rxvt_perl_term  perl;
#endif
  struct mbstate  mbstate;              // current input multibyte state

  unsigned char   want_refresh:1,
                  current_screen:1,	/* primary or secondary              */
                  num_scr_allow:1,
                  bypass_keystate:1,
#if ENABLE_FRILLS
                  urgency_hint:1,
                  rewrap_always:1,
                  rewrap_never:1,
#endif
#if CURSOR_BLINK
                  hidden_cursor:1,
#endif
#if TEXT_BLINK
                  hidden_text:1,
#endif
#if POINTER_BLANK
                  hidden_pointer:1,
#endif
                  enc_utf8:1,		/* whether locale uses utf-8 */
                  seen_input:1,         /* whether we have seen some program output yet */
                  seen_resize:1,	/* whether we had a resize event */
                  init_done:1,
                  parsed_geometry:1;

  unsigned char   refresh_type,
#ifdef META8_OPTION
                  meta_char;            /* Alt-key prefix */
#endif
/* ---------- */
  bool            rvideo_state, rvideo_mode;
#ifndef NO_BELL
  bool            rvideo_bell;
#endif
  int             num_scr;              /* screen: number of lines scrolled */
  int             prev_ncol,            /* screen: previous number of columns */
                  prev_nrow;            /* screen: previous number of rows */
/* ---------- */
  rend_t          rstyle;
/* ---------- */
#ifdef SELECTION_SCROLLING
  int             scroll_selection_lines;
  int             selection_save_x,
                  selection_save_y,
                  selection_save_state;
#endif
/* ---------- */
  int             csrO,       /* Hops - csr offset in thumb/slider to      */
                              /*   give proper Scroll behaviour            */
#if defined(MOUSE_WHEEL) && defined(MOUSE_SLIP_WHEELING)
                  mouse_slip_wheel_speed,
#endif
                  refresh_count,
                  window_vt_x,
                  window_vt_y,
                  mouse_row,
                  mouse_col,
# ifdef POINTER_BLANK
                  pointerBlankDelay,
# endif
                  multiClickTime,
                  cursor_type,
                  allowedxerror;
/* ---------- */
  unsigned int    ModLevel3Mask,
                  ModMetaMask,
                  ModNumLockMask;
  unsigned long   priv_modes,
                  SavedModes;
/* ---------- */
  Atom            *xa;
/* ---------- */
  Time            selection_time,
                  clipboard_time;
  rxvt_selection *selection_req;
  pid_t           cmd_pid;    /* process id of child */
/* ---------- */
  struct mouse_event MEvent;
  XComposeStatus  compose;
  static struct termios def_tio;
  row_col_t       oldcursor;

#ifdef HAVE_IMG
  enum {
    BG_IS_TRANSPARENT    = 1 << 1,
    BG_NEEDS_REFRESH     = 1 << 2,
  };

  uint8_t bg_flags;

  rxvt_img *bg_img;
#endif

#if ENABLE_OVERLAY
  overlay_base ov;

  void scr_swap_overlay () noexcept;
  void scr_overlay_new (int x, int y, int w, int h) noexcept;
  void scr_overlay_off () noexcept;
  void scr_overlay_set (int x, int y,
                        text_t text,
                        rend_t rend = OVERLAY_RSTYLE) noexcept;
  void scr_overlay_set (int x, int y, const char *s) noexcept;
  void scr_overlay_set (int x, int y, const wchar_t *s) noexcept;
#endif

  vector<void *> allocated;           // free these memory blocks with free()

  int            parent_x, parent_y; // parent window position relative to root, only updated on demand

  char           *locale;
  char            charsets[4];
  char           *v_buffer;           /* pointer to physical buffer */
  unsigned int    v_buflen;           /* size of area to write */
  stringvec      *argv, *envv;        /* if != 0, will be freed at destroy time */
  char           **env;

#ifdef KEYSYM_RESOURCE
  keyboard_manager *keyboard;
#endif
#ifndef NO_RESOURCES
  XrmDatabase option_db;
#endif

  const char     *rs[NUM_RESOURCES];
  /* command input buffering */
  char           *cmdbuf_ptr, *cmdbuf_endp;
  char            cmdbuf_base[CBUFSIZ];

  ptytty         *pty;

  // chunk contains all line_t's as well as rend_t and text_t buffers
  // for drawn_buf, swap_buf and row_buf, in this order
  void           *chunk;
  size_t          chunk_size;

  uint32_t        rgb24_color[RGB24_CUBE_SIZE];   // the 24-bit color value
  uint16_t        rgb24_seqno[RGB24_CUBE_SIZE];   // which one is older?
  uint16_t        rgb24_sequence;

  static vector<rxvt_term *> termlist; // a vector of all running rxvt_term's

#if ENABLE_FRILLS || ISO_14755
  // ISO 14755 entry support
  unicode_t iso14755buf;
  void commit_iso14755 ();
# if ISO_14755
  void iso14755_51 (unicode_t ch, rend_t r = DEFAULT_RSTYLE, int x = 0, int y = -1, int y2 = -1);
  void iso14755_54 (int x, int y);
# endif
#endif

  long vt_emask, vt_emask_perl, vt_emask_xim, vt_emask_mouse;

  void vt_select_input () const noexcept
  {
    XSelectInput (dpy, vt, vt_emask | vt_emask_perl | vt_emask_xim | vt_emask_mouse);
  }

#if ENABLE_PERL
  void rootwin_cb (XEvent &xev);
  xevent_watcher rootwin_ev;
#endif

  void x_cb (XEvent &xev);
  xevent_watcher termwin_ev;
  xevent_watcher vt_ev;
  xevent_watcher scrollbar_ev;

  void child_cb (ev::child &w, int revents); ev::child child_ev;
  void destroy_cb (ev::idle &w, int revents); ev::idle destroy_ev;
  void refresh_check ();
  void flush ();
  void flush_cb (ev::timer &w, int revents); ev::timer flush_ev;
  void cmdbuf_reify ();
  void cmdbuf_append (const char *str, size_t count);
  bool pty_fill ();
  void pty_cb (ev::io &w, int revents); ev::io pty_ev;

#ifdef CURSOR_BLINK
  void cursor_blink_reset ();
  void cursor_blink_cb (ev::timer &w, int revents); ev::timer cursor_blink_ev;
#endif
#ifdef TEXT_BLINK
  void text_blink_cb (ev::timer &w, int revents); ev::timer text_blink_ev;
#endif
#ifndef NO_BELL
  void bell_cb (ev::timer &w, int revents); ev::timer bell_ev;
#endif

#ifndef NO_SCROLLBAR_BUTTON_CONTINUAL_SCROLLING
  void cont_scroll_cb (ev::timer &w, int revents); ev::timer cont_scroll_ev;
#endif
#ifdef SELECTION_SCROLLING
  void sel_scroll_cb (ev::timer &w, int revents); ev::timer sel_scroll_ev;
#endif
#if defined(MOUSE_WHEEL) && defined(MOUSE_SLIP_WHEELING)
  void slip_wheel_cb (ev::timer &w, int revents); ev::timer slip_wheel_ev;
#endif

#ifdef POINTER_BLANK
  void pointer_cb (ev::timer &w, int revents); ev::timer pointer_ev;
  void pointer_blank ();
#endif
  void pointer_unblank ();

  void tt_printf (const char *fmt,...);
  void tt_write_ (const char *data, unsigned int len);
  void tt_write (const char *data, unsigned int len);
  void tt_write_user_input (const char *data, unsigned int len);
  void pty_write ();

  void make_current () const // make this the "currently active" urxvt instance
  {
    SET_R (this);
    set_environ (env);
    rxvt_set_locale (locale);
  }

#if USE_XIM
  rxvt_xim *input_method;
  XIC      Input_Context;
  XIMStyle input_style;
  XPoint   spot; // most recently sent spot position

  void im_destroy ();
  void im_cb (); im_watcher im_ev;
  void im_set_size (XRectangle &size);
  void im_set_position (XPoint &pos) noexcept;
  void im_set_color (unsigned long &fg, unsigned long &bg);
  void im_set_preedit_area (XRectangle &preedit_rect, XRectangle &status_rect, const XRectangle &needed_rect);

  bool im_is_running ();
  void im_send_spot ();
  bool im_get_ic (const char *modifiers);
  void im_set_position ();
#endif

  // command.C
  void key_press (XKeyEvent &ev);
  void key_release (XKeyEvent &ev);

  wchar_t next_char () noexcept;
  wchar_t cmd_getc ();
  uint32_t next_octet () noexcept;
  uint32_t cmd_get8 ();

  void cmd_parse ();
  void mouse_report (XButtonEvent &ev);
  void button_press (XButtonEvent &ev);
  void button_release (XButtonEvent &ev);
  void focus_in ();
  void focus_out ();
#if ENABLE_FRILLS
  void set_urgency (bool enable);
#else
  void set_urgency (bool enable) { }
#endif
  void update_fade_color (unsigned int idx, bool first_time = false);
#ifdef PRINTPIPE
  FILE *popen_printer ();
  int pclose_printer (FILE *stream);
#endif
  void process_print_pipe ();
  void process_nonprinting (unicode_t ch);
  void process_escape_vt52 (unicode_t ch);
  void process_escape_seq ();
  void process_csi_seq ();
  void process_window_ops (const int *args, unsigned int nargs);
  char *get_to_st (string_term &st);
  void process_dcs_seq ();
  void process_osc_seq ();
  void process_color_seq (int report, int color, const char *str, string_term &st);
  void process_xterm_seq (int op, char *str, string_term &st);
  unsigned int map_rgb24_color (unsigned int r, unsigned int g, unsigned int b, unsigned int a);
  int privcases (int mode, unsigned long bit);
  void process_terminal_mode (int mode, int priv, unsigned int nargs, const int *arg);
  void process_sgr_mode (unsigned int nargs, const int *arg);
  void set_cursor_style (int style);
  // init.C
  void init (stringvec *argv, stringvec *envv);
  void init (int argc, const char *const *argv, const char *const *envv);
  void init2 (int argc, const char *const *argv);
  void init_vars ();
  const char **init_resources (int argc, const char *const *argv);
  void init_env ();
  void set_locale (const char *locale);
  void init_xlocale ();
  void init_command (const char *const *argv);
  void run_command (const char *const *argv);
  int run_child (const char *const *argv);
  void color_aliases (int idx);
  void create_windows (int argc, const char *const *argv);
  void get_colors ();
  void get_ourmods ();
  void set_icon (const char *file);
  // main.C
  void tt_winch ();
  rxvt_term ();
  ~rxvt_term ();
  void destroy ();
  void emergency_cleanup ();
  void recolor_cursor ();
  void resize_all_windows (unsigned int newwidth, unsigned int newheight, int ignoreparent);
  void window_calc (unsigned int newwidth, unsigned int newheight);
  bool set_fonts ();
  void set_string_property (Atom prop, const char *str, int len = -1);
  void set_mbstring_property (Atom prop, const char *str, int len = -1);
  void set_utf8_property (Atom prop, const char *str, int len = -1);
  void set_title (const char *str);
  void set_icon_name (const char *str);
  void set_window_color (int idx, const char *color);
  char *get_colorfgbg ();
  bool set_color (rxvt_color &color, const char *name);
  void alias_color (int dst, int src);
  void set_widthheight (unsigned int newwidth, unsigned int newheight);
  void get_window_origin (int &x, int &y);

  // screen.C

  bool option (uint8_t opt) const noexcept
  {
    return options[opt >> 3] & (1 << (opt & 7));
  }

  void set_option (uint8_t opt, bool set = true) noexcept;

  int fgcolor_of (rend_t r) const noexcept
  {
    int base = GET_BASEFG (r);
#ifndef NO_BRIGHTCOLOR
    if (r & RS_Bold
# if ENABLE_STYLES
        && option (Opt_intensityStyles)
# endif
        && IN_RANGE_EXC (base, minCOLOR, minBrightCOLOR))
      base += minBrightCOLOR - minCOLOR;
#endif
    return base;
  }

  int bgcolor_of (rend_t r) const noexcept
  {
    int base = GET_BASEBG (r);
#ifndef NO_BRIGHTCOLOR
    if (r & RS_Blink
# if ENABLE_STYLES
        && option (Opt_intensityStyles)
# endif
        && IN_RANGE_EXC (base, minCOLOR, minBrightCOLOR))
      base += minBrightCOLOR - minCOLOR;
#endif
    return base;
  }

  // modifies first argument(!)
  void tt_paste (char *data, unsigned int len) noexcept;
  void paste (char *data, unsigned int len) noexcept;
  void scr_alloc () noexcept;
  void scr_blank_line (line_t &l, unsigned int col, unsigned int width, rend_t efs) const noexcept;
  void scr_blank_screen_mem (line_t &l, rend_t efs) const noexcept;
  void scr_kill_char (line_t &l, int col) const noexcept;
  void scr_set_char_rend (line_t &l, int col, rend_t rend);
  int scr_scroll_text (int row1, int row2, int count) noexcept;
  void copy_line (line_t &dst, line_t &src);
  void scr_reset ();
  void scr_release () noexcept;
  void scr_clear (bool really = false) noexcept;
  void scr_refresh () noexcept;
  bool scr_refresh_rend (rend_t mask, rend_t value) noexcept;
  void scr_erase_screen (int mode) noexcept;
#if ENABLE_FRILLS
  void scr_erase_savelines () noexcept;
  void scr_backindex () noexcept;
  void scr_forwardindex () noexcept;
#endif
  void scr_touch (bool refresh) noexcept;
  void scr_expose (int x, int y, int width, int height, bool refresh) noexcept;
  void scr_recolor (bool refresh = true) noexcept;
  void scr_remap_chars () noexcept;
  void scr_remap_chars (line_t &l) noexcept;

  enum cursor_mode { SAVE, RESTORE };

  void scr_poweron ();
  void scr_soft_reset () noexcept;
  void scr_cursor (cursor_mode mode) noexcept;
  void scr_do_wrap () noexcept;
  void scr_swap_screen () noexcept;
  void scr_change_screen (int scrn);
  void scr_color (unsigned int color, int fgbg) noexcept;
  void scr_rendition (int set, int style) noexcept;
  void scr_add_lines (const wchar_t *str, int len, int minlines = 0) noexcept;
  void scr_backspace () noexcept;
  void scr_tab (int count, bool ht = false) noexcept;
  void scr_gotorc (int row, int col, int relative) noexcept;
  void scr_index (enum page_dirn direction) noexcept;
  void scr_erase_line (int mode) noexcept;
  void scr_E () noexcept;
  void scr_insdel_lines (int count, int insdel) noexcept;
  void scr_insdel_chars (int count, int insdel) noexcept;
  void scr_scroll_region (int top, int bot) noexcept;
  void scr_cursor_visible (int mode) noexcept;
  void scr_autowrap (int mode) noexcept;
  void scr_relative_origin (int mode) noexcept;
  void scr_insert_mode (int mode) noexcept;
  void scr_set_tab (int mode) noexcept;
  void scr_rvideo_mode (bool on) noexcept;
  void scr_report_position () noexcept;
  void set_font_style () noexcept;
  void scr_charset_choose (int set) noexcept;
  void scr_charset_set (int set, unsigned int ch) noexcept;
  void scr_move_to (int y, int len) noexcept;
  bool scr_page (int nlines) noexcept;
  bool scr_page (enum page_dirn direction, int nlines) noexcept
  {
    return scr_page (direction * nlines);
  }
  bool scr_changeview (int new_view_start) noexcept;
  void scr_bell () noexcept;
  void scr_printscreen (int fullhist) noexcept;
  void scr_xor_rect (int beg_row, int beg_col, int end_row, int end_col, rend_t rstyle1, rend_t rstyle2) noexcept;
  void scr_xor_span (int beg_row, int beg_col, int end_row, int end_col, rend_t rstyle) noexcept;
  void scr_reverse_selection () noexcept;
  void scr_dump (int fd) noexcept;

  void selection_check (int check_more) noexcept;
  void selection_changed () noexcept; /* sets want_refresh, corrects coordinates */
  void selection_request (Time tm, int selnum = Sel_Primary) noexcept;
  void selection_clear (bool clipboard = false) noexcept;
  void selection_make (Time tm);
  bool selection_grab (Time tm, bool clipboard = false) noexcept;
  void selection_start_colrow (int col, int row) noexcept;
  void selection_delimit_word (enum page_dirn dirn, const row_col_t *mark, row_col_t *ret) noexcept;
  void selection_extend_colrow (int32_t col, int32_t row, int button3, int buttonpress, int clickchange) noexcept;
  void selection_remove_trailing_spaces () noexcept;
  void selection_send (const XSelectionRequestEvent &rq) noexcept;
  void selection_click (int clicks, int x, int y) noexcept;
  void selection_extend (int x, int y, int flag) noexcept;
  void selection_rotate (int x, int y) noexcept;

  // xdefaults.C
  void rxvt_usage (int type);
  const char **get_options (int argc, const char *const *argv);
  int parse_keysym (const char *str, unsigned int &state);
  int bind_action (const char *str, const char *arg);
  const char *x_resource (const char *name);
  void extract_resources ();
  void enumerate_resources (void (*cb)(rxvt_term *, const char *, const char *), const char *name_p = 0, const char *class_p = 0);
  void enumerate_keysym_resources (void (*cb)(rxvt_term *, const char *, const char *))
  {
    enumerate_resources (cb, "keysym", "Keysym");
  }
  void extract_keysym_resources ();
};

#endif /* _RXVT_H_ */

