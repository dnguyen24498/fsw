{ pkgs }: {
  deps = [
    pkgs.file
    pkgs.valgrind
    pkgs.libselinux.bin
    pkgs.gdb
    pkgs.nano
    pkgs.minicom
    pkgs.socat
    pkgs.screen
  ];
}