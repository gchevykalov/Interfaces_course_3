#include "fileScreenView.h"

/* Initializes the view
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure filled with zero values
*/
void InitView(HWND hwnd, view_t *view)
{
    HDC hdc;
    TEXTMETRIC tm;

    view->Data = NULL;
    view->NumOfLines = 0;
    view->VScrollPos = 0;
    view->Mode = DEFAULT;
    view->HScrollPos = 0;
    view->HScale = 1;
    view->VScale = 1;
    view->LinesInWindow = 0;
    view->SymbolsInWindowLine = 0;
    view->MaxLineLenght = 0;

    /* Setting default font settings */
    view->Font.HFont = NULL;
    hdc = GetDC(hwnd);
    GetTextMetrics(hdc, &tm);
    view->Font.LineHeight = tm.tmHeight + tm.tmExternalLeading;
    view->Font.SymbolWidth = tm.tmAveCharWidth;

    view->WindowWidth = 0;
    view->WindowHeight = 0;
}

/*  Sets the font for displaying text
INPUT:
    HWND hwnd - window handle for setting the font
    view_t *view - pointer on view structure
    TCHAR *fontname - name of the font to setting
    unsigned long height - height of the font
*/
void SetViewFont(HWND hwnd, view_t *view, TCHAR *fontname, unsigned long height)
{
    HDC hdc;
    TEXTMETRIC tm;

    DeleteObject(view->Font.HFont);

    /* Creating a monospaced font */
    view->Font.HFont = CreateFont(height, 0, 0, 0, 0, 0, 0,
                                   0, DEFAULT_CHARSET, 0, 0, 0, FIXED_PITCH, fontname);

    hdc = GetDC(hwnd);
    SelectObject(hdc, view->Font.HFont);
    SetBkMode(hdc, TRANSPARENT);

    GetTextMetrics(hdc, &tm);
    view->Font.LineHeight = tm.tmHeight + tm.tmExternalLeading;
    view->Font.SymbolWidth = tm.tmAveCharWidth;
}

/*  Builds the view without layout
INPUT:
    view_t *view - pointer on view structure
    model_t *model - pointer on model structure
RETURN:
    error_t - error code
*/
static error_t BuildViewDefault(view_t *view, model_t *model)
{
    unsigned long curLine = 0;
    unsigned long modelLineIndex = 0;
    unsigned long lineLen = view->WindowWidth / view->Font.SymbolWidth;

    if (lineLen == 0)
        lineLen = 1;

    view->SymbolsInWindowLine = lineLen;
    view->LinesInWindow = view->WindowHeight / view->Font.LineHeight;
    if (view->LinesInWindow == 0)
        view->LinesInWindow = 1;

    /* Setting the maximum position value horizontally of the scroll caret */
    view->MaxLineLenght = model->MaxLength;

    /* Counting the number of lines depending on the display mode */
    view->NumOfLines = model->NumOfLines;

    view->Data = calloc(view->NumOfLines, sizeof(char *));
    if (view->Data == NULL)
    {
        ClearView(view);
        return MEMORY_SHORTAGE;
    }

    /* The actual construction of the view */
    for (curLine = 0, modelLineIndex = 0; modelLineIndex < model->NumOfLines; ++modelLineIndex, ++curLine)
        view->Data[curLine] = model->Lines[modelLineIndex];

    return SUCCESS;
}

