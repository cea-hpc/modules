
my $filecontent	= do{
	local  $/  = undef;                 # input record separator undefined
	<$INFILE>
};
