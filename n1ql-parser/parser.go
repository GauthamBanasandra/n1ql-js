package main

import (
	"github.com/couchbase/query/parser/n1ql"
	"github.com/couchbase/query/algebra"
	"github.com/couchbase/query/expression"
	"fmt"
)

type queryStmt struct {
}

func (q queryStmt) VisitSelect(stmt *algebra.Select) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	
	return stmt, nil
}
func (q queryStmt) VisitInsert(stmt *algebra.Insert) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitUpsert(stmt *algebra.Upsert) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitDelete(stmt *algebra.Delete) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitUpdate(stmt *algebra.Update) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitMerge(stmt *algebra.Merge) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitCreatePrimaryIndex(stmt *algebra.CreatePrimaryIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitCreateIndex(stmt *algebra.CreateIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitDropIndex(stmt *algebra.DropIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitAlterIndex(stmt *algebra.AlterIndex) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitBuildIndexes(stmt *algebra.BuildIndexes) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitGrantRole(stmt *algebra.GrantRole) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitRevokeRole(stmt *algebra.RevokeRole) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitExplain(stmt *algebra.Explain) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitPrepare(stmt *algebra.Prepare) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitExecute(stmt *algebra.Execute) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}
func (q queryStmt) VisitInferKeyspace(stmt *algebra.InferKeyspace) (interface{}, error) {
	for _, expr := range stmt.Expressions() {
		fmt.Println("stmt:", expr)
		qv := queryExpr{}
		expr.Accept(qv)
	}
	return stmt, nil
}

type queryExpr struct {
}

func (q queryExpr) VisitAdd(expr *expression.Add) (interface{}, error) {
	fmt.Println("add expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitDiv(expr *expression.Div) (interface{}, error) {
	fmt.Println("div expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitMod(expr *expression.Mod) (interface{}, error) {
	fmt.Println("mod expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitMult(expr *expression.Mult) (interface{}, error) {
	fmt.Println("mult expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitNeg(expr *expression.Neg) (interface{}, error) {
	fmt.Println("neg expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSub(expr *expression.Sub) (interface{}, error) {
	fmt.Println("sub expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSearchedCase(expr *expression.SearchedCase) (interface{}, error) {
	fmt.Println("searched case expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSimpleCase(expr *expression.SimpleCase) (interface{}, error) {
	fmt.Println("simple case expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitAny(expr *expression.Any) (interface{}, error) {
	fmt.Println("any expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitEvery(expr *expression.Every) (interface{}, error) {
	fmt.Println("every expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitAnyEvery(expr *expression.AnyEvery) (interface{}, error) {
	fmt.Println("every expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitArray(expr *expression.Array) (interface{}, error) {
	fmt.Println("array expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitFirst(expr *expression.First) (interface{}, error) {
	fmt.Println("first expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitObject(expr *expression.Object) (interface{}, error) {
	fmt.Println("object expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitExists(expr *expression.Exists) (interface{}, error) {
	fmt.Println("exists expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIn(expr *expression.In) (interface{}, error) {
	fmt.Println("in expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitWithin(expr *expression.Within) (interface{}, error) {
	fmt.Println("within expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitBetween(expr *expression.Between) (interface{}, error) {
	fmt.Println("betweenexpr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitEq(expr *expression.Eq) (interface{}, error) {
	fmt.Println("eq expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitLE(expr *expression.LE) (interface{}, error) {
	fmt.Println("le expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitLike(expr *expression.Like) (interface{}, error) {
	fmt.Println("like expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitLT(expr *expression.LT) (interface{}, error) {
	fmt.Println("lt expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsMissing(expr *expression.IsMissing) (interface{}, error) {
	fmt.Println("is missing expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsNotMissing(expr *expression.IsNotMissing) (interface{}, error) {
	fmt.Println("is not missing expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsNotNull(expr *expression.IsNotNull) (interface{}, error) {
	fmt.Println("not null expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsNotValued(expr *expression.IsNotValued) (interface{}, error) {
	fmt.Println("not valued expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsNull(expr *expression.IsNull) (interface{}, error) {
	fmt.Println("is null expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIsValued(expr *expression.IsValued) (interface{}, error) {
	fmt.Println("is valued expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitConcat(expr *expression.Concat) (interface{}, error) {
	fmt.Println("concat expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitConstant(expr *expression.Constant) (interface{}, error) {
	fmt.Println("constant expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitIdentifier(expr *expression.Identifier) (interface{}, error) {
	fmt.Println("identifier expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitArrayConstruct(expr *expression.ArrayConstruct) (interface{}, error) {
	fmt.Println("array construct expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitObjectConstruct(expr *expression.ObjectConstruct) (interface{}, error) {
	fmt.Println("object construct expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitAnd(expr *expression.And) (interface{}, error) {
	fmt.Println("and expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitNot(expr *expression.Not) (interface{}, error) {
	fmt.Println("not expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitOr(expr *expression.Or) (interface{}, error) {
	fmt.Println("or expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitElement(expr *expression.Element) (interface{}, error) {
	fmt.Println("element expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitField(expr *expression.Field) (interface{}, error) {
	//fmt.Printf("field expr: %v\tchildren len:%v\n", expr, len(expr.Children()))
	fmt.Println("field expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitFieldName(expr *expression.FieldName) (interface{}, error) {
	fmt.Println("field name expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSlice(expr *expression.Slice) (interface{}, error) {
	fmt.Println("slice expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSelf(expr *expression.Self) (interface{}, error) {
	fmt.Println("self expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitFunction(expr expression.Function) (interface{}, error) {
	fmt.Println("function expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitSubquery(expr expression.Subquery) (interface{}, error) {
	fmt.Println("subquery expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitNamedParameter(expr expression.NamedParameter) (interface{}, error) {
	//fmt.Printf("named parameter expr: %v\tchildren len:%v\n", expr, len(expr.Children()))
	fmt.Println("named parameter expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitPositionalParameter(expr expression.PositionalParameter) (interface{}, error) {
	fmt.Println("positional parameter expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitCover(expr *expression.Cover) (interface{}, error) {
	fmt.Println("cover expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}
func (q queryExpr) VisitAll(expr *expression.All) (interface{}, error) {
	fmt.Println("all expr:", expr)
	for _, expr := range expr.Children() {
		qv := queryExpr{}
		expr.Accept(qv)
	}

	return expr, nil
}

func main() {
	q := "select * from `beer-sample` where name == $name"
	ast, err := n1ql.ParseStatement(q)
	if err != nil {
		fmt.Println(err)
		return
	}

	qv := queryStmt{}
	ast.Accept(qv)

	fmt.Println(ast)
}
