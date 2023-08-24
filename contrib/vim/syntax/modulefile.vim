" Vim syntax file
" Language:     Tcl Modulefile
" Maintainer:   Xavier Delaruelle <xavier.delaruelle@cea.fr>
" Original:     Felix Neumärker <xdch47@posteo.de>
" Reference:    https://modules.readthedocs.io/en/latest/modulefile.html

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
    finish
endif

runtime syntax/tcl.vim

syn iskeyword @,48-57,_,192-255,-

" Modulefile commands
syn keyword modCommand  always-load append-path chdir complete conflict depends-on family
syn keyword modCommand  getenv getvariant is-avail is-loaded is-saved is-used module
syn keyword modCommand  module-alias module-forbid module-hide module-info module-tag
syn keyword modCommand  module-version module-virtual module-whatis modulepath-label
syn keyword modCommand  prepend-path prereq prereq-all prereq-any pushenv remove-path
syn keyword modCommand  reportError reportWarning require-fullname set-alias set-function
syn keyword modCommand  setenv source-sh system uname uncomplete unset-alias unset-function
syn keyword modCommand  unsetenv variant versioncmp x-resource

" Modulefile procedures
syn keyword modProc     ModulesHelp ModulesDisplay ModulesTest

" Modulefile variables
syn keyword modVar      ModulesVersion ModulesCurrentModulefile ModuleTool ModuleToolVersion
syn keyword modVar      ModuleVariant

hi def link modCommand  Special
hi def link modProc     Special
hi def link modVar      Special

let b:current_syntax = "modulefile"

" vim:set tabstop=4 shiftwidth=4 expandtab textwidth=100:
