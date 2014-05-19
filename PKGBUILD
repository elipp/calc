#epon 

pkgname=elias_calc
pkgver=0.1.1
pkgrel=1
pkgdesc="calc: command-line calculator"
url="https://github.com/elipp/calc"
arch=('x86_64')
depends=('gmp' 'mpfr')
license=(custom)

source=("$pkgname"::'git://github.com/elipp/calc.git')

md5sums=('SKIP')

build() {
	cd "$srcdir/$pkgname"
	make
}

package() {
    cd "$srcdir/$pkgname"
    install -D -m755 "calc" "${pkgdir}/usr/local/bin/calc"
}
