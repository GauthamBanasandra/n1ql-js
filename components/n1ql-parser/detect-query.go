package main

import (
	"fmt"

	"github.com/couchbase/query/algebra"
	"github.com/couchbase/query/parser/n1ql"
)

func main() {
	stmt := `INSERT INTO beer VALUES ("a", "b")`
	alg, _ := n1ql.ParseStatement(stmt)
	fmt.Printf("Got:%%#v %#v\n", alg)
	switch t := alg.(type) {
	case *algebra.Insert:
		fmt.Printf("Got:%%#v %#v\n", t)
		ksp := t.KeyspaceRef()
		fmt.Printf("Got:%%#v %#v\n", ksp)
		fmt.Printf("Bucket is %v, bingo!!\n", ksp.Keyspace())
		break;

	case *algebra.Delete:
		fmt.Printf("Got:%%#v %#v\n", t)
		ksp := t.KeyspaceRef()
		fmt.Printf("Got:%%#v %#v\n", ksp)
		fmt.Printf("Bucket is %v, bingo!!\n", ksp.Keyspace())
		break;
	}
}