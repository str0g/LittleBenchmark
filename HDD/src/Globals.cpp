/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
#include "Globals.h"
//Globals Varuabels
namespace myColors{
    std::string ConsoleColors_[MYCOLORS] = { ("\033[22;30m"), //!<black
                                        ("\033[22;31m"), //!< red
                                        ("\033[22;32m"), //!< green
                                        ("\033[22;33m"), //!< brown
                                        ("\033[22;34m"), //!< blue
                                        ("\033[22;35m"), //!< magenta
                                        ("\033[22;36m"), //!< cyan
                                        ("\033[22;37m"), //!< gray
                                        ("\033[01;30m"), //!< dark gray
                                        ("\033[01;31m"), //!< light red
                                        ("\033[01;32m"),//!< light green
                                        ("\033[01;33m"),//!< yellow
                                        ("\033[01;34m"),//!< light blue
                                        ("\033[01;35m"),//!< light magenta
                                        ("\033[01;36m"),//!< light cyan
                                        ("\033[01;37m"),//!< white
    };
    int *TextColor;
}
