#ifndef __CONTROLLER_H_INCLUDED
#define __CONTROLLER_H_INCLUDED

#include "../view/fileScreenView.h"
#include "../menu/menu.h"

#include <time.h>

#define FONTHEIGHT 18
#define HOLD 0.1 * CLOCKS_PER_SEC

/*  The structure that implements the controller */
typedef struct
{
    int IsNotActive;   /* Contains 0 if the controller is active */

    model_t Model;     /* An instance of the model that the controller is working with */
    view_t View;       /* An instance of the view that the controller is working with */
} controller_t;

/*  Sets the mode of displaying text
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    mode_t mode - mode of displaying text
*/
void SetMode(controller_t *controller, mode_t mode);

/*  Initializes the controller
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    HWND hwnd - window handle for which the displaying will be performed
*/
void InitController(controller_t *controller, HWND hwnd);

/*  Fills the model with data from the file
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    const char *filename - the name of the file from which the data is taken
RETURN:
    error_t - error code
*/
error_t ReadFileIntoModel(controller_t *controller, const char *filename);

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
error_t SetRectSize(HWND hwnd, controller_t *controller, long windowWidth, long windowHeight);

/*  Handles vertical scrollbar events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void VScroll(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Handles horizontal scrollbar events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void HScroll(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Processes the keystroke
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void Keydown(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Handles mouse wheel scrolling
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void MouseWheel(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Handles menu events
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
RETURN:
    error_t - error code
*/
error_t Menu(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Sets the font for displaying text
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    HWND hwnd - window handle for setting the font
    TCHAR *fontname - name of the font to setting
    unsigned long height - height of the font
*/
void SetFont(controller_t *controller, HWND hwnd, TCHAR *fontname, unsigned long height);

/*  Displays the view
INPUT:
    controller_t *controller - pointer to an instance of a structure containing a model and a view
    WPARAM wParam - data that was sent to WndProc
    LPARAM lParam - data that was sent to WndProc
    HWND hwnd - data that was sent to WndProc
*/
void Display(controller_t *controller, WPARAM wParam, LPARAM lParam, HWND hwnd);

/*  Clears the model and view data
INPUT:
    controller_t *controller - pointer to the instance of a structure containing the model and the view
*/
void ClearControllerData(controller_t *controller);

/*  Clears the controller
INPUT:
    controller_t *controller - pointer to the instance of a structure containing the model and the view
*/
void ClearController(controller_t *controller);

#endif // __CONTROLLER_H_INCLUDED
