#ifndef  ERROR_HANDLER_H
#define  ERROR_HANDLER_H

#include "MantidQtImageViewer/DllOptionIV.h"

/**
    @class ErrorHandler 
  
      This class has static methods that do various basic calculations 
    needed by other parts of the ImageView package.
 
    @author Dennis Mikkelson 
    @date   2012-05-18 
     
    Copyright © 2012 ORNL, STFC Rutherford Appleton Laboratories
  
    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    Code Documentation is available at 
                 <http://doxygen.mantidproject.org>
 */

namespace MantidQt
{
namespace ImageView
{


class EXPORT_OPT_MANTIDQT_IMAGEVIEWER ErrorHandler 
{
  public:

    /// Display the specified string in an error message 
    static void Error( std::string  text );

    /// Display the specified string in a warning message 
    static void Warning( std::string  text );

    /// Display the specified string in a notice message 
    static void Notice( std::string  text );

};

} // namespace MantidQt 
} // namespace ImageView 


#endif   // ERROR_HANLDER_H
