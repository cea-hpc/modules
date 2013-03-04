#
# Devel::SmallProf : variables which can be used to affect what gets profiled.
#
use Devel::SmallProf;

$DB::drop_zeros  = 0;            # Do not show lines which were never called: 1
$DB::grep_format = 0;            # Output on a format similar to grep : 1
$DB::profile     = 1;            # Turn off profiling for a time: 0
%DB::packages    = ('main'=>1);  # Only profile code in a certain package.

