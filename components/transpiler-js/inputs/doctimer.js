function OnUpdate(doc, meta) {
    if (doc.register) {
        docTimer(fn, meta.id, '5m');
    }
}

function fn(doc) {

}