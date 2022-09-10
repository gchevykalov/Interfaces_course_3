#include "fileModel.h"

/* Initializes the model
INPUT:
    model_t *model - pointer on model structure
OUTPUT:
    model_t *model - pointer on model structure filled with zero values
*/
void InitModel(model_t *model)
{
    model->Data = NULL;
    model->Lines = NULL;
    model->Size = 0;
    model->NumOfLines = 0;
    model->MaxLength = 0;
}

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
error_t FillModel(model_t *model, const char *filename)
{
    FILE *file = NULL;
    char *tmp = NULL;
    unsigned long curLine = 0;
    unsigned long lineLenght = 0;

    if ((file = fopen(filename, "rt")) == NULL)
        return NO_INPUT_FILE;

    /*  Getting the file size */
    fseek(file, 0, SEEK_END);
    model->Size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /*  Reading the file and save it to the model */
    model->Data = malloc((model->Size + 1) * sizeof(char));
    if (model->Data == NULL)
    {
        model->Size = 0;
        fclose(file);
        return MEMORY_SHORTAGE;
    }

    model->Size = fread(model->Data, sizeof(char), model->Size, file);
    model->Data[model->Size] = 0;

    fclose(file);

    /* Counting the number of lines */
    model->NumOfLines = 1;
    for (tmp = model->Data; *tmp != 0; tmp++)
        if (*tmp == '\n')
            model->NumOfLines++;

    model->Lines = malloc(model->NumOfLines * sizeof(char *));
    if (model->Lines == NULL)
    {
        free(model->Data);
        model->Data = NULL;
        model->Size = 0;
        model->NumOfLines = 0;
        return MEMORY_SHORTAGE;
    }

    /* Split data on lines */
    model->Lines[curLine++] = model->Data;
    for (tmp = model->Data; *tmp != 0; tmp++)
    {
        lineLenght++;
        if (*tmp == '\n')
        {
            if(model->MaxLength < --lineLenght)
                model->MaxLength = lineLenght;
            *tmp = 0;
            model->Lines[curLine++] = tmp + 1;
            lineLenght = 0;
        }
    }
    /* Checking the lenght of the last line */
    if(model->MaxLength < lineLenght)
        model->MaxLength = lineLenght;

    return SUCCESS;
}

/*  Clears the model
INPUT:
    model_t *model - pointer on model structure
OUTPUT:
    model_t *model - pointer on model structure filled with zero values
*/
void ClearModel(model_t *model)
{
    if (model == NULL)
        return;

    free(model->Lines);
    model->Lines = NULL;

    free(model->Data);
    model->Data = NULL;

    model->NumOfLines = 0;
    model->MaxLength = 0;
    model->Size = 0;
}
