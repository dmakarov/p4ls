# P4LS

[![Join the chat at https://gitter.im/p4ls/Lobby](https://badges.gitter.im/p4ls/Lobby.svg)](https://gitter.im/p4ls/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Build Status](https://travis-ci.org/dmakarov/p4ls.svg?branch=master)](https://travis-ci.org/dmakarov/p4ls)

P4LS is an implementation of an [LSP](https://microsoft.github.io/language-server-protocol/specification)
server for the [P4](https://p4.org/) programming language.

Install
=======

MacOS
-----

Prebuilt binary packages for installing `p4lsd` server are available
for MacOS.  To install the homebrew package use the following command

``` sh
brew install dmakarov/homebrew-p4/p4ls
```


Build
=====

``` sh
brew install bison gmp protobuf
git clone https://github.com/dmakarov/p4ls.git
cd p4ls
git submodule update --init --recursive
mkdir -p build/ninja/release
cd build/ninja/release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_GC=OFF -DENABLE_BMV2=OFF -DENABLE_EBPF=OFF -DENABLE_GTESTS=OFF -DENABLE_P4C_GRAPHS=OFF -DENABLE_P4TEST=OFF <path to p4ls repository>
cmake --build . --target install
```

Installing Eclipse P4lang plugin
================================

In addition to the P4 language server, the repository includes two LSP
clients, one for Eclipse and another for Emacs.  To build and install
the Eclipse P4lang LSP client plugin follow these steps

``` sh
cd clients/eclipse
mvn clean verify
```

If maven finished running without any error there should have been
created a subdirectory `repository/target/repository` in
`clients/eclipse` directory that contains the plugin jar file along
with the required dependencies.  To install the plugin in Eclipse

1. open the Eclipse menu _Help_ >> _Install New Software..._
2. Type or paste the absolute path to `repository/target/repository`
   directory,
   e.g. `/Users/dmakarov/work/try/p4ls/clients/eclipse/repository/target/repository`
   in the _Work with:_ text entry field and click _Add..._ button.
3. Check _Dependencies_ and _P4lang: P4 editing in Eclipse IDE_
   checkboxes and finish installing.

After restarting the Eclipse a new P4 perspective should be available
via _Window_ >> _Perspective_ >> _Open Perspective_ >> _Other..._
menu.  If `p4lsd` is in one of the known paths, p4lang plugin should
be able to find it, otherwise set the path to the p4lsd executable in
P4 _Preferences_.

`P4lsd` searches for `compile_commands.json` in subdirectory hierarchy
for each P4 source file it is processing.  If you don't use cmake or
similar tool to build your P4 project, you can create
`compile_commands.json` file manually in an editor. For each file you
want p4lsd to analyze there should be an entry like the following

``` json
[
{
  "directory": "/Users/dmakarov/work/try/p4ls/test",
  "command": "/Users/dmakarov/work/try/opt/bin/p4c /Users/dmakarov/work/try/p4ls/test/main.p4",
  "file": "/Users/dmakarov/work/try/p4ls/test/main.p4"
}
]
```
