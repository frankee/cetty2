#include "WebSocketServerIndexPage.h"


const char* WebSocketServerIndexPage::precontent =

"<html><head><title>Web Socket Test</title></head>" "\r\n"
"<body>" "\r\n"
"<script type=\"text/javascript\">" "\r\n"
"var socket;" "\r\n"
"if (window.WebSocket) {" "\r\n"
"  socket = new WebSocket(\"";

const char* WebSocketServerIndexPage::postcontent =
"\");" "\r\n"
"  socket.onmessage = function(event) { alert(event.data); };" "\r\n"
"  socket.onopen = function(event) { alert(\"Web Socket opened!\"); };" "\r\n"
"  socket.onclose = function(event) { alert(\"Web Socket closed.\"); };" "\r\n"
"} else {" "\r\n"
"  alert(\"Your browser does not support Web Socket.\");" "\r\n"
"}" "\r\n"
"" "\r\n"
"function send(message) {" "\r\n"
"  if (!window.WebSocket) { return; }" "\r\n"
"  if (socket.readyState == WebSocket.OPEN) {" "\r\n"
"    socket.send(message);" "\r\n"
"  } else {" "\r\n"
"    alert(\"The socket is not open.\");" "\r\n"
"  }" "\r\n"
"}" "\r\n"
"</script>" "\r\n"
"<form onsubmit=\"return false;\">" "\r\n"
"<input type=\"text\" name=\"message\" value=\"Hello, World!\"/>" "\r\n"
"<input type=\"button\" value=\"Send Web Socket Data\" onclick=\"send(this.form.message.value)\" />" "\r\n"
"</form>" "\r\n"
"</body>" "\r\n"
"</html>" "\r\n";