#include "interpret.h"

static      int         RunParser ();

int RunInterpret (const int input) {

    SECURITY_FUNCTION (input < 0, {printf ("Bad input file descriptor!\n");}, input);

    off_t fileSize = GetFileSize (input);
    SECURITY_FUNCTION (fileSize < 0, {}, (int)fileSize);
    printf ("File size = %d\n", fileSize);

    char* buffer = (char*)calloc ((size_t)fileSize, sizeof (char));
    SECURITY_FUNCTION (IS_NULL (buffer), {printf ("Bad alloc for char* buffer in function %s\n"
                                                    , __func__);}, -1);

    

    free (buffer);

    return 0;

}

off_t GetFileSize (const int fd) {

    off_t res = lseek (fd, 0, SEEK_END);
    SECURITY_FUNCTION (res < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    off_t lseekErr = lseek (fd, 0, SEEK_SET);
    SECURITY_FUNCTION (lseekErr < 0, {perror ("Bad execution for lseek");}, (off_t)errno);

    return res;

}
