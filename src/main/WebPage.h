const char* htmlPage = 
R"rawliteral(
    <!DOCTYPE html>
<html>
<head>
  <meta charset='UTF-8'>
  <title>Outside Panel</title>
  <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no'>
  <style>
    :root {
      --bg-color: #121212;
      --text-color: #fff;
      --status-ok-bg: #003d1a;
      --status-ok-color: #00e676;
      --status-alert-bg: #330b00;
      --status-alert-color: #ff3d00;
      --btn-bg: #00c853;
      --btn-hover-bg: #00e676;
      --btn-active-bg: #00a843;
      --input-bg: #222;
      --input-focus-bg: #333;
      --notif-bg: #ff3d00;
      --notif-color: #fff;
    }

    body.light {
      --bg-color: #ffffff;
      --text-color: #000000;
      --status-ok-bg: #c8fdd9;
      --status-ok-color: #007d3f;
      --status-alert-bg: #ffd1c1;
      --status-alert-color: #b00000;
      --btn-bg: #00c853;
      --btn-hover-bg: #00e676;
      --btn-active-bg: #00a843;
      --input-bg: #eee;
      --input-focus-bg: #ddd;
      --notif-bg: #b00000;
      --notif-color: #fff;
    }

    body {
      background: var(--bg-color);
      color: var(--text-color);
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      text-align: center;
      padding: 30px 15px;
      margin: 0;
      display: flex;
      flex-direction: column;
      min-height: 100vh;
      justify-content: center;
      transition: background 0.3s, color 0.3s;
    }

    h1 {
      font-size: 2.2em;
      margin-bottom: 30px;
      user-select: none;
    }

    .status {
      font-size: 1.6em;
      margin: 20px auto 30px;
      max-width: 320px;
      padding: 15px 25px;
      border-radius: 12px;
      background: var(--input-bg);
      box-shadow: 0 0 15px rgba(0,0,0,0.5);
      user-select: none;
      transition: background-color 0.3s ease, color 0.3s ease;
    }

    .ok {
      color: var(--status-ok-color);
      background-color: var(--status-ok-bg);
      box-shadow: 0 0 15px var(--status-ok-color);
    }

    .alert {
      color: var(--status-alert-color);
      background-color: var(--status-alert-bg);
      box-shadow: 0 0 15px var(--status-alert-color);
    }

    .controls {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 15px;
      width: 100%;
      max-width: 320px;
      margin: 0 auto 20px;
      position: relative;
    }

    .power-button {
      width: 100px;
      height: 100px;
      border-radius: 50%;
      font-size: 2.5em;
      background: var(--btn-bg);
      color: white;
      border: none;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: center;
      transition: background 0.3s ease;
      box-shadow: 0 0 10px rgba(0, 200, 83, 0.5);
      position: relative;
      z-index: 1;
    }

    .power-button:hover {
      background: var(--btn-hover-bg);
    }

    .power-button:active {
      background: var(--btn-active-bg);
    }

    .theme-toggle {
      display: flex;
      align-items: center;
      gap: 10px;
      margin-top: 25px;
      justify-content: center;
    }

    input[type="password"] {
      padding: 10px;
      font-size: 1em;
      border-radius: 8px;
      border: none;
      background: var(--input-bg);
      color: var(--text-color);
      width: 75%;          
      text-align: center;
    }

    .password-btn {
      padding: 10px 20px;
      font-size: 1em;
      background: var(--btn-bg);
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
    }

    .password-btn:hover {
      background: var(--btn-hover-bg);
    }

    #notification {
      position: absolute;
      top: 110px;
      left: 50%;
      transform: translateX(-50%);
      background-color: var(--notif-bg);
      color: var(--notif-color);
      padding: 10px 20px;
      border-radius: 20px;
      font-weight: 600;
      opacity: 0;
      pointer-events: none;
      box-shadow: 0 0 15px var(--notif-bg);
      user-select: none;
      transition: opacity 0.4s ease;
      white-space: nowrap;
      z-index: 10;
    }

    #notification.show {
      opacity: 1;
      pointer-events: auto;
    }

    @media (max-width: 350px) {
      h1 {
        font-size: 1.8em;
      }
      .status {
        font-size: 1.3em;
        padding: 12px 18px;
      }
      .power-button {
        width: 80px;
        height: 80px;
        font-size: 2em;
      }
      #notification {
        top: 95px;
        font-size: 0.9em;
        padding: 8px 16px;
      }
    }
  </style>
</head>
<body>
  <h1>Server outside panel</h1>
  <div id="status" class="status">Awaiting your action</div>

  <div class="controls">
    <button class="power-button" onclick="toggleSystem()">⏻</button>
    <div id="notification"></div>

    <input id="Password" type="password" maxlength="20" placeholder="Password">
  </div>

  <div class="theme-toggle">
    <label for="themeSwitch">🌞 / 🌙</label>
    <input type="checkbox" id="themeSwitch" onchange="toggleTheme()">
  </div>

  <script>
    let ws = new WebSocket('ws://' + location.host + '/ws');

    ws.onmessage = function(event) {
      let status = document.getElementById('status');
      status.textContent = event.data;
      status.className = 'status ' + (event.data.includes('ALARM') ? 'alert' : 'ok');
    };

    window.onload = function() {
      fetch("/loaded")
        .then(response => ws.send("load"));
      };

    function toggleSystem() {
      const passwordInput = document.getElementById('Password');
      const notification = document.getElementById('notification');

      if (passwordInput.value.length !== 20) {
        showNotification('password only 20 chars');
        return;
      }

      // Очистить уведомление, если оно было
      notification.classList.remove('show');
      notification.textContent = '';

      let pswd = passwordInput.value
      switch(getRandomInt(0,2)) {
        case 0:
          ws.send(xorEncryptDecrypt('tog' + randomAlphaNumString(17), pswd));
          break;
        case 1:
          ws.send(xorEncryptDecrypt(randomAlphaNumString(17) + 'tog', pswd));
          break;
        case 2:
          ws.send(xorEncryptDecrypt(randomAlphaNumString(9) + 'tog' + randomAlphaNumString(8), pswd));
          break;
      }
    }

    function showNotification(message) {
      const notification = document.getElementById('notification');
      notification.textContent = message;
      notification.classList.add('show');

      // Очистка старого таймаута, если он есть
      if (notification._timeoutId) {
        clearTimeout(notification._timeoutId);
      }

      notification._timeoutId = setTimeout(() => {
        notification.classList.remove('show');
      }, 2000);
    }

    function toggleTheme() {
      document.body.classList.toggle('light');
    }

    function xorEncryptDecrypt(input, key) {
      if (input.length !== key.length) {
        throw new Error("Input and key must be the same length");
      }

      let output = '';
      for (let i = 0; i < input.length; i++) {
        const charCode = input.charCodeAt(i) ^ key.charCodeAt(i); 
        output += String.fromCharCode(charCode);
      }
      return output;
    }


    function randomAlphaNumString(length) {
      const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890';
      let result = '';
      for (let i = 0; i < length; i++) {
        result += chars.charAt(Math.floor(Math.random() * chars.length));
      }
      return result;
    }

    function getRandomInt(min, max) {
      min = Math.ceil(min);
      max = Math.floor(max);
      return Math.floor(Math.random() * (max - min + 1)) + min;
    }
  </script>
</body>
</html>
)rawliteral";