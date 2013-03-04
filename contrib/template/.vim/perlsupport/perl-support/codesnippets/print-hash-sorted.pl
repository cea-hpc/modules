
#----------------------------------------------------------------------
#  subroutine : print_hash_sorted
#----------------------------------------------------------------------
sub print_hash_sorted {
  my  $hashref  = shift;      # 1. parameter : hash reference
  print "\n";
  foreach my $key ( sort keys %$hashref ) {
    print "'$key'\t=>\t'$$hashref{$key}'\n";
  }       # -----  end foreach  -----
} # ----------  end of subroutine print_hash_sorted  ----------

