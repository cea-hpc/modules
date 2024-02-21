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
syn keyword modCommand  getenv getvariant hide-modulefile hide-version is-avail is-loaded
syn keyword modCommand  is-saved is-used module module-alias module-forbid module-hide
syn keyword modCommand  module-info module-tag module-version module-virtual module-whatis
syn keyword modCommand  modulepath-label prepend-path prereq prereq-all prereq-any pushenv
syn keyword modCommand  remove-path reportError reportWarning require-fullname set-alias
syn keyword modCommand  set-function setenv source-sh system uname uncomplete unset-alias
syn keyword modCommand  unset-function unsetenv variant versioncmp x-resource

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
