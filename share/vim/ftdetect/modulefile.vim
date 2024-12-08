" Vim filetype detection file
" Language:     Tcl Modulefile
" Maintainer:   Xavier Delaruelle <xavier.delaruelle@cea.fr>
" Original:     Felix Neumärker <xdch47@posteo.de>
" Reference:    https://modules.readthedocs.io/en/latest/modulefile.html

au BufNewFile,BufRead *
    \ if (getline(1) =~? "^#%Module") |
    \     set filetype=modulefile |
    \ endif

" vim:set tabstop=4 shiftwidth=4 expandtab textwidth=100:
