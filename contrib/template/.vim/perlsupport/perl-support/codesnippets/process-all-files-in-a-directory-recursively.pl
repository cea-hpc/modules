
use File::Find;                   # File::Find - Traverse a directory tree

my  @directory_list   = ( '.' );  # directory_list used by File::Find::find()

my  $files_processed  = 0;        # counts the files processed by process_file()

#-----------------------------------------------------------------------
# Process a single file in a directory
#-----------------------------------------------------------------------
sub process_file {
  my  $filename      = $_;                # filename without directory
  my  $filename_full = $File::Find::name; # filename with    directory
  my  $directory     = $File::Find::dir;  # directory only

  # print "$directory  :  $filename  :  $filename_full\n";

  $files_processed++;

  return ;
} # ----------  end of subroutine process_file  ----------


#-----------------------------------------------------------------------
#  Process all files in a directory recursively
#-----------------------------------------------------------------------
find( \&process_file, @directory_list );

print "\nfiles processed : $files_processed\n";

