# Distributed under the terms of the GNU General Public License v2
#
# Gentoo build script
#	contributed by:	Peter Fischer <peter.fischer@terrasel.de>
#		date:	02/09/2007

inherit eutils

DESCRIPTION=" The Environment Modules package provides for the dynamic modification of a user's environment via modulefiles."
HOMEPAGE="http://modules.sourceforge.net/"
SRC_URI="mirror://sourceforge/modules/${P}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86"
IUSE="X static"
DEPEND="X? ( virtual/x11 )"

src_compile() {
	local myconf

	if use X ; then
		myconf="${myconf} --with-x"
	fi

	if use static ; then
		myconf="${myconf} --with-static"
	fi

	econf "${myconf}" || die "econf failed"

	emake -j1 || die "emake failed"
}

src_install() {
	# This is the preferred way to install.
	make -j1 DESTDIR=${D} install || die

#	einstall -j1 || die "einstall failed"
}
