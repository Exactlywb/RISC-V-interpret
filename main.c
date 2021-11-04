#include "interpret/interpret.h"

typedef enum InputErr {

    NO_INPUT_ERR,
    NO_INPUT_FILE,
    TOO_MUCH_ARGS,
    NULL_PTR_ARGV

} InputErr;

int     CheckInput      (const int argc, char** argv);
void    PrintInputErr   (const int err);

int main (int argc, char** argv) {

    int inputErr = CheckInput (argc, argv);
    SECURITY_FUNCTION (inputErr != NO_INPUT_ERR, {PrintInputErr (inputErr);}, inputErr);

    int inputFile = open (argv [1], O_RDONLY);
    SECURITY_FUNCTION (inputFile < 0, {perror ("Can't open input file");}, errno);

    int interpretErr = RunInterpret (inputFile);
    SECURITY_FUNCTION (interpretErr < 0, {}, interpretErr);

    int checkClose = close (inputFile);
    SECURITY_FUNCTION (checkClose < 0, {perror ("Bad close for input file");}, errno);

    return 0;

}

int CheckInput (const int argc, char** argv) {

    if (argc < 2)
        return NO_INPUT_FILE;
    if (argc > 2)
        return TOO_MUCH_ARGS;
    
    if (IS_NULL (argv))
        return NULL_PTR_ARGV;
    
    return NO_INPUT_ERR;

}

void PrintInputErr (const int err) {

    switch (err) {

        case NO_INPUT_ERR:
            return;
        case NO_INPUT_FILE:
            printf ("You have to enter input file name!\n");
            return;
        case TOO_MUCH_ARGS:
            printf ("You have to enter only input file name!\n");
            return;
        case NULL_PTR_ARGV:
            printf ("Null pointer on char** argv was catched. (?!?!)\n");
            return;
        default:
            printf ("Unexpected error in function %s\n", __func__);

    }

}
