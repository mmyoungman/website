import os
import markdown

import subprocess
from ctypes import cdll
from ctypes import c_char_p

def get_filename(filepath):
    filename = os.path.split(filepath)[1] # remove path
    return filename.rsplit(".", 1)[0] # remove extension

def get_postname(post_filepath):
    f = open(post_filepath)
    result = f.readline().rstrip("\n") # first line of post.md should be postname
    f.close()
    return result

def get_postdate(post_filepath):
    f = open(post_filepath)
    for i, line in enumerate(f):
        if i == 3: # the post date should be on the fourth line of the .md file
            result = line.rstrip("\n")
            break
    f.close()
    return result

if __name__ == "__main__":
    print("Compiling post_to_html.o...")
    os.remove("post-to-html/post-to-html.o")
    subprocess.call('post-to-html/compile.sh')
    post_to_html_lib = cdll.LoadLibrary("./post-to-html/post-to-html.o")
    post_to_html_lib.convert_body.restype=c_char_p
    print("Done!")

    print("Generating html...")
    # create public/ or delete files in public/
    if not os.path.exists("public/"):
        os.makedirs("public/")
    else:
        for filename in os.listdir("public/"):
            if filename in ["fonts", "images", "style.css"]:
                continue
            os.remove("public/" + filename)

    # create post_list list and postlist_html string
    def is_a_post(name):
        if "about.post" in name:
            return False
        elif name.startswith("0000-"):
            return False
        elif name.endswith(".md"):
            return True
        else:
            return False
    post_list = [post for post in sorted(os.listdir("content/")) if is_a_post(post)]

    postlist_html = ""
    i = 0
    for post in reversed(post_list):
        if i >= 10: # limit length of postlist
            postlist_html += "<a href=archive.html>[More Posts]</a>\n"
            break
        i += 1

        post_filepath = "content/" + post
        postlist_html += '<a class="postlink" href="' + get_filename(post_filepath) + '.html">' + get_postname(post_filepath) + '</a>\n'

    # create about_template string
    f = open("templates/about.template")
    about_template = f.read()
    f.close()

    # create post_template string
    f = open("templates/post.template")
    post_template = f.read()
    f.close()

    # create about.html
    f = open('content/about.post')
    about_post = f.read()
    f.close()

    about_post_b = bytes(about_post, encoding='utf-8')
    about_html_b = post_to_html_lib.convert_body(about_post_b)
    full_about_html = about_template.replace('{{{post}}}', about_html_b.decode('utf-8'))

    f = open('public/about.html', 'w')
    f.write(full_about_html)
    f.close()

    # create index.html
    blog_markdown = ""
    if len(post_list) >= 5:
        for i in range(len(post_list)-1, len(post_list)-6, -1): # latest 5 posts on the page
            f = open('content/' + post_list[i])
            post_content = f.read()
            f.close()
            blog_markdown += post_content + "<br><br><br>\n\n"
    else:
        for i in range(len(post_list)-1, -1, -1):
            f = open('content/' + post_list[i])
            post_content = f.read()
            f.close()
            blog_markdown += post_content + "<br><br><br>\n\n"

    blog_html = markdown.markdown(blog_markdown, extensions=['tables'])
    full_blog_html = post_template.replace("{{{post}}}", blog_html)
    full_blog_html = full_blog_html.replace("{{{postlist}}}", postlist_html)

    f = open('public/index.html', "w")
    f.write(full_blog_html)
    f.close()
    
    # create archive.html
    archive_html = "<h1>Archive</h1>\n"

    for post in reversed(sorted(post_list)):
        postname = get_postname("content/" + post)
        postdate = get_postdate("content/" + post)
        archive_html += "<h2>" + postdate + "</h2>\n"
        archive_html += '<a href="' + post.rsplit(".", 1)[0] + '.html">' + postname + '</a>\n<br><br>\n'

    full_archive_html = post_template.replace("{{{post}}}", archive_html)
    full_archive_html = full_archive_html.replace("{{{postlist}}}", postlist_html)
    
    f = open('public/archive.html', "w")
    f.write(full_archive_html)
    f.close()

    # create html post files
    for filename in post_list:
        f = open("content/" + filename)
        post_markdown = f.read()
        f.close()

        post_html = markdown.markdown(post_markdown, extensions=['tables'])
        full_post_html = post_template.replace("{{{post}}}", post_html)
        full_post_html = full_post_html.replace("{{{postlist}}}", postlist_html)

        if not os.path.exists("public/"):
            os.makedirs("public/")

        new_filepath = 'public/' + filename.rsplit(".", 1)[0] + '.html'
        f = open(new_filepath, "w")
        f.write(full_post_html)
        f.close()

    print("Finished!")
