#!/bin/bash
export FOOENV="$1"
PATH=$(dirname "${BASH_SOURCE[0]}")/bin:$PATH
export PATH
alias foo='foobin -q -l'
