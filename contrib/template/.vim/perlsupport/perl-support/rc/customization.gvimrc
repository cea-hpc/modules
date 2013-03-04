"===================================================================================
"         FILE:  .gvimrc
"  DESCRIPTION:  suggestion for a personal configuration file ~/.gvimrc
"       AUTHOR:  Dr.-Ing. Fritz Mehner
"      VERSION:  1.0
"      CREATED:  23.05.2008
"     REVISION:  $Id: customization.gvimrc,v 1.5 2009/04/04 08:26:53 mehner Exp $
"===================================================================================
"
"===================================================================================
" GENERAL SETTINGS
"===================================================================================
"
set cmdheight=2                       " Make command line two lines high
set mousehide                         " Hide the mouse when typing text

highlight Normal   guibg=grey90
highlight Cursor   guibg=Blue   guifg=NONE
highlight lCursor  guibg=Cyan   guifg=NONE
highlight NonText  guibg=grey80
highlight Constant gui=NONE     guibg=grey95
highlight Special  gui=NONE     guibg=grey95

"
"-------------------------------------------------------------------------------
" Moving cursor to other windows:
" shift down   : change window focus to lower one (cyclic)
" shift up     : change window focus to upper one (cyclic)
" shift left   : change window focus to one on left
" shift right  : change window focus to one on right
"-------------------------------------------------------------------------------
nnoremap <s-down>   <c-w>w
nnoremap <s-up>     <c-w>W
nnoremap <s-left>   <c-w>h
nnoremap <s-right>  <c-w>l
"
"-------------------------------------------------------------------------------
"  Some additional hot keys:
"    S-F3  -  call gvim file browser
"-------------------------------------------------------------------------------
 noremap  <silent> <s-F3>       :silent browse confirm e<CR>
inoremap  <silent> <s-F3>  <Esc>:silent browse confirm e<CR>
"
"-------------------------------------------------------------------------------
" toggle insert mode <--> 'normal mode with the <RightMouse>-key
"-------------------------------------------------------------------------------
nnoremap  <RightMouse> <Insert>
inoremap  <RightMouse> <ESC>
"
"-------------------------------------------------------------------------------
" use font with clearly distinguishable brackets : ()[]{}
"-------------------------------------------------------------------------------
set guifont=Luxi\ Mono\ 14
"
