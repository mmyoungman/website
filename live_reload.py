from livereload import Server, shell

server = Server()
server.watch('templates/*', shell('python generate_website.py'))
server.watch('posts/*', shell('python generate_website.py'))
server.watch('generate_website.py', shell('python generate_website.py'))
server.serve(root='public/')
