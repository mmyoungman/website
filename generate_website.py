import os

def get_filename(filepath):
    filename = os.path.split(filepath)[1] # remove path
    return filename.rsplit(".", 1)[0] # remove extension

def get_postname(post_filepath):
    f = open(post_filepath)
    result = f.readline().rstrip("\n") # first line of post.html should be postname
    f.close()
    return result

def get_postdate(post_filepath):
    f = open(post_filepath)
    for i, line in enumerate(f):
        if i == 1: # the post date should be on the second line of the .html file
            result = line.rstrip("\n")
            break
    f.close()
    return result

if __name__ == "__main__":
    print("Generating html...")
    # create public/ or delete files in public/
    if not os.path.exists("public/"):
        os.makedirs("public/")
    else:
        for filename in os.listdir("public/"):
            if filename in ["fonts", "images", "style.css", "keybase.txt"]:
                continue
            os.remove("public/" + filename)

    # create post_list list and postlist_html string
    def is_a_post(name):
        if "about.html" in name:
            return False
        elif name.startswith("0000-"):
            return False
        elif name.endswith(".html"):
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
    f = open('content/about.html')
    about_html = f.read()
    f.close()

    about_html += "\n<br><br><br>\n"
    full_about_html = about_template.replace("{{{posts}}}", about_html)

    f = open('public/about.html', "w")
    f.write(full_about_html)
    f.close()

    # create index.html
    blog_html = ""
    if len(post_list) >= 5:
        for i in range(len(post_list)-1, len(post_list)-6, -1): # latest 5 posts on the page
            f = open('content/' + post_list[i])
            post_content = f'<h1>{f.readline()}</h1>\n'
            post_content += f'<h2>{f.readline()}</h2>\n'
            post_content += f.read()
            f.close()
            blog_html += post_content + "<br><br><br>\n\n"
    else:
        for i in range(len(post_list)-1, -1, -1):
            f = open('content/' + post_list[i])
            post_content = f'<h1>{f.readline()}</h1>\n'
            post_content += f'<h2>{f.readline()}</h2>\n'
            post_content += f.read()
            f.close()
            blog_html += post_content + "<br><br><br>\n\n"

    full_blog_html = post_template.replace("{{{posts}}}", blog_html)
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

    full_archive_html = post_template.replace("{{{posts}}}", archive_html)
    full_archive_html = full_archive_html.replace("{{{postlist}}}", postlist_html)
    
    f = open('public/archive.html', "w")
    f.write(full_archive_html)
    f.close()

    # create html post files
    for filename in post_list:
        f = open("content/" + filename)
        post_html = f'<h1>{f.readline()}</h1>\n'
        post_html += f'<h2>{f.readline()}</h2>\n'
        post_html += f.read()
        f.close()

        full_post_html = post_template.replace("{{{posts}}}", post_html)
        full_post_html = full_post_html.replace("{{{postlist}}}", postlist_html)

        if not os.path.exists("public/"):
            os.makedirs("public/")

        new_filepath = 'public/' + filename.rsplit(".", 1)[0] + '.html'
        f = open(new_filepath, "w")
        f.write(full_post_html)
        f.close()

    print("Finished!")
