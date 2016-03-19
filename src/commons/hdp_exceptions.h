/* 
 * File:   hdp_exceptions.h
 * Author: Manzil
 *
 * Created on February 29, 2016, 2:09 AM
 */

#ifndef HDP_EXCEPTIONS_H
#define	HDP_EXCEPTIONS_H

#include <string>
#include <stdexcept>

class hdp_exception : public std::runtime_error {

    std::string e_msg;
    
    public:
        hdp_exception(std::string msg = "MyException\n") : e_msg(msg), std::runtime_error(msg)
        { }
  
        int add_exceptions(std::string msg)
        {
            e_msg += msg;
            return 0;
        }
        
        const char* what () const noexcept
        {
            return e_msg.c_str();
        }
};

#endif	/* HDP_EXCEPTIONS_H */

