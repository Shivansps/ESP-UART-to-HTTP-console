const char* indexHtml PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP UART HTTP Console</title>
  <style>
    body { font-family: monospace; background: #111; color: #0f0; }
    #log { background: black; height: 80vh; overflow-y: auto; padding: 10px; border: 1px solid #444; }
    input {width: 70vw;}, select, button { font-size: 1em; }
  </style>
</head>
<body>
  <h2>ESP UART HTTP Console</h2>
  <pre id="log"></pre>
  <input id="input" placeholder="Send..." onkeydown="if(event.key==='Enter') send()">
  <button onclick="send()">Send</button>
  <hr>
  <label for="baud">Baudrate: </label>
  <select id="baud">
    <option value="9600">9600</option>
    <option value="19200">19200</option>
    <option value="38400">38400</option>
    <option value="57600">57600</option>
    <option value="115200" selected>115200</option>
    <option value="230400">230400</option>
    <option value="460800">460800</option>
  </select>
  <button onclick="applyBaud()">Apply</button>
  <button onclick="saveBaud()">Save</button>
  <script>
    const log = document.getElementById('log');
    const input = document.getElementById('input');
    let ws;

    function connect() {
      ws = new WebSocket(`ws://${location.host}/ws`);
      ws.onmessage = (event) => {
        log.textContent += event.data;
        log.scrollTop = log.scrollHeight;
      };

      ws.onopen = (event) => {
        log.textContent += ">= CONNECTED\n\r";
        log.scrollTop = log.scrollHeight;
      };

      ws.onclose = (event) => {
        log.textContent += ">= DISCONNECTED\n\r";
        log.textContent += ">= RECONECTING\n\r";
        log.scrollTop = log.scrollHeight;
        setTimeout(function() {
          connect();
          getOldMsgs();
        }, 1000);
      };
    }

    function send() {
      ws.send(input.value + "\r\n");
      log.textContent += `=> ${input.value}\r\n`;
      input.value = "";
    }

    function applyBaud() {
      const val = document.getElementById('baud').value;
      const msg = `=> Changing baudrate to ${val}\r\n`;
      log.textContent += msg;
      ws.send(msg);
      fetch(`/setBaud?baud=${val}`);
    }

    function saveBaud() {
      fetch('/saveBaud').then(() => log.textContent += "=> Baudrate saved!\r\n");
    }

    window.onload = () => {
      connect();
      fetch("/getBaud")
        .then(r => r.text())
        .then(baud => {
          const select = document.getElementById('baud');
          if ([...select.options].some(opt => opt.value === baud)) {
            select.value = baud;
          }
        });

      getOldMsgs();
    };

    function getOldMsgs(){
      fetch("/getStored")
        .then(r => r.text())
        .then(data => {
          if(data.length > 0)
          {
            log.textContent += data + "\r\n";
            log.scrollTop = log.scrollHeight;
          }
        });
    }
  </script>
</body>
</html>
)=====";