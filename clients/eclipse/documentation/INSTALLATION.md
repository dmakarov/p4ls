## Installing P4lang

### Using Eclipse Marketplace
Download [P4lang from the Eclipse Marketplace](https://marketplace.eclipse.org/content/p4lang-editing-eclipse-ide)
For further instructions on how to install using Eclipse Marketplace,
see [their tutorial](https://marketplace.eclipse.org/marketplace-client-intro?mpc_install=3835145)

### Using p2 repository
The p2 site: http://download.eclipse.org/p4lang/snapshots/
 - Open Eclipse IDE
 - Open the Install New Software Wizard (Under the Help menu)
 - Enter http://download.eclipse.org/p4lang/snapshots/ in the site field
 - Select P4lang in Eclipse IDE and click Next
 - Wait for the dependencies to load
 - Press Next again
 - Accept the license and press Finish
 - Restart Eclipse

### Using Github
##### For contributors and testers, this will allow running the plugin within a child Eclipse to test and develop new features
 - Download [Eclipse for Eclipse Contributors](https://www.eclipse.org/downloads/packages/eclipse-ide-eclipse-committers/oxygen2)
   or any version of Eclipse with the
   `Eclipse Plug-in Development Environment` package
 - Clone the repo: https://github.com/dmakarov/p4ls
 - In the clients/eclipse subdirectory of the repository,
   run `mvn clean verify`
   (You will need [Maven](http://maven.apache.org/))

For running P4lang in a child Eclipse instance:
 - Open the following projects in Eclipse:
   - org.eclipse.p4lang
   - target-platform
   - org.eclipse.p4lang.tests (If you intend on contributing)
 - Set the Target Platform
   - Preferences > Plug-in Development > Target Platform
   - Select the `p4lang` target defininition
 - Run the `org.eclipse.p4lang` project as an `Eclipse Application`

For using a local p2 repository:
 - Follow the "Using p2 repository" above
 - Use `/{path_to_repository}/p4lang/repository/target/repository` as the p2 site
