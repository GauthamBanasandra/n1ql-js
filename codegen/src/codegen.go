package main

import (
	"fmt"
	"io/ioutil"
	"os/exec"
	"bytes"
)

func main() {
	// Input path.
	input_path := "../n1ql_js/inputs/input_db_test.txt"

	// Read input file.
	input, err_read := ioutil.ReadFile(input_path)
	if err_read != nil {
		panic(err_read)
	}

	// Binary to execute.
	cmd := exec.Command("bin/n1ql_js")

	// Set I/O.
	cmd.Stdin = bytes.NewReader(input)
	var output bytes.Buffer
	cmd.Stdout = &output

	// Run the binary.
	err := cmd.Run()
	if err != nil {
		panic(err)
	}

	// Output.
	fmt.Println(output.String())
}
