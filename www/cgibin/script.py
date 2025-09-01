#!/usr/bin/env python3

import cgi, cgitb, os

print("Content-Type: text/html", end='\r\n\r\n')

while(1):
    continue

print("<!DOCTYPE html><html lang=\"fr\"><head><meta charset=\"utf-8\"><title>CGI: Python</title></head><body>", end='\n')
print("<h1>Hello from the Python CGI script!</h1>", end='')
print("</br><h2>Variables:</h2></br>", end='')
print("<pre>")

get = cgi.parse()
for i in get:
    print(f"{i} => {get[i]}</br>", end='')

print("</pre>", end='')
print("</br><h2>Environment:</h2></br>", end='\n')
print("<pre>", end='')

for i in os.environ:
    print(f"{i} => {os.environ[i]}</br>", end='')

print("</pre>", end='')
print("</body></html>", end='')
