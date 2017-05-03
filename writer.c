#include "writer.h"
#include "errors.h"
#include <fcntl.h> /* open */
#include <unistd.h> /* close, write, read */
#include <sys/stat.h> /* S_IWUSR, S_IRUSR */
#include <stdlib.h> /* malloc, free */
#include <stdio.h> /* sprintf */
#include <string.h> /* memset */
#include <errno.h> /* errno */

int bp__writer_create(bp__writer_t *w, const char *filename)
{
    off_t filesize;
    size_t filename_length;

    /* copy filename + '\0' char */
    filename_length = strlen(filename) + 1;
    w->filename = malloc(filename_length);
    if (w->filename == NULL) return BP_EALLOC;
    memcpy(w->filename, filename, filename_length);

    w->fd = open(filename,
                 O_RDWR | O_APPEND | O_CREAT,
                 S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    if (w->fd == -1) goto error;

    /* Determine filesize */
    filesize = lseek(w->fd, 0, SEEK_END);
    if (filesize == -1) goto error;

    w->filesize = (uint64_t) filesize;

    /* Nullify padding to shut up valgrind */
    memset(&w->padding, 0, sizeof(w->padding));

    return BP_OK;

error:
    free(w->filename);
    return BP_EFILE;
}

int bp__writer_destroy(bp__writer_t *w)
{
    free(w->filename);
    w->filename = NULL;
    if (close(w->fd)) return BP_EFILE;
    return BP_OK;
}

int bp__writer_fsync(bp__writer_t *w)
{
#ifdef F_FULLFSYNC
    /* OSX support */
    return fcntl(w->fd, F_FULLFSYNC);
#else
    return fdatasync(w->fd) == 0 ? BP_OK : BP_EFILEFLUSH;
#endif
}

int bp__writer_write(bp__writer_t *w,
                     const void *data,
                     uint64_t *offset,
                     uint64_t *size)
{
    ssize_t written;
    uint32_t padding = sizeof(w->padding) - (w->filesize % sizeof(w->padding));

    /* Write padding */
    if (padding != sizeof(w->padding)) {
        written = write(w->fd, &w->padding, (size_t) padding);
        if ((uint32_t) written != padding) return BP_EFILEWRITE;
        w->filesize += padding;
    }

    /* Ignore empty writes */
    if (size == NULL || *size == 0) {
        if (offset != NULL) *offset = w->filesize;
        return BP_OK;
    }

    /* head shouldn't be compressed */
    written = write(w->fd, data, *size);

    if ((uint64_t) written != *size) return BP_EFILEWRITE;

    /* change offset */
    *offset = w->filesize;
    w->filesize += written;

    return BP_OK;
}


int bp__writer_read(bp__writer_t *w,
                    const uint64_t offset,
                    uint64_t *size,
                    void **data)
{
    ssize_t bytes_read;
    char *cdata;

    if (w->filesize < offset + *size) return BP_EFILEREAD_OOB;

    /* Ignore empty reads */
    if (*size == 0) {
        *data = NULL;
        return BP_OK;
    }

    cdata = malloc(*size);
    if (cdata == NULL) return BP_EALLOC;

    bytes_read = pread(w->fd, cdata, (size_t) *size, (off_t) offset);
    if ((uint64_t) bytes_read != *size) {
        free(cdata);
        return BP_EFILEREAD;
    }
    *data = cdata;
    return BP_OK;
}
