#include "controller.h"

/*  Sets the mode of displaying text
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    mode_t mode - mode of displaying text
*/
void SetMode(controller_t *controller, mode_t mode)
{
    controller->View.Mode = mode;
    if(mode == LAYOUT)
        controller->View.HScrollPos = 0;
}

/*  Initializes the controller
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    HWND hwnd - window handle for which the displaying will be performed
*/
void InitController(controller_t *controller, HWND hwnd)
{
    controller->IsNotActive = 1;
    InitModel(&controller->Model);
    InitView(hwnd, &controller->View);
}

/*  Fills the model with data from the file
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    const char *filename - the name of the file from which the data is taken
RETURN:
    error_t - error code
*/
error_t ReadFileIntoModel(controller_t *controller, const char *filename)
{
    if (filename == NULL)
        return NO_INPUT_FILE;

    return (controller->IsNotActive = FillModel(&controller->Model, filename));
}

/*  Rebuilds the view according to the new window sizes and performs
    the necessary changes in the display of scrollbars
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    HWND hwnd - window handle for which the displaying will be performed
    long windowWidth - the width of the workspace or a negative number to use the past value
    long windowHeight - the height of the workspace or a negative number to use the past value
RETURN:
    error_t - error code
*/
error_t SetRectSize(HWND hwnd, controller_t *controller, long windowWidth, long windowHeight)
{
    if(windowWidth < 0 || windowHeight < 0) /* Use the same window size as last time */
        return ViewRectResize(hwnd, &controller->Model, &controller->View,
                              controller->View.WindowWidth, controller->View.WindowHeight);
    else
        return ViewRectResize(hwnd, &controller->Model, &controller->View, windowWidth, windowHeight);
}

