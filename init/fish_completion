#completion for modules

function __fish_module_no_subcommand --description 'Test if modulecmd has yet to be given the subcommand'
   for i in (commandline -opc)
      if contains -- $i add load rm unload swap switch show display list avail is-loaded is-saved is-used is-avail info-loaded aliases use unuse refresh reload purge source whatis apropos keyword search test save restore saverm saveshow savelist initadd initprepend initrm initswitch initlist initclear path paths append-path prepend-path remove-path
         return 1
      end
   end
   return 0
end

function __fish_module_use_avail --description 'Test if module command should have available packages as potential completion'
   for i in (commandline -opc)
      if contains -- $i help add load swap switch show display list avail is-loaded is-avail info-loaded source whatis test initadd initprepend path paths
         return 0
      end
   end
   return 1
end

function __fish_module_use_list --description 'Test if module command should have loaded packages as potential completion'
   for i in (commandline -opc)
      if contains -- $i swap switch unload
         return 0
      end
   end
   return 1
end

function __fish_module_use_initlist --description 'Test if module command should have packages in init file as potential completion'
   for i in (commandline -opc)
      if contains -- $i initrm initswitch
         return 0
      end
   end
   return 1
end

function __fish_module_use_savelist --description 'Test if module command should have saved collections as potential completion'
   for i in (commandline -opc)
      if contains -- $i save restore saverm saveshow savelist is-saved
         return 0
      end
   end
   return 1
end

complete -c module -n '__fish_module_use_avail' -f -a "(module avail -t ^&1 | sed '\
   /^-\+/d; /^\s*\$/d; \
   /->.*\$/d; \
   /:\$/d; \
   /:ERROR:/d; \
   s#^\(.*\)/\(.\+\)(.*default.*)#\1\n\1\/\2#; \
   s#(.*)\$##g; \
   s#\s*\$##g; \
   s#/*\$##g;')"
complete -c module -n '__fish_module_use_list' -f -a "(module list -t ^&1 | sed '\
   /Currently Loaded Modulefiles:\$/d;')"
complete -c module -n '__fish_module_use_initlist' -f -a "(module initlist ^&1 | sed '\
   / loads modules:\$/d;')"
complete -c module -n '__fish_module_use_savelist' -f -a "(module savelist -t ^&1 | sed '\
   /Named collection list\$/d; \
   /:\$/d; \
   /:ERROR:/d;')"

complete -f -n '__fish_module_no_subcommand' -c module -a 'help' --description 'Print this or modulefile(s) help info'
complete -f -n '__fish_module_no_subcommand' -c module -a 'avail' --description 'List all or matching available modules'
complete -f -n '__fish_module_no_subcommand' -c module -a 'is-avail' --description 'Is any of the modulefile(s) available'
complete -f -n '__fish_module_no_subcommand' -c module -a 'add load' --description 'Load modulefile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'rm unload' --description 'Remove modulefile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'swap switch' --description 'Unload mod1 and load mod2'
complete -f -n '__fish_module_no_subcommand' -c module -a 'show display' --description 'Display information about modulefile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'list' --description 'List loaded modules'
complete -f -n '__fish_module_no_subcommand' -c module -a 'is-loaded' --description 'Test if any of the modulefile(s) are loaded'
complete -f -n '__fish_module_no_subcommand' -c module -a 'info-loaded' --description 'Get full name of matching loaded module(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'aliases' --description 'List all module aliases'
complete -f -n '__fish_module_no_subcommand' -c module -a 'use' --description 'Add dir(s) to MODULEPATH variable'
complete -f -n '__fish_module_no_subcommand' -c module -a 'unuse' --description 'Remove dir(s) from MODULEPATH variable'
complete -f -n '__fish_module_no_subcommand' -c module -a 'is-used' --description 'Is any of the dir(s) enabled in MODULEPATH'
complete -f -n '__fish_module_no_subcommand' -c module -a 'refresh reload' --description 'Unload then load all loaded modulefiles'
complete -f -n '__fish_module_no_subcommand' -c module -a 'purge' --description 'Unload all loaded modulefiles'
complete -f -n '__fish_module_no_subcommand' -c module -a 'source' --description 'Execute scriptfile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'whatis' --description 'Print whatis information of modulefile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'apropos keyword search' --description 'Search all name and whatis containing str'
complete -f -n '__fish_module_no_subcommand' -c module -a 'test' --description 'Test modulefile(s)'
complete -f -n '__fish_module_no_subcommand' -c module -a 'save' --description 'Save current module list to collection'
complete -f -n '__fish_module_no_subcommand' -c module -a 'restore' --description 'Restore module list from collection or file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'saverm' --description 'Remove saved collection'
complete -f -n '__fish_module_no_subcommand' -c module -a 'saveshow' --description 'Display information about collection'
complete -f -n '__fish_module_no_subcommand' -c module -a 'savelist' --description 'List all saved collections'
complete -f -n '__fish_module_no_subcommand' -c module -a 'is-saved' --description 'Test if any of the collection(s) exists'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initadd' --description 'Add modulefile to shell init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initprepend' --description 'Add to beginning of list in init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initrm' --description 'Remove modulefile from shell init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initswitch' --description 'Switch mod1 with mod2 from init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initlist' --description 'List all modules loaded from init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'initclear' --description 'Clear all modulefiles from init file'
complete -f -n '__fish_module_no_subcommand' -c module -a 'path' --description 'Print modulefile path'
complete -f -n '__fish_module_no_subcommand' -c module -a 'paths' --description 'Print path of matching available modules'
complete -f -n '__fish_module_no_subcommand' -c module -a 'append-path' --description 'Append value to environment variable'
complete -f -n '__fish_module_no_subcommand' -c module -a 'prepend-path' --description 'Prepend value to environment variable'
complete -f -n '__fish_module_no_subcommand' -c module -a 'remove-path' --description 'Remove value from environment variable'

complete -c module -s V -l version --description 'Module version'
complete -c module -s D -l debug --description 'Enable debug messages'
complete -c module -s t -l terse --description 'Display output in terse format'
complete -c module -s l -l long --description 'Display output in long format'
complete -c module -s d -l default --description 'Only show default versions available'
complete -c module -s L -l latest --description 'Only show latest versions available'
