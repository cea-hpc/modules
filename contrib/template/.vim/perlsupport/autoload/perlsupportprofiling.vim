"===============================================================================
"
"          File:  perlsupportprofiling.vim
"
"   Description:  Plugin perl-support:
"                 Profiler support
"
"   VIM Version:  7.0+
"        Author:  Dr. Fritz Mehner (mn), mehner@fh-swf.de
"       Company:  FH Südwestfalen, Iserlohn
"       Version:  1.0
"       Created:  22.02.2009
"      Revision:  $Id: perlsupportprofiling.vim,v 1.3 2009/02/23 17:16:12 mehner Exp $
"       License:  Copyright 2009 Dr. Fritz Mehner
"===============================================================================
"
" Exit quickly when:
" - this plugin was already loaded
" - when 'compatible' is set
"
if exists("g:loaded_perlsupportprofiling") || &compatible
  finish
endif
let g:loaded_perlsupportregex = "v1.0"
"
let s:MSWIN = has("win16") || has("win32")   || has("win64")    || has("win95")
let s:UNIX	= has("unix")  || has("macunix") || has("win32unix")

if  s:MSWIN
  " ==========  MS Windows  ======================================================
  let s:escfilename 	= ''
  let s:plugin_dir  	= $VIM.'\vimfiles\'
	let s:installation	= 'system'
	"
else
  " ==========  Linux/Unix  ======================================================
	let s:escfilename = ' \%#[]'
	let s:installation	= 'local'
	if match( expand("<sfile>"), $VIM ) >= 0
		" system wide installation
		let s:plugin_dir  	= $VIM.'/vimfiles/'
		let s:installation	= 'system'
	else
		" user installation assumed
		let s:plugin_dir  	= $HOME.'/.vim/'
	end
	"
endif
"
"------------------------------------------------------------------------------
"  run : SmallProf, data structures     {{{1
"------------------------------------------------------------------------------
let s:Perl_ProfTempSrc		      = ''
let s:Perl_ProfTempErr		      = ''
let s:Perl_ProfTempDir		      = ''
let s:Perl_CWD									= ''
let s:Perl_SmallProfOutput   		= 'smallprof.out'
let s:Perl_SmallProfErrorFormat	= '%f:%l:%m'

let s:Perl_SmallProfSortQuickfixField	= {
	\		 'file-name'   : 1 , 
	\		 'line-number' : 2 , 
	\		 'line-count'  : 3 , 
	\		 'time'        : 4 , 
	\		 'ctime'       : 5 , 
	\		 }

let s:Perl_SmallProfSortQuickfixHL	= {
	\		 'file-name'   : '/^[^|]\+/' , 
	\		 'line-number' : '/|\d\+|/' , 
	\		 'line-count'  : '/ \@<=\d\+:\@=/' , 
	\		 'time'        : '/:\@<=\d\+\(:\d\+:\)\@=/' , 
	\		 'ctime'       : '/:\@<=\d\+\(: \)\@=/' , 
	\		 }

"------------------------------------------------------------------------------
"  run : SmallProf, generate statistics     {{{1
"  Also called in the filetype plugin perl.vim
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_Smallprof ()
  let Sou   = escape( expand("%:p"), s:escfilename ) " name of the file in the current buffer
  if &filetype != "perl"
    echohl WarningMsg | echo Sou.' seems not to be a Perl file' | echohl None
    return
  endif
  silent exe  ":update"
  "
  let l:arguments       = exists("b:Perl_CmdLineArgs") ? " ".b:Perl_CmdLineArgs : ""
  "
	let s:Perl_CWD	= getcwd()
  echohl Search | echon ' ... profiling ... ' | echohl None
  "
	let	profilercmd	= 'SMALLPROF_CONFIG=gz perl -d:SmallProf '.Sou.l:arguments
	let errortext	= system(profilercmd)
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
	"
	redraw!
  echon ' profiling done '
  "
  exe ':setlocal errorformat='.s:Perl_SmallProfErrorFormat
	exe ':cfile '.s:Perl_SmallProfOutput
	exe ':copen'
	exe ':match Visual '.s:Perl_SmallProfSortQuickfixHL['time']
  exe ':setlocal nowrap'

