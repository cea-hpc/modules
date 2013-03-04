
sub new {
    my  $class  = shift;
    my  $self   = {
        _name1  => $_[0],
        _name2  => $_[1],
    };
    bless( $self, $class );
    return $self;
}   # ----------  end of subroutine new  ----------
