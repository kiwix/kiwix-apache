# README for the Web Tests
Web tests enable the testing of the module once it's installed and available.

Currently I am experimenting with several approaches to test automation for the module. My aims include comparing several offerings informally to determine which might help us test both this module and perhaps also kiwix-serve. For now, the tests can reside here - we can consider moving them if they add enough value.

# Approaches

* phantomjs: a lightweight, opensource JavaScript environment that can retrieve web pages directly. Their documentation indicates it's supported in travis-ci which we already use for various kiwix projects, so using phantomjs may align well with overall aspects of how kiwix does CB and automated testing.

# Known unresolved topics

* How to deploy and run the kiwix-apache module. This is performed manually for now. See the main README for more information on how to build and deploy the module.
* How to run the tests and determine successes and failures.

