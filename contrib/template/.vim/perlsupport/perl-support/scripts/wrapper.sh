#!/bin/bash
#===============================================================================
#          FILE:  wrapper.sh
#         USAGE:  ./wrapper.sh executable [cmd-line-args] 
#   DESCRIPTION:  Wraps the execution of a programm or script.
#                 Use with xterm: xterm -e wrapper.sh executable cmd-line-args
#                 This script is used by several plugins:
#                  bash-support.vim, c.vim and perl-support.vim
#       OPTIONS:  ---
#  REQUIREMENTS:  which(1) - shows the full path of (shell) commands.
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR:  Dr.-Ing. Fritz Mehner (Mn), mehner@fh-swf.de
#       COMPANY:  Fachhochschule Südwestfalen, Iserlohn
#       CREATED:  23.11.2004 18:04:01 CET
#      REVISION:  $Id: wrapper.sh,v 1.4 2009/05/25 16:10:18 mehner Exp $
#===============================================================================

command="${@}"                           # the complete command line
executable="${1}"                        # name of the executable; may be quoted

fullname=$(which "$executable")
[ $? -eq 0 ] && executable="$fullname"

if [ ${#} -ge 1 ] && [ -x "$executable" ]
then
  shift
  "$executable" "${@}"
  echo -e "\"${command}\" returned ${?}"
else
  echo -e "\n  !! file \"${executable}\" does not exist or is not executable !!"
fi
read -p "  ... press return key ... " dummy
