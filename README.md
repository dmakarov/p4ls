# P4LS

[![Join the chat at https://gitter.im/p4ls/Lobby](https://badges.gitter.im/p4ls/Lobby.svg)](https://gitter.im/p4ls/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/dmakarov/p4ls.svg?branch=master)](https://travis-ci.org/dmakarov/p4ls)

P4LS is an implementation of an [LSP](https://microsoft.github.io/language-server-protocol/specification)
server for the [P4](https://p4.org/) programming language.

Build
=====

```
$ git clone https://github.com/dmakarov/p4ls.git
$ cd p4ls
$ git submodule update --init --recursive
$ mkdir -p build/ninja/release
$ cd build/ninja/release
$ cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<path where to install> -DENABLE_GC=OFF -DENABLE_BMV2=OFF -DENABLE_EBPF=OFF -DENABLE_GTESTS=OFF -DENABLE_P4C_GRAPHS=OFF -DENABLE_P4TEST=OFF <path to repository p4ls>
$ cmake --build . --target install
```
