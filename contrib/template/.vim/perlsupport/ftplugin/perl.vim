" Vim filetype plugin file
"
"   Language :  Perl
"     Plugin :  perl-support.vim
" Maintainer :  Fritz Mehner <mehner@fh-swf.de>
"   Revision :  $Id: perl.vim,v 1.53 2009/05/25 08:06:11 mehner Exp $
"
" ----------------------------------------------------------------------------
"
" Only do this when not done yet for this buffer
"
if exists("b:did_PERL_ftplugin")
  finish
endif
let b:did_PERL_ftplugin = 1
"
let s:UNIX  = has("unix") || has("macunix") || has("win32unix")
let s:MSWIN = has("win16") || has("win32")   || has("win64")    || has("win95")
"
" ---------- tabulator / shiftwidth ------------------------------------------
"  Set tabulator and shift width to 4 conforming to the Perl Style Guide.
"  Uncomment the next two lines to force these settings for all files with
"  filetype 'perl' .
"
setlocal  tabstop=4
setlocal  shiftwidth=4
"
" ---------- Add ':' to the keyword characters -------------------------------
"            Tokens like 'File::Find' are recognized as
"            one keyword
"
setlocal iskeyword+=:
"
" ---------- Do we have a mapleader other than '\' ? ------------
"
if exists("g:Perl_MapLeader")
  let maplocalleader  = g:Perl_MapLeader
endif
"
" ---------- Perl dictionary -------------------------------------------------
" This will enable keyword completion for Perl
" using Vim's dictionary feature |i_CTRL-X_CTRL-K|.
"
if exists("g:Perl_Dictionary_File")
    silent! exec 'setlocal dictionary+='.g:Perl_Dictionary_File
endif
"
" ---------- commands --------------------------------------------------
"
command! -nargs=? CriticOptions         call Perl_PerlCriticOptions  (<f-args>)
command! -nargs=1 -complete=customlist,Perl_PerlCriticSeverityList   CriticSeverity   call Perl_PerlCriticSeverity (<f-args>)
command! -nargs=1 -complete=customlist,Perl_PerlCriticVerbosityList  CriticVerbosity  call Perl_PerlCriticVerbosity(<f-args>)
command! -nargs=1 RegexSubstitutions    call perlsupportregex#Perl_PerlRegexSubstitutions(<f-args>)
"
"command! -nargs=1 RegexCodeEvaluation    call Perl_RegexCodeEvaluation(<f-args>)
"
command! -nargs=1 -complete=customlist,perlsupportprofiling#Perl_SmallProfSortList SmallProfSort
        \ call  perlsupportprofiling#Perl_SmallProfSortQuickfix ( <f-args> )
"
command! -nargs=1 -complete=customlist,perlsupportprofiling#Perl_FastProfSortList FastProfSort
        \ call  perlsupportprofiling#Perl_FastProfSortQuickfix ( <f-args> )
"
command! -nargs=1 -complete=customlist,perlsupportprofiling#Perl_NYTProfSortList NYTProfSort
        \ call  perlsupportprofiling#Perl_NYTProfSortQuickfix ( <f-args> )
"
command! -nargs=0  NYTProfCSV call perlsupportprofiling#Perl_NYTprofReadCSV  ()
"
" ---------- Key mappings : function keys ------------------------------------
"
"   Ctrl-F9   run script
"    Alt-F9   run syntax check
"  Shift-F9   set command line arguments
"  Shift-F1   read Perl documentation
" Vim (non-GUI) : shifted keys are mapped to their unshifted key !!!
"
if has("gui_running")
  "
   map    <buffer>  <silent>  <A-F9>             :call Perl_SyntaxCheck()<CR>:redraw!<CR>:call Perl_SyntaxCheckMsg()<CR>
  imap    <buffer>  <silent>  <A-F9>        <C-C>:call Perl_SyntaxCheck()<CR>:redraw!<CR>:call Perl_SyntaxCheckMsg()<CR>
  "
   map    <buffer>  <silent>  <C-F9>             :call Perl_Run()<CR>
  imap    <buffer>  <silent>  <C-F9>        <C-C>:call Perl_Run()<CR>
  "
   map    <buffer>  <silent>  <S-F9>             :call Perl_Arguments()<CR>
  imap    <buffer>  <silent>  <S-F9>        <C-C>:call Perl_Arguments()<CR>
  "
   map    <buffer>  <silent>  <S-F1>             :call Perl_perldoc()<CR><CR>
  imap    <buffer>  <silent>  <S-F1>        <C-C>:call Perl_perldoc()<CR><CR>
