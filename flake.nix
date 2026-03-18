{
  description = "Dungeon project dev environment with Raylib 5.5, raytmx, and hoxml";

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

      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };

          myRaylib = pkgs.raylib.overrideAttrs (oldAttrs: {
            version = "5.5";
            src = pkgs.fetchFromGitHub {
              owner = "raysan5";
              repo = "raylib";
              rev = "c1ab645ca298a2801097931d1079b10ff7eb9df8";
              hash = "sha256-J99i4z4JF7d6mJNuJIB0rHNDhXJ5AEkG0eBvvuBLHrY=";
            };
          });

          raytmx = pkgs.stdenv.mkDerivation {
            pname = "raytmx";
            version = "d4e09bc";
            src = pkgs.fetchFromGitHub {
              owner = "luphi";
              repo = "raytmx";
              rev = "d4e09bcfa7a2fb553853b15b25f0fa74f6cb8912";
              hash = "sha256-uahAiQ2HucRV86/ZOoYw03mhd8hLVcGPmheG5cy44oU=";
            };
            installPhase = ''
              mkdir -p $out/include
              cp raytmx.h $out/include/
            '';
          };

          hoxml = pkgs.stdenv.mkDerivation {
            pname = "hoxml";
            version = "12938da";
            src = pkgs.fetchFromGitHub {
              owner = "luphi";
              repo = "hoxml";
              rev = "12938da4ce3217ce7986acfc7745744acb7c5dfb";
              hash = "sha256-h5fouCX0+GRtEnJ5KlaIyDTj5I0oFmlUj8NsvjKzGho=";
            };
            installPhase = ''
              mkdir -p $out/include
              cp hoxml.h $out/include/
            '';
          };

        in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "dungeon";
            version = "0.1.0";

            src = ./.;

            buildInputs = [
              myRaylib
              raytmx
              hoxml
            ];

            nativeBuildInputs = [
              pkgs.pkg-config
              pkgs.cmake
            ];

            cmakeFlags = [ "-DUSE_NIX=ON" ];
          };
        }
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.default ];
          };
        }
      );

      apps = forAllSystems (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/dungeon_game";
        };
      });
    };
}
