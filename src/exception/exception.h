/**
 * \file exception.h
 * \brief the sofs16 exception support
 * \author Artur Pereira - 2016
 */
#ifndef __SOFS16_EXCEPTION__
#define __SOFS16_EXCEPTION__

#include <exception>
#include <string>
#include <sstream>

#include <string.h>

/**
 * \brief sofs16 exception \c struct
 */
struct SOException:public std::exception
{
    int en;             ///< (system) error number
    const char *msg;    ///< name of function that has thrown the exception
    /**
     * \brief the constructor
     * \param _en (system) error number 
     * \param _msg name of function throwing the exception
     */
    SOException(int _en, const char *_msg):en(_en), msg(_msg)
    {
    }

    /**
     * \brief default exception message
     * \return pointer to exception message
     */
    const char *what() const throw()
    {
        std::ostringstream ss;
        ss << "\e[01;31m" << std::
            string(msg) << ": error " << en << " (" << strerror(en) << ")\e[0m";
        return ss.str().c_str();
    }
};

#endif                          /* __SOFS16_EXCEPTION__ */
