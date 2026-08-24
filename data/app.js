const log = (t) => {
  const d = document.getElementById('log');
  const p = document.createElement('div');
  p.textContent = t;
  d.prepend(p);
};

const statusEl = document.getElementById('status');

const start = async () => {
  const host = "127.0.0.1";
  const port = 8080;
  const urlHttp = `http://${host}:${port}/`;
  const urlWs = `ws://${host}:${port}/ws`;

  statusEl.textContent = `Connecting to ${urlWs}`;

  // Setup WebSocket
  let ws;
  try {
    ws = new WebSocket(urlWs);
    ws.onopen = () => {
      statusEl.textContent = "WebSocket connected";
      log("[ws] connected");
    };
    ws.onmessage = (ev) => {
      log("[ws] recv: " + ev.data);
    };
    ws.onclose = () => {
      statusEl.textContent = "WebSocket closed";
      log("[ws] closed");
    };
    ws.onerror = (e) => {
      statusEl.textContent = "WebSocket error";
      log("[ws] error");
    };
  } catch (e) {
    statusEl.textContent = "WebSocket failed";
    log("[ws] open error " + e);
  }

  document.getElementById('btn-send-ws').onclick = () => {
    const text = document.getElementById('ws-msg').value || "hello native";
    if (ws && ws.readyState === WebSocket.OPEN) {
      ws.send(text);
      log("[ws] send: " + text);
    } else {
      log("[ws] not open");
    }
  };

  // POST to native
  document.getElementById('btn-native').onclick = async () => {
    try {
      const resp = await fetch(`${urlHttp}api/native`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ cmd: "ping", when: Date.now() })
      });
      const json = await resp.json();
      log("[api] " + JSON.stringify(json));
    } catch (e) {
      log("[api] error: " + e);
    }
  };

  log("Client ready. If the UI can't connect, ensure the native server is running on the Vita and you opened the browser to " + urlHttp);
};

start();
