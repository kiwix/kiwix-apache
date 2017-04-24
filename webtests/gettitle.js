/*
Another example based on phantomjs Getting Started.
*/

var url;
var page = require('webpage').create();
page.onConsoleMessage = function(msg) {
  console.log('Page title is ' + msg);
};

url = 'http://localhost/kiwix/';

page.open(url, function(status) {
  page.evaluate(function() {
    console.log(document.title);
  });
  phantom.exit();
});