/*  Builds the view with layout
INPUT:
    view_t *view - pointer on view structure
    model_t *model - pointer on model structure
RETURN:
    error_t - error code
*/
static error_t BuildViewLayout(view_t *view, model_t *model)
{
    unsigned long curLine = 0;
    unsigned long modelLineIndex = 0;
    unsigned long lineLen = view->WindowWidth / view->Font.SymbolWidth;
    unsigned long counter = 0;

    if (lineLen == 0)
        lineLen = 1;

    view->SymbolsInWindowLine = lineLen;
    view->LinesInWindow = view->WindowHeight / view->Font.LineHeight;
    if (view->LinesInWindow == 0)
        view->LinesInWindow = 1;

    /* Setting the maximum position value horizontally of the scroll caret */
    view->MaxLineLenght = lineLen;

    /* Counting the number of lines depending on the display mode */
    view->NumOfLines = 0;
    for (; counter < model->NumOfLines - 1; counter++)
        view->NumOfLines += (model->Lines[counter + 1] - model->Lines[counter]) / lineLen + 1;
    view->NumOfLines += (&model->Data[model->Size] - model->Lines[counter]) / lineLen + 1;

    view->Data = calloc(view->NumOfLines, sizeof(char *));
    if (view->Data == NULL)
    {
        ClearView(view);
        return MEMORY_SHORTAGE;
    }

    /* The actual construction of the view */
    {
        unsigned long count = 0;
        unsigned long modelLineLen = 0;
        char *pointerToLineStart = NULL;

        curLine = 0;
        for (modelLineIndex = 0; modelLineIndex < model->NumOfLines - 1; modelLineIndex++)
        {
            pointerToLineStart = model->Lines[modelLineIndex];
            count = 0;
            modelLineLen = model->Lines[modelLineIndex + 1] - model->Lines[modelLineIndex];

            /* Processing empty lines */
            if (*pointerToLineStart == 0)
            {
                view->Data[curLine++] = pointerToLineStart;
                continue;
            }

            while(count * lineLen < modelLineLen)
            {
                view->Data[curLine++] = pointerToLineStart;
                pointerToLineStart += lineLen;
                ++count;
            }
        }
        /* Processing last line */
        modelLineLen = &model->Data[model->Size] - model->Lines[modelLineIndex];
        pointerToLineStart = model->Lines[modelLineIndex];
        count = 0;

        if (*pointerToLineStart == 0)
            view->Data[curLine++] = pointerToLineStart;

        while(count * lineLen < modelLineLen)
        {
            view->Data[curLine++] = pointerToLineStart;
            pointerToLineStart += lineLen;
            ++count;
        }

    }

    /* Update number of lines in the view
    (due to the integer division, their number was taken with a small margin) */
    {
        unsigned long i;
        for(i = view->NumOfLines - 1; view->Data[i] == NULL; i--);
        view->NumOfLines = i + 1;
    }

    return SUCCESS;
}

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
error_t ViewRectResize(HWND hwnd, model_t *model, view_t *view, long windowWidth, long windowHeight)
{
    error_t err;
    const char *upperLeft = NULL;

    if (view->Data != NULL)
        upperLeft = view->Data[view->VScrollPos] + view->HScrollPos;

    /* Rebuild the view */
    ClearViewData(view);
    view->WindowHeight = windowHeight;
    view->WindowWidth = windowWidth;
    if(view->Mode == DEFAULT)
        err = BuildViewDefault(view, model);
    else
        err = BuildViewLayout(view, model);

    if(err)
        return err;

    if (upperLeft != NULL)
    {
        unsigned long r = view->NumOfLines;
        unsigned long l = 0;
        unsigned long midle = 0;

        while (TRUE)
        {
            midle = (r - l) / 2 + l;
            if (upperLeft - view->Data[midle] >= view->Data[midle + 1] - view->Data[midle])
                l = midle + 1;
            else if (view->Data[midle] - upperLeft > 0)
                r = midle - 1;
            else
                break;
        }
        view->VScrollPos = midle;
    }

    /* Update scrollbar status */
    if (view->Mode == LAYOUT || view->MaxLineLenght < view->SymbolsInWindowLine)
    {
        ShowScrollBar(hwnd, SB_HORZ, FALSE);
    }
    else
    {
        ShowScrollBar(hwnd, SB_HORZ, TRUE);
        view->HScale = (double)MAX_SCROLL / (view->MaxLineLenght - view->SymbolsInWindowLine);
        SetScrollRange(hwnd, SB_HORZ, 0, MAX_SCROLL, FALSE);
        SetHScroll(hwnd, view, view->HScrollPos);
    }

    if (view->LinesInWindow > view->NumOfLines)
    {
        ShowScrollBar(hwnd, SB_VERT, FALSE);
    }
    else
    {
        ShowScrollBar(hwnd, SB_VERT, TRUE);
        view->VScale = (double)MAX_SCROLL / (view->NumOfLines - view->LinesInWindow);
        SetScrollRange(hwnd, SB_VERT, 0, MAX_SCROLL, FALSE);
        SetVScroll(hwnd, view, view->VScrollPos);
    }

    return SUCCESS;
}

