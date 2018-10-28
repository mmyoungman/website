Standard Format for Blog Posts -- This Website Part Two
=

28th October 2018
-

After being dissatisfied with using markdown to write these posts, and despite knowing nothing about creating a standardised format, I decided to design my own format for blog posts, and write something to convert those files into HTML. My ineptitude gave me confidence that, by tradition, my new format would soon be standard across the web.

My new post file is inspired by LaTeX for no more reason than I wanted a simple yet extensible markup language that wasn't markdown or XML, and no other syntax came to mind.

Despite my static website relying on Python, I thought I'd write the post-to-html program in C, ostensibly because Python strings are immutable, but really because I like the idea of writing a compiler in C and this is a step in that direction. It was also an opportunity to learn how to use C with Python, which was, for me, one of the attractions of Python. I also had already written some [C string manipulation functions](https://github.com/mmyoungman/lib-mmy/blob/628479aff54d1577383d468f8c5fe4b46194503f/lib-mmy.h) when working through [Advent of Code](https://github.com/mmyoungman/advent-of-code).

To write it in C, I needed it to play nice with my generate\_website.py script. There were a few options here:

+ Creating a C executable that includes the Python runtime so it can include the generate\_website.py script.
+ Creating a Python C extension.
+ Using ctypes to interface with a C executable.

I didn't want to compile the entire Python runtime each time I pushed a change, so I started by looking at an extension.

### Creating a Python C extension

Turned out that creating a Python C extension is as easy as copying and pasting [this example](https://tutorialedge.net/python/python-c-extensions-tutorial/), so I was up and running in two shakes. I then tested to see if it would work with my [GitLab pipeline](https://gitlab.com/mmyoungman/homepage/-/jobs/113444065), which it did. 

Things were looking good, but I knew debugging was going to be essential, and after looking at [the hoops I'd have to jump through](http://droettboom.com/blog/2015/11/20/gdb-python-extensions/), I noped out and decided to try ctypes.

### Using ctypes

The ctypes module allows you to call a C dynamic library -- something I'd [previously messed around with](https://github.com/mmyoungman/code-snippets/tree/19e130c993dae7fd2f5661554badb1189a8289bc/c/system-calls/dynamicLibrary) -- in Python. It also meant I could debug with QtCreator, as I usually do with C.

I quickly printed "hello world" with ctypes, and then managed to work around unicode to pass and return ASCII strings from the compiled C code. 

I previously had an idea to write C library to handle unicode strings with a kind of built-in memory management, but that had to be postponed in the name of progress. It was time to write some C.

### Writing post-to-html.c

I had no idea how to proceed, so I decided to bang out some code to produce the About page as a start, and resigned myself to the fact that the code, even if it works, would be a mess. As usual, I tended to avoid adding dependencies, as this is ultimately a learning exercise.

To say the result is a mess is an [understatement](https://gitlab.com/mmyoungman/homepage/blob/1904776a5b073c9140430aaa9d466dea5d6351e6/post-to-html/post-to-html.c), but the emphasis was on getting something working quickly, and the About page does now generate from a .post file. I expect the clean up will be a complete rewrite, if not several rewrites, not just of the C code, but of everything about how this website is currently generated. 

Before I start cleaning the code up, I think first I'll add functionality for paragraphs, sections, lists, blockquotes, images, and tables, so I can change all the existing posts from .md to .post and remove the dependency on the Python markdown library. Then it'll be time to settle down and read about parsing, check out a few examples, and hopefully write some code that isn't bat-shit insane.