endfunction   " ---------- end of function  Perl_Smallprof  ----------
"
"------------------------------------------------------------------------------
"  run : SmallProf, sort report     {{{1
"  Rearrange the profiler report.
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_SmallProfSortQuickfix ( mode )
	"
	if &filetype == 'qf'
		"
		if ! has_key( s:Perl_SmallProfSortQuickfixField, a:mode )
			echomsg	'Allowed sort keys : ['.join( keys(s:Perl_SmallProfSortQuickfixField), '|' ).'].'
			return
		endif
		let	reverse	= ' --reverse'
		if s:Perl_SmallProfSortQuickfixField[a:mode] <= 2
			let	reverse	= ''
		endif
		if s:Perl_ProfTempErr	== ''
			let	s:Perl_ProfTempErr	= tempname()
		endif
		let sortcmd		= 'sort '.reverse
		let sortcmd	 .= ' --numeric-sort'
		let sortcmd	 .= ' --field-separator=:'
		let sortcmd	 .= ' --key='.s:Perl_SmallProfSortQuickfixField[a:mode]
		let sortcmd	 .= ' --output='.s:Perl_ProfTempErr
		let sortcmd	 .= ' '.s:Perl_CWD.'/smallprof.out'
		let	rval		= system( sortcmd )
		if v:shell_error
			echomsg	"Shell command '".sortcmd."' failed."
			return
		endif
		exe ':setlocal errorformat='.s:Perl_SmallProfErrorFormat
		exe ':cfile '.s:Perl_ProfTempErr
		exe ':copen'
		exe ':match Visual '.s:Perl_SmallProfSortQuickfixHL[a:mode]
		exe ':setlocal nowrap'
		"
	else
		echomsg 'the current buffer is not a QuickFix List (error list)'
	endif
	"
endfunction    " ----------  end of function Perl_SmallProfSortQuickfix  ----------
"
"------------------------------------------------------------------------------
"  run : Profiler; ex command tab expansion     {{{1
"------------------------------------------------------------------------------
function!	perlsupportprofiling#Perl_ProfSortList ( ArgLead, List )
	" show all types
	if a:ArgLead == ''
		return a:List
	endif
	" show types beginning with a:ArgLead
	let	expansions	= []
	for item in a:List
		if match( item, '\<'.a:ArgLead.'\w*' ) == 0
			call add( expansions, item )
		endif
	endfor
	return	expansions
endfunction    " ----------  end of function Perl_ProfSortList  ----------

"------------------------------------------------------------------------------
"  run : SmallProf, ex command tab expansion     {{{1
"------------------------------------------------------------------------------
function!	perlsupportprofiling#Perl_SmallProfSortList ( ArgLead, CmdLine, CursorPos )
	return	perlsupportprofiling#Perl_ProfSortList( a:ArgLead, keys(s:Perl_SmallProfSortQuickfixField) )
endfunction    " ----------  end of function Perl_SmallProfSortList  ----------

"------------------------------------------------------------------------------
"  run : FastProf, data structures     {{{1
"------------------------------------------------------------------------------
let s:Perl_FastProfOutput   		= 'fastprof.out'
let s:Perl_Fprofpp              = ''
let s:Perl_FastProfErrorFormat	= '%f:%l\ %m'

let s:Perl_FastProfSortQuickfixField	= {
	\		 'file-name'   : 1 , 
	\		 'line-number' : 2 , 
	\		 'time'        : 2 , 
	\		 'line-count'  : 3 , 
	\		 }

let s:Perl_FastProfSortQuickfixHL	= {
	\		 'file-name'   : '/^[^|]\+/' , 
	\		 'line-number' : '/|\d\+|/' , 
	\		 'time'        : '/\(| \)\@<=\d\+\.\d\+/' , 
	\		 'line-count'  : '/ \@<=\d\+:\@=/' , 
	\		 }

