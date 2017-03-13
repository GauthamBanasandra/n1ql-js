package main

import (
	"os/exec"
	"bytes"
	"fmt"
)

func main() {
	cmd:=exec.Command("node", "/Users/gautham/projects/github/linq-js/transpiler/out/production/transpiler/logs/transpiler_log.txt")
	var output bytes.Buffer
	cmd.Stdout = &output

	// Run the binary.
	err := cmd.Run()
	if err != nil {
		panic(err)
	}

	fmt.Println(output.String())
}