/* Sets the vertical scroll caret by the specified position
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    unsigned long pos - position of vertical scroll
*/
void SetVScroll(HWND hwnd, view_t *view, unsigned long pos)
{
    if (pos < 0)
        return;

    if(view->NumOfLines < view->LinesInWindow)
        return;

    view->VScrollPos = pos;
    if (view->VScrollPos > view->NumOfLines - view->LinesInWindow)  /* Going out of range */
        view->VScrollPos = view->NumOfLines - view->LinesInWindow;

    InvalidateRect(hwnd, NULL, TRUE);
    SetScrollPos(hwnd, SB_VERT, view->VScrollPos * view->VScale, TRUE);
}

/* Sets the horizontal scroll caret by the specified position
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    unsigned long pos - position of horizontal scroll
*/
void SetHScroll(HWND hwnd, view_t *view, unsigned long pos)
{
    if (pos < 0 || view->Mode != DEFAULT)
        return;

    view->HScrollPos = pos;
    if (view->HScrollPos > view->MaxLineLenght - view->SymbolsInWindowLine) /* Going out of range */
        view->HScrollPos = view->MaxLineLenght - view->SymbolsInWindowLine;

    InvalidateRect(hwnd, NULL, TRUE);
    SetScrollPos(hwnd, SB_HORZ, view->HScrollPos * view->HScale, TRUE);
}

/* Shifts the vertical scroll caret by the specified amount
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    long delta - shift of vertical scroll
*/
void SetWithDeltaVScroll(HWND hwnd, view_t *view, long delta)
{
    if ((double)view->VScrollPos + (double)delta < 0) /* Going out of range */
        delta = -view->VScrollPos;
    SetVScroll(hwnd, view, view->VScrollPos + delta);
}

/* Shifts the horizontal scroll caret by the specified amount
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
    long delta - shift of horizontal scroll
*/
void SetWithDeltaHScroll(HWND hwnd, view_t *view, long delta)
{
    if ((double)view->HScrollPos + (double)delta < 0) /* Going out of range */
        delta = -view->HScrollPos;
    SetHScroll(hwnd, view, view->HScrollPos + delta);
}

/*  Displays the view
INPUT:
    HWND hwnd - window handle for which the displaying will be performed
    view_t *view - pointer on view structure
*/
void DisplayView(HWND hwnd, view_t *view)
{
    HDC hdc;
    PAINTSTRUCT ps;
    unsigned long counter = 0;

    if (view->Data == NULL)
        return;

    hdc = BeginPaint(hwnd, &ps);
    RECT windowRect;

    GetClientRect(hwnd, &windowRect);

    /* Display a part of the file according to the shifts and sizes of the window */
    for (; counter < view->NumOfLines && counter < view->LinesInWindow; counter++)
    {
        unsigned long index = counter + view->VScrollPos;
        unsigned long len;

        if(index != view->NumOfLines - 1)
        {
            len = view->Data[index + 1] - view->Data[index];
            if(view->Data[index][len - 1] == 0)
                len -= 1;
        }
        else
            len = strlen(view->Data[index]);

        TextOut(hdc, windowRect.left, windowRect.top + counter * view->Font.LineHeight,
            &view->Data[index][view->HScrollPos], len - view->HScrollPos);
    }

    EndPaint(hwnd, &ps);
}

/* Clears the data field of the view
INPUT:
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure with zero values in data field and NumOfLines
*/
void ClearViewData(view_t *view)
{
    if (view == NULL)
        return;

    free(view->Data);
    view->Data = NULL;

    view->NumOfLines = 0;
}

/*  Clears the view
INPUT:
    view_t *view - pointer on view structure
OUTPUT:
    view_t *view - pointer on view structure filled with zero values
*/
void ClearView(view_t *view)
{
    if (view == NULL)
        return;

    free(view->Data);
    view->Data = NULL;

    view->NumOfLines = 0;
    view->VScrollPos = 0;
    view->Mode = DEFAULT;
    view->HScrollPos = 0;
    view->LinesInWindow = 0;
    view->SymbolsInWindowLine = 0;
    view->MaxLineLenght = 0;

    DeleteObject(view->Font.HFont);
    view->Font.HFont = NULL;

    view->WindowWidth = 0;
    view->WindowHeight = 0;
}
