Coding choices
=

9th August 2019
-

After threatening numerous times, I've recently created a test framework. As with most things I do in my spare time, it's small and more for the sake of learning -- more of a dream of a final product than an actual ambition.

It's already been a success: I've learned about python metaclasses, and its encouraged me to be considerate of end users. But of most interest to me -- and that which is the subject of this post -- is how it made me think about code readability.

You can see the current state of the project [here](https://gitlab.com/mmyoungman/test-test-framework/tree/544e1422e8f03f98469ac9b0c0b2042c37228bda).

### The functionality in question

The test framework allows users to create test suites by subclassing the TestSuite class. They can then add tests to that subclass by creating a new method and prefixing `test__` to its name.

To implement the adding of tests in python, we need to:  
- get a list of all the method names of the class  
- filter for those who's name starts `test__`  
- use those strings to grab their associated methods  
- and then add those methods to a list of tests to be run

It turns out this can be achieved concisely:  

    for test_name in filter(lambda name: name.startswith('test__'), dir(self)):
        self.tests_to_run.append(getattr(self, test_name))

### Clever code is dumb code?

It's the kind of code that is satisfying to iterate to, but after looking at it for a while, I started to wonder whether it was bad code.

If the reader has a deep knowledge of python, they might not have to reach for a search engine. But even in that case, they still have some unpacking to do.

The start of what this code does is found at the `dir(self)`, which returns a list of method names for the current class. It is this data that we will use to obtain a list of test methods to run.

Unhelpfully, this isn't the first thing a reader would see as it is at the end of the first line. This means that to understand this code, the reader must jump backwards and forwards to appreciate the order of execution. They're forced to read it multiple times and identify the flow for themselves.

### Target audience

It might be argued that for an experienced python developer, this isn't too much mental overhead. But even for them it would require some mental overhead, and that all adds on top of the mental overhead of the code's larger context.

And do I want to only write for an audience of experienced developers and damn the rest? This reaches to a larger topic of how a developers intentions can influence the code they write.

### The narrative of the data

For me, the problem with the above code is that it prioritises conciseness of code over demonstrating the flow of data. The reader is trying to understand the narrative of the data -- its journey from start to end.

Here is the code I currently have:

    method_name_list = dir(self)
    is_test = lambda name: name.startswith('test__')
    test_name_list = filter(is_test, method_name_list)
    for name in test_name_list:
        method = getattr(self, name)
        self.tests_to_run.append(method)

The transformation of data can be followed in order by reading from top to bottom. And with the additional variable names, it's possible for a reader to infer what the code does.

It's possibly a basic point, but as I'm only just starting as a software engineer, it's these basics of coding that I want to make sure I'm doing right. And if my experience in other areas is anything to go by, the basics are often the hardest thing to get right.
