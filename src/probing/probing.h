/**
 *  \file probing.h
 *  \brief A monitoring toolkit.
 *
 *  This toolkit provides a simple monitoring system which allows 
 *  the programmer to include messages into his/her code.
 *  The system may be turned on or off.
 *  The system uses a probing depth to determine which messages must be displayed.
 *  The depth is a positive value.
 *  Upon writing the code, one should assign a depth to every probing message.
 *  Upon activating the probing system, one sets the range of depths that must be logged or displayed.
 *
 *  \author Artur Carneiro Pereira - 2008-2009, 2016
 *  \author Miguel Oliveira e Silva - 2009
 *  \author António Rui Borges - 2010-2015
 *
 *  \remarks In case an error occurs, every function throws an error code (an int)
 */

#ifndef __SOFS16_PROBING__
#define __SOFS16_PROBING__

#include <stdio.h>
#include <stdint.h>

/**
 *  \brief Opening of the probing system.
 *
 *  \param fp a file pointer of the output stream to be used
 */
void soOpenProbe(FILE * fp = stderr);

/**
 *  \brief Closing the probing system.
 */
void soCloseProbe(void);

/**
 *  \brief Set probing depths.
 *
 *  \param lower the minimum probing depth to be activated
 *  \param upper the maximum probing depth to be activated
 */
void soSetProbeDepths(uint32_t lower, uint32_t upper);

/**
 *  \brief Print a probing message with the given depth.
 *
 *  Apart from the \e depth argument it works like the \e fprintf function.
 *
 *  \param depth the probing depth of the message
 *  \param fmt the format string (as in \e fprintf)
 */
void soProbe(uint32_t depth, const char *fmt, ...);

/**
 *  \brief Print a probing message with the given depth and color.
 *
 *  Apart from the \e depth argument, it works like the \e fprintf function.
 *  The color is a string in ANSI terminal format. 
 *  For instance "01;31" means red font.
 *
 *  \param depth the probing depth of the message
 *  \param color string defining the probing color
 *  \param fmt the format string (as in \e fprintf)
 */
void soColorProbe(uint32_t depth, const char *color, const char *fmt, ...);

#endif                          /* __SOFS16_PROBING__ */
