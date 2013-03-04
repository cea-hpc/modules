" File Name: fluxkeys.vim
" Maintainer: Mathias Gumz <akira at fluxbox dot org>
" Original Date: 040127 00:40:33 
" Last Update: 050624 11:20:25
" Description: fluxbox key syntax
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" quit when a syntax file was already loaded
if exists("b:current_syntax")
    finish
endif

syntax keyword fbActionNames ArrangeWindows  contained
syntax keyword fbActionNames Bindkey  contained
syntax keyword fbActionNames Close  contained
syntax keyword fbActionNames CommandDialog  contained
syntax keyword fbActionNames Deiconify  contained
syntax keyword fbActionNames DetachClient  contained
syntax keyword fbActionNames Export  contained
syntax keyword fbActionNames Exec  contained
syntax keyword fbActionNames ExecCommand  contained
syntax keyword fbActionNames Execute  contained
syntax keyword fbActionNames Exit  contained
syntax keyword fbActionNames FocusUp  contained
syntax keyword fbActionNames FocusDown  contained
syntax keyword fbActionNames FocusLeft  contained
syntax keyword fbActionNames FocusRight  contained
syntax keyword fbActionNames Fullscreen  contained
syntax keyword fbActionNames Iconify  contained
syntax keyword fbActionNames KillWindow  contained
syntax keyword fbActionNames LeftWorkspace  contained
syntax keyword fbActionNames Lower  contained
syntax keyword fbActionNames LowerLayer  contained
syntax keyword fbActionNames MacroCmd  contained
syntax keyword fbActionNames Maximize  contained
syntax keyword fbActionNames MaximizeHorizontal  contained
syntax keyword fbActionNames MaximizeVertical  contained
syntax keyword fbActionNames MaximizeWindow  contained
syntax keyword fbActionNames Minimize  contained
syntax keyword fbActionNames MinimizeWindow  contained
syntax keyword fbActionNames MoveTo  contained
syntax keyword fbActionNames Move  contained
syntax keyword fbActionNames MoveDown  contained
syntax keyword fbActionNames MoveLeft  contained
syntax keyword fbActionNames MoveRight  contained
syntax keyword fbActionNames MoveTabLeft  contained
syntax keyword fbActionNames MoveTabRight  contained
syntax keyword fbActionNames MoveUp  contained
syntax keyword fbActionNames Next\Group  contained
syntax keyword fbActionNames NextTab  contained
syntax keyword fbActionNames NextWindow  contained
syntax keyword fbActionNames NextWorkspace  contained
syntax keyword fbActionNames PrevGroup  contained
syntax keyword fbActionNames PrevTab  contained
syntax keyword fbActionNames PrevWindow  contained
syntax keyword fbActionNames PrevWorkspace  contained
syntax keyword fbActionNames Quit  contained
syntax keyword fbActionNames Raise  contained
syntax keyword fbActionNames RaiseLayer  contained
syntax keyword fbActionNames Reconfig  contained
syntax keyword fbActionNames Reconfigure  contained
syntax keyword fbActionNames ReloadStyle  contained
syntax keyword fbActionNames ResizeTo  contained
syntax keyword fbActionNames Resize  contained
syntax keyword fbActionNames ResizeHorizontal  contained
syntax keyword fbActionNames ResizeVertical  contained
syntax keyword fbActionNames Restart  contained
syntax keyword fbActionNames RightWorkspace  contained
syntax keyword fbActionNames RootMenu  contained
syntax keyword fbActionNames SaveRc  contained
syntax keyword fbActionNames Setenv  contained
syntax keyword fbActionNames SetHead  contained
syntax keyword fbActionNames SendToWorkspace  contained
syntax keyword fbActionNames SendToNextWorkspace  contained
syntax keyword fbActionNames SendToPrevWorkspace  contained
syntax keyword fbActionNames SetStyle  contained
syntax keyword fbActionNames SetWorkspacename  contained
syntax keyword fbActionNames SetWorkspacenameDialog  contained
syntax keyword fbActionNames SetResourcevalue  contained
syntax keyword fbActionNames SetResourcevalueDialog  contained
syntax keyword fbActionNames Shade  contained
syntax keyword fbActionNames ShadeWindow  contained
syntax keyword fbActionNames ShowDesktop  contained
syntax keyword fbActionNames Stick  contained
syntax keyword fbActionNames StickWindow  contained
syntax keyword fbActionNames Tab  contained
syntax keyword fbActionNames TakeToWorkspace  contained
syntax keyword fbActionNames TakeToNextWorkspace  contained
syntax keyword fbActionNames TakeToPrevWorkspace  contained
syntax keyword fbActionNames ToggleDecor  contained
syntax keyword fbActionNames WindowMenu  contained
syntax keyword fbActionNames Workspace  contained
syntax keyword fbActionNames Workspace1  contained
syntax keyword fbActionNames Workspace2  contained
syntax keyword fbActionNames Workspace3  contained
syntax keyword fbActionNames Workspace4  contained
syntax keyword fbActionNames Workspace5  contained
syntax keyword fbActionNames Workspace6  contained
syntax keyword fbActionNames Workspace7  contained
syntax keyword fbActionNames Workspace8  contained
syntax keyword fbActionNames Workspace9  contained
syntax keyword fbActionNames Workspace10  contained
syntax keyword fbActionNames Workspace11  contained
syntax keyword fbActionNames Workspace12  contained
syntax keyword fbActionNames WorkspaceMenu  contained

