#! /usr/bin/env python

from livereload import Server, shell

server = Server()
server.watch('templates/*', shell('python generate_website.py'))
server.watch('content/*', shell('python generate_website.py'))
server.watch('public/style.css', shell('python generate_website.py'))
server.watch('generate_website.py', shell('python generate_website.py'))
server.serve(root='public/')
