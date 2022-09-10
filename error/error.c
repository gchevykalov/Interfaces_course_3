#include "error.h"
#include <string.h>

/* Displays a window with an error message
INPUT:
    HWND hwnd - window handle for which the displaying of message box was called
    error_t err - error code
*/
void DisplayMessageBox(HWND hwnd, error_t err)
{
    char buffer [20];
    switch(err)
    {
        case NO_INPUT_FILE:
            strcpy(buffer, "File not found!");
            break;
        case MEMORY_SHORTAGE:
            strcpy(buffer, "Not enough memory!");
            break;
        default:
            strcpy(buffer, "Unexpected error!");
    }
    MessageBox(hwnd, buffer, "Error details", MB_ICONERROR | MB_OK);
}