syntax keyword fbModifierNames Control Shift  contained
syntax keyword fbModifierNames Mod1 Mod2 Mod3 Mod4 Mod5  contained
syntax keyword fbModifierNames None  contained

" reference corners
syntax keyword fbParameterNames UpperLeft Upper UpperRight contained
syntax keyword fbParameterNames Left Right contained
syntax keyword fbParameterNames LowerLeft Lower LowerRight contained
" deiconfiy
syntax keyword fbParameterNames LastWorkspace Last All OriginQuiet contained

" parameter numbers
syntax match   fbParameterNumber /\([+-]\)*\d\+/ contained


"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"  match the right parts
syntax match   fbParameter /.*/ contained contains=fbParameterNames,fbParameterNumber

" anything with an unknown ActionName is colored Error
syntax match   fbAction /\w\+/ contained contains=fbActionNames nextgroup=fbParameter
syntax match   fbExecAction /Exec\(ute\|Command\)*\s\+.*$/ contained contains=fbActionNames

" macro magic
syntax region  fbMacroParameter start=/./ end=/.\{-\}}/ contained contains=fbParameterNames,fbParameterNumber
syntax match   fbMacroAction /\w\+/ contained contains=fbActionNames nextgroup=fbMacroParameter
syntax region  fbMacroExecAction start=/{Exec\(ute\|Command\)*\s\+/hs=s+1 end=/.\{-}}/he=e-1 contained contains=fbActionNames oneline
syntax region  fbMacro start=/{/ end=/.\{-}}/ contained contains=fbMacroExecAction,fbMacroAction oneline nextgroup=fbMacro skipwhite
syntax match   fbMacroStart /MacroCmd\s\+/ contained contains=fbActionNames nextgroup=fbMacro

" anything but a valid modifier is colored Error
syntax match   fbKeyStart /^\w\+/  contained contains=fbModifierNames

" anything but a comment or a valid key line is colored Error
syntax match   fbNoKeyline /.\+$/ display skipwhite
syntax region  fbKeys start=/\w\+/ end=/.\{-}:/he=e-1 contains=fbKeyStart,fbModifierNames nextgroup=fbMacroStart,fbExecAction,fbAction oneline
syntax match   fbComment /[#!].*$/ display 

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" coloring

highlight link fbNoKeyline Error
highlight link fbAction Error
highlight link fbKeyStart Error

highlight link fbComment Comment
highlight link fbKeys Number
highlight link fbExecAction String
highlight link fbMacroExecAction String
highlight link fbActionNames Type 
highlight link fbModifierNames Macro
highlight link fbParameter Number
highlight link fbParameterNames Function
highlight link fbParameterNumber Conditional

syntax sync fromstart

let b:current_syntax = 'fluxkeys'

