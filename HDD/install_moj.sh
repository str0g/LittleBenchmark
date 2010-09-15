#! /bin/bash

# Project "Little Benchmark:HDD": Hard drive benchmark tool for Linux
# Module: install.sh
# Purpose: Install/uninstall script
#
# @author  Łukasz Buśko
# Copyright (C) 2008-2010  Łukasz Buśko

#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#directories
ULOCAL="/usr/local"
MANDIR=$ULOCAL"/man/man1/"
BINDIR=$ULOCAL"/bin"
PDFDOC="/usr/share/doc/littlebenchmark"
PDFDOCHDD=$PDFDOC"/hdd/"
MAN="man"
PDF="doc_pdf"

#function
function createDir(){
 if [ ! -d $1 ]; then
  mkdir $1
  if [ $2 ]; then
   cp $2/* $1
  fi
 fi
}

if test `id -u` != "0"; then
 echo Requring root permisions to perfrom installetion of:
 echo Man pages
 echo Pdf documentation
 echo Program compiled Binaries
 exit -1
fi


createDir $MANDIR $MAN
createDir $PDFDOC
createDir $PDFDOCHDD $PDF




