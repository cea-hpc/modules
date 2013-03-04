" Vim color file
" Name:       herald.vim
" Author:     Fabio Cevasco <h3rald@h3rald.com>
" Version:    0.1.0
" Notes: Gvim-only

hi clear

if exists("syntax_on")
  syntax reset
endif

let colors_name = "herald"

set background=dark

" Set some syntax-related variables
let ruby_operators = 1

if has("gui_running")

  " Standard Highlighting
  hi Normal         guibg=#1F1F1F guifg=#D0D0D0 gui=none
  hi Cursor         guibg=#FFEE68 guifg=#1F1F1F gui=none
  hi CursorColumn   guibg=#000000               gui=none
  hi CursorLine     guibg=#000000               gui=none
  hi FoldColumn     guibg=#1F1F1F guifg=#70BDF1 gui=none
  hi LineNr         guibg=#000000 guifg=#696567 gui=none
  hi StatusLine     guibg=#000000 guifg=#696567 gui=none
  hi StatusLineNC   guibg=#25365a guifg=#696567 gui=none
  hi ErrorMsg       guibg=#A32024 guifg=#D0D0D0 gui=none
  hi Search         guibg=#FF9E16 guifg=#000000 gui=none
  hi IncSearch      guibg=#FF9E16 guifg=#000000 gui=none

  " Code Highlighting
  hi Comment        guibg=#1F1F1F guifg=#696567 gui=none
  hi Constant       guibg=#1F1F1F guifg=#6DF584 gui=none
  hi Title          guibg=#1F1F1F guifg=#6DF584 gui=bold   
  hi String         guibg=#1F1F1F guifg=#FFB539 gui=none
  hi Error          guibg=#1F1F1F guifg=#FC4234 gui=none
  hi Identifier     guibg=#1F1F1F guifg=#70BDF1 gui=none
  hi Function       guibg=#1F1F1F guifg=#90CBF1 gui=none
  hi Ignore         guibg=#1F1F1F guifg=#1F1F1F gui=none
  hi MatchParen     guibg=#FFA500 guifg=#1F1F1F gui=none
  hi PreProc        guibg=#1F1F1F guifg=#BF81FA gui=none
  hi Special        guibg=#1F1F1F guifg=#FFB539 gui=none
  hi Todo           guibg=#1F1F1F guifg=#FC4234 gui=bold
  hi Underlined     guibg=#1F1F1F guifg=#FC4234 gui=underline    
  hi Statement      guibg=#1F1F1F guifg=#E783E9 gui=none
  hi Operator       guibg=#1F1F1F guifg=#FC6984 gui=none
  hi Delimiter      guibg=#1F1F1F guifg=#FFEC48 gui=none
  hi Type           guibg=#1F1F1F guifg=#FFEE68 gui=none
  hi Exception      guibg=#1F1F1F guifg=#FC4234 gui=none

  " HTML-specific formatting
  hi htmlBold                 guibg=#1F1F1F guifg=#D0D0D0 gui=bold
  hi htmlBoldItalic           guibg=#1F1F1F guifg=#D0D0D0 gui=bold,italic
  hi htmlBoldUnderline        guibg=#1F1F1F guifg=#D0D0D0 gui=bold,underline
  hi htmlBoldUnderlineItalic  guibg=#1F1F1F guifg=#D0D0D0 gui=bold,underline,italic
  hi htmlItalic               guibg=#1F1F1F guifg=#D0D0D0 gui=italic
  hi htmlUnderline            guibg=#1F1F1F guifg=#D0D0D0 gui=underline
  hi htmlUnderlineItalic      guibg=#1F1F1F guifg=#D0D0D0 gui=underline,italic

elseif &t_Co == 256

    " Standard Highlighting
    hi Normal         ctermbg=234 ctermfg=252 cterm=none
    hi Cursor         ctermbg=227 ctermfg=234 cterm=none
    hi CursorColumn   ctermbg=0               cterm=none
    hi CursorLine     ctermbg=0               cterm=none
    hi FoldColumn     ctermbg=234 ctermfg=75  cterm=none
    hi LineNr         ctermbg=0   ctermfg=241 cterm=none
    hi StatusLine     ctermbg=0   ctermfg=241 cterm=none
    hi StatusLineNC   ctermbg=237 ctermfg=241 cterm=none
    hi ErrorMsg       ctermbg=124 ctermfg=252 cterm=none
    hi Search         ctermbg=214 ctermfg=0   cterm=none
    hi IncSearch      ctermbg=214 ctermfg=0   cterm=none

    " Code Highlighting
    hi Comment        ctermbg=234 ctermfg=241 cterm=none
    hi Constant       ctermbg=234 ctermfg=84  cterm=none   
    hi Title          ctermbg=234 ctermfg=84  cterm=bold   
    hi String         ctermbg=234 ctermfg=215 cterm=none
    hi Error          ctermbg=234 ctermfg=203 cterm=none
    hi Identifier     ctermbg=234 ctermfg=75  cterm=none
    hi Function       ctermbg=234 ctermfg=117 cterm=none
    hi Ignore         ctermbg=234 ctermfg=234 cterm=none
    hi MatchParen     ctermbg=214 ctermfg=234 cterm=none
    hi PreProc        ctermbg=234 ctermfg=141 cterm=none
    hi Special        ctermbg=234 ctermfg=215 cterm=none
    hi Todo           ctermbg=234 ctermfg=203 cterm=bold
    hi Underlined     ctermbg=234 ctermfg=203 cterm=underline    
    hi Statement      ctermbg=234 ctermfg=176 cterm=none
    hi Operator       ctermbg=234 ctermfg=204 cterm=none
    hi Delimiter      ctermbg=234 ctermfg=227 cterm=none
    hi Type           ctermbg=234 ctermfg=227 cterm=none
    hi Exception      ctermbg=234 ctermfg=203 cterm=none

    " HTML-specific formatting
    hi htmlBold                 ctermbg=234  ctermfg=252   cterm=bold
    hi htmlBoldItalic           ctermbg=234  ctermfg=252   cterm=bold,italic
    hi htmlBoldUnderline        ctermbg=234  ctermfg=252   cterm=bold,underline
    hi htmlBoldUnderlineItalic  ctermbg=234  ctermfg=252   cterm=bold,underline,italic
    hi htmlItalic               ctermbg=234  ctermfg=252   cterm=italic
    hi htmlUnderline            ctermbg=234  ctermfg=252   cterm=underline
    hi htmlUnderlineItalic      ctermbg=234  ctermfg=252   cterm=underline,italic

endif

hi! default link bbcodeBold htmlBold
hi! default link bbcodeBoldItalic htmlBoldItalic
hi! default link bbcodeBoldItalicUnderline htmlBoldUnderlineItalic
hi! default link bbcodeBoldUnderline htmlBoldUnderline
hi! default link bbcodeItalic htmlItalic
hi! default link bbcodeItalicUnderline htmlUnderlineItalic
hi! default link bbcodeUnderline htmlUnderline

" TODO Spelling formatting
if has("spell")
  hi SpellBad   guisp=#ee2c2c gui=undercurl
  hi SpellCap   guisp=#2c2cee gui=undercurl
  hi SpellLocal guisp=#2ceeee gui=undercurl
  hi SpellRare  guisp=#ee2cee gui=undercurl
endif
