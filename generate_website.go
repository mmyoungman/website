package main

import (
	"bufio"
	"fmt"
	"html/template"
	"log"
	"net/url"
	"os"
	"strings"
	"time"

	"github.com/mmyoungman/nostr_backup"
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
	// fetch latest nostr notes
	FetchNewNostrMessages()

	// fetch nostr notes from DB
	db := nostr_backup.DBConnect()
	nostrEvents := nostr_backup.DBGetEvents(db)

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
			name == "keybase.txt" {
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

	//create about.html
	var aboutContent template.HTML
	func() {
		f, err := os.Open("content/about.html")
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()

		scanner := bufio.NewScanner(f)

		for scanner.Scan() {
			aboutContent += template.HTML(scanner.Text())
		}

		if err := scanner.Err(); err != nil {
			log.Fatal(err)
		}
	}()
	func() {
		about_template := template.Must(template.ParseFiles("templates/about.template"))

		f, err := os.Create("./public/about.html")
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()

		about_template.Execute(f, struct {
			Content template.HTML
		}{aboutContent})
	}()

	// create notes.html
	notesContent := template.HTML("<h1>Notes</h1>\n")
eventLoop:
	for i := len(nostrEvents) - 1; i >= 0; i-- {
		if nostrEvents[i].Kind != nostr_backup.KindTextNote { // && nostrEvents[i].Kind != nostr_backup.KindRepost { // @MarkFix TODO support reposts
			continue
		}

		content := nostrEvents[i].Content
		for _, tag := range nostrEvents[i].Tags {
			if tag[0] == "r" && len(tag) == 2 {
				link := tag[1]
				_, err := url.Parse(link)
				if err != nil {
					log.Printf("note link could not be parsed - %s", link)
					continue
				}
				if strings.Contains(link, "\"") {
					log.Printf("note links shouldn't contain a '\"' - %s", link)
					continue
				}
				if !strings.Contains(content, link) {
					log.Printf("note contents should have contained link")
					continue
				}
				if !strings.HasPrefix(link, "https://") {
					log.Printf("note link needs 'https://' to added to it - %s", link)
					link = "https://" + link
				}

				if strings.HasSuffix(link, ".jpg") {
					content = strings.Replace(content, link, fmt.Sprintf("<br><img style=\"max-width: 40%%;\" src=\"%s\" />", link), 1)
				// @MarkFix Something for YouTube videos?
				} else {
					content = strings.Replace(content, link, fmt.Sprintf("<a href=\"%s\">%s</a>", link, link), 1)
				}
			}
			// @MarkFix TODO What to do with replies?
			if tag[0] == "e" && len(tag) == 4 && tag[3] == "reply" {
				log.Printf("skipping note reply")
				continue eventLoop
			}

			if tag[0] == "e" && len(tag) == 4 && tag[3] == "mention" {
				log.Printf("skipping note mention")
				continue eventLoop
			}
		}
		date := time.Unix(nostrEvents[i].CreatedAt, 0)
		notesContent += template.HTML(fmt.Sprintf("<h2>%02d:%02d, %d %s %d</h2>\n", date.Hour(), date.Minute(), date.Day(), date.Month().String(), date.Year()))

		notesContent += template.HTML(fmt.Sprintf("<p>%s</p>\n\n", content))
		//notesContent += template.HTML(fmt.Sprintf("<p>%s</p><p><a href=\"https://snort.social/nevent%s\">Link</a></p>\n\n", content, nostrEvents[i].Id)) // @MarkFix need bech32
	}

	func() {
		f, err := os.Create("./public/notes.html")
		if err != nil {
			log.Fatal(err)
		}
		defer f.Close()
		post_template.Execute(f, struct {
			PostList []PostLink
			Content  template.HTML
		}{postlinks, notesContent})
	}()

	// create archive.html
	archiveContent := template.HTML("<h1>Archive</h1>\n")
	for i := len(posts) - 1; i >= 0; i-- {
		archiveContent += template.HTML("<h2>" + posts[i].Date + "</h2>\n")
		archiveContent += template.HTML("<a href='" + posts[i].FileName + "'>" + posts[i].Title + "</a>\n<br><br>\n")
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
