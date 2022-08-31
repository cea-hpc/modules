#!/bin/bash
PATH=$(dirname "${BASH_SOURCE[0]}")/bin:$PATH
export PATH
bar() {
    barbin -q -l
}
