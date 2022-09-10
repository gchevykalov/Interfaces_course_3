#ifndef __MODEL_H_INCLUDED
#define __MODEL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "../error/error.h"

/*  The structure that implements the model */
typedef struct
{
    char *Data;                   /* Data */
    unsigned long Size;           /* The number of characters */
    char **Lines;                 /* Pointers on file lines */
    unsigned long NumOfLines;     /* Number of lines */
    unsigned long MaxLength;      /* Maximum line length */
} model_t;

/* Initializes the model
INPUT:
    model_t *model - pointer on model structure
OUTPUT:
    model_t *model - pointer on model structure filled with zero values
*/
void InitModel(model_t *model);

/*  Fills the model with data from the file
INPUT:
    model_t *model - pointer on model structure
    const char *filename - path to file
OUTPUT:
    model_t *model - pointer on model structure filled with data if operation
                     ended successfully, otherwise filled with zeroes
RETURN:
    error_t - error code
*/
error_t FillModel(model_t *model, const char *filename);

/*  Clears the model
INPUT:
    model_t *model - pointer on model structure
OUTPUT:
    model_t *model - pointer on model structure filled with zero values
*/
void ClearModel(model_t *model);

#endif // __MODEL_H_INCLUDED
