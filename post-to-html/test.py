from ctypes import cdll
from ctypes import c_char_p

post_to_html_lib = cdll.LoadLibrary("./post-to-html.o")
post_to_html_lib.helloworld.restype=c_char_p

name = b"Mark"

print(post_to_html_lib.helloworld(name))
