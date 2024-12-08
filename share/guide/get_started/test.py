import os
exec(open('/usr/local/Modules/init/python.py').read())
module('load', 'null')
if module('is-loaded', 'null'):
    print 'modulefile \'null\' is loaded'
