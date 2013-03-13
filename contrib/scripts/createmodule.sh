#!/bin/bash
#
# createmodule.sh - Takes the name of a environment init script and 
# produces a modulefile that duplicates the changes made by the init script

if [ -z "$1" ]
then
  echo "usage: $0 <initscript>" 1>&2
  exit 1
fi

#Will print out array assignment list
printenvarray () {
  env | while read x
  do
    key=${x%%=*}
    value=${x#*=}
    echo [$key]="'$value'"
  done
}

declare -A env1 env2

#Record starting environment
eval env1=(`printenvarray`)

#Source the environment script
. "$@"

#Record ending environment
eval env2=(`printenvarray`)

echo "#%Module 1.0"

#Keys that changed
for key in "${!env1[@]}"
do
   if [ "${env1[$key]}" != "${env2[$key]}" ]
   then
      if [ "$key" = PWD ]
      then
	echo -e "chdir\t\t${env2[PWD]}"
      #Test for delete
      elif [ -z "${env2[$key]}" ]
      then
         echo -e "unsetenv\t${key}\t${env2[$key]}"
      #Test for prepend
      elif [ "${env2[$key]%${env1[$key]}}" != "${env2[$key]}" ]
      then
         added="${env2[$key]%${env1[$key]}}"
         echo -e "prepend-path\t$key\t${added%:}"
      #Test for append
      elif [ "${env2[$key]#${env1[$key]}}" != "${env2[$key]}" ]
      then
         added="${env2[$key]#${env1[$key]}}"
         echo -e "append-path\t$key\t${added#:}"
      else
         echo $key "${env1[$key]} != ${env2[$key]}"
      fi
   fi
   #Delete keys we've handled
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
   if [ "${key/PATH/}" != "$key" ]
   then
     echo -e "prepend-path\t${key}\t${env2[$key]}"
   else
     echo -e "setenv\t\t${key}\t${env2[$key]}"
   fi
done