"------------------------------------------------------------------------------
"  run : FastProf, generate statistics     {{{1
"  Also called in the filetype plugin perl.vim
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_Fastprof ()
  let Sou   = escape( expand("%:p"), s:escfilename ) " name of the file in the current buffer
  if &filetype != "perl"
    echohl WarningMsg | echo Sou.' seems not to be a Perl file' | echohl None
    return
  endif
  silent exe  ":update"
  "
  let l:arguments       = exists("b:Perl_CmdLineArgs") ? " ".b:Perl_CmdLineArgs : ""
  "
	let s:Perl_CWD	= getcwd()
  echohl Search | echon ' ... profiling ... ' | echohl None
  "
	let	profilercmd	= 'perl -d:FastProf '.Sou.l:arguments
	let errortext	= system(profilercmd)
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
  "
	if s:Perl_Fprofpp	== ''
		let	s:Perl_Fprofpp	= tempname()
	endif
	let	profilercmd	= 'fprofpp > '.s:Perl_Fprofpp
	let errortext	= system( profilercmd )
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
	"
	redraw!
  echon ' profiling done '
  "
  exe ':setlocal errorformat='.s:Perl_FastProfErrorFormat
	exe ':cfile '.s:Perl_Fprofpp
	exe ':copen'
	exe ':match Visual '.s:Perl_FastProfSortQuickfixHL['time']
  exe ':setlocal nowrap'

endfunction   " ---------- end of function  Perl_Fastprof  ----------
"
"------------------------------------------------------------------------------
"  run : FastProf, sort report     {{{1
"  Rearrange the profiler report.
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_FastProfSortQuickfix ( mode )
	"
	if &filetype == 'qf'
		"
		if ! has_key( s:Perl_FastProfSortQuickfixField, a:mode )
			echomsg	'Allowed sort keys : ['.join( keys(s:Perl_FastProfSortQuickfixField), '|' ).'].'
			return
		endif
		let	reverse	= ' --reverse'
		if a:mode == 'file-name' || a:mode == 'line-number'
			let	reverse	= ''
		endif
		if s:Perl_ProfTempErr	== ''
			let	s:Perl_ProfTempErr	= tempname()
		endif
		let sortcmd		= 'sort '.reverse
		let sortcmd	 .= ' --numeric-sort'
		if a:mode == 'line-number'
			let sortcmd	 .= ' --field-separator=:'
		endif
		let sortcmd	 .= ' --key='.s:Perl_FastProfSortQuickfixField[a:mode]
		let sortcmd	 .= ' --output='.s:Perl_ProfTempErr
		let sortcmd	 .= ' '.s:Perl_Fprofpp
		let	rval		= system( sortcmd )
		if v:shell_error
			echomsg	"Shell command '".sortcmd."' failed."
			return
		endif
		exe ':setlocal errorformat='.s:Perl_FastProfErrorFormat
		exe ':cfile '.s:Perl_ProfTempErr
		exe ':copen'
		exe ':match Visual '.s:Perl_FastProfSortQuickfixHL[a:mode]
		exe ':setlocal nowrap'
		"
	else
		echomsg 'the current buffer is not a QuickFix List (error list)'
	endif
	"
endfunction    " ----------  end of function Perl_FastProfSortQuickfix  ----------
"
"------------------------------------------------------------------------------
"  run : FastProf, ex command tab expansion     {{{1
"------------------------------------------------------------------------------
function!	perlsupportprofiling#Perl_FastProfSortList ( ArgLead, CmdLine, CursorPos )
	return	perlsupportprofiling#Perl_ProfSortList( a:ArgLead, keys(s:Perl_FastProfSortQuickfixField) )
endfunction    " ----------  end of function Perl_FastProfSortList  ----------

"------------------------------------------------------------------------------
"  run : NYTProf, data structures     {{{1
"------------------------------------------------------------------------------
let s:Perl_NYTProf_html			 	= 'no'
if exists( 'g:Perl_NYTProf_html' )
	let s:Perl_NYTProf_html	= g:Perl_NYTProf_html
