"""A small Chrome DevTools Protocol client over a local WebSocket.

Standard library only. It speaks just enough of RFC 6455 for the DevTools
endpoint of a Chromium process this tool started itself: one client, text
frames, no extensions. Every read is bounded by a byte count; nothing here
waits on a clock. A Chromium that exits closes the socket, which ends any
read with an error.
"""

from __future__ import annotations

import base64
import json
import os
import socket
import struct
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from urllib.parse import urlsplit

# The largest single message accepted from Chromium. A capture of a corpus
# fixture is a few kilobytes; this bounds a misbehaving peer.
MAXIMUM_MESSAGE_BYTES = 64 * 1024 * 1024
# How many unrelated messages (events) may arrive before a reply. Events
# are only enabled for the page domain, so this is generous.
MAXIMUM_MESSAGES_PER_REPLY = 100_000
# How many stderr lines Chromium may print before its DevTools endpoint.
MAXIMUM_STARTUP_LINES = 10_000

_OPCODE_CONTINUATION = 0x0
_OPCODE_TEXT = 0x1
_OPCODE_CLOSE = 0x8
_OPCODE_PING = 0x9
_OPCODE_PONG = 0xA


class DevToolsError(RuntimeError):
    """A protocol failure, a refused command or a closed connection."""


def _read_exact(connection: socket.socket, count: int) -> bytes:
    received = bytearray()
    while len(received) < count:
        chunk = connection.recv(count - len(received))
        if not chunk:
            raise DevToolsError("Chromium closed the DevTools connection")
        received.extend(chunk)
    return bytes(received)


class WebSocket:
    """One client WebSocket connection, text frames only."""

    def __init__(self, url: str) -> None:
        parts = urlsplit(url)
        if parts.scheme != "ws" or parts.hostname not in ("127.0.0.1", "localhost"):
            raise DevToolsError(f"refusing a DevTools endpoint that is not local: {url}")
        port = parts.port if parts.port is not None else 80
        self._socket = socket.create_connection((parts.hostname, port))
        key = base64.b64encode(os.urandom(16)).decode("ascii")
        path = parts.path or "/"
        request = (
            f"GET {path} HTTP/1.1\r\n"
            f"Host: {parts.hostname}:{port}\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            f"Sec-WebSocket-Key: {key}\r\n"
            "Sec-WebSocket-Version: 13\r\n\r\n"
        )
        self._socket.sendall(request.encode("ascii"))
        header = bytearray()
        while not header.endswith(b"\r\n\r\n"):
            header.extend(_read_exact(self._socket, 1))
            if len(header) > 16 * 1024:
                raise DevToolsError("DevTools handshake reply is too long")
        status_line = bytes(header).split(b"\r\n", 1)[0]
        if b" 101 " not in status_line:
            raise DevToolsError(f"DevTools handshake refused: {status_line!r}")

    def close(self) -> None:
        try:
            self._send_frame(_OPCODE_CLOSE, b"")
        except OSError:
            pass
        self._socket.close()

    def _send_frame(self, opcode: int, payload: bytes) -> None:
        mask = os.urandom(4)
        header = bytearray([0x80 | opcode])
        length = len(payload)
        if length < 126:
            header.append(0x80 | length)
        elif length < 65536:
            header.append(0x80 | 126)
            header.extend(struct.pack(">H", length))
        else:
            header.append(0x80 | 127)
            header.extend(struct.pack(">Q", length))
        header.extend(mask)
        masked = bytearray(payload)
        for index in range(length):
            masked[index] ^= mask[index % 4]
        self._socket.sendall(bytes(header) + bytes(masked))

    def send_text(self, text: str) -> None:
        self._send_frame(_OPCODE_TEXT, text.encode("utf-8"))

    def receive_text(self) -> str:
        message = bytearray()
        while True:
            first, second = _read_exact(self._socket, 2)
            final = bool(first & 0x80)
            opcode = first & 0x0F
            if second & 0x80:
                raise DevToolsError("a server frame must not be masked")
            length = second & 0x7F
            if length == 126:
                length = struct.unpack(">H", _read_exact(self._socket, 2))[0]
            elif length == 127:
                length = struct.unpack(">Q", _read_exact(self._socket, 8))[0]
            if len(message) + length > MAXIMUM_MESSAGE_BYTES:
                raise DevToolsError("DevTools message exceeds the size limit")
            payload = _read_exact(self._socket, length)
            if opcode == _OPCODE_PING:
                self._send_frame(_OPCODE_PONG, payload)
                continue
            if opcode == _OPCODE_PONG:
                continue
            if opcode == _OPCODE_CLOSE:
                raise DevToolsError("Chromium closed the DevTools connection")
            if opcode not in (_OPCODE_TEXT, _OPCODE_CONTINUATION):
                raise DevToolsError(f"unexpected WebSocket opcode {opcode}")
            message.extend(payload)
            if final:
                return message.decode("utf-8")


