//go:build !prod

package main

import (
	"database/sql"

	"github.com/mmyoungman/website/nostr"
)

func UNUSED(x ...interface{}) {}

func FetchNewNostrMessages(db *sql.DB) {
	nostr.FetchNewNostrMessages(db)
}
