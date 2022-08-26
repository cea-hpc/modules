#!/bin/bash
export PATH=$(dirname "${BASH_SOURCE[0]}")/bin:$PATH
bar() {
    barbin -q -l
}
