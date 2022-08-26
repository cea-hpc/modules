#!/bin/bash
export FOOENV="$1"
export PATH=$(dirname "${BASH_SOURCE[0]}")/bin:$PATH
alias foo='foobin -q -l'
