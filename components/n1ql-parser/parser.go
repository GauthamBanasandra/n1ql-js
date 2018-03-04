package main

import (
	"github.com/couchbase/query/parser/n1ql"
	"github.com/couchbase/query/algebra"
	"github.com/couchbase/query/expression"
	"fmt"
)

type queryStmt struct {
	namedParams map[string]int
}

type queryExpr struct {
	namedParams map[string]int
}

type ParseInfo struct {
	IsValid bool   `json:"is_valid"`
	Info    string `json:"info"`
}

type NamedParamsInfo struct {
	PInfo ParseInfo	`json:"p_info"`
	NamedParams	[]string `json:"named_params"`
}

func Parse(query string) (info *ParseInfo) {
	info = &ParseInfo{IsValid:true}

	_, err := n1ql.ParseStatement(query)
	if err != nil {
		info.IsValid = false
		info.Info = fmt.Sprintf("%v", err)
		return
	}

	return
}

func GetNamedParams(query string) (info *NamedParamsInfo) {
	info = &NamedParamsInfo{}
	info.PInfo.IsValid = true

	alg, err := n1ql.ParseStatement(query)
	if err != nil {
		info.PInfo.IsValid = false
		info.PInfo.Info = fmt.Sprintf("%v", err)
		return
	}

	qs := queryStmt{}
	alg.Accept(&qs)
	for namedParam := range qs.namedParams{
		info.NamedParams = append(info.NamedParams, namedParam)
	}

	return
}

func handleStmt(qs *queryStmt, expressions expression.Expressions) {
	if qs.namedParams == nil {
		qs.namedParams = make(map[string]int)
	}

	for _, expr := range expressions {
		qe := &queryExpr{}

		expr.Accept(qe)
		for param := range qe.namedParams {
			qs.namedParams[param] = 1
		}
	}
}

func handleExpr(qe *queryExpr, expressions expression.Expressions) {
	for _, expr := range expressions {
		expr.Accept(qe)
	}
}

// Visitors for N1QL statement
func (qs *queryStmt) VisitSelect(stmt *algebra.Select) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitInsert(stmt *algebra.Insert) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitUpsert(stmt *algebra.Upsert) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitDelete(stmt *algebra.Delete) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitUpdate(stmt *algebra.Update) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitMerge(stmt *algebra.Merge) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitCreatePrimaryIndex(stmt *algebra.CreatePrimaryIndex) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitCreateIndex(stmt *algebra.CreateIndex) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitDropIndex(stmt *algebra.DropIndex) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitAlterIndex(stmt *algebra.AlterIndex) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitBuildIndexes(stmt *algebra.BuildIndexes) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitGrantRole(stmt *algebra.GrantRole) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitRevokeRole(stmt *algebra.RevokeRole) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitExplain(stmt *algebra.Explain) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitPrepare(stmt *algebra.Prepare) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitExecute(stmt *algebra.Execute) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

func (qs *queryStmt) VisitInferKeyspace(stmt *algebra.InferKeyspace) (interface{}, error) {
	handleStmt(qs, stmt.Expressions())
	return stmt, nil
}

// Visitors for N1QL expressions
func (qe *queryExpr) VisitAdd(expr *expression.Add) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitDiv(expr *expression.Div) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitMod(expr *expression.Mod) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitMult(expr *expression.Mult) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitNeg(expr *expression.Neg) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSub(expr *expression.Sub) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSearchedCase(expr *expression.SearchedCase) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSimpleCase(expr *expression.SimpleCase) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitAny(expr *expression.Any) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitEvery(expr *expression.Every) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitAnyEvery(expr *expression.AnyEvery) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitArray(expr *expression.Array) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitFirst(expr *expression.First) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitObject(expr *expression.Object) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitExists(expr *expression.Exists) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIn(expr *expression.In) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitWithin(expr *expression.Within) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitBetween(expr *expression.Between) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitEq(expr *expression.Eq) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitLE(expr *expression.LE) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitLike(expr *expression.Like) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitLT(expr *expression.LT) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsMissing(expr *expression.IsMissing) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsNotMissing(expr *expression.IsNotMissing) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsNotNull(expr *expression.IsNotNull) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsNotValued(expr *expression.IsNotValued) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsNull(expr *expression.IsNull) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIsValued(expr *expression.IsValued) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitConcat(expr *expression.Concat) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitConstant(expr *expression.Constant) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitIdentifier(expr *expression.Identifier) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitArrayConstruct(expr *expression.ArrayConstruct) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitObjectConstruct(expr *expression.ObjectConstruct) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitAnd(expr *expression.And) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitNot(expr *expression.Not) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitOr(expr *expression.Or) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitElement(expr *expression.Element) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitField(expr *expression.Field) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitFieldName(expr *expression.FieldName) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSlice(expr *expression.Slice) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSelf(expr *expression.Self) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitFunction(expr expression.Function) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitSubquery(expr expression.Subquery) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitNamedParameter(expr expression.NamedParameter) (interface{}, error) {
	if qe.namedParams == nil {
		qe.namedParams = make(map[string]int)
	}

	qe.namedParams[expr.Name()] = 1
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitPositionalParameter(expr expression.PositionalParameter) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitCover(expr *expression.Cover) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func (qe *queryExpr) VisitAll(expr *expression.All) (interface{}, error) {
	handleExpr(qe, expr.Children())
	return expr, nil
}

func main() {
	//q := "select * from `beer-sample` where name == $name"
	//q := "UPDATE defaulters USE KEYS $ssn SET reminded = $dt"
	//q := "(SELECT * FROM src_bucket WHERE NOT ($NUMERIC_FIELD IS NOT NULL) ORDER BY NUMERIC_FIELD_LIST, STRING_FIELD_LIST, BOOL_FIELD_LIST DESC) UNION (SELECT * FROM src_bucket WHERE (NUMERIC_FIELD IS NULL OR (($STRING_FIELD IS NOT NULL) OR (STRING_FIELD <= STRING_VALUES)) AND (STRING_FIELD NOT BETWEEN LOWER_BOUND_VALUE and UPPER_BOUND_VALUE)) ORDER BY STRING_FIELD_LIST);"
	//q:= "MERGE INTO product p USING orders o ON KEY o.productId WHEN MATCHED THEN UPDATE SET p.lastSaleDate = o.orderDate WHEN MATCHED THEN UPDATE SET p.lastSaleDate = SomethingElse"
	//q:= "SELECT t1.city FROM `travel-sample` t1 WHERE t1.type = \"landmark\" AND t1.city IN (SELECT RAW city FROM `travel-sample` WHERE type = $type);"
	q:= "SELECT t1.country, array_agg(t1.city), sum(t1.city_cnt) as apnum FROM (SELECT city, city_cnt, array_agg(airportname) as apnames, country FROM `travel-sample` WHERE type = $where GROUP BY city, country LETTING city_cnt = count($city) ) AS t1 WHERE t1.city_cnt > 5 GROUP BY t1.country;"
	info := GetNamedParams(q)
	if !info.PInfo.IsValid {
		fmt.Print(info.PInfo.IsValid)
	}

	fmt.Printf("%v", info.NamedParams)
}
