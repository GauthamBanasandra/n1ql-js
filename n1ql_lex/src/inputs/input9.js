function OnUpdate(doc, meta) {
    try {
        var doc_id = meta.id; // GET operation
        log('creating document for : ', doc);
        dst_bucket[doc_id] = {'doc_id' : doc_id}; // SET operation
    } catch (e) {
        log(e);
    }
}

// This is intentionally left blank


























function OnDelete(meta) {
    try {
        log('deleting document', meta.id);
        delete dst_bucket[meta.id];
    } catch (e) {
        log(e);
    }
}
