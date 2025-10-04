package nostr

import (
	"strings"
)

type Tag []string
type Tags []Tag

func (tags Tags) ToJson() string {
	var result strings.Builder

	result.WriteString("[")
	for i, tag := range tags {
		if i > 0 {
			result.WriteString(",")
		}

		writeTag(&result, tag)
	}
	result.WriteString("]")

	DevBuildValidJson(result.String())

	return result.String()
}

func writeTag(result *strings.Builder, tag Tag) {
	result.WriteString("[")

	for i, str := range tag {
		if i > 0 {
			result.WriteString(",")
		}
		escapedStr := DecorateJsonStr(str)
		result.WriteString(escapedStr)
	}

	result.WriteString("]")
}
