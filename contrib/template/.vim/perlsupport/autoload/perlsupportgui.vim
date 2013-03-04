"===============================================================================
"
"          File:  perlsupportgui.vim
"
"   Description:  Plugin perl-support: Menu definitions.
"
"   VIM Version:  7.0+
"        Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
"       Company:  FH Südwestfalen, Iserlohn
"       Version:  1.0
"       Created:  16.12.2008 18:16:55
"      Revision:  $Id: perlsupportgui.vim,v 1.18 2009/04/20 18:30:01 mehner Exp $
"       License:  Copyright 2008 Dr. Fritz Mehner
"===============================================================================
"
" Exit quickly when:
" - this plugin was already loaded
" - when 'compatible' is set
"
if exists("g:loaded_perlsupportgui") || &compatible
  finish
endif
let g:loaded_perlsupportgui = "v1.0"

let s:MSWIN = has("win16") || has("win32")   || has("win64")    || has("win95")
"
"------------------------------------------------------------------------------
" Perl Menu Initializations     {{{1
" Against the advice of every style guide this function has overlong lines
" to enable the use of block commands when editing.
"------------------------------------------------------------------------------
function! perlsupportgui#Perl_InitMenu ()

  "===============================================================================================
  "----- Menu : Main menu                              {{{2
  "===============================================================================================
  if g:Perl_Root != ""
    if g:Perl_MenuHeader == "yes"
      exe "amenu ".g:Perl_Root.'Perl     <Nop>'
      exe "amenu ".g:Perl_Root.'-Sep0-        :'
    endif
  endif
  "
  "===============================================================================================
  "----- Menu : Comments menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Comments.&Comments<Tab>Perl     <Nop>'
    exe "amenu ".g:Perl_Root.'&Comments.-Sep0-        :'
  endif

  exe "amenu <silent>  ".g:Perl_Root.'&Comments.end-of-&line\ com\.                   :call Perl_LineEndComment("")<CR>A'
  exe "imenu <silent>  ".g:Perl_Root.'&Comments.end-of-&line\ com\.              <C-C>:call Perl_LineEndComment("")<CR>A'
  exe "vmenu <silent>  ".g:Perl_Root.'&Comments.end-of-&line\ com\.              <C-C>:call Perl_MultiLineEndComments()<CR>A'
  "
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.ad&just\ end-of-line\ com\.           :call Perl_AlignLineEndComm("a")<CR>'
  exe "vmenu <silent>  ".g:Perl_Root.'&Comments.ad&just\ end-of-line\ com\.      <C-C>:call Perl_AlignLineEndComm("v")<CR>'
  "
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.&set\ end-of-line\ com\.\ col\.       :call Perl_GetLineEndCommCol()<CR>'

  exe "amenu <silent>  ".g:Perl_Root.'&Comments.&frame\ comm\.                        :call Perl_InsertTemplate("comment.frame")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.f&unction\ descr\.                    :call Perl_InsertTemplate("comment.function")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.&method\ descr\.                      :call Perl_InsertTemplate("comment.method")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.file\ &header\ (\.pl)                 :call Perl_InsertTemplate("comment.file-description-pl")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.file\ h&eader\ (\.pm)                 :call Perl_InsertTemplate("comment.file-description-pm")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.file\ he&ader\ (\.t)                  :call Perl_InsertTemplate("comment.file-description-t")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.file\ hea&der\ (\.pod)                :call Perl_InsertTemplate("comment.file-description-pod")<CR>'

  exe "amenu ".g:Perl_Root.'&Comments.-SEP1-                     :'
  "
  exe "amenu <silent>  ".g:Perl_Root."&Comments.toggle\\ &comment         :call Perl_CommentToggle()<CR>j"
  exe "imenu <silent>  ".g:Perl_Root."&Comments.toggle\\ &comment    <C-C>:call Perl_CommentToggle()<CR>j"
  exe "vmenu <silent>  ".g:Perl_Root."&Comments.toggle\\ &comment    <C-C>:'<,'>call Perl_CommentToggle()<CR>j"

  exe "amenu <silent>  ".g:Perl_Root.'&Comments.comment\ &block           :call Perl_CommentBlock("a")<CR>'
  exe "imenu <silent>  ".g:Perl_Root.'&Comments.comment\ &block      <C-C>:call Perl_CommentBlock("a")<CR>'
  exe "vmenu <silent>  ".g:Perl_Root.'&Comments.comment\ &block      <C-C>:call Perl_CommentBlock("v")<CR>'
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.u&ncomment\ block         :call Perl_UncommentBlock()<CR>'
  "
  exe "amenu ".g:Perl_Root.'&Comments.-SEP2-               :'
  "
  exe " menu  ".g:Perl_Root.'&Comments.&date               <Esc>:call Perl_InsertDateAndTime("d")<CR>'
  exe "imenu  ".g:Perl_Root.'&Comments.&date               <Esc>:call Perl_InsertDateAndTime("d")<CR>a'
  exe "vmenu  ".g:Perl_Root.'&Comments.&date              s<Esc>:call Perl_InsertDateAndTime("d")<CR>a'
  exe " menu  ".g:Perl_Root.'&Comments.date\ &time         <Esc>:call Perl_InsertDateAndTime("dt")<CR>'
  exe "imenu  ".g:Perl_Root.'&Comments.date\ &time         <Esc>:call Perl_InsertDateAndTime("dt")<CR>a'
  exe "vmenu  ".g:Perl_Root.'&Comments.date\ &time        s<Esc>:call Perl_InsertDateAndTime("dt")<CR>a'

  exe "amenu ".g:Perl_Root.'&Comments.-SEP3-                     :'
  "
  "--------- submenu : KEYWORD -------------------------------------------------------------
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Comments.&KEYWORD+comm\..Comments-1<Tab>Perl   <Nop>'
    exe "amenu ".g:Perl_Root.'&Comments.&KEYWORD+comm\..-Sep0-      :'
  endif
  "
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&BUG\:               $:call Perl_InsertTemplate("comment.keyword-bug")<CR>'
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&TODO\:              $:call Perl_InsertTemplate("comment.keyword-todo")<CR>'
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:T&RICKY\:            $:call Perl_InsertTemplate("comment.keyword-tricky")<CR>'
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&WARNING\:           $:call Perl_InsertTemplate("comment.keyword-warning")<CR>'
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:W&ORKAROUND\:        $:call Perl_InsertTemplate("comment.keyword-workaround")<CR>'
  exe "amenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&new\ keyword\:      $:call Perl_InsertTemplate("comment.keyword-keyword")<CR>'
