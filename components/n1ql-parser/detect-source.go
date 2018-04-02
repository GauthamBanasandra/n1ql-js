package main

import (
	"fmt"

	"github.com/couchbase/query/algebra"
	"github.com/couchbase/query/parser/n1ql"
)

func main() {
	stmt := `INSERT INTO beer VALUES ("a", "b");`
	alg, err := n1ql.ParseStatement(stmt)
	if err != nil {
		fmt.Printf("Error parsing N1QL query, err : %v\n", err)
		return
	}

	switch queryType := alg.(type) {
	case *algebra.Insert:
		fmt.Println(queryType.KeyspaceRef().Keyspace())

	case *algebra.Upsert:
		fmt.Println(queryType.KeyspaceRef().Keyspace())

	case *algebra.Delete:
		fmt.Println(queryType.KeyspaceRef().Keyspace())

	case *algebra.Update:
		fmt.Println(queryType.KeyspaceRef().Keyspace())

	case *algebra.Merge:
		fmt.Println(queryType.KeyspaceRef().Keyspace())
	}
}
