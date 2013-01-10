Submitted by tbennett@nvidia.com, Nov 18, 2004


How to init emacs lisp modules from .emacs:

    ;; load and init emacs lisp modules if not already loaded...
    (if (and (getenv "MODULESHOME")
             (file-directory-p (getenv "MODULESHOME"))
             (not (fboundp 'module)))
        (progn
          (if (load-file (concat (getenv "MODULESHOME") "/init/lisp"))
              (define-key global-map "\C-cm" 'Modules-module))))

After this, from within your emacs process you can run any module
command, eg:

    C-c m load modules
    C-c m list
    etc.


What follows describes an optional usage.

Since I use env variables frequently within emacs (mostly to
specify filename paths) and also use shell-mode, I want to have
all modules commands run within both emacs and in my *shell*
buffer.  The following works for me, but may need tweaking in
other environments...

    ;; send a cmd to my shell
    ;; not very flexible, knows about my setup, shell, etc.
    ;; cribbed from switch-to-shell.el

    (defvar my-shell-command-shell-buffer-name "*shell*"
      "The name of the shell buffer.")

    (defun my-shell-command (command)
      (interactive "sCommand: ")
      ;; Find shell buffer
      (and (get-buffer my-shell-command-shell-buffer-name)
           (pop-to-buffer my-shell-command-shell-buffer-name)
           (get-buffer-process (current-buffer))
           (progn
             (goto-char (point-max))
             (comint-kill-input)          ;; delete any junk on line
             (insert-string command)
             (comint-send-input))))

    ;; send the same module cmd to both emacs and my shell buffer
    (defun m-doit (cmd)
      (and (Modules-module cmd)
           (my-shell-command (concat "module " cmd))))

    ;; a sample module command using above
    (defun m-projects-A ()
       (interactive)
       (m-doit "unload projects") 
       (m-doit "load projects/A"))
    (defun m-projects-B ()
       (interactive)
       (m-doit "unload projects") 
       (m-doit "load projects/B"))

    Then use

        M-x m-projects-A <RET>

    to switch to project-A setup.
