Brief overview of whats here:
	template: contains a template user we use at the University of
		Minnesota.  It has initalization files for both bash
		and tcsh.

	modulefiles: contains the basic modulefiles we use at the University
		of Minnesota, for our template user.  These are not being
		kept upto date, but they are some simple examples to get
		you started.

	scripts:
		createmodule.sh A utliity to help in generating a module file.
			It attempts to take a .file and duplicate what it does
			in modules own language.

		resetenv	A utility we use to reset a users environment
			to match our template user.
		
		resetgnome	A utility we use to reset a users gnome
			settings similar to resetenv.
	
