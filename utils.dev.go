//go:build !prod

package main

import "github.com/mmyoungman/website/nostr"

func UNUSED(x ...interface{}) {}

func FetchNewNostrMessages() {
	nostr.FetchNewNostrMessages()
}
