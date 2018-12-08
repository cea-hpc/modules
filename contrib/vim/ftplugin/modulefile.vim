
if exists("b:did_modulefile_ftplugin")
  finish
endif
let b:did_modulefile_ftplugin = 1

set syntax=modulefile

" adopted from tcl ftplugin:
let s:cpo_save = &cpo
set cpo-=C

setlocal comments=:#
setlocal commentstring=#%s
setlocal formatoptions+=croql

" Undo the stuff we changed.
let b:undo_ftplugin = "setlocal fo< com< cms< inc< inex< def< isf< kp<" .
            \         " | unlet! b:browsefilter"

" Restore the saved compatibility options.
let &cpo = s:cpo_save
unlet s:cpo_save
