# =========================================================
# Author: Tom Payerle <payerle@umd.edu>

# This file is a collection of common utilities (Tcl Procedures)
# used by the the cookbook "Compiler, etc. Dependencies"
# Particularly for the homebrewed flavors and modulerc flavors strategy

# In production, all of these utilities would likely be best put in
# a site configuration file and registered, as per the cookbook
# "Expose procedures and variables to modulefiles"
#
# In lieu of that (e.g. modulecmd version < 4.2.4), at least put in
# a single file.
#
# We only split up the files to make for better displaying in the
# cookbook.
#
# This file assumes the Tcl variable $tcllibdir has been set and
# points to the location of this directory.

source $tcllibdir/WarningUtils.tcl
source $tcllibdir/LoadedModules.tcl
source $tcllibdir/CompilerUtils.tcl
source $tcllibdir/MpiUtils.tcl
source $tcllibdir/ChildModules.tcl
source $tcllibdir/GetFamVer.tcl
source $tcllibdir/LoadedCompMatches.tcl
source $tcllibdir/LoadedMpiMatches.tcl
