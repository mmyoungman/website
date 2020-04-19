import os

def get_post_name(post_filepath):
    with open(post_filepath) as f:
        return f.readline().rstrip("\n") # first line of post.html is postname

def get_post_details(post_filepath):
    with open(post_filepath) as f:
        postname = f.readline().rstrip("\n") # first line of post.html is postname
        postdate = f.readline().rstrip("\n") # second line of post.html is postdate
    return postname, postdate

def get_post_html(filepath):
    with open(filepath) as f:
        post_html = f'<h1>{f.readline()}</h1>\n'
        post_html += f'<h2>{f.readline()}</h2>\n'
        post_html += f.read()
    return post_html

print("Generating html...")

# delete any generated files in public/
for filename in os.listdir("public/"):
    if filename in ["fonts", "images", "style.css", "keybase.txt"]:
        continue
    os.remove(f"public/{filename}")

# create postlist
def is_a_post(name):
    if "about.html" in name:
        return False
    elif name.startswith("0000-"):
        return False
    elif name.endswith(".html"):
        return True
    else:
        return False
content_dir = sorted(os.listdir("content/"))
postlist = [post for post in content_dir if is_a_post(post)]

# create postlist_html
postlist_html = ""
for i, post in enumerate(reversed(postlist)):
    if i >= 10: # limit length of postlist
        postlist_html += "<a href=archive.html>[More Posts]</a>\n"
        break
    post_name = get_post_name(f'content/{post}')
    postlist_html += f'<a class="postlink" href="{post}">{post_name}</a>\n'

# create about_template string
with open("templates/about.template") as f:
    about_template = f.read()

# create post_template string
with open("templates/post.template") as f:
    post_template = f.read()

# create index.html
blog_html = ""
if len(postlist) >= 5:
    for i in range(len(postlist)-1, len(postlist)-6, -1): # only show latest 5 posts
        post_html = get_post_html(f'content/{postlist[i]}')
        blog_html += post_html + "<br><br><br>\n\n"
else:
    for i in range(len(postlist)-1, -1, -1):
        post_html = get_post_html(f'content/{postlist[i]}')
        blog_html += post_html + "<br><br><br>\n\n"

full_blog_html = post_template.replace("{{{posts}}}", blog_html)
full_blog_html = full_blog_html.replace("{{{postlist}}}", postlist_html)

with open('public/index.html', "w") as f:
    f.write(full_blog_html)

# create about.html
with open('content/about.html') as f:
    about_html = f.read()
about_html += "\n<br><br><br>\n"

full_about_html = about_template.replace("{{{about}}}", about_html)

with open('public/about.html', "w") as f:
    f.write(full_about_html)

# create archive.html
archive_html = "<h1>Archive</h1>\n"
for post in reversed(postlist):
    postname, postdate = get_post_details(f'content/{post}')
    archive_html += f'<h2>{postdate}</h2>\n'
    archive_html += f'<a href="{post}">{postname}</a>\n<br><br>\n'

full_archive_html = post_template.replace("{{{posts}}}", archive_html)
full_archive_html = full_archive_html.replace("{{{postlist}}}", postlist_html)

with open('public/archive.html', "w") as f:
    f.write(full_archive_html)

# create html post files
for filename in postlist:
    post_html = get_post_html(f"content/{filename}")
    full_post_html = post_template.replace("{{{posts}}}", post_html)
    full_post_html = full_post_html.replace("{{{postlist}}}", postlist_html)

    with open(f'public/{filename}', "w") as f:
        f.write(full_post_html)

print("Finished!")