@dataclass
class Session:
    """A DevTools session attached to one page target (flattened mode)."""

    socket: WebSocket
    session_id: str
    # Shared with the Browser: one id counter per connection.
    next_id: list[int]

    def call(self, method: str, params: dict[str, Any] | None = None) -> dict[str, Any]:
        return _call(self.socket, self.next_id, method, params, self.session_id)

    def wait_for_event(self, method: str) -> dict[str, Any]:
        for _ in range(MAXIMUM_MESSAGES_PER_REPLY):
            message = json.loads(self.socket.receive_text())
            if message.get("method") == method and message.get("sessionId") == self.session_id:
                return message.get("params", {})
        raise DevToolsError(f"no {method} event within the message limit")


def _call(connection: WebSocket, next_id: list[int], method: str,
          params: dict[str, Any] | None, session_id: str | None) -> dict[str, Any]:
    identifier = next_id[0]
    next_id[0] += 1
    request: dict[str, Any] = {"id": identifier, "method": method, "params": params or {}}
    if session_id is not None:
        request["sessionId"] = session_id
    connection.send_text(json.dumps(request))
    for _ in range(MAXIMUM_MESSAGES_PER_REPLY):
        message = json.loads(connection.receive_text())
        if message.get("id") != identifier:
            continue
        if "error" in message:
            raise DevToolsError(f"{method} refused: {message['error']}")
        return message.get("result", {})
    raise DevToolsError(f"no reply to {method} within the message limit")


class Browser:
    """A Chromium headless shell started for one capture run.

    The process gets a fresh profile directory and a DevTools port chosen by
    the system. Its endpoint is read from its own startup line on stderr, so
    no polling or timeout is needed. close() ends the process.
    """

    def __init__(self, executable: Path, profile_directory: Path, extra_arguments: list[str]) -> None:
        command = [
            str(executable),
            "--headless",
            "--disable-gpu",
            "--disable-background-networking",
            "--disable-component-update",
            "--disable-default-apps",
            "--disable-extensions",
            "--disable-sync",
            "--no-first-run",
            "--no-default-browser-check",
            "--hide-scrollbars",
            "--allow-file-access-from-files",
            "--force-color-profile=srgb",
            "--font-render-hinting=none",
            "--remote-debugging-port=0",
            f"--user-data-dir={profile_directory}",
            *extra_arguments,
            "about:blank",
        ]
        self._process = subprocess.Popen(
            command,
            stdin=subprocess.DEVNULL,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
            text=True,
            encoding="utf-8",
            errors="replace",
        )
        endpoint = None
        assert self._process.stderr is not None
        for _ in range(MAXIMUM_STARTUP_LINES):
            line = self._process.stderr.readline()
            if not line:
                break
            marker = "DevTools listening on "
            if line.startswith(marker):
                endpoint = line[len(marker):].strip()
                break
        if endpoint is None:
            self._process.kill()
            self._process.wait()
            raise DevToolsError("Chromium did not report a DevTools endpoint")
        # Chromium keeps writing to stderr; drain it on a thread so a full
        # pipe never blocks it. The thread ends when the process exits.
        self._drain = _StderrDrain(self._process)
        self._drain.start()
        self._socket = WebSocket(endpoint)
        self._next_id = [1]

    def open_page(self) -> Session:
        target = _call(self._socket, self._next_id, "Target.createTarget", {"url": "about:blank"}, None)
        attached = _call(self._socket, self._next_id, "Target.attachToTarget",
                         {"targetId": target["targetId"], "flatten": True}, None)
        return Session(self._socket, attached["sessionId"], self._next_id)

    def close(self) -> None:
        try:
            _call(self._socket, self._next_id, "Browser.close", None, None)
        except (DevToolsError, OSError):
            pass
        try:
            self._socket.close()
        except OSError:
            pass
        if self._process.poll() is None:
            self._process.kill()
        self._process.wait()
        self._drain.join()


class _StderrDrain:
    """Reads and discards Chromium's stderr until the process closes it."""

    def __init__(self, process: subprocess.Popen[str]) -> None:
        import threading

        self._process = process
        self._thread = threading.Thread(target=self._run, name="chromium-stderr", daemon=True)

    def start(self) -> None:
        self._thread.start()

    def join(self) -> None:
        self._thread.join()

    def _run(self) -> None:
        stream = self._process.stderr
        if stream is None:
            return
        for _ in iter(stream.readline, ""):
            pass
