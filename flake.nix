{
  description = "Squall Dev Shell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    devShells.${system}.default = pkgs.mkShell rec {
      buildInputs = with pkgs; [
        libGL
        libGL.dev
        libx11
        libxi
        libxkbcommon
        wayland
        wayland-protocols
        libffi
      ];

      nativeBuildInputs = with pkgs; [
        clang-tools
        cmake
        pkg-config
        wayland-scanner
        ninja

        emscripten
      ];

      LD_LIBRARY_PATH = "${pkgs.stdenv.cc.cc.lib}/lib/:${pkgs.lib.makeLibraryPath buildInputs}:$NIX_LD_LIBRARY_PATH";
    };
  };
}
