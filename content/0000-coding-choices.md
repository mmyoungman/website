Coding choices
=

9th August 2019
-

While working on a test framework for fun over the last week, a particular piece of functionality inspired some thought about the relationship between the order of execution and code readability. I found that iterating to conciseness meant the code no longer represented the order of execution. The job of determining that order was therefore offloaded onto the reader.

### The framework

You can see the current state of the test framework [here](https://gitlab.com/mmyoungman/test-test-framework/tree/544e1422e8f03f98469ac9b0c0b2042c37228bda).

The test framework allows users to create test suites by subclassing the TestSuite class. They can then add tests to that subclass by creating a new method and prefixing `test__` to its name.

To implement this in python, we need to:  
- get a list of all the method names of the class  
- filter for those who's name starts `test__`  
- use those strings to grab their associated methods  
- and then add those methods to a list of tests to run

It turns out this can be achieved concisely:  

    for test_name in filter(lambda name: name.startswith('test__'), dir(self)):
        self.tests_to_run.append(getattr(self, test_name))

It's code that is satisfying to arrive at, but is it good code?

### Clever code is dumb code?

If the reader has a deep knowledge of python -- understands filter, dir, etc. -- they might not have to reach for a search engine. But even in that case, they still have some unpacking to do.

If we put the code aside and consider how the data is processed, the start of what this code does is actually found at the `dir(self)`, which returns a list of method names for the current class. It is this data that we need at the start to obtain a list of test methods to run.

Unhelpfully, this isn't the first thing a reader would see as it is at the end of the first line, nested inside a function call. This means that to understand this code, the reader is forced to read the code multiple times, jump back and forwards, to identify the order of execution for themselves.

### Target audience

It might be argued that for an experienced python developer this isn't too much mental overhead. But even for them it would require some overhead, and that all adds to the overhead of the code's larger context. People can only hold so many things in their head at once.

And do I want to only write for an audience of experienced developers? This reaches to a larger topic of what constitutes impressive code. Who is the target audience and what reaction does the coder want from them? It's natural to want to impress people, for my code to to stand out. But does this desire for code that stands out highlight misplaced priorities?

I'm inclined to believe that this is much the same issue found in writing. Consider ("The School" by Donald Barthelme)[https://docs.google.com/viewer?a=v&pid=sites&srcid=ZGVmYXVsdGRvbWFpbnxmaWN0aW9ud2l0aGdyZWd5d3cyMDE1fGd4OmJjNGQ1ZWE1NWI0MDBlMw]. On the surface, the writing seems simple, but behind that simplicity is great skill -- which is apparent to anyone who has tried to emulate Barthelme's style.

### The narrative of the data

For me, the problem with the above code is that it prioritises conciseness over accurately representing the flow of data. The reader is trying to understand the narrative of the data -- its journey from start to end.

Here is the code I now have:

    method_name_list = dir(self)
    is_test = lambda name: name.startswith('test__')
    test_name_list = filter(is_test, method_name_list)
    for name in test_name_list:
        method = getattr(self, name)
        self.tests_to_run.append(method)

The transformation of data can be followed in order by reading from top to bottom. And with the additional variable names, it's possible for a reader to infer what the code does.

Maybe it is a basic point, but it's these basics of coding that I want to make sure I'm doing right. And, in my experience, the basics are often the hardest thing.
