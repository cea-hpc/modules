
" include tcl syntax
set syntax=tcl

"Clear tcl keywords to match to avoid priority of keyword
" (e.g append vs append-path)
syn clear tclCommand

" set again tclCommand with `syn match`
" Basic Tcl commands: http://www.tcl.tk/man/tcl8.6/TclCmd/contents.htm
syn match tclCommand		"\v<(after|append|array|bgerror|binary|cd|chan|clock|close|concat)>"
syn match tclCommand		"\v<(dde|dict|encoding|eof|error|eval|exec|exit|expr|fblocked)>"
syn match tclCommand		"\v<(fconfigure|fcopy|file|fileevent|flush|format|gets|glob)>"
syn match tclCommand		"\v<(global|history|http|incr|info|interp|join|lappend|lassign)>"
syn match tclCommand		"\v<(lindex|linsert|list|llength|lmap|load|lrange|lrepeat)>"
syn match tclCommand		"\v<(lreplace|lreverse|lsearch|lset|lsort|memory|my|namespace)>"
syn match tclCommand		"\v<(next|nextto|open|package|pid|puts|pwd|read|refchan|regexp)>"
syn match tclCommand		"\v<(registry|regsub|rename|scan|seek|self|set|socket|source)>"
syn match tclCommand		"\v<(split|string|subst|tell|time|trace|unknown|unload|unset)>"
syn match tclCommand		"\v<(update|uplevel|upvar|variable|vwait)>"

" The 'Tcl Standard Library' commands: http://www.tcl.tk/man/tcl8.6/TclCmd/library.htm
syn match tclCommand		"\v<(auto_execok|auto_import|auto_load|auto_mkindex|auto_reset)>"
syn match tclCommand		"\v<(auto_qualify|tcl_findLibrary|parray|tcl_endOfWord)>"
syn match tclCommand		"\v<(tcl_startOfNextWord|tcl_startOfPreviousWord)>"
syn match tclCommand		"\v<(tcl_wordBreakAfter|tcl_wordBreakBefore)>"

" Modulefile commands: https://modules.readthedocs.io/en/stable/modulefile.html
syn match modCommand "\v<(append|prepend|remove)-path>"
syn match modCommand "\v<prereq>"
syn match modCommand "\v<(module|conflict|chdir|prereq)>"
syn match modCommand "\v<module-(alias|info|log|trace|user|verbosity|version|virtual|whatis)>"
syn match modCommand "\v<(un)?set-(alias|function)>"
syn match modCommand "\v<(un)?setenv>"
syn match modCommand "\v<getenv>"
syn match modCommand "\v<(system|uname|x-resource)>"

syn match modCommand "\v<is-(loaded|saved|used|avail)>"

hi def link modCommand tclSpecial

syn match  modProc "\v<ModulesHelp>"
syn match  modProc "\v<ModulesDisplay>"
syn match  modProc "\v<ModulesTest>"

hi def link modProc tclSpecial


syn match modVar "\v<ModulesVersion>"
hi def link modVar tclSpecial

