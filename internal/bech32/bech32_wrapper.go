package bech32

import (
	"encoding/hex"
	"log"

	"github.com/btcsuite/btcutil/bech32"
)

func Encode(hrp string, hexStr string) string {
	data, err := hex.DecodeString(hexStr)
	if err != nil {
		log.Fatal("Failed to decode hex string", hexStr)
	}

	bits5, err := bech32.ConvertBits(data, 8, 5, true)
	if err != nil {
		log.Fatal("Failed to convert bits when encoding")
	}

	result, err := bech32.Encode(hrp, bits5)
	if err != nil {
		log.Fatal("Failed to encode to bech32")
	}
	return result
}

func Decode(str string) (hrp string, hexStr string) {
	hrp, bits5, err := bech32.Decode(str)
	if err != nil {
		log.Fatal("Failed to decode from bech32", str)
	}

	data, err := bech32.ConvertBits(bits5, 5, 8, false)
	if err != nil {
		log.Fatal("Failed to convert bits when decoding", str)
	}

	hexStr = hex.EncodeToString(data)
	return hrp, hexStr
}
