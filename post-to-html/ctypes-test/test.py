from ctypes import cdll
from ctypes import c_char_p

hello = cdll.LoadLibrary("./helloworld.o")
hello.helloworld.restype=c_char_p

name = b"Mark"

print(hello.helloworld(name))