endif

let s:Perl_csv2err            = s:plugin_dir.'perl-support/scripts/csv2err.pl'
let s:Perl_NYTProfErrorFormat	= '%f:%l:%m'
let s:Perl_NYTProfCSVfile			= ''

let s:Perl_NYTProfSortQuickfixField	= {
	\		 'file-name'   : 1 , 
	\		 'line-number' : 2 , 
	\		 'time'        : 3 , 
	\		 'calls'       : 4 , 
	\		 'time-call'   : 5 , 
	\		 }

let s:Perl_NYTProfSortQuickfixHL	= {
	\		 'file-name'   : '/^[^|]\+/' , 
	\		 'line-number' : '/|\d\+|/' , 
	\		 'time'        : '/\(| \)\@<=\d\+\.\d\+:\@=/' , 
	\		 'calls'       : '/:\@<=\d\+:\@=/' , 
	\		 'time-call'   : '/:\@<=\d\+\.\d\+\(: \)\@=/' , 
	\		 }

"------------------------------------------------------------------------------
"  run : NYTProf, generate statistics     {{{1
"  Also called in the filetype plugin perl.vim
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_NYTprof ()
  let Sou   = escape( expand("%:p"), s:escfilename ) " name of the file in the current buffer
  if &filetype != "perl"
    echohl WarningMsg | echo Sou.' seems not to be a Perl file' | echohl None
    return
  endif
  silent exe  ":update"
  "
  let l:arguments       = exists("b:Perl_CmdLineArgs") ? " ".b:Perl_CmdLineArgs : ""
  "
	let s:Perl_CWD	= getcwd()
  echohl Search | echon ' ... profiling ... ' | echohl None
  "
	let	profilercmd	= 'perl -d:NYTProf '.Sou.l:arguments
	let errortext	= system(profilercmd)
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
  "
  if s:Perl_NYTProf_html == 'yes'
		let errortext	= system( 'nytprofhtml' )
		if v:shell_error
			redraw
			echon errortext
			return
		endif
	endif
  "
	let errortext	= system( 'nytprofcsv' )
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
	"
	redraw!
  echon ' profiling done -- read a CSV file'
  "
endfunction   " ---------- end of function  Perl_NYTprof  ----------

"------------------------------------------------------------------------------
"  run : NYTProf, generate statistics     {{{1
"  Also called in the filetype plugin perl.vim
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_NYTprofReadCSV ()

"	let s:Perl_CWD	= getcwd()
	if has("gui_running")
		let s:Perl_NYTProfCSVfile	= browse( 0, 'read a Devel::NYTProf CSV-file', '', '' )
	else
		let	s:Perl_NYTProfCSVfile	= input( 'read a Devel::NYTProf CSV-file : ', '', "file" )
	end
	let s:Perl_NYTProfCSVfile	= substitute( s:Perl_NYTProfCSVfile, '^\s\+', '', '' )
	let s:Perl_NYTProfCSVfile	= substitute( s:Perl_NYTProfCSVfile, '\s\+$', '', '' )
	"
	" return if command canceled
	if s:Perl_NYTProfCSVfile =~ '^$'
		return
	endif
	"
	" return if not a CSV file
	if s:Perl_NYTProfCSVfile !~ '-\(block\|line\|sub\)\.csv$'
    echohl WarningMsg | echo s:Perl_NYTProfCSVfile.' seems not to be a CSV file' | echohl None
    return
  endif

	let	s:Perl_CWD = fnamemodify( s:Perl_NYTProfCSVfile, ":p:h:h" )

  " get the name of the current temp directory
	if s:Perl_ProfTempDir	== ''
		let	s:Perl_ProfTempDir	= fnamemodify( tempname(), ":p:h" )
	endif
	" get the name of the source file corresponding to the CSSV file
	let	s:Perl_ProfTempSrc	= substitute( s:Perl_NYTProfCSVfile, '-\(block\|line\|sub\)\.csv$', '', '' )
	if s:Perl_ProfTempSrc =~ '/'
		let	s:Perl_ProfTempSrc	= split( s:Perl_ProfTempSrc, '/' )[-1]
	endif

	let	sourcepresent	= 'yes'

	if findfile( s:Perl_ProfTempSrc, s:Perl_CWD ) != ''
		let	s:Perl_ProfTempErr	= s:Perl_ProfTempDir.'/'.s:Perl_ProfTempSrc.'.err'
		let	s:Perl_ProfTempSrc	= s:Perl_CWD.'/'.s:Perl_ProfTempSrc
	else
		let	s:Perl_ProfTempSrc	= s:Perl_ProfTempDir.'/'.s:Perl_ProfTempSrc
		let	s:Perl_ProfTempErr	= s:Perl_ProfTempSrc.'.err'
		let	sourcepresent	= 'no'
	endif

	let	profilercmd	= 'perl '.s:Perl_csv2err.' -H -i '.s:Perl_NYTProfCSVfile.' -t '.s:Perl_ProfTempDir
	let errortext	= system( profilercmd )
  "
  if v:shell_error
    redraw
		echon errortext
    return
  endif
  "
  exe ':setlocal errorformat='.s:Perl_NYTProfErrorFormat
	if sourcepresent == 'yes'
		exe ':edit  '.s:Perl_ProfTempSrc
	else
		exe ':view  '.s:Perl_ProfTempSrc
		exe ':lchdir '.s:Perl_ProfTempDir
	endif
	exe ':cfile '.s:Perl_ProfTempErr
	exe ':copen'
  exe ':setlocal nowrap'

