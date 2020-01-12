#!/bin/bash
#
# createmodule.sh - Takes the name of a environment init script and 
# produces a modulefile that duplicates the changes made by the init script
#
# Copyright (C) 2010-2012 by Orion E. Poplawski <orion@cora.nwra.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

usage="Usage: $0 [-p prefix] <initscript> [args]"

usage() {
  echo $usage 1>&2
  exit 1
}

while getopts "p:" opt
do
  case $opt in
    p) prefix=$OPTARG; shift 2;;
    *) usage;;
  esac
done

# Need a script name
[ -z "$1" ] && usage

# Need to be a readable script
if [ ! -r "$1" ]
then
  echo "ERROR: Cannot read $1" 1>&2
  exit 1 
fi

#Will print out array assignment list
printenvarray () {
  env | while read x
  do
    key=${x%%=*}
    value=`printenv "$key"`
    if [ $? -eq 0 ]
    then
      echo [$key]="'$value'"
    fi
  done
}

#Apparently we need to declare the associative arrays
declare -A env1 env2

#Record starting environment
eval env1=(`printenvarray`)

#Source the environment script
. "$@"

#Record ending environment
eval env2=(`printenvarray`)

#Print out the modulefile
echo "#%Module 1.0"

#Prefix
[ -n "$prefix" ] && echo -e "\nset prefix $prefix\n"

# dedup - remove duplicate entries from a list
#Subshell so we can sort the output
(
dedup() {
  list=`mktemp`
  [ -n "$2" ] && sep=$2 || sep=:
  echo $1 | sed -r -e 's,[^/]+/\.\./,,g' -e 's,[^/]+/\.\./,,g' -e "s/\\$sep/\n/g" |
    while read x
    do
      grep -Fx ${x} $list && continue
      if [ -n "$prefix" ]
      then
        echo $x | sed -e s,$prefix,\$prefix,
      else
        echo $x
      fi
      echo $x >> $list
    done | tr '\n' $sep | sed -e "s/\\$sep\$//"
  rm $list
} 
    
#Keys that changed
for key in "${!env1[@]}"
do
  if [ "${env1[$key]}" != "${env2[$key]}" ]
  then
    #Working directory change
    if [ "$key" = PWD ]
    then
      if [ -n "$prefix" ]
      then
        echo -e "chdir\t\t${env2[PWD]}" | sed -e s,$prefix,\$prefix,g
      else
        echo -e "chdir\t\t${env2[PWD]}"
      fi
    #Test for delete
    elif [ -z "${env2[$key]}" ]
    then
      echo -e "unsetenv\t${key}\t${env2[$key]}"
    #Test for prepend
    elif [ "${env2[$key]%${env1[$key]}}" != "${env2[$key]}" ]
    then
      added=${env2[$key]%${env1[$key]}}
      sep=${added: -1}
      added=${added%$sep}
      added=$(dedup $added $sep)
      if [ $sep = : ]
      then
        echo -e "prepend-path\t$key\t${added}"
      else
        echo -e "prepend-path\t--delim $sep\t$key\t${added}"
      fi
    #Test for prepend plus : added at end (MANPATH)
    elif [ "${key: -4}" = PATH -a "${env2[$key]%${env1[$key]}:}" != "${env2[$key]}" ]
    then
      added=$(dedup ${env2[$key]%${env1[$key]}:})
      echo -e "prepend-path\t$key\t${added}"
    #Test for append
    elif [ "${env2[$key]#${env1[$key]}}" != "${env2[$key]}" ]
    then
      added=${env2[$key]#${env1[$key]}}
      sep=${added:0:1}
      added=${added#$sep}
      added=$(dedup $added $sep)
      if [ $sep = : ]
      then
        echo -e "append-path\t$key\t${added}"
      else
        echo -e "append-path\t--delim $sep\t$key\t${added}"
      fi
    #Test for prepend plus append
    elif [ "${env2[$key]%${env1[$key]}*}" != "${env2[$key]}" ]
    then
      prepended=${env2[$key]%${env1[$key]}*}
      presep=${prepended: -1}
      prepended=${prepended%$presep}
      prepended=$(dedup $prepended $presep)
      appended=${env2[$key]#*${env1[$key]}}
      appsep=${appended:0:1}
      appended=${appended#$appsep}
      appended=$(dedup $appended $appsep)
      if [ $presep != $appsep -o -z "$prepended" -o -z "$appended" ]
      then
        #Unhandled
        echo "Unhandled change of $key" 1>&2
        echo "Before <${env1[$key]}>" 1>&2
        echo "After  <${env2[$key]}>" 1>&2
      else
        if [ $presep = : ]
        then
          echo -e "prepend-path\t$key\t${prepended}"
          echo -e "append-path\t$key\t${appended}"
        else
          echo -e "prepend-path\t--delim $presep\t$key\t${prepended}"
          echo -e "append-path\t--delim $appsep\t$key\t${appended}"
        fi
      fi
    else
      #Unhandled
      echo "Unhandled change of $key" 1>&2
      echo "Before <${env1[$key]}>" 1>&2
      echo "After  <${env2[$key]}>" 1>&2
    fi
  fi
  #Delete keys we have handled
  unset env1[$key]
  unset env2[$key]
done

#New keys
for key in "${!env2[@]}"
do
  if [ "$key" = OLDPWD ]
  then
    continue
  fi
  #Use prepend-path for new paths
  if [ "${key: -4}" = PATH -o "${key: -4}" = DIRS -o "${key: -4}" = FILES ]
  then
    # TODO - Need to handle stripping of default MANPATH
    echo -e "prepend-path\t${key}\t"$(dedup ${env2[$key]})
  else
    if [ -n "$prefix" ]
    then
      echo -e "setenv\t\t${key}\t${env2[$key]}" | sed -e s,$prefix,\$prefix,g
    else
      echo -e "setenv\t\t${key}\t${env2[$key]}"
    fi
  fi
done
) | sort
