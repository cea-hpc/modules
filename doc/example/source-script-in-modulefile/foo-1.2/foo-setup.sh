#!/bin/bash
export FOOENV="$1"
export PATH=$(dirname $BASH_SOURCE)/bin:$PATH                                                             
alias foo='foobin -q -l'
