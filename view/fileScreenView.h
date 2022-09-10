#ifndef __VIEW_H_INCLUDED
#define __VIEW_H_INCLUDED

#include <windows.h>
#include "../model/fileModel.h"

#define MAX_SCROLL 65530

/* Font parameters */
typedef struct
{
    HFONT HFont;
    unsigned long LineHeight;    /* Symbol height plus line spacing */
    unsigned long SymbolWidth;   /* The width of the symbol to determine the length of the string */
} font_params_t;

/* Display modes */
typedef enum
{
    DEFAULT,    /* Switches the display to the non-layout mode */
    LAYOUT      /* Switches the display to the layout mode */
} mode_t;

/*  The structure that implements the view */
typedef struct
{
    const char **Data;                  /* Lines */
    unsigned long NumOfLines;           /* Number of lines */
    unsigned long VScrollPos;           /* Vertical scroll caret position */
    mode_t Mode;                        /* Display mode */
    unsigned long HScrollPos;           /* Horizontal scroll caret position */
    double HScale;                      /* Horizontal scrollbar scale */
    double VScale;                      /* Vertical scrollbar scale */
    unsigned long LinesInWindow;        /* The number of lines that fit in the window */
    unsigned long SymbolsInWindowLine;  /* The number of characters that fit in a line in the window */
    unsigned long MaxLineLenght;        /* Maximum lenght of line in view */
    font_params_t Font;                 /* Font for displaying text */
    unsigned long WindowWidth;          /* The width of the window */
    unsigned long WindowHeight;         /* The height of the window */
} view_t;

/* Initializes the view
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure filled with zero values
*/
void InitView(HWND hwnd, view_t *view);

/*  Sets the font for displaying text
INPUT:
    HWND hwnd - window handle for setting the font
    view_t *view - pointer on view structure
    TCHAR *fontname - name of the font to setting
    unsigned long height - height of the font
*/
void SetViewFont(HWND hwnd, view_t *view, TCHAR *fontname, unsigned long height);

/*  Rebuilds the view according to the new window sizes and performs
    the necessary changes in the display of scrollbars
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    model_t *model - pointer on model structure
    long windowWidth - the width of the workspace or a negative number to use the past value
    long windowHeight - the height of the workspace or a negative number to use the past value
RETURN:
    error_t - error code
*/
error_t ViewRectResize(HWND hwnd, model_t *model, view_t *view, long windowWidth, long windowHeight);

/* Sets the vertical scroll caret by the specified position
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    unsigned long pos - position of vertical scroll
*/
void SetVScroll(HWND hwnd, view_t *view, unsigned long pos);

/* Sets the horizontal scroll caret by the specified position
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    unsigned long pos - position of horizontal scroll
*/
void SetHScroll(HWND hwnd, view_t *view, unsigned long pos);

/* Shifts the vertical scroll caret by the specified amount
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    long delta - shift of vertical scroll
*/
void SetWithDeltaVScroll(HWND hwnd, view_t *view, long delta);

/* Shifts the horizontal scroll caret by the specified amount
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    long delta - shift of horizontal scroll
*/
void SetWithDeltaHScroll(HWND hwnd, view_t *view, long delta);

/*  Displays the view
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
*/
void DisplayView(HWND hwnd, view_t *view);

/* Clears the data field of the view
INPUT:
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure with zero values in data field and NumOfLines
*/
void ClearViewData(view_t *view);

/*  Clears the view
INPUT:
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure filled with zero values
*/
void ClearView(view_t *view);

#endif // __VIEW_H_INCLUDED
