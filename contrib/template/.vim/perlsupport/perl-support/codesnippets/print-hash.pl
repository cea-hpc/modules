
#----------------------------------------------------------------------
#  subroutine : print_hash
#----------------------------------------------------------------------
sub print_hash {
  my  $hashref  = shift;      # 1. parameter : hash reference
  print "\n";
  while ( my ( $key, $value ) = each %$hashref ) {
    print "'$key'\t=>\t'$value'\n";
  }       # -----  end while  -----
} # ----------  end of subroutine print_hash  ----------

