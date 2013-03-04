#-----------------------------------------------------------------------------
#   Default user .login
#     (last changed Sept 5, 1996)
#
#   This is an important setup file for your account. If you decide to change 
#   this file, keep a working copy until you are certain that your changes 
#   work.
#

stty erase ^H kill ^U intr ^C susp ^Z

# Try and fix some key bindings.
if ( `uname` == "IRIX") then
  stty dec
endif

