Standard Format for Blog Posts -- This Website Part Two
=

28th October 2018
-

After being dissatisfied with using markdown to write these posts, and despite knowing nothing about creating a standardised format, I decided to design my own format for blog posts, and write something to convert those files into HTML. My ineptitude gave me confidence that, by tradition, my new format would soon be standard across the web.

My new post file is inspired by LaTeX for no more reason than I wanted a simple yet extensible markup language that wasn't markdown or XML, and no other syntax came to mind.

Despite my static website relying on python, I thought I'd write the post-to-html program in C, ostensibly because python strings are immutable, but really because I like the idea of writing a compiler in C and this is a step in that direction. It was also an opportunity to learn how to use C with python, which was one of the attractions of python in the first place. And I had already written some C string manipulation functions when working through [Advent of Code](https://github.com/mmyoungman/advent-of-code). And this is good learning for if I ever write a compiler.

To write it in C, I needed it to play nice with my generate\_website.py script. There are a few options here:

+ Creating a C executable that includes the python runtime so it can include the generate\_website.py script.
+ Creating a python C extension.
+ Using ctypes to interface with a C executable.

I didn't want to compile the entire python runtime each time I pushed a change, so I initially decided against that option.

### Creating a python C extension

Turns out that creating a python C extension isn't tricky, as copying and pasting [this example](https://tutorialedge.net/python/python-c-extensions-tutorial/) meant I was up and running in two shakes. I then tested to see if it would work with my [GitLab pipeline](https://gitlab.com/mmyoungman/homepage/-/jobs/113444065), which it did. 

Things were looking good, but I knew debugging was going to be essential, and after looking at [http://droettboom.com/blog/2015/11/20/gdb-python-extensions/](the hoops I'd have to jump through), I noped out and decided to try ctypes.

### Using ctypes

The ctypes module allows you to call a C dynamic library -- something I'd [previously messed around with](https://github.com/mmyoungman/code-snippets/tree/19e130c993dae7fd2f5661554badb1189a8289bc/c/system-calls/dynamicLibrary) -- in python. It also meant I could debug with QtCreator, as I usually do with C.

I quickly printed "hello world" with ctypes, and then managed to work around unicode to pass and return ASCII strings.

It was time to write some C.

### Writing post-to-html.c

I had no idea how to proceed, so I decided to bang out some code to produce the About page as a start, and resigned myself to the fact that the code, even if functional, would be a mess. 

### The result

To say the result is a mess is an understatement, but the emphasis was on getting something working quickly, and the About page does now generate from a .post file. I expect the clean up will be a complete rewrite, if not several rewrites, not just of the C code, but of everything about how this website is currently generated. 

Before I start cleaning the code up, I think first I'll add the functionality for paragraphs, blockquotes, images, and tables, so I can change all the existing posts from .md to .post and remove the dependency on the python markdown library. Then it'll be time to settle down and read about parsing, check out a few examples, and hopefully write some code that isn't bat-shit insane.
