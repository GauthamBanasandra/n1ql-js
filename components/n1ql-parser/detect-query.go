package main

import (
	"fmt"

	"github.com/couchbase/query/algebra"
	"github.com/couchbase/query/parser/n1ql"
)

func main() {
	//stmt := `INSERT INTO beer VALUES ("a", "b")`
	stmt := "SELECT t1.country, array_agg(t1.city), sum(t1.city_cnt) as apnum FROM (SELECT city, city_cnt, array_agg(airportname) as apnames, country FROM `travel-sample` WHERE type = $where GROUP BY city, country LETTING city_cnt = count($city) ) AS t1 WHERE t1.city_cnt > 5 GROUP BY t1.country;"

	alg, _ := n1ql.ParseStatement(stmt)
	/*fmt.Printf("Got:%%#v %#v\n", alg)
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
	}*/

	isSelectQuery := false
	switch alg.(type) {
	case *algebra.Select:
		isSelectQuery = true
	}

	fmt.Printf("isSelectQuery : %v\n", isSelectQuery)
}