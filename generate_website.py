#!/usr/bin/env python

import os
import subprocess
from ctypes import cdll
from ctypes import c_char_p

def get_filename(filepath):
    filename = os.path.split(filepath)[1] # remove path
    return filename.rsplit(".", 1)[0] # remove extension

def get_postname(post_filepath):
    f = open(post_filepath)
    file_b = bytes(f.read(), encoding='utf-8')
    postname = post_to_html_lib.get_title(file_b)
    f.close()
    return postname.decode('utf-8')

def get_postdate(post_filepath):
    f = open(post_filepath)
    file_b = bytes(f.read(), encoding='utf-8')
    postdate = post_to_html_lib.get_date(file_b)
    f.close()
    return postdate.decode('utf-8')

if __name__ == "__main__":
    print("Compiling post_to_html.o...")
    #os.remove("post-to-html/post-to-html.o")
    subprocess.call('post-to-html/compile.sh')
    post_to_html_lib = cdll.LoadLibrary("./post-to-html/post-to-html.o")
    post_to_html_lib.convert_body.restype=c_char_p
    post_to_html_lib.get_title.restype=c_char_p
    post_to_html_lib.get_date.restype=c_char_p
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
        elif name.endswith(".post"):
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
    blog_posts_b = bytes("", encoding='utf-8')
    if len(post_list) >= 5:
        for i in range(len(post_list)-1, len(post_list)-6, -1): # latest 5 posts on the page
            f = open('content/' + post_list[i])
            post_content_b = bytes(f.read(), encoding='utf-8')
            f.close()
            post_content_b = post_to_html_lib.convert_body(post_content_b)
            blog_posts_b += post_content_b + bytes("\n\\newline\\newline\n\x00", encoding='utf-8')
    else:
        for i in range(len(post_list)-1, -1, -1):
            f = open('content/' + post_list[i])
            post_content_b = bytes(f.read(), encoding='utf-8')
            f.close()
            post_content_b = post_to_html_lib.convert_body(post_content_b)
            blog_posts_b += post_content_b + bytes("\n\\newline\\newline\n\x00", encoding='utf-8')

    blog_html = blog_posts_b.decode('utf-8')
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
        post_b = bytes(f.read(), encoding='utf-8')
        f.close()

        post_b = post_to_html_lib.convert_body(post_b)
        post_html = post_b.decode('utf-8')
        full_post_html = post_template.replace("{{{post}}}", post_html)
        full_post_html = full_post_html.replace("{{{postlist}}}", postlist_html)

        if not os.path.exists("public/"):
            os.makedirs("public/")

        new_filepath = 'public/' + filename.rsplit(".", 1)[0] + '.html'
        f = open(new_filepath, "w")
        f.write(full_post_html)
        f.close()

    print("Finished!")
