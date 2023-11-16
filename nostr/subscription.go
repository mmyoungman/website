package nostr

type Subscription struct {
	Id      string
	Filters Filters
	Eose    bool
}
