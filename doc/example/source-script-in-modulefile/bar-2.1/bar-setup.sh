#!/bin/bash
export PATH=$(dirname $BASH_SOURCE)/bin:$PATH
bar() {
    barbin -q -l
}
