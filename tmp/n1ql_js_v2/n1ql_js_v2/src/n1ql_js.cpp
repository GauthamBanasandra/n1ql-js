//
//  n1ql_js.cpp
//  n1ql_js_v2
//
//  Created by Gautham Banasandra on 15/03/17.
//  Copyright © 2017 Couchbase. All rights reserved.
//

#include "n1ql_js.hpp"
#include "jsify.hpp"
#include "utils.hpp"
#include "v8_env.hpp"
#include "query_engine.hpp"

QueryEngine *q_engine;

std::string TranspileAndExec(std::string input_source){
    std::string plain_js;
    parse(input_source.c_str(), &plain_js);
    
    std::string transpiler_js_src = N1qlUtils::ReadFile(N1qlUtils::GetTranspilerJsPath());
    transpiler_js_src += N1qlUtils::ReadFile(N1qlUtils::GetEsToolsPath());
    
    V8Env v8Env;
    std::string transpiled_js = v8Env.Build(transpiler_js_src, plain_js, EXEC_TRANSPILER);
    std::cout <<"transpiled js" << std::endl << transpiled_js << std::endl;
    
    q_engine = new QueryEngine();
    
    v8Env.ExecJs(transpiled_js);
    
    return transpiled_js;
}
