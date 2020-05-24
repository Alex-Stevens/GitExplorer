# GitExplorer #

A Windows Shell Namespace Extension to browse GitHub repositories.

Presently, this project is simply an ATL-ised version of Microsoft's [sample](https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/Win7Samples/winui/shell/shellextensibility/explorerdataprovider) shell namespace extension.

I created it to preserve what I had learnt from previous work with the Windows Shell.

Things added:
 - Refactored to use ATL.
 - Optional ribbon support.
 - Custom icon handler.
 - Support for properties which populate the details pane, contents view, etc.
 - [HTTP client](https://github.com/microsoft/cpprestsdk) for making requests to the GitHub API.

<p align="center">
  <img src="https://raw.githubusercontent.com/Alex-Stevens/GitExplorer/master/docs/demo.gif" width="95%"/>
</p>