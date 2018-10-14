import os
import markdown

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

def generate():
    print("Generating html...")
    # create public/ or delete files in public/
    if not os.path.exists("public/"):
        os.makedirs("public/")
    else:
        for filename in os.listdir("public/"):
            os.remove("public/" + filename)

    # create post_list list and postlist_html string
    post_list = sorted(os.listdir("posts/"))
    postlist_html = ""
    i = 0
    for post in reversed(post_list):
        if i >= 15: # limit postlist to 15 posts
            break
        i += 1

        post_filepath = "posts/" + post
        postlist_html += '<a class="postlink" href="' + get_filename(post_filepath) + '.html">' + get_postname(post_filepath) + '</a>\n'

    # create post_template string
    f = open("templates/post.template")
    post_template = f.read()
    f.close()

    # create index.html
    index_markdown = ""
    if len(post_list) >= 5:
        for i in range(len(post_list)-1, len(post_list)-6, -1): # latest 5 posts on the index page
            f = open('posts/' + post_list[i])
            post_content = f.read()
            f.close()
            index_markdown += post_content + "<br><br><br>"
    else:
        for i in range(len(post_list)-1, -1, -1):
            f = open('posts/' + post_list[i])
            post_content = f.read()
            f.close()
            index_markdown += post_content + "<br><br><br>"


    index_html = markdown.markdown(index_markdown)
    full_index_html = post_template.replace("{post}", index_html)
    full_index_html = full_index_html.replace("{postlist}", postlist_html)

    f = open('public/index.html', "w")
    f.write(full_index_html)
    f.close()
    
    # create archive.html
    archive_html = "<h1>Archive</h1>\n"

    for post in reversed(sorted(os.listdir("posts/"))):
        postname = get_postname("posts/" + post)
        postdate = get_postdate("posts/" + post)
        archive_html += "<h2>" + postdate + "</h2>\n"
        archive_html += '<a href="' + post.rsplit(".", 1)[0] + '.html">' + postname + '</a>\n<br><br>\n'

    full_archive_html = post_template.replace("{post}", archive_html)
    full_archive_html = full_archive_html.replace("{postlist}", postlist_html)
    
    f = open('public/archive.html', "w")
    f.write(full_archive_html)
    f.close()

    # create html post files
    for filename in post_list:
        f = open("posts/" + filename)
        post_markdown = f.read()
        f.close()

        post_html = markdown.markdown(post_markdown)
        full_post_html = post_template.replace("{post}", post_html)
        full_post_html = full_post_html.replace("{postlist}", postlist_html)

        if not os.path.exists("public/"):
            os.makedirs("public/")

        new_filepath = 'public/' + filename.rsplit(".", 1)[0] + '.html'
        f = open(new_filepath, "w")
        f.write(full_post_html)
        f.close()

    print("Finished!")

if __name__ == "__main__":
    generate()

