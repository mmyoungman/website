Code readability and data flow
=

11th August 2019
-

While working on a test framework for fun over the last week, a particular piece of functionality inspired some thought about the relationship between the flow of data and code readability. I found that iterating towards less code meant the code no longer represented the flow of data. The job of determining that flow was therefore offloaded onto the reader.

### The functionality in question

You can see the current state of the test framework [here](https://gitlab.com/mmyoungman/test-test-framework/tree/544e1422e8f03f98469ac9b0c0b2042c37228bda).

The test framework allows users to create test suites by subclassing the TestSuite class. They can then add tests to that subclass by creating a new method and prefixing `test__` to its name.

To implement this in Python, we need to:  
- get a list of all the method names of the class  
- filter for names that start `test__`  
- use those test names to grab their associated methods  
- and then add those methods to a list of tests to run

It turns out this can be achieved concisely:  

    for test_name in filter(lambda name: name.startswith('test__'), dir(self)):
        self.tests_to_run.append(getattr(self, test_name))

After starting with something much longer, it's satisfying code to iterate to, but is it good code? I think in this case the answer is obviously no, but this is a stark example and I've seen code that makes the same mistake more subtly. I felt there was something to learn by digging into details.

### Clever code is dumb code?

If the reader has a good knowledge of Python -- understands filter, dir, etc. -- they may not need a search engine. But even so, they still have unpacking to do.

If we put the code aside and consider how the data is processed, it starts at the `dir(self)`, which returns a list of method names for the current class. It's the first thing we need toprogress towards what we want: a list of test methods to run.

Unhelpfully, this isn't the first thing a reader would see. Instead, `dir(self)` is at the end of the first line, nested inside a function call. This means that to understand this code, the reader is forced to read the code multiple times, jump back and forwards, to identify the flow of data for themselves.

### Target audience

It might be argued that for an experienced Python developer this isn't too much mental overhead. But even for them it would require some overhead, and that all adds to the overhead of the code's larger context. People can only hold so many things in their head at once.

And do I want to only write for an audience of experienced developers? This reaches to a larger topic: who is the target audience and what does the coder wish to communicate to them.

I believe the same issue is found in writing. Consider [The School by Donald Barthelme](https://docs.google.com/viewer?a=v&pid=sites&srcid=ZGVmYXVsdGRvbWFpbnxmaWN0aW9ud2l0aGdyZWd5d3cyMDE1fGd4OmJjNGQ1ZWE1NWI0MDBlMw). On the surface the writing seems simple, but if you try to emulate his style it become apparent that behind that simplicity is great skill. Compare that with [Irene Iddesleigh by Amanda McKttrick Ros](http://www.gutenberg.org/files/34181/34181-h/34181-h.htm#chapI). Ros is out to impress, to the comical detriment of the writing itself.

### The narrative of the data

For me, the problem with the above code is that it prioritises conciseness over accurately representing the flow of data. The reader wants to understand the narrative of the data -- its journey from start to end.

Here is the code I now have:

    method_name_list = dir(self)
    is_test = lambda name: name.startswith('test__')
    test_name_list = filter(is_test, method_name_list)
    for name in test_name_list:
        method = getattr(self, name)
        self.tests_to_run.append(method)

The transformation of data can be followed in order by reading from top to bottom. And with the additional variable names, it's possible for a reader to infer what the code does.

Maybe it is a basic point, but I think it's important to build on a strong foundation. In the rush towards functionality and features, the foundations are easily overlooked.
