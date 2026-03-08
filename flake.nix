{
  description = "Raylib C/C++ Dev Environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forEachSystem = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      packages = forEachSystem (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
        in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "raylib-shooter";
            version = "0.1.0";
            src = ./.;

            nativeBuildInputs = with pkgs; [
              pkg-config
              gnumake
            ];

            buildInputs = with pkgs; [
              raylib
            ] ++ pkgs.lib.optionals pkgs.stdenv.isLinux [
              libGL
              libX11
              libXrandr
              libXi
              libXcursor
              libXinerama
            ];

            buildPhase = ''
              make
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp bin/dungeon_game $out/bin/raylib-shooter
            '';
          };
        });

      apps = forEachSystem (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/raylib-shooter";
        };
      });

      devShells = forEachSystem (
        system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.default ];
            packages = with pkgs; [
              gcc
							clang-tools
              gdb
              tiled
              zsh
            ];
            shellHook = ''
              export SHELL=${pkgs.zsh}/bin/zsh
              echo "Raylib dev shell loaded."
            '';
          };
        }
      );
    };
}
