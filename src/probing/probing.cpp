/*
 *  \author Artur Carneiro Pereira - 2008, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "probing.h"
#include "exception.h"

/* *************************************** */

/* Internal data structure */

/* stream where probing messages are sent to */
static FILE *fp = stderr;

/* lower limit of probe */
static uint32_t lowerDepth = 0;

/* upper limit of probe */
static uint32_t upperDepth = 0;

/* *************************************** */

void soOpenProbe(FILE * fs)
{
    /* close previous stream, if one is opened */
    if (fp != NULL)
    {
        fflush(fp);
        fclose(fp);
    }

    /* set output stream */
    fp = fs;
}

/* *************************************** */

void soCloseProbe(void)
{
    /* close previous stream, if one is opened */
    if (fp != NULL)
    {
        fflush(fp);
        fclose(fp);
    }

    /* set output stream */
    fp = NULL;
}

/* *************************************** */

void soSetProbeDepths(uint32_t lower, uint32_t upper)
{
    /* check arguments */
    if (upper < lower)
        throw SOException(EINVAL, __FUNCTION__);

    /* set depths */
    lowerDepth = lower;
    upperDepth = upper;
}

/* *************************************** */

void soProbe(uint32_t depth, const char *fmt, ...)
{
    /* do nothing, if out of active range */
    if ((depth < lowerDepth) || (depth > upperDepth))
        return;

    /* print the message */
    fprintf(fp, "\e[01;34m(%d)-->\e[0m ", depth);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
}

/* *************************************** */

void soColorProbe(uint32_t depth, const char *color, const char *fmt, ...)
{
    /* do nothing, if out of active range */
    if ((depth < lowerDepth) || (depth > upperDepth))
        return;

    /* print the message */
    fprintf(fp, "\e[%sm(%d)-->\e[0m ", color, depth);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
}

/* *************************************** */