endfunction   " ---------- end of function  Perl_NYTprofReadCSV  ----------
"
"------------------------------------------------------------------------------
"  run : NYTProf, sort report     {{{1
"  Rearrange the profiler report.
"------------------------------------------------------------------------------
function! perlsupportprofiling#Perl_NYTProfSortQuickfix ( mode )
	"
	if &filetype == 'qf'
		"
		if ! has_key( s:Perl_NYTProfSortQuickfixField, a:mode )
			echomsg	'Allowed sort keys : ['.join( keys(s:Perl_NYTProfSortQuickfixField), '|' ).'].'
			return
		endif
		let	reverse	= ' --reverse'
		if s:Perl_NYTProfSortQuickfixField[a:mode] <= 2
			let	reverse	= ''
		endif
		if s:Perl_ProfTempErr	== ''
			let	s:Perl_ProfTempErr	= tempname()
		endif
		let sortcmd		= 'sort '.reverse
		let sortcmd	 .= ' --numeric-sort'
		let sortcmd	 .= ' --field-separator=:'
		let sortcmd	 .= ' --key='.s:Perl_NYTProfSortQuickfixField[a:mode]
		let sortcmd	 .= ' --output='.s:Perl_ProfTempErr
		let sortcmd	 .= ' '.s:Perl_ProfTempErr
		let	rval		= system( sortcmd )
		if v:shell_error
			echomsg	"Shell command '".sortcmd."' failed."
			return
		endif
		exe ':setlocal errorformat='.s:Perl_NYTProfErrorFormat
		exe ':cfile '.s:Perl_ProfTempErr
		exe ':copen'
		exe ':match Visual '.s:Perl_NYTProfSortQuickfixHL[a:mode]
		exe ':setlocal nowrap'
		"
	else
		echomsg 'the current buffer is not a QuickFix List (error list)'
	endif
	"
endfunction    " ----------  end of function Perl_NYTProfSortQuickfix  ----------
"
"------------------------------------------------------------------------------
"  run : NYTProf, ex command tab expansion     {{{1
"------------------------------------------------------------------------------
function!	perlsupportprofiling#Perl_NYTProfSortList ( ArgLead, CmdLine, CursorPos )
	return	perlsupportprofiling#Perl_ProfSortList( a:ArgLead, keys(s:Perl_NYTProfSortQuickfixField) )
endfunction    " ----------  end of function Perl_NYTProfSortList  ----------

" vim: tabstop=2 shiftwidth=2 foldmethod=marker