"
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&BUG\:          <Esc>$:call Perl_InsertTemplate("comment.keyword-bug")<CR>'
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&TODO\:         <Esc>$:call Perl_InsertTemplate("comment.keyword-todo")<CR>'
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:T&RICKY\:       <Esc>$:call Perl_InsertTemplate("comment.keyword-tricky")<CR>'
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&WARNING\:      <Esc>$:call Perl_InsertTemplate("comment.keyword-warning")<CR>'
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:W&ORKAROUND\:   <Esc>$:call Perl_InsertTemplate("comment.keyword-workaround")<CR>'
  exe "imenu   ".g:Perl_Root.'&Comments.&KEYWORD+comm\..\:&new\ keyword\: <Esc>$:call Perl_InsertTemplate("comment.keyword-keyword")<CR>'
  "
  "
  "----- Submenu :  Tags  ----------------------------------------------------------
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Comments.ta&gs\ (plugin).Comments-2<Tab>Perl   <Nop>'
    exe "amenu ".g:Perl_Root.'&Comments.ta&gs\ (plugin).-Sep0-      :'
  endif
  "
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&AUTHOR                :call Perl_InsertMacroValue("AUTHOR")<CR>'
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).AUTHOR&REF             :call Perl_InsertMacroValue("AUTHORREF")<CR>'
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&COMPANY               :call Perl_InsertMacroValue("COMPANY")<CR>'
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).C&OPYRIGHTHOLDER       :call Perl_InsertMacroValue("COPYRIGHTHOLDER")<CR>'
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&EMAIL                 :call Perl_InsertMacroValue("EMAIL")<CR>'
  exe "anoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&PROJECT               :call Perl_InsertMacroValue("PROJECT")<CR>'
  "
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&AUTHOR           <Esc>:call Perl_InsertMacroValue("AUTHOR")<CR>a'
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).AUTHOR&REF        <Esc>:call Perl_InsertMacroValue("AUTHORREF")<CR>a'
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&COMPANY          <Esc>:call Perl_InsertMacroValue("COMPANY")<CR>a'
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).C&OPYRIGHTHOLDER  <Esc>:call Perl_InsertMacroValue("COPYRIGHTHOLDER")<CR>a'
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&EMAIL            <Esc>:call Perl_InsertMacroValue("EMAIL")<CR>a'
  exe "inoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&PROJECT          <Esc>:call Perl_InsertMacroValue("PROJECT")<CR>a'
  "
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&AUTHOR          s<Esc>:call Perl_InsertMacroValue("AUTHOR")<CR>a'
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).AUTHOR&REF       s<Esc>:call Perl_InsertMacroValue("AUTHORREF")<CR>a'
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&COMPANY         s<Esc>:call Perl_InsertMacroValue("COMPANY")<CR>a'
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).C&OPYRIGHTHOLDER s<Esc>:call Perl_InsertMacroValue("COPYRIGHTHOLDER")<CR>a'
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&EMAIL           s<Esc>:call Perl_InsertMacroValue("EMAIL")<CR>a'
  exe "vnoremenu  ".g:Perl_Root.'&Comments.ta&gs\ (plugin).&PROJECT         s<Esc>:call Perl_InsertMacroValue("PROJECT")<CR>a'
  "
  exe "amenu <silent>  ".g:Perl_Root.'&Comments.&vim\ modeline           :call Perl_CommentVimModeline()<CR>'

  "===============================================================================================
  "----- Menu : Statements Menu                              {{{2
  "===============================================================================================

  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Statements.&Statements<Tab>Perl     <Nop>'
    exe "amenu ".g:Perl_Root.'&Statements.-Sep0-        :'
  endif
  "
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&do\ \{\ \}\ while              :call Perl_InsertTemplate("statements.do-while")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&for\ \{\ \}                    :call Perl_InsertTemplate("statements.for")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.f&oreach\ \{\ \}                :call Perl_InsertTemplate("statements.foreach")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&if\ \{\ \}                     :call Perl_InsertTemplate("statements.if")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.if\ \{\ \}\ &else\ \{\ \}       :call Perl_InsertTemplate("statements.if-else")<CR>'
	exe "amenu <silent> ".g:Perl_Root.'&Statements.&else\ \{\ \}                   :call Perl_InsertTemplate("statements.else")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.e&lsif\ \{\ \}                  :call Perl_InsertTemplate("statements.elsif")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&unless\ \{\ \}                 :call Perl_InsertTemplate("statements.unless")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.u&nless\ \{\ \}\ else\ \{\ \}   :call Perl_InsertTemplate("statements.unless-else")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.un&til\ \{\ \}                  :call Perl_InsertTemplate("statements.until")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&while\ \{\ \}                  :call Perl_InsertTemplate("statements.while")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Statements.&\{\ \}                         :call Perl_InsertTemplate("statements.block")<CR>'
  "
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&do\ \{\ \}\ while              <C-C>:call Perl_InsertTemplate("statements.do-while")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&for\ \{\ \}                    <C-C>:call Perl_InsertTemplate("statements.for")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.f&oreach\ \{\ \}                <C-C>:call Perl_InsertTemplate("statements.foreach")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&if\ \{\ \}                     <C-C>:call Perl_InsertTemplate("statements.if")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.if\ \{\ \}\ &else\ \{\ \}       <C-C>:call Perl_InsertTemplate("statements.if-else")<CR>'
	exe "imenu <silent> ".g:Perl_Root.'&Statements.&else\ \{\ \}                   <C-C>:call Perl_InsertTemplate("statements.else")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.e&lsif\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.elsif")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&unless\ \{\ \}                 <C-C>:call Perl_InsertTemplate("statements.unless")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.u&nless\ \{\ \}\ else\ \{\ \}   <C-C>:call Perl_InsertTemplate("statements.unless-else")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.un&til\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.until")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&while\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.while")<CR>'
  exe "imenu <silent> ".g:Perl_Root.'&Statements.&\{\ \}                         <C-C>:call Perl_InsertTemplate("statements.block")<CR>'
  "
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&do\ \{\ \}\ while              <C-C>:call Perl_InsertTemplate("statements.do-while", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&for\ \{\ \}                    <C-C>:call Perl_InsertTemplate("statements.for", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.f&oreach\ \{\ \}                <C-C>:call Perl_InsertTemplate("statements.foreach", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&if\ \{\ \}                     <C-C>:call Perl_InsertTemplate("statements.if", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.if\ \{\ \}\ &else\ \{\ \}       <C-C>:call Perl_InsertTemplate("statements.if-else", "v" )<CR>'
	exe "vmenu <silent> ".g:Perl_Root.'&Statements.&else\ \{\ \}                   <C-C>:call Perl_InsertTemplate("statements.else", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.e&lsif\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.elsif", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&unless\ \{\ \}                 <C-C>:call Perl_InsertTemplate("statements.unless", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.u&nless\ \{\ \}\ else\ \{\ \}   <C-C>:call Perl_InsertTemplate("statements.unless-else", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.un&til\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.until", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&while\ \{\ \}                  <C-C>:call Perl_InsertTemplate("statements.while", "v" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Statements.&\{\ \}                         <C-C>:call Perl_InsertTemplate("statements.block", "v" )<CR>'
  "
  "===============================================================================================
  "----- Menu : Idioms menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Idioms.&Idioms<Tab>Perl    <Nop>'
    exe "amenu ".g:Perl_Root.'&Idioms.-Sep0-       :'
  endif
  "
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &$;                              :call Perl_InsertTemplate("idioms.scalar")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ $\ &=\ ;                         :call Perl_InsertTemplate("idioms.scalar-assign")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ (\ $&,\ $\ );                    :call Perl_InsertTemplate("idioms.scalar2")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &@;                              :call Perl_InsertTemplate("idioms.array")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ @\ =\ (,,);\ \ \ \ \ (&1)        :call Perl_InsertTemplate("idioms.array-assign")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &%;                              :call Perl_InsertTemplate("idioms.hash")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ %\ =\ (=>,);\ \ \ \ \ (&2)       :call Perl_InsertTemplate("idioms.hash-assign")<CR>'
  exe "nnoremenu <silent> ".g:Perl_Root.'&Idioms.my\ $rgx_\ =\ qr//;\ \ \ (&3)        :call Perl_InsertTemplate("idioms.regex")<CR>'
  "
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &$;                         <C-C>:call Perl_InsertTemplate("idioms.scalar")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ $\ &=\ ;                    <C-C>:call Perl_InsertTemplate("idioms.scalar-assign")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ (\ $&,\ $\ );               <C-C>:call Perl_InsertTemplate("idioms.scalar2")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &@;                         <C-C>:call Perl_InsertTemplate("idioms.array")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ @\ =\ (,,);\ \ \ \ \ (&1)   <C-C>:call Perl_InsertTemplate("idioms.array-assign")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ &%;                         <C-C>:call Perl_InsertTemplate("idioms.hash")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ %\ =\ (=>,);\ \ \ \ \ (&2)  <C-C>:call Perl_InsertTemplate("idioms.hash-assign")<CR>'
  exe "inoremenu <silent> ".g:Perl_Root.'&Idioms.my\ $rgx_\ =\ qr//;\ \ \ (&3)   <C-C>:call Perl_InsertTemplate("idioms.regex")<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.-SEP3-                        :'
  exe "anoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &m/\ /                 :call Perl_InsertTemplate("idioms.match")<CR>'
  exe "anoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &s/\ /\ /              :call Perl_InsertTemplate("idioms.substitute")<CR>'
  exe "anoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &tr/\ /\ /             :call Perl_InsertTemplate("idioms.translate")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &m/\ /            <C-C>:call Perl_InsertTemplate("idioms.match")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &s/\ /\ /         <C-C>:call Perl_InsertTemplate("idioms.substitute")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.$\ =~\ &tr/\ /\ /        <C-C>:call Perl_InsertTemplate("idioms.translate")<CR>'
  "
  exe " noremenu ".g:Perl_Root.'&Idioms.-SEP4-                    :'
  exe "anoremenu ".g:Perl_Root.'&Idioms.s&ubroutine                     :call Perl_InsertTemplate("idioms.subroutine")<CR>'
  exe "vnoremenu ".g:Perl_Root.'&Idioms.s&ubroutine                <C-C>:call Perl_InsertTemplate("idioms.subroutine", "v")<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.&print\ \"\.\.\.\\n\";          :call Perl_InsertTemplate("idioms.print")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.&print\ \"\.\.\.\\n\";     <C-C>:call Perl_InsertTemplate("idioms.print")<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.open\ &input\ file              :call Perl_InsertTemplate("idioms.open-input-file")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.open\ &input\ file         <C-C>:call Perl_InsertTemplate("idioms.open-input-file")<CR>'
  exe "vnoremenu ".g:Perl_Root.'&Idioms.open\ &input\ file         <C-C>:call Perl_InsertTemplate("idioms.open-input-file", "v" )<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.open\ &output\ file             :call Perl_InsertTemplate("idioms.open-output-file")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.open\ &output\ file        <C-C>:call Perl_InsertTemplate("idioms.open-output-file")<CR>'
  exe "vnoremenu ".g:Perl_Root.'&Idioms.open\ &output\ file        <C-C>:call Perl_InsertTemplate("idioms.open-output-file", "v" )<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.open\ pip&e                     :call Perl_InsertTemplate("idioms.open-pipe")<CR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.open\ pip&e                <C-C>:call Perl_InsertTemplate("idioms.open-pipe")<CR>'
  exe "vnoremenu ".g:Perl_Root.'&Idioms.open\ pip&e                <C-C>:call Perl_InsertTemplate("idioms.open-pipe", "v" )<CR>'
  "
  exe "anoremenu ".g:Perl_Root.'&Idioms.-SEP5-                    :'
  exe "anoremenu ".g:Perl_Root.'&Idioms.<STDIN>                         a<STDIN>'
  exe "anoremenu ".g:Perl_Root.'&Idioms.<STDOUT>                        a<STDOUT>'
  exe "anoremenu ".g:Perl_Root.'&Idioms.<STDERR>                        a<STDERR>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.<STDIN>                         <STDIN>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.<STDOUT>                        <STDOUT>'
  exe "inoremenu ".g:Perl_Root.'&Idioms.<STDERR>                        <STDERR>'
  "
  "===============================================================================================
  "----- Menu : Regex menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'S&nippets.S&nippets<Tab>Perl      <Nop>'
    exe "amenu ".g:Perl_Root.'S&nippets.-Sep0-         :'
  endif
  "
  " The menu entries for code snippet support will not appear if the following string is empty
  if g:Perl_CodeSnippets != ""
    exe "amenu <silent>  ".g:Perl_Root.'S&nippets.&read\ code\ snippet    :call Perl_CodeSnippet("r")<CR>'
    exe "amenu <silent>  ".g:Perl_Root.'S&nippets.&write\ code\ snippet   :call Perl_CodeSnippet("w")<CR>'
    exe "vmenu <silent>  ".g:Perl_Root.'S&nippets.&write\ code\ snippet   :call Perl_CodeSnippet("wv")<CR>'
    exe "amenu <silent>  ".g:Perl_Root.'S&nippets.e&dit\ code\ snippet    :call Perl_CodeSnippet("e")<CR>'
    exe "amenu <silent>  ".g:Perl_Root.'S&nippets.-SEP1-                  :'
  endif
  "
  exe "amenu  <silent>  ".g:Perl_Root.'S&nippets.edit\ &local\ templates          :call Perl_EditTemplates("local")<CR>'
  exe "amenu  <silent>  ".g:Perl_Root.'S&nippets.edit\ &global\ templates         :call Perl_EditTemplates("global")<CR>'
  exe "amenu  <silent>  ".g:Perl_Root.'S&nippets.reread\ &templates               :call Perl_RereadTemplates()<CR>'
  exe "imenu  <silent>  ".g:Perl_Root.'S&nippets.edit\ &local\ templates     <C-C>:call Perl_EditTemplates("local")<CR>'
  exe "imenu  <silent>  ".g:Perl_Root.'S&nippets.edit\ &global\ templates    <C-C>:call Perl_EditTemplates("global")<CR>'
  exe "imenu  <silent>  ".g:Perl_Root.'S&nippets.reread\ &templates          <C-C>:call Perl_RereadTemplates()<CR>'
  "
  "===============================================================================================
  "----- Menu : Regex menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Rege&x.Rege&x<Tab>Perl      <Nop>'
    exe "amenu ".g:Perl_Root.'Rege&x.-Sep0-         :'
  endif
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.&grouping<Tab>(\ )               a()<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.&alternation<Tab>(\ \|\ )        a(\|)<Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.char\.\ &class<Tab>[\ ]          a[]<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.c&ount<Tab>{\ }                  a{}<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.co&unt\ (at\ least)<Tab>{\ ,\ }  a{,}<Left><Left>'
  "
  exe "inoremenu ".g:Perl_Root.'Rege&x.&grouping<Tab>(\ )               ()<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.&alternation<Tab>(\ \|\ )        (\|)<Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.char\.\ &class<Tab>[\ ]          []<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.c&ount<Tab>{\ }                  {}<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.co&unt\ (at\ least)<Tab>{\ ,\ }  {,}<Left><Left>'

  exe "vnoremenu ".g:Perl_Root.'Rege&x.&grouping<Tab>(\ )               s()<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.&alternation<Tab>(\ \|\ )        s(\|)<Esc>hPf)i'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.char\.\ &class<Tab>[\ ]          s[]<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.c&ount<Tab>{\ }                  s{}<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.co&unt\ (at\ least)<Tab>{\ ,\ }  s{,}<Esc>hPf}i'
  "
  exe " menu ".g:Perl_Root.'Rege&x.-SEP3-                             :'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.word\ &boundary<Tab>\\b              a\b'
  exe "inoremenu ".g:Perl_Root.'Rege&x.word\ &boundary<Tab>\\b               \b'
  exe "anoremenu ".g:Perl_Root.'Rege&x.&digit<Tab>\\d                       a\d'
  exe "inoremenu ".g:Perl_Root.'Rege&x.&digit<Tab>\\d                        \d'
  exe "anoremenu ".g:Perl_Root.'Rege&x.white&space<Tab>\\s                  a\s'
  exe "inoremenu ".g:Perl_Root.'Rege&x.white&space<Tab>\\s                   \s'
  exe "anoremenu ".g:Perl_Root.'Rege&x.&word\ character<Tab>\\w             a\w'
  exe "inoremenu ".g:Perl_Root.'Rege&x.&word\ character<Tab>\\w              \w'
  exe "anoremenu ".g:Perl_Root.'Rege&x.match\ &property<Tab>\\p{}           a\p{}<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.match\ &property<Tab>\\p{}            \p{}<Left>'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.match\ &property<Tab>\\p{}           s\p{}<Esc>P'

  exe "anoremenu ".g:Perl_Root.'Rege&x.-SEP4-                         :'
  exe "anoremenu ".g:Perl_Root.'Rege&x.non-(word\ &bound\.)<Tab>\\B         a\B'
  exe "inoremenu ".g:Perl_Root.'Rege&x.non-(word\ &bound\.)<Tab>\\B          \B'
  exe "anoremenu ".g:Perl_Root.'Rege&x.non-&digit<Tab>\\D                   a\D'
  exe "inoremenu ".g:Perl_Root.'Rege&x.non-&digit<Tab>\\D                    \D'
  exe "anoremenu ".g:Perl_Root.'Rege&x.non-white&space<Tab>\\S              a\S'
  exe "inoremenu ".g:Perl_Root.'Rege&x.non-white&space<Tab>\\S               \S'
  exe "anoremenu ".g:Perl_Root.'Rege&x.non-(&word\ char\.)<Tab>\\W          a\W'
  exe "inoremenu ".g:Perl_Root.'Rege&x.non-(&word\ char\.)<Tab>\\W           \W'
  exe "anoremenu ".g:Perl_Root.'Rege&x.do\ not\ match\ &prop\.<Tab>\\P{}    a\P{}<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.do\ not\ match\ &prop\.<Tab>\\P{}     \P{}<Left>'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.do\ not\ match\ &prop\.<Tab>\\P{}    s\P{}<Esc>P'
  "
  "---------- submenu : POSIX character classes --------------------------------------------
  "
  exe " noremenu ".g:Perl_Root.'Rege&x.-SEP5-                               :'
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Rege&x.CharC&ls.Regex-1<Tab>Perl   <Nop>'
    exe "amenu ".g:Perl_Root.'Rege&x.CharC&ls.-Sep0-             :'
  endif
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&alnum:]   a[:alnum:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:alp&ha:]   a[:alpha:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:asc&ii:]   a[:ascii:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&blank:]   a[:blank:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&cntrl:]   a[:cntrl:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&digit:]   a[:digit:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&graph:]   a[:graph:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&lower:]   a[:lower:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&print:]   a[:print:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:pu&nct:]   a[:punct:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&space:]   a[:space:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&upper:]   a[:upper:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&word:]    a[:word:]'
  exe "anoremenu ".g:Perl_Root.'Rege&x.CharC&ls.[:&xdigit:]  a[:xdigit:]'
  "
  "---------- submenu : Unicode properties  --------------------------------------------
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Regex-2<Tab>Perl   <Nop>'
    exe "amenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.-Sep0-             :'
  endif
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.L&etter<Tab>\\p{L}                        a\p{Letter}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.&Lowercase_Letter<Tab>\\p{Ll}             a\p{Lowercase_Letter}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.&Uppercase_Letter<Tab>\\p{Lu}             a\p{Uppercase_Letter}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.&Titlecase_Letter<Tab>\\p{Lt}             a\p{Titlecase_Letter}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.&Modifier_Letter<Tab>\\p{Lm}              a\p{Modifier_Letter}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Letter.&Other_Letter<Tab>\\p{Lo}                 a\p{Other_Letter}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Mark.&Mark<Tab>\\p{M}                            a\p{Mark}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Mark.&Non_Spacing_Mark<Tab>\\p{Mn}               a\p{Non_Spacing_Mark}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Mark.Spacing_&Combining_Mark<Tab>\\p{Mc}         a\p{Spacing_Combining_Mark}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Mark.&Enclosing_Mark<Tab>\\p{Me}                 a\p{Enclosing_Mark}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Separator\ (&Z).S&eparator<Tab>\\p{Z}             a\p{Separator}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Separator\ (&Z).&Space_Separator<Tab>\\p{Zs}      a\p{Space_Separator}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Separator\ (&Z).&Line_Separator<Tab>\\p{Zl}       a\p{Line_Separator}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Separator\ (&Z).&Paragraph_Separator<Tab>\\p{Zp}  a\p{Paragraph_Separator}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Symbol.&Symbol<Tab>\\p{S}                        a\p{Symbol}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Symbol.&Math_Symbol<Tab>\\p{Sm}                  a\p{Math_Symbol}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Symbol.&Currency_Symbol<Tab>\\p{Sc}              a\p{Currency_Symbol}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Symbol.Modifier_Symbol\ (&k)<Tab>\\p{Sk}         a\p{Modifier_Symbol}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Symbol.&Other_Symbol<Tab>\\p{So}                 a\p{Other_Symbol}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Number.&Number<Tab>\\p{N}                        a\p{Number}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Number.&Decimal_Digit_Number<Tab>\\p{Nd}         a\p{Decimal_Digit_Number}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Number.&Letter_Number<Tab>\\p{Nl}                a\p{Letter_Number}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Number.&Other_Number<Tab>\\p{No}                 a\p{Other_Number}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Punctuation<Tab>\\p{P}              a\p{Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Dash_Punctuation<Tab>\\p{Pd}        a\p{Dash_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.Open_Punctuation\ (&s)<Tab>\\p{Ps}   a\p{Open_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.Close_Punctuation\ (&e)<Tab>\\p{Pe}  a\p{Close_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Initial_Punctuation<Tab>\\p{Pi}     a\p{Initial_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Final_Punctuation<Tab>\\p{Pf}       a\p{Final_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Connector_Punctuation<Tab>\\p{Pc}   a\p{Connector_Punctuation}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.&Punctuation.&Other_Punctuation<Tab>\\p{Po}       a\p{Other_Punctuation}'
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Other\ (&C).O&ther<Tab>\\p{C}                     a\p{Other}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Other\ (&C).&Control<Tab>\\p{Cc}                  a\p{Control}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Other\ (&C).&Format<Tab>\\p{Cf}                   a\p{Format}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Other\ (&C).Private_Use\ (&o)<Tab>\\p{Co}         a\p{Private_Use}'
  exe "anoremenu ".g:Perl_Root.'Rege&x.Unicode\ propert&y.Other\ (&C).U&nassigned<Tab>\\p{Cn}               a\p{Unassigned}'
  "
  "---------- subsubmenu : Regular Expression Suport  -----------------------------------------
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.Regex-3<Tab>Perl      <Nop>'
    exe "amenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.-Sep0-         :'
  endif
  "
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&comment<Tab>(?#\ \.\.\.\ )                       a(?#)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.cl&uster\ only\ paren\.<Tab>(?-imsx:\ \.\.\.\ )   a(?-imsx:)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.pattern\ &modifier<Tab>(?-imsx)                   a(?-imsx)<Left><Left><Left><Left><Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.e&xecute\ code<Tab>(?\{\ \.\.\.\ \})              a(?{})<Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match\ &regex\ from\ code<Tab>(??\{\ \.\.\.\ \})  a(??{})<Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-&then<Tab>(?(\.\.)\.\.)                  a(?())<Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-t&hen-else<Tab>(?(\.\.)\.\.\|\.\.)       a(?()\|)<Left><Left><Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.-SEP11-                                     :'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&ahead\ succeeds<Tab>(?=\ \.\.\.\ )           a(?=)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.looka&head\ fails<Tab>(?!\ \.\.\.\ )              a(?!)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&behind\ succeeds<Tab>(?<=\ \.\.\.\ )         a(?<=)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.lookb&ehind\ fails<Tab>(?<!\ \.\.\.\ )            a(?<!)<Left>'
  exe "anoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&prohibit\ backtracking<Tab>(?>\ \.\.\.\ )        a(?>)<Left>'
  "
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&comment<Tab>(?#\ \.\.\.\ )                        (?#)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.cl&uster\ only\ paren\.<Tab>(?-imsx:\ \.\.\.\ )    (?-imsx:)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.pattern\ &modifier<Tab>(?-imsx)                    (?-imsx)<Left><Left><Left><Left><Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.e&xecute\ code<Tab>(?\{\ \.\.\.\ \})               (?{})<Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match\ &regex\ from\ code<Tab>(??\{\ \.\.\.\ \})   (??{})<Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-&then<Tab>(?(\.\.)\.\.)                   (?())<Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-t&hen-else<Tab>(?(\.\.)\.\.\|\.\.)        (?()\|)<Left><Left><Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.-SEP11-                                     :'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&ahead\ succeeds<Tab>(?=\ \.\.\.\ )            (?=)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.looka&head\ fails<Tab>(?!\ \.\.\.\ )               (?!)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&behind\ succeeds<Tab>(?<=\ \.\.\.\ )          (?<=)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.lookb&ehind\ fails<Tab>(?<!\ \.\.\.\ )             (?<!)<Left>'
  exe "inoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&prohibit\ backtracking<Tab>(?>\ \.\.\.\ )         (?>)<Left>'

  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&comment<Tab>(?#\ \.\.\.\ )                       s(?#)<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.cl&uster\ only\ paren\.<Tab>(?-imsx:\ \.\.\.\ )   s(?-imsx:)<Esc>P'
"
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.e&xecute\ code<Tab>(?\{\ \.\.\.\ \})              s(?{})<Esc>hP'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match\ &regex\ from\ code<Tab>(??\{\ \.\.\.\ \})  s(??{})<Esc>hP'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-&then<Tab>(?(\.\.)\.\.)                  s(?())<Esc>hPla'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.match-if-t&hen-else<Tab>(?(\.\.)\.\.\|\.\.)       s(?()\|)<Esc>3hlPla'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.-SEP11-                                           :'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&ahead\ succeeds<Tab>(?=\ \.\.\.\ )           s(?=)<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.looka&head\ fails<Tab>(?!\ \.\.\.\ )              s(?!)<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.look&behind\ succeeds<Tab>(?<=\ \.\.\.\ )         s(?<=)<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.lookb&ehind\ fails<Tab>(?<!\ \.\.\.\ )            s(?<!)<Esc>P'
  exe "vnoremenu ".g:Perl_Root.'Rege&x.e&xtended\ Regex.&prohibit\ backtracking<Tab>(?>\ \.\.\.\ )        s(?>)<Esc>P'
  "
  "
  exe " noremenu ".g:Perl_Root.'Rege&x.-SEP7-                               :'
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ &regex          :call perlsupportregex#Perl_RegexPick( "regexp", "n" )<CR>j'
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ s&tring         :call perlsupportregex#Perl_RegexPick( "string", "n" )<CR>j'
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ &flag(s)        :call perlsupportregex#Perl_RegexPickFlag( "n" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ &regex     <C-C>:call perlsupportregex#Perl_RegexPick( "regexp", "v" )<CR>'."'>j"
  exe "vmenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ s&tring    <C-C>:call perlsupportregex#Perl_RegexPick( "string", "v" )<CR>'."'>j"
  exe "vmenu <silent> ".g:Perl_Root.'Rege&x.pick\ up\ &flag(s)   <C-C>:call perlsupportregex#Perl_RegexPickFlag( "v" )<CR>'."'>j"
  "
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.&match                     :call perlsupportregex#Perl_RegexVisualize( )<CR>'
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.matc&h\ several\ targets   :call perlsupportregex#Perl_RegexMatchSeveral( )<CR>'
  exe "amenu <silent> ".g:Perl_Root.'Rege&x.&explain\ regex            :call perlsupportregex#Perl_RegexExplain( "n" )<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'Rege&x.&explain\ regex       <C-C>:call perlsupportregex#Perl_RegexExplain( "v" )<CR>'
  "
  "===============================================================================================
  "----- Menu : File-Tests menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&File-Tests.&File-Tests<Tab>Perl             <Nop>'
    exe "amenu ".g:Perl_Root.'&File-Tests.-Sep0-                          :'
  endif
  "
  exe "anoremenu ".g:Perl_Root.'&File-Tests.exists<Tab>-e                     a-e '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.has\ zero\ size<Tab>-z            a-z '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.has\ nonzero\ size<Tab>-s         a-s '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.plain\ file<Tab>-f                a-f '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.directory<Tab>-d                  a-d '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.symbolic\ link<Tab>-l             a-l '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.named\ pipe<Tab>-p                a-p '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.socket<Tab>-S                     a-S '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.block\ special\ file<Tab>-b       a-b '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.character\ special\ file<Tab>-c   a-c '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.exists<Tab>-e                      -e '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.has\ zero\ size<Tab>-z             -z '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.has\ nonzero\ size<Tab>-s          -s '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.plain\ file<Tab>-f                 -f '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.directory<Tab>-d                   -d '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.symbolic\ link<Tab>-l              -l '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.named\ pipe<Tab>-p                 -p '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.socket<Tab>-S                      -S '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.block\ special\ file<Tab>-b        -b '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.character\ special\ file<Tab>-c    -c '
  "
  exe " menu ".g:Perl_Root.'&File-Tests.-SEP1-                              :'
  "
  exe "anoremenu ".g:Perl_Root.'&File-Tests.readable\ by\ eff\.\ UID/GID<Tab>-r   a-r '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.writable\ by\ eff\.\ UID/GID<Tab>-w   a-w '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.executable\ by\ eff\.\ UID/GID<Tab>-x a-x '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.owned\ by\ eff\.\ UID<Tab>-o          a-o '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.readable\ by\ eff\.\ UID/GID<Tab>-r    -r '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.writable\ by\ eff\.\ UID/GID<Tab>-w    -w '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.executable\ by\ eff\.\ UID/GID<Tab>-x  -x '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.owned\ by\ eff\.\ UID<Tab>-o           -o '
  "
  exe "anoremenu ".g:Perl_Root.'&File-Tests.-SEP2-                          :'
  "
  exe "anoremenu ".g:Perl_Root.'&File-Tests.readable\ by\ real\ UID/GID<Tab>-R    a-R '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.writable\ by\ real\ UID/GID<Tab>-W    a-W '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.executable\ by\ real\ UID/GID<Tab>-X  a-X '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.owned\ by\ real\ UID<Tab>-O           a-O '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.readable\ by\ real\ UID/GID<Tab>-R     -R '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.writable\ by\ real\ UID/GID<Tab>-W     -W '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.executable\ by\ real\ UID/GID<Tab>-X   -X '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.owned\ by\ real\ UID<Tab>-O            -O '

  exe "anoremenu ".g:Perl_Root.'&File-Tests.-SEP3-                           :'
  exe "anoremenu ".g:Perl_Root.'&File-Tests.setuid\ bit\ set<Tab>-u               a-u '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.setgid\ bit\ set<Tab>-g               a-g '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.sticky\ bit\ set<Tab>-k               a-k '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.setuid\ bit\ set<Tab>-u                -u '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.setgid\ bit\ set<Tab>-g                -g '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.sticky\ bit\ set<Tab>-k                -k '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.-SEP4-                           :'
  exe "anoremenu ".g:Perl_Root.'&File-Tests.age\ since\ modification<Tab>-M       a-M '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.age\ since\ last\ access<Tab>-A       a-A '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.age\ since\ inode\ change<Tab>-C      a-C '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.age\ since\ modification<Tab>-M        -M '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.age\ since\ last\ access<Tab>-A        -A '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.age\ since\ inode\ change<Tab>-C       -C '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.-SEP5-                           :'
  exe "anoremenu ".g:Perl_Root.'&File-Tests.text\ file<Tab>-T                     a-T '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.binary\ file<Tab>-B                   a-B '
  exe "anoremenu ".g:Perl_Root.'&File-Tests.handle\ opened\ to\ a\ tty<Tab>-t     a-t '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.text\ file<Tab>-T                      -T '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.binary\ file<Tab>-B                    -B '
  exe "inoremenu ".g:Perl_Root.'&File-Tests.handle\ opened\ to\ a\ tty<Tab>-t      -t '
  "
  "===============================================================================================
  "----- Menu : Special-Variables menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.Spec-&Var<Tab>Perl      <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.-Sep0-         :'
  endif
  "
  "-------- submenu errors -------------------------------------------------
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.&errors.Spec-Var-1<Tab>Perl       <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.&errors.-Sep0-                    :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&errors.$CHILD_ERROR<Tab>$?         a$CHILD_ERROR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&errors.$ERRNO<Tab>$!               a$ERRNO'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&errors.$EVAL_ERROR<Tab>$@          a$EVAL_ERROR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&errors.$EXTENDED_OS_ERROR<Tab>$^E  a$EXTENDED_OS_ERROR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&errors.$WARNING<Tab>$^W            a$WARNING'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&errors.$CHILD_ERROR<Tab>$?          $CHILD_ERROR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&errors.$ERRNO<Tab>$!                $ERRNO'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&errors.$EVAL_ERROR<Tab>$@           $EVAL_ERROR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&errors.$EXTENDED_OS_ERROR<Tab>$^E   $EXTENDED_OS_ERROR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&errors.$WARNING<Tab>$^W             $WARNING'

  "-------- submenu files -------------------------------------------------
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.&files.Spec-Var-2<Tab>Perl     <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.&files.-Sep0-                  :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$AUTOFLUSH<Tab>$\|                   a$AUTOFLUSH'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINES_LEFT<Tab>$-            a$FORMAT_LINES_LEFT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINES_PER_PAGE<Tab>$=        a$FORMAT_LINES_PER_PAGE'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINE_BREAK_CHARACTER<Tab>$:  a$FORMAT_LINES_PER_PAGE'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_NAME<Tab>$~                  a$FORMAT_NAME'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_PAGE_NUMBER<Tab>$%           a$FORMAT_PAGE_NUMBER'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_TOP_NAME<Tab>$^              a$FORMAT_TOP_NAME'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&files.$OUTPUT_AUTOFLUSH<Tab>$\|            a$OUTPUT_AUTOFLUSH'

  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$AUTOFLUSH<Tab>$\|                   $AUTOFLUSH'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINES_LEFT<Tab>$-            $FORMAT_LINES_LEFT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINES_PER_PAGE<Tab>$=        $FORMAT_LINES_PER_PAGE'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_LINE_BREAK_CHARACTER<Tab>$:  $FORMAT_LINES_PER_PAGE'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_NAME<Tab>$~                  $FORMAT_NAME'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_PAGE_NUMBER<Tab>$%           $FORMAT_PAGE_NUMBER'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$FORMAT_TOP_NAME<Tab>$^              $FORMAT_TOP_NAME'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&files.$OUTPUT_AUTOFLUSH<Tab>$\|            $OUTPUT_AUTOFLUSH'

  "-------- submenu IDs -------------------------------------------------
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.&IDs.Spec-Var-3<Tab>Perl    <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.&IDs.-Sep0-                 :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$PID<Tab>$$                   a$PID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$PROCESS_ID<Tab>$$            a$PROCESS_ID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$UID<Tab>$<                   a$UID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$REAL_USER_ID<Tab>$<          a$REAL_USER_ID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EUID<Tab>$>                  a$EUID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EFFECTIVE_USER_ID<Tab>$>     a$EFFECTIVE_USER_ID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$GID<Tab>$(                   a$GID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$REAL_GROUP_ID<Tab>$(         a$REAL_GROUP_ID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EGID<Tab>$)                  a$EGID'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EFFECTIVE_GROUP_ID<Tab>$)    a$EFFECTIVE_GROUP_ID'

  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$PID<Tab>$$                    $PID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$PROCESS_ID<Tab>$$             $PROCESS_ID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$UID<Tab>$<                    $UID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$REAL_USER_ID<Tab>$<           $REAL_USER_ID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EUID<Tab>$>                   $EUID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EFFECTIVE_USER_ID<Tab>$>      $EFFECTIVE_USER_ID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$GID<Tab>$(                    $GID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$REAL_GROUP_ID<Tab>$(          $REAL_GROUP_ID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EGID<Tab>$)                   $EGID'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&IDs.$EFFECTIVE_GROUP_ID<Tab>$)     $EFFECTIVE_GROUP_ID'

  "-------- submenu IO -------------------------------------------------
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.I&O.Spec-Var-4<Tab>Perl       <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.I&O.-Sep0-                    :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$ARG<Tab>$_                        a$ARG'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$INPUT_LINE_NUMBER<Tab>$\.         a$INPUT_LINE_NUMBER'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$NR<Tab>$\.                        a$NR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$INPUT_RECORD_SEPARATOR<Tab>$/     a$INPUT_RECORD_SEPARATOR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$RS<Tab>$/                         a$RS'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$LIST_SEPARATOR<Tab>$"             a$LIST_SEPARATOR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OUTPUT_FIELD_SEPARATOR<Tab>$,     a$OUTPUT_FIELD_SEPARATOR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OFS<Tab>$,                        a$OFS'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OUTPUT_RECORD_SEPARATOR<Tab>$\\   a$OUTPUT_RECORD_SEPARATOR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$ORS<Tab>$\\                       a$ORS'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$SUBSCRIPT_SEPARATOR<Tab>$;        a$SUBSCRIPT_SEPARATOR'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.I&O.$SUBSEP<Tab>$;                     a$SUBSEP'

  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$ARG<Tab>$_                         $ARG'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$INPUT_LINE_NUMBER<Tab>$\.          $INPUT_LINE_NUMBER'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$NR<Tab>$\.                         $NR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$INPUT_RECORD_SEPARATOR<Tab>$/      $INPUT_RECORD_SEPARATOR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$RS<Tab>$/                          $RS'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$LIST_SEPARATOR<Tab>$"              $LIST_SEPARATOR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OUTPUT_FIELD_SEPARATOR<Tab>$,      $OUTPUT_FIELD_SEPARATOR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OFS<Tab>$,                         $OFS'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$OUTPUT_RECORD_SEPARATOR<Tab>$\\    $OUTPUT_RECORD_SEPARATOR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$ORS<Tab>$\\                        $ORS'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$SUBSCRIPT_SEPARATOR<Tab>$;         $SUBSCRIPT_SEPARATOR'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.I&O.$SUBSEP<Tab>$;                      $SUBSEP'

  "-------- submenu regexp -------------------------------------------------
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.&regexp.Spec-Var-5<Tab>Perl       <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.&regexp.-Sep0-                    :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$MATCH<Tab>$&                      a$MATCH'
  exe "anoremenu ".g:Perl_Root."Spec-&Var.&regexp.$POSTMATCH<Tab>$'                  a$POSTMATCH"
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$PREMATCH<Tab>$`                   a$PREMATCH'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$digits                            a$digits'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.@LAST_MATCH_END<Tab>@+             a@LAST_MATCH_END'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.@LAST_MATCH_START<Tab>@-           a@LAST_MATCH_START'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_PAREN_MATCH<Tab>$+           a$LAST_PAREN_MATCH'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_SUBMATCH_RESULT<Tab>$^N      a$LAST_SUBMATCH_RESULT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_REGEXP_CODE_RESULT<Tab>$^R   a$LAST_REGEXP_CODE_RESULT'

  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$MATCH<Tab>$&                       $MATCH'
  exe "inoremenu ".g:Perl_Root."Spec-&Var.&regexp.$POSTMATCH<Tab>$'                   $POSTMATCH"
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$PREMATCH<Tab>$`                    $PREMATCH'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$digits                             $digits'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.@LAST_MATCH_END<Tab>@+              @LAST_MATCH_END'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.@LAST_MATCH_START<Tab>@-            @LAST_MATCH_START'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_PAREN_MATCH<Tab>$+            $LAST_PAREN_MATCH'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_SUBMATCH_RESULT<Tab>$^N       $LAST_SUBMATCH_RESULT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.&regexp.$LAST_REGEXP_CODE_RESULT<Tab>$^R    $LAST_REGEXP_CODE_RESULT'

  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$BASETIME<Tab>$^T         a$BASETIME'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$PERL_VERSION<Tab>$^V     a$PERL_VERSION'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$PROGRAM_NAME<Tab>$0      a$PROGRAM_NAME'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$OSNAME<Tab>$^O           a$OSNAME'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$SYSTEM_FD_MAX<Tab>$^F    a$SYSTEM_FD_MAX'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$ENV{\ }                  a$ENV{}<Left>'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$INC{\ }                  a$INC{}<Left>'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.$SIG{\ }                  a$SIG{}<Left>'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$BASETIME<Tab>$^T          $BASETIME'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$PERL_VERSION<Tab>$^V      $PERL_VERSION'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$PROGRAM_NAME<Tab>$0       $PROGRAM_NAME'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$OSNAME<Tab>$^O            $OSNAME'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$SYSTEM_FD_MAX<Tab>$^F     $SYSTEM_FD_MAX'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$ENV{\ }                   $ENV{}<Left>'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$INC{\ }                   $INC{}<Left>'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.$SIG{\ }                   $SIG{}<Left>'
  "
  "---------- submenu : POSIX signals --------------------------------------
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.Spec-Var-6<Tab>Perl     <Nop>'
    exe "amenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.-Sep0-        :'
  endif
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.HUP    aHUP'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.INT    aINT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.QUIT   aQUIT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ILL    aILL'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ABRT   aABRT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.FPE    aFPE'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.KILL   aKILL'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.SEGV   aSEGV'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.PIPE   aPIPE'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ALRM   aALRM'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TERM   aTERM'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.USR1   aUSR1'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.USR2   aUSR2'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.CHLD   aCHLD'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.CONT   aCONT'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.STOP   aSTOP'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TSTP   aTSTP'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TTIN   aTTIN'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TTOU   aTTOU'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.HUP     HUP'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.INT     INT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.QUIT    QUIT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ILL     ILL'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ABRT    ABRT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.FPE     FPE'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.KILL    KILL'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.SEGV    SEGV'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.PIPE    PIPE'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.ALRM    ALRM'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TERM    TERM'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.USR1    USR1'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.USR2    USR2'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.CHLD    CHLD'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.CONT    CONT'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.STOP    STOP'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TSTP    TSTP'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TTIN    TTIN'
  exe "inoremenu ".g:Perl_Root.'Spec-&Var.POSIX\ signals.TTOU    TTOU'
  "
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.-SEP2-                :'
  exe "anoremenu ".g:Perl_Root."Spec-&Var.\'IGNORE\'            a'IGNORE'"
  exe "anoremenu ".g:Perl_Root."Spec-&Var.\'DEFAULT\'           a'DEFAULT'"
  exe "inoremenu ".g:Perl_Root."Spec-&Var.\'IGNORE\'             'IGNORE'"
  exe "inoremenu ".g:Perl_Root."Spec-&Var.\'DEFAULT\'            'DEFAULT'"
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.-SEP3-                :'
  exe "anoremenu ".g:Perl_Root.'Spec-&Var.use\ English;         ouse English qw( -no_match_vars );<ESC>'
  "
  "===============================================================================================
  "----- Menu : POD menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&POD.&POD<Tab>Perl          <Nop>'
    exe "amenu ".g:Perl_Root.'&POD.-Sep0-                 :'
  endif
  "
  exe "amenu ".g:Perl_Root.'&POD.=&pod\ /\ =cut                 :call Perl_InsertTemplate("pod.pod-cut")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=&pod\ /\ =cut            <C-C>:call Perl_InsertTemplate("pod.pod-cut")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=&pod\ /\ =cut            <C-C>:call Perl_InsertTemplate("pod.pod-cut", "v")<CR>'
  "
  exe "amenu ".g:Perl_Root.'&POD.=c&ut                          :call Perl_InsertTemplate("pod.cut")<CR>'
  "
  exe "amenu ".g:Perl_Root.'&POD.=fo&r\ /\ =cut                 :call Perl_InsertTemplate("pod.for-cut")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=fo&r\ /\ =cut            <C-C>:call Perl_InsertTemplate("pod.for-cut")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=fo&r\ /\ =cut            <C-C>:call Perl_InsertTemplate("pod.for-cut", "v")<CR>'
  "
  exe "amenu ".g:Perl_Root.'&POD.=begin\ &html\ /\ =end         :call Perl_InsertTemplate("pod.html")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.=begin\ &man\ /\ =end          :call Perl_InsertTemplate("pod.man")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.=begin\ &text\ /\ =end         :call Perl_InsertTemplate("pod.text")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=begin\ &html\ /\ =end    <C-C>:call Perl_InsertTemplate("pod.html")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=begin\ &man\ /\ =end     <C-C>:call Perl_InsertTemplate("pod.man")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=begin\ &text\ /\ =end    <C-C>:call Perl_InsertTemplate("pod.text")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=begin\ &html\ /\ =end    <C-C>:call Perl_InsertTemplate("pod.html", "v")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=begin\ &man\ /\ =end     <C-C>:call Perl_InsertTemplate("pod.man", "v")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=begin\ &text\ /\ =end    <C-C>:call Perl_InsertTemplate("pod.text", "v")<CR>'
  "
  exe "amenu ".g:Perl_Root.'&POD.=head&1                        :call Perl_InsertTemplate("pod.head1")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.=head&2                        :call Perl_InsertTemplate("pod.head2")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.=head&3                        :call Perl_InsertTemplate("pod.head3")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.-Sep1-                    :'
  "
  exe "amenu ".g:Perl_Root.'&POD.=&over\ \.\.\ =back            :call Perl_InsertTemplate("pod.over-back")<CR>'
  exe "imenu ".g:Perl_Root.'&POD.=&over\ \.\.\ =back       <C-C>:call Perl_InsertTemplate("pod.over-back")<CR>'
  exe "vmenu ".g:Perl_Root.'&POD.=&over\ \.\.\ =back       <C-C>:call Perl_InsertTemplate("pod.over-back", "v")<CR>'
  "
  exe "amenu ".g:Perl_Root.'&POD.=item\ &*                      :call Perl_InsertTemplate("pod.item")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.-Sep2-                    :'
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&POD.in&visible\ POD.invisible\ POD<Tab>Perl     <Nop>'
    exe "amenu ".g:Perl_Root.'&POD.in&visible\ POD.-Sep0-        :'
  endif
  exe "amenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Improvement        :call Perl_InsertTemplate("pod.invisible-pod-improvement")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Optimization       :call Perl_InsertTemplate("pod.invisible-pod-optimization")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Rationale          :call Perl_InsertTemplate("pod.invisible-pod-rationale")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Workaround         :call Perl_InsertTemplate("pod.invisible-pod-workaround")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Improvement   <C-C>:call Perl_InsertTemplate("pod.invisible-pod-improvement")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Optimization  <C-C>:call Perl_InsertTemplate("pod.invisible-pod-optimization")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Rationale     <C-C>:call Perl_InsertTemplate("pod.invisible-pod-rationale")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&POD.in&visible\ POD.&Workaround    <C-C>:call Perl_InsertTemplate("pod.invisible-pod-workaround")<CR>'
  exe "amenu ".g:Perl_Root.'&POD.-Sep3-                    :'
  "
  "---------- submenu : Sequences --------------------------------------
  "
  exe "anoremenu ".g:Perl_Root.'&POD.&B<><Tab>bold             aB<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&C<><Tab>literal          aC<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&E<><Tab>escape           aE<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&F<><Tab>filename         aF<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&I<><Tab>italic           aI<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&L<><Tab>link             aL<\|><Left><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&S<>\ \ nonbr\.\ spaces   aS<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&X<><Tab>index            aX<><Left>'
  exe "anoremenu ".g:Perl_Root.'&POD.&Z<><Tab>zero-width       aZ<><Left>'
  "
  exe "inoremenu ".g:Perl_Root.'&POD.&B<><Tab>bold              B<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&C<><Tab>literal           C<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&E<><Tab>escape            E<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&F<><Tab>filename          F<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&I<><Tab>italic            I<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&L<><Tab>link              L<\|><Left><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&S<>\ \ nonbr\.\ spaces    S<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&X<><Tab>index             X<><Left>'
  exe "inoremenu ".g:Perl_Root.'&POD.&Z<><Tab>zero-width        Z<><Left>'
  "
  exe "vnoremenu ".g:Perl_Root.'&POD.&B<><Tab>bold             sB<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&C<><Tab>literal          sC<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&E<><Tab>escape           sE<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&F<><Tab>filename         sF<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&I<><Tab>italic           sI<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&L<><Tab>link             sL<\|><Esc>hPl'
  exe "vnoremenu ".g:Perl_Root.'&POD.&S<>\ \ nonbr\.\ spaces   sS<><Esc>P2l'
  exe "vnoremenu ".g:Perl_Root.'&POD.&X<><Tab>index            sX<><Esc>P2l'

  exe "amenu          ".g:Perl_Root.'&POD.-SEP4-                  :'
  exe "amenu <silent> ".g:Perl_Root.'&POD.run\ podchecker\ \ (&4) :call Perl_PodCheck()<CR>:redraw<CR>:call Perl_PodCheckMsg()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.POD\ ->\ html\ \ (&5)   :call Perl_POD("html")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.POD\ ->\ man\ \ (&6)    :call Perl_POD("man")<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&POD.POD\ ->\ text\ \ (&7)   :call Perl_POD("text")<CR>'
  "
  "===============================================================================================
  "----- Menu : Run menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu ".g:Perl_Root.'&Run.&Run<Tab>Perl                  <Nop>'
    exe "amenu ".g:Perl_Root.'&Run.-Sep0-                         :'
  endif
  "
  "   run the script from the local directory
  "   ( the one which is being edited; other versions may exist elsewhere ! )
  "
  exe "amenu <silent> ".g:Perl_Root.'&Run.update,\ &run\ script<Tab><C-F9>         :call Perl_Run()<CR>'
  "
  exe "amenu ".g:Perl_Root.'&Run.update,\ check\ &syntax<Tab><A-F9>                :call Perl_SyntaxCheck()<CR>:redraw<CR>:call Perl_SyntaxCheckMsg()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.cmd\.\ line\ &arg\.<Tab><S-F9>           :call Perl_Arguments()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.perl\ s&witches                          :call Perl_PerlSwitches()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.start\ &debugger<Tab><F9>                :call Perl_Debugger()<CR>'
  "
  "   set execution rights for user only ( user may be root ! )
  "
  if !s:MSWIN
    exe "amenu <silent> ".g:Perl_Root.'&Run.make\ script\ &executable              :call Perl_MakeScriptExecutable()<CR>'
  endif
  exe "amenu          ".g:Perl_Root.'&Run.-SEP2-                     :'

  exe "amenu <silent> ".g:Perl_Root.'&Run.read\ &perldoc<Tab><S-F1>        :call Perl_perldoc()<CR><CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.show\ &installed\ Perl\ modules  :call Perl_perldoc_show_module_list()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.&generate\ Perl\ module\ list    :call Perl_perldoc_generate_module_list()<CR><CR>'
  "
  exe "amenu          ".g:Perl_Root.'&Run.-SEP4-                     :'
  exe "amenu <silent> ".g:Perl_Root.'&Run.run\ perltid&y                        :call Perl_Perltidy("n")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Run.run\ perltid&y                   <C-C>:call Perl_Perltidy("v")<CR>'
	
	if s:MSWIN
		exe "amenu <silent> ".g:Perl_Root.'&Run.run\ S&mallProf                         :call Perl_Smallprof()<CR>'
	else
		"
		" Perl_SmallProfSortQuickfix needs sort(1)
		"
		if g:Perl_MenuHeader == "yes"
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.Profiler     <Nop>'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.-Sep41-       :'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.SmallProf     <Nop>'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.-Sep411-       :'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.FastProf     <Nop>'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.-Sep412-       :'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.NYTProf     <Nop>'
			exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.-Sep413-       :'
		endif
		"
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.&run\ profiler                         :call perlsupportprofiling#Perl_Smallprof()<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.sort\ report:\ &file\ name             :call perlsupportprofiling#Perl_SmallProfSortQuickfix("file-name")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.sort\ report:\ &line\ number           :call perlsupportprofiling#Perl_SmallProfSortQuickfix("line-number")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.sort\ report:\ line\ &count            :call perlsupportprofiling#Perl_SmallProfSortQuickfix("line-count")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.sort\ report:\ &wall\ time\ (time)     :call perlsupportprofiling#Perl_SmallProfSortQuickfix("time")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&SmallProf.sort\ report:\ &cpu\ time\ \ (ctime)   :call perlsupportprofiling#Perl_SmallProfSortQuickfix("ctime")<CR>'
		"                                                  
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.&run\ profiler                          :call perlsupportprofiling#Perl_Fastprof()<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.sort\ report:\ &file\ name              :call perlsupportprofiling#Perl_FastProfSortQuickfix("file-name")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.sort\ report:\ &line\ number            :call perlsupportprofiling#Perl_FastProfSortQuickfix("line-number")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.sort\ report:\ &time                    :call perlsupportprofiling#Perl_FastProfSortQuickfix("time")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&FastProf.sort\ report:\ line\ &count             :call perlsupportprofiling#Perl_FastProfSortQuickfix("line-count")<CR>'
		"                                                  
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.&run\ profiler                          :call perlsupportprofiling#Perl_NYTprof()<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.read\ &CSV\ file                        :call perlsupportprofiling#Perl_NYTprofReadCSV()<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.sort\ report:\ &file\ name              :call perlsupportprofiling#Perl_NYTProfSortQuickfix("file-name")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.sort\ report:\ &line\ number            :call perlsupportprofiling#Perl_NYTProfSortQuickfix("line-number")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.sort\ report:\ &time                    :call perlsupportprofiling#Perl_NYTProfSortQuickfix("time")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.sort\ report:\ &calls                   :call perlsupportprofiling#Perl_NYTProfSortQuickfix("calls")<CR>'
		exe "amenu <silent> ".g:Perl_Root.'&Run.Profiler.&NYTProf.sort\ report:\ t&ime/call               :call perlsupportprofiling#Perl_NYTProfSortQuickfix("time-call")<CR>'
		"
	endif

  exe "amenu          ".g:Perl_Root.'&Run.-SEP3-                     :'
  exe "amenu <silent> ".g:Perl_Root.'&Run.run\ perl&critic                      :call Perl_Perlcritic()<CR>:call Perl_PerlcriticMsg()<CR>'
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ severity\ \ (&1).perlcritic   :call Perl_Perlcritic()<CR>:call Perl_PerlcriticMsg()<CR>'
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ severity\ \ (&1).severity     <Nop>'
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ severity\ \ (&1).-Sep5-       :'
  endif

  let levelnumber = 1
  for level in [ "brutal", "cruel", "harsh", "stern", "gentle" ]
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ severity\ \ (&1).&'.level.'<Tab>(='.levelnumber.')    :call Perl_PerlCriticSeverity("'.level.'")<CR>'
    let levelnumber = levelnumber+1
  endfor
  "
  if g:Perl_MenuHeader == "yes"
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ &verbosity.perlcritic    :call Perl_Perlcritic()<CR>:call Perl_PerlcriticMsg()<CR>'
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ &verbosity.verbosity     <Nop>'
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ &verbosity.-Sep6-            :'
  endif

  for level in [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ]
    exe "amenu <silent> ".g:Perl_Root.'&Run.perlcritic\ &verbosity.&'.level.'   :call Perl_PerlCriticVerbosity('.level.')<CR>'
  endfor

  exe "amenu          ".g:Perl_Root.'&Run.-SEP5-                     :'
  exe "amenu <silent> ".g:Perl_Root.'&Run.save\ buffer\ with\ &timestamp        :call Perl_SaveWithTimestamp()<CR>'
  exe "amenu <silent> ".g:Perl_Root.'&Run.&hardcopy\ to\ FILENAME\.ps           :call Perl_Hardcopy("n")<CR>'
  exe "vmenu <silent> ".g:Perl_Root.'&Run.&hardcopy\ to\ FILENAME\.ps      <C-C>:call Perl_Hardcopy("v")<CR>'
  exe "amenu          ".g:Perl_Root.'&Run.-SEP6-                     :'
  exe "amenu <silent> ".g:Perl_Root.'&Run.settings\ and\ hot\ &keys             :call Perl_Settings()<CR>'
  "
  if  !s:MSWIN
    exe "amenu  <silent>  ".g:Perl_Root.'&Run.&xterm\ size                          :call Perl_XtermSize()<CR>'
  endif
  if g:Perl_OutputGvim == "vim"
    exe "amenu  <silent>  ".g:Perl_Root.'&Run.&output:\ VIM->buffer->xterm          :call Perl_Toggle_Gvim_Xterm()<CR>'
  else
    if g:Perl_OutputGvim == "buffer"
      exe "amenu  <silent>  ".g:Perl_Root.'&Run.&output:\ BUFFER->xterm->vim        :call Perl_Toggle_Gvim_Xterm()<CR>'
    else
      exe "amenu  <silent>  ".g:Perl_Root.'&Run.&output:\ XTERM->vim->buffer        :call Perl_Toggle_Gvim_Xterm()<CR>'
    endif
  endif
  "
  "===============================================================================================
  "----- Menu : help menu                              {{{2
  "===============================================================================================
  "
  if g:Perl_Root != ""
    exe "amenu  <silent>  ".g:Perl_Root.'&help\ \(plugin\)        :call Perl_HelpPerlsupport()<CR>'
  endif
  "
endfunction   " ---------- end of function  Perl_InitMenu  ----------
"
" vim: tabstop=2 shiftwidth=2 foldmethod=marker
