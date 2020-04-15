with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "chemilang";
  src = ./.;

  buildInputs = [ cmake clang boost flex bison gnuplot gtest ];

  buildPhase = "cmake . && make && ./bin/tests";

  installPhase = ''
    mkdir -p $out/bin
    cp bin/crnsimul $out/bin/
  '';
}