endif
"
"-------------------------------------------------------------------------------
"   Key mappings for menu entries
"   The mappings can be switched on and off by g:Perl_NoKeyMappings
"-------------------------------------------------------------------------------
"
if !exists("g:Perl_NoKeyMappings") || ( exists("g:Perl_NoKeyMappings") && g:Perl_NoKeyMappings!=1 )
  " ---------- plugin help -----------------------------------------------------
  "
   map    <buffer>  <silent>  <LocalLeader>hp         :call Perl_HelpPerlsupport()<CR>
  imap    <buffer>  <silent>  <LocalLeader>hp    <C-C>:call Perl_HelpPerlsupport()<CR>
  "
  " ----------------------------------------------------------------------------
  " Comments
  " ----------------------------------------------------------------------------
  "
  inoremap    <buffer>  <silent>  <LocalLeader>cj    <C-C>:call Perl_AlignLineEndComm("a")<CR>a
  inoremap    <buffer>  <silent>  <LocalLeader>cl    <C-C>:call Perl_LineEndComment("")<CR>A
  nnoremap    <buffer>  <silent>  <LocalLeader>cj         :call Perl_AlignLineEndComm("a")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cl         :call Perl_LineEndComment("")<CR>A
  vnoremap    <buffer>  <silent>  <LocalLeader>cj    <C-C>:call Perl_AlignLineEndComm("v")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>cl    <C-C>:call Perl_MultiLineEndComments()<CR>A

  nnoremap    <buffer>  <silent>  <LocalLeader>cs         :call Perl_GetLineEndCommCol()<CR>

  nnoremap    <buffer>  <silent>  <LocalLeader>cfr        :call Perl_InsertTemplate("comment.frame")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cfu        :call Perl_InsertTemplate("comment.function")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cm         :call Perl_InsertTemplate("comment.method")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ch         :call Perl_InsertTemplate("comment.file-description-pl")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ce         :call Perl_InsertTemplate("comment.file-description-pm")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ca         :call Perl_InsertTemplate("comment.file-description-t")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cp         :call Perl_InsertTemplate("comment.file-description-pod")<CR>

  inoremap    <buffer>  <silent>  <LocalLeader>cfr   <C-C>:call Perl_InsertTemplate("comment.frame")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>cfu   <C-C>:call Perl_InsertTemplate("comment.function")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>cm    <C-C>:call Perl_InsertTemplate("comment.method")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ch    <C-C>:call Perl_InsertTemplate("comment.file-description-pl")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ce    <C-C>:call Perl_InsertTemplate("comment.file-description-pm")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ca    <C-C>:call Perl_InsertTemplate("comment.file-description-t")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>cp    <C-C>:call Perl_InsertTemplate("comment.file-description-pod")<CR>

  nnoremap    <buffer>  <silent>  <LocalLeader>ckb        $:call Perl_InsertTemplate("comment.keyword-bug")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ckt        $:call Perl_InsertTemplate("comment.keyword-todo")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ckr        $:call Perl_InsertTemplate("comment.keyword-tricky")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ckw        $:call Perl_InsertTemplate("comment.keyword-warning")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cko        $:call Perl_InsertTemplate("comment.keyword-workaround")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ckn        $:call Perl_InsertTemplate("comment.keyword-keyword")<CR>

  inoremap    <buffer>  <silent>  <LocalLeader>ckb   <C-C>$:call Perl_InsertTemplate("comment.keyword-bug")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ckt   <C-C>$:call Perl_InsertTemplate("comment.keyword-todo")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ckr   <C-C>$:call Perl_InsertTemplate("comment.keyword-tricky")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ckw   <C-C>$:call Perl_InsertTemplate("comment.keyword-warning")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>cko   <C-C>$:call Perl_InsertTemplate("comment.keyword-workaround")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ckn   <C-C>$:call Perl_InsertTemplate("comment.keyword-keyword")<CR>

  nnoremap    <buffer>  <silent>  <LocalLeader>cc              :call Perl_CommentToggle()<CR>j
  vnoremap    <buffer>  <silent>  <LocalLeader>cc    <C-C>:'<,'>call Perl_CommentToggle()<CR>j

  nnoremap    <buffer>  <silent>  <LocalLeader>cd    <Esc>:call Perl_InsertDateAndTime("d")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>cd    <Esc>:call Perl_InsertDateAndTime("d")<CR>a
  nnoremap    <buffer>  <silent>  <LocalLeader>ct    <Esc>:call Perl_InsertDateAndTime("dt")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ct    <Esc>:call Perl_InsertDateAndTime("dt")<CR>a

  nnoremap    <buffer>  <silent>  <LocalLeader>cv         :call Perl_CommentVimModeline()<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cb         :call Perl_CommentBlock("a")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>cb    <C-C>:call Perl_CommentBlock("v")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>cn         :call Perl_UncommentBlock()<CR>
  "
  " ----------------------------------------------------------------------------
  " Statements
  " ----------------------------------------------------------------------------
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>sd              :call Perl_InsertTemplate("statements.do-while")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sf              :call Perl_InsertTemplate("statements.for")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sfe             :call Perl_InsertTemplate("statements.foreach")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>si              :call Perl_InsertTemplate("statements.if")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sie             :call Perl_InsertTemplate("statements.if-else")<CR>
	nnoremap    <buffer>  <silent>  <LocalLeader>se              :call Perl_InsertTemplate("statements.else")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sei             :call Perl_InsertTemplate("statements.elsif")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>su              :call Perl_InsertTemplate("statements.unless")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sue             :call Perl_InsertTemplate("statements.unless-else")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>st              :call Perl_InsertTemplate("statements.until")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sw              :call Perl_InsertTemplate("statements.while")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>s{              :call Perl_InsertTemplate("statements.block")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>sb              :call Perl_InsertTemplate("statements.block")<CR>

  vnoremap    <buffer>  <silent>  <LocalLeader>sd    <C-C>:call Perl_InsertTemplate("statements.do-while", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sf    <C-C>:call Perl_InsertTemplate("statements.for", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sfe   <C-C>:call Perl_InsertTemplate("statements.foreach", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>si    <C-C>:call Perl_InsertTemplate("statements.if", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sie   <C-C>:call Perl_InsertTemplate("statements.if-else", "v" )<CR>
	vnoremap    <buffer>  <silent>  <LocalLeader>se    <C-C>:call Perl_InsertTemplate("statements.else", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sei   <C-C>:call Perl_InsertTemplate("statements.elsif", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>su    <C-C>:call Perl_InsertTemplate("statements.unless", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sue   <C-C>:call Perl_InsertTemplate("statements.unless-else", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>st    <C-C>:call Perl_InsertTemplate("statements.until", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sw    <C-C>:call Perl_InsertTemplate("statements.while", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>s{    <C-C>:call Perl_InsertTemplate("statements.block", "v" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>sb    <C-C>:call Perl_InsertTemplate("statements.block", "v" )<CR>

  inoremap    <buffer>  <silent>  <LocalLeader>sd    <C-C>:call Perl_InsertTemplate("statements.do-while")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sf    <C-C>:call Perl_InsertTemplate("statements.for")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sfe   <C-C>:call Perl_InsertTemplate("statements.foreach")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>si    <C-C>:call Perl_InsertTemplate("statements.if")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sie   <C-C>:call Perl_InsertTemplate("statements.if-else")<CR>
	inoremap    <buffer>  <silent>  <LocalLeader>se    <C-C>:call Perl_InsertTemplate("statements.else")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sei   <C-C>:call Perl_InsertTemplate("statements.elsif")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>su    <C-C>:call Perl_InsertTemplate("statements.unless")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sue   <C-C>:call Perl_InsertTemplate("statements.unless-else")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>st    <C-C>:call Perl_InsertTemplate("statements.until")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>sw    <C-C>:call Perl_InsertTemplate("statements.while")<CR>
  "
  " ----------------------------------------------------------------------------
  " Snippets
  " ----------------------------------------------------------------------------
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>nr    <C-C>:call Perl_CodeSnippet("r")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>nw    <C-C>:call Perl_CodeSnippet("w")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>nw    <C-C>:call Perl_CodeSnippet("wv")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ne    <C-C>:call Perl_CodeSnippet("e")<CR>
  "
  noremap    <buffer>  <silent>  <LocalLeader>ntl        :call Perl_EditTemplates("local")<CR>
  noremap    <buffer>  <silent>  <LocalLeader>ntg        :call Perl_EditTemplates("global")<CR>
  noremap    <buffer>  <silent>  <LocalLeader>ntr        :call Perl_RereadTemplates()<CR>
  "
  " ----------------------------------------------------------------------------
  " Idioms
  " ----------------------------------------------------------------------------
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>$         :call Perl_InsertTemplate("idioms.scalar")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>$=        :call Perl_InsertTemplate("idioms.scalar-assign")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>$$        :call Perl_InsertTemplate("idioms.scalar2")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>@         :call Perl_InsertTemplate("idioms.array")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>@=        :call Perl_InsertTemplate("idioms.array-assign")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>%         :call Perl_InsertTemplate("idioms.hash")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>%=        :call Perl_InsertTemplate("idioms.hash-assign")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>ir        :call Perl_InsertTemplate("idioms.regex")<CR>
  "
  inoremap    <buffer>  <silent>  <LocalLeader>$    <C-C>:call Perl_InsertTemplate("idioms.scalar")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>$=   <C-C>:call Perl_InsertTemplate("idioms.scalar-assign")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>$$   <C-C>:call Perl_InsertTemplate("idioms.scalar2")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>@    <C-C>:call Perl_InsertTemplate("idioms.array")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>@=   <C-C>:call Perl_InsertTemplate("idioms.array-assign")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>%    <C-C>:call Perl_InsertTemplate("idioms.hash")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>%=   <C-C>:call Perl_InsertTemplate("idioms.hash-assign")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ir   <C-C>:call Perl_InsertTemplate("idioms.regex")<CR>
  "

  nnoremap    <buffer>  <silent>  <LocalLeader>im         :call Perl_InsertTemplate("idioms.match")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>is         :call Perl_InsertTemplate("idioms.substitute")<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>it         :call Perl_InsertTemplate("idioms.translate")<CR>
  "
  inoremap    <buffer>  <silent>  <LocalLeader>im    <C-C>:call Perl_InsertTemplate("idioms.match")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>is    <C-C>:call Perl_InsertTemplate("idioms.substitute")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>it    <C-C>:call Perl_InsertTemplate("idioms.translate")<CR>
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>ip         :call Perl_InsertTemplate("idioms.print")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ip    <C-C>:call Perl_InsertTemplate("idioms.print")<CR>
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>ii         :call Perl_InsertTemplate("idioms.open-input-file")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ii    <C-C>:call Perl_InsertTemplate("idioms.open-input-file")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>ii    <C-C>:call Perl_InsertTemplate("idioms.open-input-file", "v" )<CR>

  nnoremap    <buffer>  <silent>  <LocalLeader>io         :call Perl_InsertTemplate("idioms.open-output-file")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>io    <C-C>:call Perl_InsertTemplate("idioms.open-output-file")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>io    <C-C>:call Perl_InsertTemplate("idioms.open-output-file", "v" )<CR>

  nnoremap    <buffer>  <silent>  <LocalLeader>ipi        :call Perl_InsertTemplate("idioms.open-pipe")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ipi   <C-C>:call Perl_InsertTemplate("idioms.open-pipe")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>ipi   <C-C>:call Perl_InsertTemplate("idioms.open-pipe", "v" )<CR>
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>isu        :call Perl_InsertTemplate("idioms.subroutine")<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>isu   <C-C>:call Perl_InsertTemplate("idioms.subroutine")<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>isu   <C-C>:call Perl_InsertTemplate("idioms.subroutine", "v")<CR>
  "
  " ----------------------------------------------------------------------------
  " Regex
  " ----------------------------------------------------------------------------
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>xr        :call perlsupportregex#Perl_RegexPick( "regexp", "n" )<CR>j
  nnoremap    <buffer>  <silent>  <LocalLeader>xs        :call perlsupportregex#Perl_RegexPick( "string", "n" )<CR>j
  nnoremap    <buffer>  <silent>  <LocalLeader>xf        :call perlsupportregex#Perl_RegexPickFlag( "n" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>xr   <C-C>:call perlsupportregex#Perl_RegexPick( "regexp", "v" )<CR>'>j
  vnoremap    <buffer>  <silent>  <LocalLeader>xs   <C-C>:call perlsupportregex#Perl_RegexPick( "string", "v" )<CR>'>j
  vnoremap    <buffer>  <silent>  <LocalLeader>xf   <C-C>:call perlsupportregex#Perl_RegexPickFlag( "v" )<CR>'>j
  nnoremap    <buffer>  <silent>  <LocalLeader>xm        :call perlsupportregex#Perl_RegexVisualize( )<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>xmm       :call perlsupportregex#Perl_RegexMatchSeveral( )<CR>
  nnoremap    <buffer>  <silent>  <LocalLeader>xe        :call perlsupportregex#Perl_RegexExplain( "n" )<CR>
  vnoremap    <buffer>  <silent>  <LocalLeader>xe   <C-C>:call perlsupportregex#Perl_RegexExplain( "v" )<CR>
  "
  " ----------------------------------------------------------------------------
  " POSIX character classes
  " ----------------------------------------------------------------------------
  "
  nnoremap    <buffer>  <silent>  <LocalLeader>pa    a[:alnum:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>ph    a[:alpha:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pi    a[:ascii:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pb    a[:blank:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pc    a[:cntrl:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pd    a[:digit:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pg    a[:graph:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pl    a[:lower:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pp    a[:print:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pn    a[:punct:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>ps    a[:space:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pu    a[:upper:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>pw    a[:word:]<Esc>
  nnoremap    <buffer>  <silent>  <LocalLeader>px    a[:xdigit:]<Esc>
  "
  inoremap    <buffer>  <silent>  <LocalLeader>pa    [:alnum:]
  inoremap    <buffer>  <silent>  <LocalLeader>ph    [:alpha:]
  inoremap    <buffer>  <silent>  <LocalLeader>pi    [:ascii:]
  inoremap    <buffer>  <silent>  <LocalLeader>pb    [:blank:]
  inoremap    <buffer>  <silent>  <LocalLeader>pc    [:cntrl:]
  inoremap    <buffer>  <silent>  <LocalLeader>pd    [:digit:]
  inoremap    <buffer>  <silent>  <LocalLeader>pg    [:graph:]
  inoremap    <buffer>  <silent>  <LocalLeader>pl    [:lower:]
  inoremap    <buffer>  <silent>  <LocalLeader>pp    [:print:]
  inoremap    <buffer>  <silent>  <LocalLeader>pn    [:punct:]
  inoremap    <buffer>  <silent>  <LocalLeader>ps    [:space:]
  inoremap    <buffer>  <silent>  <LocalLeader>pu    [:upper:]
  inoremap    <buffer>  <silent>  <LocalLeader>pw    [:word:]
  inoremap    <buffer>  <silent>  <LocalLeader>px    [:xdigit:]
  "
  " ----------------------------------------------------------------------------
  " Run
  " ----------------------------------------------------------------------------
  "
   noremap    <buffer>  <silent>  <LocalLeader>rr         :call Perl_Run()<CR>
   noremap    <buffer>  <silent>  <LocalLeader>rs         :call Perl_SyntaxCheck()<CR>:redraw!<CR>:call Perl_SyntaxCheckMsg()<CR>
   noremap    <buffer>  <silent>  <LocalLeader>ra         :call Perl_Arguments()<CR>
   noremap    <buffer>  <silent>  <LocalLeader>rw         :call Perl_PerlSwitches()<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>rr    <C-C>:call Perl_Run()<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>rs    <C-C>:call Perl_SyntaxCheck()<CR>:redraw!<CR>:call Perl_SyntaxCheckMsg()<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>ra    <C-C>:call Perl_Arguments()<CR>
  inoremap    <buffer>  <silent>  <LocalLeader>rw    <C-C>:call Perl_PerlSwitches()<CR>
  "
  if has("gui_running")
     noremap    <buffer>  <silent>  <LocalLeader>rd         :call Perl_Debugger()<CR>
     noremap    <buffer>  <silent>    <F9>             :call Perl_Debugger()<CR>
    inoremap    <buffer>  <silent>    <F9>        <C-C>:call Perl_Debugger()<CR>
  else
     noremap    <buffer>  <silent>  <LocalLeader>rd         :call Perl_Debugger()<CR>:redraw!<CR>
     noremap    <buffer>  <silent>    <F9>             :call Perl_Debugger()<CR>:redraw!<CR>
    inoremap    <buffer>  <silent>    <F9>        <C-C>:call Perl_Debugger()<CR>:redraw!<CR>
  endif
  "
  if s:UNIX
     noremap    <buffer>  <silent>  <LocalLeader>re         :call Perl_MakeScriptExecutable()<CR>
    inoremap    <buffer>  <silent>  <LocalLeader>re    <C-C>:call Perl_MakeScriptExecutable()<CR>
  endif
  "
   map    <buffer>  <silent>  <LocalLeader>rp         :call Perl_perldoc()<CR>
   map    <buffer>  <silent>  <LocalLeader>h          :call Perl_perldoc()<CR>
  "
   map    <buffer>  <silent>  <LocalLeader>ri         :call Perl_perldoc_show_module_list()<CR>
   map    <buffer>  <silent>  <LocalLeader>rg         :call Perl_perldoc_generate_module_list()<CR>:redraw!<CR>
  "
   map    <buffer>  <silent>  <LocalLeader>ry         :call Perl_Perltidy("n")<CR>
  vmap    <buffer>  <silent>  <LocalLeader>ry    <C-C>:call Perl_Perltidy("v")<CR>
  "
   map    <buffer>  <silent>  <LocalLeader>rps         :call perlsupportprofiling#Perl_Smallprof()<CR>
   map    <buffer>  <silent>  <LocalLeader>rpf         :call perlsupportprofiling#Perl_Fastprof()<CR>
   map    <buffer>  <silent>  <LocalLeader>rpn         :call perlsupportprofiling#Perl_NYTprof()<CR>
   map    <buffer>  <silent>  <LocalLeader>rpnc        :call perlsupportprofiling#Perl_NYTprofReadCSV()<CR>
   "
   map    <buffer>  <silent>  <LocalLeader>rc         :call Perl_Perlcritic()<CR>:call Perl_PerlcriticMsg()<CR>
   map    <buffer>  <silent>  <LocalLeader>rt         :call Perl_SaveWithTimestamp()<CR>
   map    <buffer>  <silent>  <LocalLeader>rh         :call Perl_Hardcopy("n")<CR>
  vmap    <buffer>  <silent>  <LocalLeader>rh    <C-C>:call Perl_Hardcopy("v")<CR>
  "
   map    <buffer>  <silent>  <LocalLeader>rk    :call Perl_Settings()<CR>
  if has("gui_running") && s:UNIX
     map    <buffer>  <silent>  <LocalLeader>rx    :call Perl_XtermSize()<CR>
  endif
  "
   map    <buffer>  <silent>  <LocalLeader>ro         :call Perl_Toggle_Gvim_Xterm()<CR>
  imap    <buffer>  <silent>  <LocalLeader>ro    <C-C>:call Perl_Toggle_Gvim_Xterm()<CR>
  "
  "
endif

" ----------------------------------------------------------------------------
"  Generate (possibly exuberant) Ctags style tags for Perl sourcecode.
"  Controlled by g:Perl_PerlTags, enabled by default.
" ----------------------------------------------------------------------------
if has('perl') && g:Perl_PerlTags == 'enabled'
	let g:Perl_PerlTagsTempfile = tempname()
	if getfsize( expand('%') ) > 0
		call Perl_do_tags( expand('%'), g:Perl_PerlTagsTempfile )
	endif
endif
"
"-------------------------------------------------------------------------------
" additional mapping : {<CR> always opens a block
"-------------------------------------------------------------------------------
inoremap    <buffer>  {<CR>  {<CR>}<Esc>O
vnoremap    <buffer>  {<CR> s{<CR>}<Esc>kp=iB
"
if !exists("g:Perl_Ctrl_j") || ( exists("g:Perl_Ctrl_j") && g:Perl_Ctrl_j != 'off' )
  nmap    <buffer>  <silent>  <C-j>    i<C-R>=Perl_JumpCtrlJ()<CR>
  imap    <buffer>  <silent>  <C-j>     <C-R>=Perl_JumpCtrlJ()<CR>
endif
" ----------------------------------------------------------------------------
