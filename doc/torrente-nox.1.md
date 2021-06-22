% QBITTORRENT-NOX(1) Command line Bittorrent client written in C++ / Qt
% Christophe Dumez <chris@torrente.org>
% January 16th 2010

# NAME
Torrente-nox - a command line Bittorrent client written in C++ / Qt


# SYNOPSIS
**torrente-nox** `[--d|--daemon] [--webui-port=x] [TORRENT_FILE | URL]...`

**torrente-nox** `--help`

**torrente-nox** `--version`


# DESCRIPTION
**Torrente-nox** is an advanced command-line Bittorrent client written in C++ / Qt
using the **libtorrent-rasterbar** library by Arvid Norberg.
Torrente-nox aims to be a good alternative to other command line bittorrent
clients and provides features similar to popular graphical clients.

Torrente-nox is fast, stable, light and it supports unicode. It also comes with
UPnP port forwarding / NAT-PMP, encryption (Vuze compatible), FAST extension (mainline)
and PeX support (utorrent compatible).

Torrente-nox is meant to be controlled via its feature-rich Web UI which is
accessible as a default on http://localhost:8080. The Web UI access is secured and
the default account user name is "admin" with "adminadmin" as a password.


# OPTIONS
**`--help`** Prints the command line options.

**`--version`** Prints torrente program version number.

**`--webui-port=x`** Changes Web UI port to x (default: 8080).


# BUGS
If you find a bug, please report it at http://bugs.torrente.org
