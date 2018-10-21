Goals of this Website -- This Website Part One
=

20th October 2018
-

You can find the code for this website, at the time of writing, [here](https://gitlab.com/mmyoungman/homepage/tree/612851edba44990c5171bd651aed49073b4e3b5e).

This post is probably only of interest to myself, but I needed a first post, so this is going to be it.

I created this website with a few goals in mind:

### Minimise dependencies
So I can learn how things work, and because it's more fun. Creating this website is predomiantly a learning exercise. With a hobby project like this, I can happily sacrifice time and roll my own, if that's what I desire.

### Write posts in a future-proof format
I want to avoid editing all posts by hand if I need them in a different format in the future.

I've decided to try markdown to start, but I'm already resorting to embedding HTML for some tables and superscript, plus using hacky workarounds for formatting. I'm considering moving to [LaTeX](https://www.latex-project.org/), as it's more powerful and should allow me to include additional information like categories, tags, blurbs.

### Allow quick iteration
The [livereload python library](https://github.com/lepture/python-livereload) does this job admirably. Interested in how that works...

### Automate where possible
Manually regenerating and uploading the webiste would be a pain, but thankfully [GitLab](http://www.gitlab.com) runs the generate\_website.py script whenever I push to the repository and serves all files in the public directory. There may be an alternate solution in git hooks.

In it's current form, there's lots of opportunity for improvement. So I've added a "Part One" to the title, as I expect further development.
