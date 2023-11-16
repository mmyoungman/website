//go:build !prod

package nostr

import "github.com/mmyoungman/website/internal/json"

func UNUSED(x ...interface{}) {}

func DevBuildValidJson(str string) {
	if !json.IsValidJson(str) {
		panic("Json is not valid!")
	}
}
