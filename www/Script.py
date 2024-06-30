#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html\r\n", end='')
print("\r\n", end='')
print("<html><head><title>CGI Script</title></head><body>", end='')
print("<h1>Hello from the CGI script!</h1>", end='')
print(f"<p>Request Method: {os.environ.get('REQUEST_METHOD')}</p>", end='')
print(f"<p>Server Protocol: {os.environ.get('SERVER_PROTOCOL')}</p>", end='')
print(f"<p>CONTENT_TYPE: {os.environ.get('CONTENT_TYPE')}</p>", end='')
print(f"<p>CONTENT_LENGTH: {os.environ.get('CONTENT_LENGTH')}</p>", end='')
print(f"<p>HTTP_ACCEPT: {os.environ.get('HTTP_ACCEPT')}</p>", end='')
print(f"<p>SERVER_NAME: {os.environ.get('SERVER_NAME')}</p>", end='')
print(f"<p>HTTP_USER_AGENT: {os.environ.get('HTTP_USER_AGENT')}</p>", end='')
print(f"<p>HTTP_ACCEPT: {os.environ.get('HTTP_ACCEPT')}</p>", end='')
print(f"<p>HTTP_ACCEPT_ENCODING: {os.environ.get('HTTP_ACCEPT_ENCODING')}</p>", end='')
print(f"<p>HTTP_ACCEPT_LANGUAGE: {os.environ.get('HTTP_ACCEPT_LANGUAGE')}</p>", end='')
print(f"<p>QUERY_STRING: {os.environ.get('QUERY_STRING')}</p>", end='')
print("</body></html>")
# sys.stdout.flush()
