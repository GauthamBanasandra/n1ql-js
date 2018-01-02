package main

import "github.com/couchbase/query/parser/n1ql"

func main() {
	q := "select * from `beer-sample` where name == $name"
	n1ql.JustParse(q)
}
