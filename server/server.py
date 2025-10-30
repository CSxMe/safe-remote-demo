#!/usr/bin/env python3
"""
Safe demo server for educational use only.

Protocol:
- All messages are length-prefixed: 4 bytes little-endian unsigned int (message length), then that many bytes of payload (utf-8).
- Client must first send AUTH_TOKEN as a message. Server replies "AUTH_OK" or "AUTH_FAILED".
- Allowed commands:
    LIST
    READ <filename>
    TIME
    QUIT

Run: python3 server.py --port 5000 --auth my_secret_token_123 --sandbox ~/sandbox
"""

import socket
import threading
import struct
import argparse
import pathlib
import os
import datetime
import logging

# ---------- helpers ----------
def recv_exact(conn, n):
    """Receive exactly n bytes or return None if EOF."""
    chunks = []
    got = 0
    while got < n:
        data = conn.recv(n - got)
        if not data:
            return None
        chunks.append(data)
        got += len(data)
    return b''.join(chunks)

def recv_message(conn):
    """Receive length-prefixed message (4 bytes little-endian) and return decoded string, or None."""
    header = recv_exact(conn, 4)
    if not header:
        return None
    (length,) = struct.unpack('<I', header)  # little-endian unsigned int
    if length == 0:
        return ''
    data = recv_exact(conn, length)
    if data is None:
        return None
    return data.decode('utf-8', errors='replace')

def send_message(conn, text):
    """Send text as length-prefixed message."""
    data = text.encode('utf-8')
    header = struct.pack('<I', len(data))
    conn.sendall(header + data)

# ---------- command handlers ----------
MAX_FILE_BYTES = 5 * 1024 * 1024  # 5 MB

def handle_LIST(sandbox_path):
    try:
        items = []
        for p in sandbox_path.iterdir():
            name = p.name + ('/' if p.is_dir() else '')
            items.append(name)
        return '\n'.join(items) if items else '(empty)'
    except Exception as e:
        return f'ERROR: cannot list sandbox: {e}'

def handle_READ(sandbox_path, filename):
    try:
        # resolve path and ensure it is inside sandbox
        target = (sandbox_path / filename).resolve()
        sandbox_real = sandbox_path.resolve()
        if not str(target).startswith(str(sandbox_real)):
            return 'ERROR: access denied'
        if not target.exists() or target.is_dir():
            return 'ERROR: file not found or is directory'
        size = target.stat().st_size
        if size > MAX_FILE_BYTES:
            return 'ERROR: file too large'
        with open(target, 'rb') as f:
            content = f.read()
        # return as utf-8 replacement; if binary, it will be shown with replacements
        return content.decode('utf-8', errors='replace')
    except Exception as e:
        return f'ERROR: cannot read file: {e}'

def handle_TIME():
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

# ---------- client thread ----------
def client_thread(conn, addr, auth_token, sandbox_path):
    logger = logging.getLogger('server')
    logger.info(f'Client connected: {addr}')
    try:
        # Expect auth token first
        token = recv_message(conn)
        if token is None:
            logger.info(f'[{addr}] connection closed before auth')
            conn.close()
            return
        if token != auth_token:
            send_message(conn, 'AUTH_FAILED')
            logger.warning(f'[{addr}] auth failed (received token: {token})')
            conn.close()
            return
        send_message(conn, 'AUTH_OK')
        logger.info(f'[{addr}] authenticated')

        while True:
            cmd = recv_message(conn)
            if cmd is None:
                logger.info(f'[{addr}] connection closed')
                break
            cmd = cmd.strip()
            if not cmd:
                continue

            logger.info(f'[{addr}] command: {cmd}')
            if cmd == 'LIST':
                out = handle_LIST(sandbox_path)
            elif cmd.startswith('READ '):
                filename = cmd[5:].strip()
                out = handle_READ(sandbox_path, filename)
            elif cmd == 'TIME':
                out = handle_TIME()
            elif cmd == 'QUIT':
                out = 'BYE'
                send_message(conn, out)
                logger.info(f'[{addr}] requested QUIT')
                break
            else:
                out = 'ERROR: unknown or disallowed command'

            send_message(conn, out)

    except Exception as e:
        logger.exception(f'[{addr}] error: {e}')
    finally:
        try:
            conn.close()
        except:
            pass
        logger.info(f'Client disconnected: {addr}')


# ---------- main ----------
def main():
    parser = argparse.ArgumentParser(description='Safe demo server (educational)')
    parser.add_argument('--host', default='0.0.0.0', help='host to bind (default 0.0.0.0)')
    parser.add_argument('--port', type=int, default=5000, help='port to listen')
    parser.add_argument('--auth', required=True, help='authentication token (shared secret)')
    parser.add_argument('--sandbox', default=str(pathlib.Path.home() / 'sandbox'), help='sandbox directory path')
    parser.add_argument('--log', default='server.log', help='log file path')
    args = parser.parse_args()

    # setup logging
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s [%(levelname)s] %(message)s',
                        handlers=[logging.FileHandler(args.log, encoding='utf-8'),
                                  logging.StreamHandler()])

    logger = logging.getLogger('server')
    sandbox_path = pathlib.Path(args.sandbox).expanduser()
    sandbox_path.mkdir(parents=True, exist_ok=True)
    logger.info(f'Sandbox directory: {sandbox_path.resolve()}')

    # start listening socket
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((args.host, args.port))
        s.listen(5)
        logger.info(f'Server listening on {args.host}:{args.port}. Waiting for clients...')

        try:
            while True:
                conn, addr = s.accept()
                # spawn thread
                t = threading.Thread(target=client_thread, args=(conn, addr, args.auth, sandbox_path), daemon=True)
                t.start()
        except KeyboardInterrupt:
            logger.info('Server shutting down (KeyboardInterrupt)')
        except Exception:
            logger.exception('Server error')

if __name__ == '__main__':
    main()
