package main

import (
	"bufio"
	"html/template"
	"log"
	"os"
	"strings"
)


type PostLink struct {
	Text string
	Link string
}

type PostFile struct {
	FileName string
	Title    string
	Date     string
	Content  string
}

func main() {
	// clear public folder
	publicFiles, err := os.ReadDir("./public")
	if err != nil {
		log.Fatal(err)
	}
	for _, file := range publicFiles {
		name := file.Name()
		if name == "fonts" ||
			name == "images" ||
			name == "style.css" ||
			name == "keybase.txt" ||
			name == "about.html" {
			continue
		}
		os.Remove("./public/" + name)
	}

	// construct posts
	files, err := os.ReadDir("./content")
	if err != nil {
		log.Fatal(err)
	}

	// TODO: sort files?

	var posts []PostFile
	for _, file := range files {
		if isPost(file.Name()) {
			post := createPostFile(file.Name())
			posts = append(posts, post)
		}
	}

	// construct sidebar post links
	var postlinks []PostLink
	numLinks := 10
	for i := len(posts) - 1; i >= 0; i-- {
		if i <= len(posts)-1-numLinks {
			postlinks = append(postlinks,
				PostLink{Text: "[More Posts]", Link: "archive.html"})
			break
		}

		postlink := PostLink{Text: posts[i].Title, Link: posts[i].FileName}
		postlinks = append(postlinks, postlink)
	}

	// create index.html
	post_template := template.Must(template.ParseFiles("templates/post.template"))

	var frontpageContent template.HTML
	numFrontpagePosts := 5
	for i := len(posts) - 1; i >= 0 && i > len(posts)-1-numFrontpagePosts; i-- {
		frontpageContent += template.HTML(posts[i].Content)
		frontpageContent += template.HTML("<br><br><br>\n\n")
	}

	func() {
		f, err := os.Create("./public/index.html")
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		post_template.Execute(f, struct {
			PostList []PostLink
			Content  template.HTML
		}{postlinks, frontpageContent})
	}()

	// create archive.html
	archiveContent := template.HTML("<h1>Archive</h1>\n")
	for i := len(posts)-1; i >= 0; i-- {
		archiveContent += template.HTML("<h2>" + posts[i].Date + "</h2>\n")
		archiveContent += template.HTML("<a href='" + posts[i].FileName + "'>" + posts[i].Title + "</a>>\n<br><br>\n")
	}

	func() {
		f, err := os.Create("./public/archive.html")
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		post_template.Execute(f, struct {
			PostList []PostLink
			Content  template.HTML
		}{postlinks, archiveContent})
	}()

	// create post files
	for _, post := range posts {
		func() {
			f, err := os.Create("./public/" + post.FileName)
			if err != nil {
				log.Fatal(err)
			}
			defer f.Close()
			postContent := template.HTML(post.Content)
			post_template.Execute(f, struct {
				PostList []PostLink
				Content  template.HTML
			}{postlinks, postContent})
		}()
	}
}

func createPostFile(fileName string) (result PostFile) {
	result.FileName = fileName

	f, err := os.Open("content/" + fileName)
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()

	scanner := bufio.NewScanner(f)

	if scanner.Scan() {
		result.Title = scanner.Text()
	}

	if scanner.Scan() {
		result.Date = scanner.Text()
	}

	result.Content = "<h1>" + result.Title + "</h1>"
	result.Content += "<h2>" + result.Date + "</h2>"
	for scanner.Scan() {
		result.Content += scanner.Text()
	}

	if err := scanner.Err(); err != nil {
		log.Fatal(err)
	}

	return result
}

func isPost(name string) (result bool) {
	if name == "about.html" || strings.HasPrefix(name, "0000-") {
		return false
	}
	if strings.HasSuffix(name, ".html") {
		return true
	}
	return false
}