/*  Handles vertical scrollbar events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void VScroll(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    if (controller->IsNotActive)
        return;

    switch (LOWORD(wParam))
    {
        case SB_LINEDOWN:
            SetWithDeltaVScroll(hwnd, &controller->View, 1);
            break;
        case SB_LINEUP:
            SetWithDeltaVScroll(hwnd, &controller->View, -1);
            break;
        case SB_PAGEUP:
            SetWithDeltaVScroll(hwnd, &controller->View, -controller->View.LinesInWindow);
            break;
        case SB_PAGEDOWN:
            SetWithDeltaVScroll(hwnd, &controller->View, controller->View.LinesInWindow);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            SetVScroll(hwnd, &controller->View, HIWORD(wParam) / controller->View.VScale);
            break;
        default:
            break;
    }
}

/*  Handles horizontal scrollbar events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void HScroll(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    if (controller->IsNotActive)
        return;

    switch (LOWORD(wParam))
    {
        case SB_LINEDOWN:
            SetWithDeltaHScroll(hwnd, &controller->View, 1);
            break;
        case SB_LINEUP:
            SetWithDeltaHScroll(hwnd, &controller->View, -1);
            break;
        case SB_PAGEUP:
            SetWithDeltaHScroll(hwnd, &controller->View, -controller->View.SymbolsInWindowLine);
            break;
        case SB_PAGEDOWN:
            SetWithDeltaHScroll(hwnd, &controller->View, controller->View.SymbolsInWindowLine);
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            SetHScroll(hwnd, &controller->View, HIWORD(wParam) / controller->View.HScale);
            break;
        default:
            break;
    }
}

/*  Processes the keystroke
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void Keydown(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    if (controller->IsNotActive)
        return;

    {
        static DWORD prevKey = -1;
        static clock_t prev = 0;
        clock_t cur = clock();

        if (prevKey == wParam && cur - prev <= HOLD)
            return;

        prevKey = wParam;
        prev = cur;

        switch (wParam)
        {
            case VK_UP:
                SetWithDeltaVScroll(hwnd, &controller->View, -1);
                break;
            case VK_DOWN:
                SetWithDeltaVScroll(hwnd, &controller->View, 1);
                break;
            case VK_LEFT:
                SetWithDeltaHScroll(hwnd, &controller->View, -1);
                break;
            case VK_RIGHT:
                SetWithDeltaHScroll(hwnd, &controller->View, 1);
                break;
            case VK_PRIOR:
                SetWithDeltaVScroll(hwnd, &controller->View, -controller->View.LinesInWindow);
                break;
            case VK_NEXT:
                SetWithDeltaVScroll(hwnd, &controller->View, controller->View.LinesInWindow);
                break;
            default:
                break;
        }
    }
}

/*  Handles mouse wheel scrolling
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void MouseWheel(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    long delta = -GET_WHEEL_DELTA_WPARAM(wParam) / 100;

    if(controller->IsNotActive)
        return;

    SetWithDeltaVScroll(hwnd, &controller->View, delta);
}

/*  Handles menu events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
RETURN:
    error_t - error code
*/
error_t Menu(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    HMENU hMenu = GetMenu(hwnd);

    switch(LOWORD(wParam))
    {
        case IDM_OPEN :
        {
            OPENFILENAME ofn;
            char buffer[MAX_PATH];

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hwnd;
            ofn.lpstrFile = buffer;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(buffer);
            ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE) {
                error_t err;
                RECT rect;

                GetClientRect(hwnd, &rect);
                unsigned long windowWidth = rect.right;
                unsigned long windowHeight = rect.bottom;
                mode_t curMode = controller->View.Mode;

                ClearControllerData(controller);
                InitController(controller, hwnd);
                SetMode(controller, curMode);
                err = ReadFileIntoModel(controller, ofn.lpstrFile);
                if(err)
                    return err;

                err = SetRectSize(hwnd, controller, windowWidth, windowHeight);
                if(err)
                    return err;

                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            break;
        }
        case IDM_EXIT :
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
            break;
        case IDM_CONSOLAS:
        {
            error_t err;

            SetFont(controller, hwnd, "Consolas", FONTHEIGHT);
            CheckMenuItem(hMenu, IDM_CONSOLAS, MF_CHECKED);
            CheckMenuItem(hMenu, IDM_COURIER, MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_LUCIDA, MF_UNCHECKED);
            EnableMenuItem(hMenu, IDM_LUCIDA, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_CONSOLAS, MF_GRAYED);
            EnableMenuItem(hMenu, IDM_COURIER, MF_ENABLED);
            err = SetRectSize(hwnd, controller, -1, -1);
            if(err)
                return err;

            break;
        }
        case IDM_COURIER:
        {
            error_t err;

            SetFont(controller, hwnd, "Courier New", FONTHEIGHT);
            CheckMenuItem(hMenu, IDM_CONSOLAS, MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_COURIER, MF_CHECKED);
            CheckMenuItem(hMenu, IDM_LUCIDA, MF_UNCHECKED);
            EnableMenuItem(hMenu, IDM_LUCIDA, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_CONSOLAS, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_COURIER, MF_GRAYED);
            err = SetRectSize(hwnd, controller, -1, -1);
            if(err)
                return err;

            break;
        }
        case IDM_LUCIDA:
        {
            error_t err;

            SetFont(controller, hwnd, "Lucida Console", FONTHEIGHT);
            CheckMenuItem(hMenu, IDM_CONSOLAS, MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_COURIER, MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_LUCIDA, MF_CHECKED);
            EnableMenuItem(hMenu, IDM_LUCIDA, MF_GRAYED);
            EnableMenuItem(hMenu, IDM_CONSOLAS, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_COURIER, MF_ENABLED);
            err = SetRectSize(hwnd, controller, -1, -1);
            if(err)
                return err;

            break;
        }
        case IDM_DEFAULT:
        {
            error_t err;

            SetMode(controller, DEFAULT);
            CheckMenuItem(hMenu, IDM_DEFAULT, MF_CHECKED);
            CheckMenuItem(hMenu, IDM_LAYOUT, MF_UNCHECKED);
            EnableMenuItem(hMenu, IDM_DEFAULT, MF_GRAYED);
            EnableMenuItem(hMenu, IDM_LAYOUT, MF_ENABLED);
            err = SetRectSize(hwnd, controller, -1, -1);
            if(err)
                return err;

            break;
        }
        case IDM_LAYOUT:
        {
            error_t err;

            SetMode(controller, LAYOUT);
            CheckMenuItem(hMenu, IDM_DEFAULT, MF_UNCHECKED);
            CheckMenuItem(hMenu, IDM_LAYOUT, MF_CHECKED);
            EnableMenuItem(hMenu, IDM_DEFAULT, MF_ENABLED);
            EnableMenuItem(hMenu, IDM_LAYOUT, MF_GRAYED);
            err = SetRectSize(hwnd, controller, -1, -1);
            if(err)
                return err;

            break;
        }
        case IDM_ABOUT :
            MessageBox(hwnd, "Interfaces Lab",
                        "About", MB_OK | MB_ICONINFORMATION);
            break;
        default:
            break;
    }

    return SUCCESS;
}

/*  Sets the font for displaying text
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    HWND hwnd - window handle for setting the font
    TCHAR *fontname - name of the font to setting
    unsigned long height - height of the font
*/
void SetFont(controller_t *controller, HWND hwnd, TCHAR *fontname, unsigned long height)
{
    SetViewFont(hwnd, &controller->View, fontname, height);
}

/*  Displays the view
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void Display(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd)
{
    if(controller->IsNotActive)
        return;

    DisplayView(hwnd, &controller->View);
}

/*  Clears the model and view data
INPUT:
    controller_t *controller - pointer to the instance of a structure containing the model and the view
*/
void ClearControllerData(controller_t *controller)
{
    if (controller == NULL)
        return;

    ClearModel(&controller->Model);
    ClearViewData(&controller->View);
    controller->IsNotActive = 1;
}

/*  Clears the controller
INPUT:
    controller_t *controller - pointer to the instance of a structure containing the model and the view
*/
void ClearController(controller_t *controller)
{
    if (controller == NULL)
        return;

    ClearModel(&controller->Model);
    ClearView(&controller->View);
    controller->IsNotActive = 1;
}
