# P4lang

Support for P4 editing in Eclipse IDE.

P4lang is a P4 development plugin for the Eclipse IDE. Both [issue
reports](https://github.com/dmakarov/p4ls/issues) and [pull
requests](https://github.com/dmakarov/p4ls/pulls) are greatly
appreciated.

![Screenshot](images/editor.png "Screenshot of P4lang editor")

## Installation
Refer to our [Installation Guide](documentation/Installation.md)

## Prerequisites

The p4lsd commands are required for accessing the language server and
performing most tasks.  Go into the P4lang preferences and either
install the commands or input their paths if not automatically found.

## Contributing
Refer to our [Contributing Guide](CONTRIBUTING.md)

## Concept

P4lang uses the
[lsp4e](https://projects.eclipse.org/projects/technology.lsp4e)
project to integrate with the [P4 Language
Server](https://github.com/dmakarov/p4ls) and
[TM4E](https://projects.eclipse.org/projects/technology.tm4e) project
to provide syntax highlighting in order to provide a rich P4 editor in
the Eclipse IDE.
