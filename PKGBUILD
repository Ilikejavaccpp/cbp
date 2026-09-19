# Maintainer: Ilikejavaccpp <genomeacc2@gmail.com>
pkgname=cbp
pkgver=1.0.0
pkgrel=1
pkgdesc="A simple way to build C/C++ projects via C++ without CMake or any DSL"
arch=('x86_64' 'any')
url="https://github.com/Ilikejavaccpp/cbp"
license=('GPL')
depends=('clang' 'make')
source=("$pkgname-$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
    cd "$srcdir/$pkgname-$pkgver"
    # Compile the build script to produce the bpp bootstrap CLI
    clang++ -std=c++17 -O3 backend/binit/build.cpp -o bpp
    # ./bin/bpp build backend/binit/build.cpp
    # Generate the project cbp builder if build setup permits
    # ./bpp build || true
}

package() {
    cd "$srcdir/$pkgname-$pkgver"

    # Install binaries to /usr/bin
    install -Dm755 bpp "$pkgdir/usr/bin/bpp"
    if [ -f "cbp" ]; then
        install -Dm755 cbp "$pkgdir/usr/bin/cbp"
    fi

    # Install library headers and source files for libcbp integration
    install -d "$pkgdir/usr/include/cbp/src"
    if [ -f "src/cbp.hpp" ]; then
        install -Dm644 src/cbp.hpp "$pkgdir/usr/include/cbp/src/cbp.hpp"
    fi
    if [ -f "src/cbp.cpp" ]; then
        install -Dm644 src/cbp.cpp "$pkgdir/usr/include/cbp/src/cbp.cpp"
    fi
}
