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

func (qs *queryStmt) VisitSelect(stmt *algebra.Select) (interface{}, error) {
	if qs.namedParams == nil {
		qs.namedParams = make(map[string]int)
	}

	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
		for param := range qe.namedParams {
			qs.namedParams[param] = 1
		}

	}

	return stmt, nil
}

func (qs *queryStmt) VisitInsert(stmt *algebra.Insert) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitUpsert(stmt *algebra.Upsert) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitDelete(stmt *algebra.Delete) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitUpdate(stmt *algebra.Update) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitMerge(stmt *algebra.Merge) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitCreatePrimaryIndex(stmt *algebra.CreatePrimaryIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitCreateIndex(stmt *algebra.CreateIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitDropIndex(stmt *algebra.DropIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitAlterIndex(stmt *algebra.AlterIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitBuildIndexes(stmt *algebra.BuildIndexes) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitGrantRole(stmt *algebra.GrantRole) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitRevokeRole(stmt *algebra.RevokeRole) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitExplain(stmt *algebra.Explain) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitPrepare(stmt *algebra.Prepare) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitExecute(stmt *algebra.Execute) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

func (qs *queryStmt) VisitInferKeyspace(stmt *algebra.InferKeyspace) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		qe := &queryExpr{}

		expr.Accept(qe)
	}

	return stmt, nil
}

type queryExpr struct {
	namedParams map[string]int
}

func (qe *queryExpr) VisitAdd(expr *expression.Add) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitDiv(expr *expression.Div) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitMod(expr *expression.Mod) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitMult(expr *expression.Mult) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitNeg(expr *expression.Neg) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSub(expr *expression.Sub) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSearchedCase(expr *expression.SearchedCase) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSimpleCase(expr *expression.SimpleCase) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitAny(expr *expression.Any) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitEvery(expr *expression.Every) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitAnyEvery(expr *expression.AnyEvery) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitArray(expr *expression.Array) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitFirst(expr *expression.First) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitObject(expr *expression.Object) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitExists(expr *expression.Exists) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIn(expr *expression.In) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitWithin(expr *expression.Within) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitBetween(expr *expression.Between) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitEq(expr *expression.Eq) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitLE(expr *expression.LE) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitLike(expr *expression.Like) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitLT(expr *expression.LT) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsMissing(expr *expression.IsMissing) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsNotMissing(expr *expression.IsNotMissing) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsNotNull(expr *expression.IsNotNull) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsNotValued(expr *expression.IsNotValued) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsNull(expr *expression.IsNull) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIsValued(expr *expression.IsValued) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitConcat(expr *expression.Concat) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitConstant(expr *expression.Constant) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitIdentifier(expr *expression.Identifier) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitArrayConstruct(expr *expression.ArrayConstruct) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitObjectConstruct(expr *expression.ObjectConstruct) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitAnd(expr *expression.And) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitNot(expr *expression.Not) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitOr(expr *expression.Or) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitElement(expr *expression.Element) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitField(expr *expression.Field) (interface{}, error) {
	//fmt.Printf("field expr: %v\tchildren len:%v\n", expr, len(expr.Children()))
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitFieldName(expr *expression.FieldName) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSlice(expr *expression.Slice) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSelf(expr *expression.Self) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitFunction(expr expression.Function) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitSubquery(expr expression.Subquery) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitNamedParameter(expr expression.NamedParameter) (interface{}, error) {
	if qe.namedParams == nil {
		qe.namedParams = make(map[string]int)
	}

	qe.namedParams[expr.Name()] = 1
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitPositionalParameter(expr expression.PositionalParameter) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitCover(expr *expression.Cover) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func (qe *queryExpr) VisitAll(expr *expression.All) (interface{}, error) {
	for _, expr := range expr.Children() {
		expr.Accept(qe)
	}

	return expr, nil
}

func main() {
	q := "select * from `beer-sample` where name == $name"
	alg, err := n1ql.ParseStatement(q)
	if err != nil {
		return
	}

	qv := queryStmt{}

	alg.Accept(&qv)
	fmt.Printf("qv named parameters: %v\n", qv.namedParams)
}
