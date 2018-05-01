## Contributing to P4LS

### Reporting Issues
Issue reports and feature requests are always appreciated and are made
in the [Issues tab](https://github.com/dmakarov/p4ls/issues)

### Working on P4lang

For setting up P4lang for testing and development follow the [Using
Github Installation
Instructions](documentation/Installation.md#using-github)

### Making Pull Requests

To keep the commit history clean and navigatable, PRs are limited to a
single commit.  If your PR has multiple commits that all work
together, they should be squashed into a single commit.  If you
believe that the different commits are too unrelated to be squashed
together, then they should be put into multiple PRs to allow single
feature PRs.

P4lang is an Eclipse project and all contributors must do the following before having a pull request merged:
 - Sign the Eclipse Contributor Agreement (ECA)
	- Create an Eclipse Account: https://accounts.eclipse.org/user/register
	- Sign the ECA: https://www.eclipse.org/contribute/cla
 - Add the Signed-off-by footer to your commits
	- Footer of all commits should contain: `Signed-off-by: @{name} <@{email}>`, see [here for an example commit](https://github.com/dmakarov/p4ls/commit/09f4fa5d771bca3de6f4e5454ad324a517fc42bf)
	- Either add manually or use `git commit -s`
