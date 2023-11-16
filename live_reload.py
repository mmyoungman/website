#! /usr/bin/env python

from livereload import Server, shell

server = Server()
server.watch('content/*', shell('go run .'))
server.watch('contrib/*', shell('go run .'))
server.watch('internal/*', shell('go run .'))
server.watch('nostr/*', shell('go run .'))
server.watch('templates/*', shell('go run .'))
server.watch('public/style.css', shell('go run .'))
server.watch('generate_website.go', shell('go run .'))
server.serve(root='public/')
