//go:build !prod

package main

import "github.com/mmyoungman/nostr_backup"

func UNUSED(x ...interface{}) {}

func FetchNewNostrMessages() {
	nostr_backup.FetchNewNostrMessages()
}
