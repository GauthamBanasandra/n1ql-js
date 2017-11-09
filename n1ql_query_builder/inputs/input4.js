function OnUpdate(doc, meta) {
	new N1qlQuery('DELETE  FROM product p USE KEYS "product10" RETURNING p');
}
function OnDelete(doc) {
}