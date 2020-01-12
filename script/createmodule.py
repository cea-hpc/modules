#!/usr/bin/python
#
# createmodule.py - Takes the name of a environment init script and 
# produces a modulefile that duplicates the changes made by the init script
#
# Copyright (C) 2012 by Orion E. Poplawski <orion@cora.nwra.com>
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
from __future__ import print_function

from optparse import OptionParser
import os,sys,re
from subprocess import *
import platform

# Handle options
usage = "Usage: %prog [-p prefix] <initscript> [args]"
parser = OptionParser()
parser.set_usage(usage)
parser.add_option('-p', '--prefix', action='store', type='string', dest='prefix', help='Specify path prefix')
parser.add_option('--noprefix', action='store_true', dest='noprefix', default=False, help='Do not generate a prefix')
(options, args) = parser.parse_args()

# Need a script name
if not args:
    parser.print_usage()
    exit(1)

# Determine if running environment is based on cmd.exe or not
def iscmdshell():
    return True if platform.system() == 'Windows' else False

# Return environment after a command
def getenv(cmd = ':'):
    env = {}
    if iscmdshell():
        # ':' command not supported by cmd.exe
        cmd = (cmd if cmd != ':' else '@echo off') + " & set"
    else:
        cmd = cmd + ";env"
    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE, universal_newlines=True)
    (stdout, stderr) = p.communicate()
    if p.returncode != 0:
        print("ERROR: Could not execute initscript:")
        print("%s returned exit code %d" % (cmd, p.returncode))
        print(stderr)
        exit(1)
    if stderr != '':
        print("WARNING: initscript sent the following to stderr:")
        print(stderr)
    # Parse the output key=value pairs
    skip = False
    for line in stdout.splitlines():
        if skip:
            if line == '}':
                skip = False
            continue
        elif iscmdshell() and line.find('=') == -1:
            continue
        try:
            (var,value) = line.split('=',1)
        except ValueError:
            print("ERROR: Could not parse output line:")
            print(line)
            exit(1)
        # Exported functions - not handled
        if value.find('() {') == 0:
            skip = True
        else:
            env[var] = value
    return env

#Record initial environment
env1=getenv()

#Record environment after sourcing the initscript
if iscmdshell():
    if len(args)>1:
        env2=getenv('"' + args[0] + '" ' + " ".join(args[1:]))
    else:
        env2=getenv('"' + args[0] + '"')
else:
    env2=getenv(". " + " ".join(args))

# Initialize our variables for storing modifications
chdir = None
appendpath = {}
prependpath = {}
unhandled = {}
setenv = {}
unsetenv = []
pathnames = []

# Function to nomalize all paths in a list of paths and remove duplicate items
def normpaths(paths):
    newpaths = []
    for path in paths:
        normpath = os.path.normpath(path)
        if normpath not in newpaths and normpath != '.':
             newpaths.append(os.path.normpath(path))
    return newpaths

# Start with existing keys and look for changes
for key in env1.keys():
    # Test for delete
    if key not in env2:
        unsetenv.append(key)
        continue
    # No change
    if env1[key] == env2[key]:
        del env2[key]
        continue
    #Working directory change
    if key == 'PWD':
        chdir=os.path.normpath(env2[key])
        pathnames.append(chdir)
        del env2[key]
        continue
    # Determine modifcations to beginning and end of the string
    try:
        (prepend,append) = env2[key].split(env1[key])
    except ValueError:
         continue
    if prepend:
        presep = prepend[-1:]
        prependpaths = prepend.strip(presep).split(presep)
        # LICENSE variables often include paths outside install directory
        if 'LICENSE' not in key:
            pathnames += prependpaths
        if presep not in prependpath:
            prependpath[presep] = {}
        newpath = presep.join(normpaths(prependpaths))
        if newpath:
            prependpath[presep][key] = newpath
        else:
            unhandled[key] = env2[key]
    if append:
        appsep = append[0:1]
        appendpaths = append.strip(appsep).split(appsep)
        # LICENSE variables often include paths outside install directory
        if 'LICENSE' not in key:
            pathnames += appendpaths
        if appsep not in appendpath:
            appendpath[appsep] = {}
        newpath = appsep.join(normpaths(appendpaths))
        if newpath:
            appendpath[appsep][key] = newpath
        else:
            unhandled[key] = env2[key]
    del env2[key]
      
# We're left with new keys in env2
for key in env2.keys():
    # Use prepend-path for new paths
    if (re.search('(DIRS|FILES|PATH)$',key)) or (':' in env2[key]):
        prependpaths = env2[key].strip(':').split(':')
        # MANPATH can have system defaults added it it wasn't previously set
        # LICENSE variables often include paths outside install directory
        if key != 'MANPATH' and 'LICENSE' not in key:
            pathnames += prependpaths
        if ':' not in prependpath:
            prependpath[':'] = {}
        prependpath[':'][key] = ':'.join(normpaths(prependpaths))
        continue
    # Set new variables
    setenv[key] = os.path.normpath(env2[key])
    if 'LICENSE' not in key:
        pathnames.append(setenv[key])

# Report unhandled keys
for key in unhandled.keys():
    print("Unhandled change of", key, file=sys.stderr)
    print("Before <%s>" % env1[key], file=sys.stderr)
    print("After <%s>" % unhandled[key], file=sys.stderr)
    for sepkey in appendpath.keys():
        appendpath[sepkey].pop(key, None)
    for sepkey in prependpath.keys():
        prependpath[sepkey].pop(key, None)

# Determine a prefix
prefix=None
if options.prefix:
    prefix = options.prefix
elif not options.noprefix:
    prefix = os.path.commonprefix(pathnames).rstrip('/')
    if prefix == '':
          prefix = None

# Print out the modulefile
print("#%Module 1.0")

# Prefix
if prefix is not None:
    print("\nset prefix " + prefix + "\n")

# Chdir
if chdir is not None:
    print("chdir\t" + chdir)

# Function to format output line with tabs and substituting prefix
def formatline(item, key, value=None):
    print(item, end=' ')
    print("\t"*(2-(len(item)+1)/8), end=' ')
    print(key, end=' ')
    if value is not None:
        print("\t"*(3-(len(key)+1)/8), end=' ')
        if prefix is not None:
            # Prefer usage of regular expression to perform a none
            # case-sensitive substitution (cygwin vs cmd.exe)
            if iscmdshell():
                print(re.sub('(?i)' + re.escape(prefix), '$prefix', value))
            else:
                print(value.replace(prefix,'$prefix'))
        else:
            print(value)

# Paths first, grouped by variable name
for sepkey in prependpath.keys():
    pathkeys = prependpath[sepkey].keys()
    pathkeys.sort()
    for key in pathkeys:
        if sepkey == ":":
            formatline("prepend-path",key,prependpath[sepkey][key])
        else:
            formatline("prepend-path --delim %s" % sepkey,key,prependpath[sepkey][key])

for sepkey in appendpath.keys():
    pathkeys = appendpath[sepkey].keys()
    pathkeys.sort()
    for key in pathkeys:
        if sepkey == ":":
            formatline("append-path",key,appendpath[sepkey][key])
        else:
            formatline("append-path --delim %s" % sepkey,key,appendpath[sepkey][key])

# Setenv
setenvkeys = list(setenv.keys())
setenvkeys.sort()
if setenvkeys:
    print()
for key in setenvkeys:
    formatline("setenv",key,setenv[key])

# Unsetenv
unsetenv.sort()
if unsetenv:
    print()
for key in unsetenv:
    formatline("unsetenv",key)
