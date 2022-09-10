#ifndef __ERROR_H_INCLUDED
#define __ERROR_H_INCLUDED

#include <windows.h>

/* Error codes */
typedef enum
{
    SUCCESS,           /* Returned in case of successful execution */
    NO_INPUT_FILE,     /* Returned if the input file cannot be opened */
    MEMORY_SHORTAGE,   /* Returned if there is not enough memory to complete the task */
} error_t;

/* Displays a window with an error message
INPUT:
    HWND hwnd - window handle for which the displaying of message box was called
    error_t err - error code
*/
void DisplayMessageBox(HWND hwnd, error_t err);

#endif //__ERROR_H_INCLUDED
