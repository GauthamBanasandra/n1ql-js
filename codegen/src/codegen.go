package main

import (
	"fmt"
	"io/ioutil"
	"os/exec"
	"bytes"
)

func main() {
	input_path := "../n1ql_js/inputs/input_db_test.txt"
	input, err_read := ioutil.ReadFile(input_path)
	if err_read != nil {
		panic(err_read)
	}

	cmd := exec.Command("bin/a.out")
	cmd.Stdin = bytes.NewReader(input)
	var output bytes.Buffer
	cmd.Stdout = &output
	err := cmd.Run()
	if err != nil {
		panic(err)
	}
	fmt.Println("output\t", output.String())
}
