var ins = new N1qlQuery("UPSERT INTO sample (KEY, VALUE) VALUES(\"key\", \"{\\\"k\\\":\\\"v\\\"}\");");
ins.execQuery();