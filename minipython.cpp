#include <iostream>
#include <fstream>
#include <regex>
#include <string>

/*

Team members:
Yihui Lin(Vincent Lin), PSID:2006325, CS82
Chengxin Lyu          , PSID:2027831, CS83

*/

// using namespace std;
std::ofstream error_log("error.log");
std::vector<std::pair<std::string, int>> variables;
std::vector<std::pair<std::string, int>> variables_d;
std::vector<std::pair<std::string, std::vector<int>>> l_variables;
std::vector<std::pair<std::string, std::vector<int>>> l_variables_d;
std::vector<std::string> COMPARER = {">=", "<=", ">", "<", "!=", "=="};

bool is_if = false;
bool TorF = false;
bool runelse = false;
bool def = false;
bool function_exist;
bool inmain = false;

std::vector<std::string> symbols = {"+", "-", "*", "/"};
std::string TEM;
std::string MSG_in_line = "";
std::string T = "";
std::vector<std::string> BUFFERPRINT;
std::vector<std::string> var;
std::vector<std::string> varR;
std::string BUFFER;
std::vector<int> BUFFERlist;
std::vector<std::string> codefrompy;

std::string VAR_UNDEFINED = "ERROR: Contain undefined variable in line: ";
std::string SYNTAX_ERROR = "ERROR: Syntax error in line: ";
std::string OUTRANGE = "ERROR: Index out of range in line: ";
std::string DATA_TYPE_ERROR = "ERROR: Data type do not match in line: ";
std::string FUNC_ERROR = "ERROR: Contain undefined function in line: ";

int find_lineNo(std::string MSG){
    for (int lineNo = 1; lineNo <= codefrompy.size(); lineNo++)
    {
        if(codefrompy[lineNo-1].find(MSG)!=-1){
            return lineNo;
        }
    }
    return 0;
}

std::string kick_out_useless_space(std::string target)
{
    target.erase(target.find_last_not_of(' ') + 1);
    target.erase(0, target.find_first_not_of(' '));
    return target;
}

bool funcExist(std::string MSG)
{
    std::string STR_BUFF;
    if(MSG.find("=")!=-1){// yes =
        STR_BUFF = MSG.substr(MSG.find("=")+1, MSG.find("(")-MSG.find("=")-1);
        STR_BUFF = kick_out_useless_space(STR_BUFF);
        for(int i =0; i<codefrompy.size();i++){
            if(codefrompy[i].find(STR_BUFF)!=-1){ //function exist
                return true;
            }
        }
    }
    else{
        STR_BUFF = MSG.substr(0, MSG.find("("));
        STR_BUFF = kick_out_useless_space(STR_BUFF);
        for(int i =0; i<codefrompy.size();i++){
            if(codefrompy[i].find(STR_BUFF)!=-1){ //function exist
                return true;
            }
        }
    }
    return false;
}

std::vector<std::string> split(const std::string &s, const std::string &seperator)
{ // https://www.cnblogs.com/dfcao/p/cpp-FAQ-split.html
    std::vector<std::string> result;
    typedef std::string::size_type string_size;
    int i = 0;

    while (i != s.size())
    {
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
        }
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0)
        {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            if (flag == 0)
                ++j;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}

bool is_number(const std::string &s)
{ // https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

std::vector<int> getList(std::string input)
{
    std::vector<std::string> lis;
    std::vector<int> ANS = {};
    //std::vector<int> ANS;
    int start_pos = input.find('[');
    int end_pos = input.find(']');
    std::string arr = input.substr(start_pos + 1, (end_pos - start_pos) - 1);
    lis = split(arr, ",");
    for (int i = 0; i < lis.size(); i++)
    {
        ANS.push_back(stoi(lis[i]));
    }
    return ANS;
}

int findTarget(std::string target, bool TF)
{
    if(!TF){
        for (int i = 0; i < variables.size(); i++)
        {
            if (variables[i].first == target)
            {
                return i;
            }
        }
        for (int i = 0; i < l_variables.size(); i++)
        {
            if (l_variables[i].first == target)
            {
                return i;
            }
        }
    }
    else{
        for (int i = 0; i < variables_d.size(); i++)
        {
            if (variables_d[i].first == target)
            {
                return i;
            }
        }
        for (int i = 0; i < l_variables_d.size(); i++)
        {
            if (l_variables_d[i].first == target)
            {
                return i;
            }
        }
    }
    
    return -1;
}

int getIndex(std::string input)
{
    int index;
    int start_pos = input.find('[');
    int end_pos = input.find(']');
    if(input.find(":")!=-1){
        if(input.find("[:]")!=-1){
            return -1;
        }
        else{ // [1:]
            end_pos = input.find(':');
        }
    }
    
    if (is_number(input.substr(start_pos + 1, (end_pos - start_pos) - 1)) != 1)
    {
        if(!def){
            index = variables[findTarget(input.substr(start_pos + 1, (end_pos - start_pos) - 1),def)].second;
        }
        else{
            index = variables_d[findTarget(input.substr(start_pos + 1, (end_pos - start_pos) - 1),def)].second;
        }
        
    }
    else
    {
        index = stoi(input.substr(start_pos + 1, (end_pos - start_pos) - 1));
    }
    
    

    return index;
}

std::string findVector(std::string target, bool TF)
{
    if(!TF){
        for (int i = 0; i < variables.size(); i++)
        {
            if (variables[i].first == target)
            {
                return "variables";
            }
        }
        for (int i = 0; i < l_variables.size(); i++)
        {
            if (l_variables[i].first == target)
            {
                return "l_variables";
            }
        }
    }
    else{
        for (int i = 0; i < variables_d.size(); i++)
        {
            if (variables_d[i].first == target)
            {
                return "variables_d";
            }
        }
        for (int i = 0; i < l_variables_d.size(); i++)
        {
            if (l_variables_d[i].first == target)
            {
                return "l_variables_d";
            }
        }
    }
    
    return " ";
}

bool ifstatement(int left, int mid, int right)
{
    switch (mid)
    {
    case 0:
        if (left >= right)
            return true;
        else
            return false;
        break;
    case 1:
        if (left <= right)
            return true;
        else
            return false;
        break;
    case 2:
        if (left > right)
            return true;
        else
            return false;
        break;
    case 3:
        if (left < right)
            return true;
        else
            return false;
        break;
    case 4:
        if (left != right)
            return true;
        else
            return false;
        break;
    case 5:
        if (left == right)
            return true;
        else
            return false;
        break;
    default:
        return NULL;
    }
}

int handleError(std::string MSG, int lineno)
{
    if (MSG == "")
        return 0;
    if (MSG.starts_with("#") || MSG.starts_with("print")||MSG.starts_with("return"))
        return 0;
    if (MSG.starts_with("def"))
    {
        if (MSG.find(":") == -1)
        { 
            error_log << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
            std::cout << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
            exit(0);
        }
        return 0;
    }
    if(def && MSG_in_line.starts_with(" ")&&!inmain){
        return 0;
    }
    else if(def && !MSG_in_line.starts_with(" ")&&!inmain){
        def = false;
        return 0;
    }
    if(MSG.find("(")!=-1 && MSG.find("print")==-1){
        if(funcExist){
            return 0;
        }
        else{
            error_log << FUNC_ERROR << find_lineNo(MSG) << "\n";
            std::cout << FUNC_ERROR << find_lineNo(MSG) << "\n";
            exit(0);
        }
    }
    if(MSG.find("=")!=-1 && MSG.find("+")!=-1 && MSG.find("[")==-1){// yes =, yes +, no []
        std::vector<std::string> str_R;
        bool haveNo = false;
        str_R=split(MSG,"=");
        str_R=split(str_R[1],"+");

        for(int i = 0; i <str_R.size();i++){
            str_R[i]=kick_out_useless_space(str_R[i]);
        }
        for(int i = 0; i <str_R.size();i++){
            if(is_number(str_R[i])){
                haveNo = true;
                break;
            }
        }
        if(haveNo){
            for(int i = 0; i <str_R.size();i++){
                if(i!=str_R.size()){
                    if(!def){
                        if(findVector(str_R[i],0)!="variables" && !is_number(str_R[i])){
                            error_log << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";
                            std::cout << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";
                            exit(0);
                        }
                    }
                    else{
                        if(findVector(str_R[i],1)!="variables_d" && !is_number(str_R[i])){
                            error_log << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";
                            std::cout << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";

                            exit(0);
                        }
                    }
                }
            }
        }
        else{
            for(int i = 0; i <str_R.size();i++){
                if(i!=str_R.size()-1){
                    if(findVector(str_R[i],def)!=findVector(str_R[i+1],def)){
                        error_log << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";
                        std::cout << DATA_TYPE_ERROR << find_lineNo(MSG) << "\n";
                        exit(0);
                    }
                    
                }
            }
        }
        return 0;
    }
    std::vector<std::string> varR;
    if (count(MSG.begin(), MSG.end(), '(') != count(MSG.begin(), MSG.end(), ')'))
    {
        error_log << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        std::cout << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        exit(0);
    }
    // if(count(MSG.begin(),MSG.end(),'"')%2 != 0 || count(MSG.begin(),MSG.end(),"\'")%2 != 0){
    if (count(MSG.begin(), MSG.end(), '"') % 2 != 0)
    {
        error_log << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        std::cout << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        exit(0);
    }
    if (count(MSG.begin(), MSG.end(), '[') != count(MSG.begin(), MSG.end(), ']'))
    {
        error_log << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        std::cout << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
        exit(0);
    }
    std::string for_if_only;
    for_if_only=kick_out_useless_space(MSG);
    if (for_if_only.starts_with("if"))
    {
        if (MSG.find(":") == -1)
        { 
            error_log << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
            std::cout << SYNTAX_ERROR << find_lineNo(MSG) << "\n";
            exit(0);
        }
        return 0;
    }
    
    if (MSG.find('=') != -1)
    {
        varR = split(MSG, "=");
        varR[1] = kick_out_useless_space(varR[1]);
        varR[0] = kick_out_useless_space(varR[0]);

        if (varR[1].find("+") == -1)
        { // no +
            if (varR[1].starts_with("["))
            {
                return 0;
            }

            if (varR[1].find('[') != -1)
            { // at least one [] at the right (variable with [])
                if (varR[1].find("[:]")==-1) { // no [:]
                    int index = getIndex(varR[1]);
                    varR[1] = varR[1].substr(0, varR[1].find("["));
                    if (findVector(varR[1],def) == " ")
                    { // no this list exist
                        error_log << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        std::cout << VAR_UNDEFINED << find_lineNo(MSG) << " \n";
                        exit(0);
                    }
                    else
                    { // this list is exist
                        if(!def){
                            if (index > l_variables[findTarget(varR[1],def)].second.size() - 1)
                            { // index out of range
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                        }
                        else{
                            if(index > l_variables_d[findTarget(varR[1],def)].second.size() - 1){
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                        }
                        
                    }
                }
            }
            if (varR[0].find('[') != -1)
            { // access list by index on the left variable (a[i] = 123)
                int index = getIndex(varR[0]);
                varR[0] = varR[0].substr(0, varR[0].find("["));
                if (findVector(varR[0],def) == " ")
                { // no this list exist
                    error_log << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                    std::cout << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                    exit(0);
                }
                else
                { // this list is exist
                    if(!def){
                        if (index > l_variables[findTarget(varR[0],def)].second.size() - 1)
                        { // index out of range
                            error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                            std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                            exit(0);
                        }
                    }
                    else{
                        if(index > l_variables_d[findTarget(varR[0],def)].second.size() - 1)
                        {
                            { // index out of range
                            error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                            std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                            exit(0);
                        }
                        }
                    }
                    
                }
            }


            if (is_number(varR[1]) != 1)
            { // not a digit
                if(varR[1].find("[")==-1){//no []on the right
                    if (findVector(varR[1],def) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        std::cout << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        exit(0);
                    }
                }
                else{// yes [] on the right
                    std::string listName = varR[1].substr(0, (varR[1].find("[")));
                    if (findVector(listName,def) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        std::cout << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        exit(0);
                    }
                }    
            }
        }
        else
        { // yes +
            varR = split(varR[1], "+");
            for (int i = 0; i < varR.size(); i++)
            {
                varR[i] = kick_out_useless_space(varR[i]);
                if (varR[i].starts_with("["))
                {
                    return 0;
                }
                if (varR[i].find('[') != -1)
                { // variable with []
                    int index = getIndex(varR[i]);
                    varR[i] = varR[i].substr(0, varR[i].find("["));
                    if (findVector(varR[i],def) == " ")
                    { // no this list exist
                        error_log << VAR_UNDEFINED << find_lineNo(MSG) << " \n";
                        std::cout << VAR_UNDEFINED << find_lineNo(MSG) << " \n";
                        exit(0);
                    }
                    else
                    { // this list is exist
                        if(!def){
                            if (index > l_variables[findTarget(varR[i],def)].second.size() - 1)
                            { // index out of range
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                            if (index > l_variables[findTarget(varR[i],def)].second.size() - 1){
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                        }
                        else{
                            if(index > l_variables_d[findTarget(varR[i],def)].second.size() - 1){
                                 // index out of range
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                            if (index > l_variables_d[findTarget(varR[i],def)].second.size() - 1){
                                error_log << OUTRANGE << find_lineNo(MSG) << "\n";
                                std::cout << OUTRANGE << find_lineNo(MSG) << "\n";
                                exit(0);
                            }
                        }
                    }
                }
                if (is_number(varR[i]) != 1)
                { // not a digit
                    if (findVector(varR[i],def) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        std::cout << VAR_UNDEFINED << find_lineNo(MSG) << "\n";
                        exit(0);
                    }
                }
            }
        }
    }
    return 0;
}

void main_function(std::vector<std::string> codefrompy);

void run_function_N(std::string MSG){
    def= true;
    //inmain = false;
    std::string function_name = MSG.substr(0,MSG.find("("));
    std::string parameter_name = MSG.substr(MSG.find("(")+1,MSG.find(")")-MSG.find("(")-1);
    std::vector<std::string> parameter_name_list;
    if(parameter_name.find(",")!=-1){//more than 1 parameter
        parameter_name_list=split(parameter_name,",");
    }
    else{// only one parameter
        parameter_name_list.push_back(parameter_name);
    }
    std::vector<std::string> function_content;
    for(int i =0; i <codefrompy.size();i++){
        if(codefrompy[i].find(function_name)!=-1){
            std::string parameter_name_with_def = codefrompy[i].substr(codefrompy[i].find("(")+1,codefrompy[i].find(")")-codefrompy[i].find("(")-1);
            std::vector<std::string> parameter_name_list_with_def;
            if(parameter_name.find(",")!=-1){//more than 1 parameter
                parameter_name_list_with_def=split(parameter_name_with_def,",");
            }
            else{// only one parameter
                parameter_name_list_with_def.push_back(parameter_name_with_def);
            }
            for(int q=0;q<parameter_name_list_with_def.size();q++){
                if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    if(findVector(parameter_name_list_with_def[q],1)!=" "){
                        variables_d[findTarget(parameter_name_list_with_def[q],1)].second=variables[findTarget(parameter_name_list[q],0)].second;
                        continue;
                    }
                    else if(findVector(parameter_name_list[q],0)=="variables"){//parameter is a variable
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q],0)].second));
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                    }
                    else{//parameter is a list
                        l_variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q],0)].second));
                    }
                }
            }
            for(i=i+1;i <codefrompy.size();i++){
                if(!codefrompy[i].starts_with(" ")||all_of(codefrompy[i].begin(),codefrompy[i].end(),isspace))
                    break;
                function_content.push_back(codefrompy[i]);
            }
            break;
        }
    }
    main_function(function_content);
    def=false;
    //inmain = true;
}

std::vector<int> run_function_L(std::string MSG){
    def = true;
    //inmain = false;
    std::string function_name = MSG.substr(0,MSG.find("("));
    std::string parameter_name = MSG.substr(MSG.find("(")+1,MSG.find(")")-MSG.find("(")-1);
    std::vector<std::string> parameter_name_list;
    if(parameter_name.find(",")!=-1){//more than 1 parameter
        parameter_name_list=split(parameter_name,",");
    }
    else{// only one parameter
        parameter_name_list.push_back(parameter_name);
    }
    std::vector<std::string> function_content;
    std::string last_line_of_function;
    std::vector<std::string> last_line_of_function_list;
    for(int i =0; i <codefrompy.size();i++){

        if(codefrompy[i].find(function_name)!=-1){
            std::string parameter_name_with_def = codefrompy[i].substr(codefrompy[i].find("(")+1,codefrompy[i].find(")")-codefrompy[i].find("(")-1);
            std::vector<std::string> parameter_name_list_with_def;
            if(parameter_name.find(",")!=-1){//more than 1 parameter
                parameter_name_list_with_def=split(parameter_name_with_def,",");
            }
            else{// only one parameter
                parameter_name_list_with_def.push_back(parameter_name_with_def);
            }
            for(int q=0;q<parameter_name_list_with_def.size();q++){
                //if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    if(findVector(parameter_name_list_with_def[q],1)!=" "){
                        if(findVector(parameter_name_list[q],0)=="variables"){
                            variables_d[findTarget(parameter_name_list_with_def[q],1)].second=variables[findTarget(parameter_name_list[q],0)].second;
                        }
                        else if(findVector(parameter_name_list[q],0)=="l_variables"){
                            l_variables_d[findTarget(parameter_name_list_with_def[q],1)].second=l_variables[findTarget(parameter_name_list[q],0)].second;
                        }
                        
                    }
                    else if(findVector(parameter_name_list[q],0)=="variables"){//parameter is a variable
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q],0)].second));
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                    }
                    else{//parameter is a list
                        l_variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q],0)].second));
                    }
                //}
            }
            for(i=i+1;i <codefrompy.size();i++){
                if(!codefrompy[i].starts_with(" ")||all_of(codefrompy[i].begin(),codefrompy[i].end(),isspace))
                    break;
                function_content.push_back(codefrompy[i]);
            }
            break;
        }
    }
    main_function(function_content);
    last_line_of_function=function_content[function_content.size()-1];
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    last_line_of_function_list = split(last_line_of_function," ");

    def=false;
    //inmain=true;
    return l_variables_d[findTarget(last_line_of_function_list[1],1)].second;
}

int run_function(std::string MSG){
    def=true;
    //inmain=false;
    std::string function_name = MSG.substr(0,MSG.find("("));
    std::string parameter_name = MSG.substr(MSG.find("(")+1,MSG.find(")")-MSG.find("(")-1);
    std::vector<std::string> parameter_name_list;
    if(parameter_name.find(",")!=-1){//more than 1 parameter
        parameter_name_list=split(parameter_name,",");
    }
    else{// only one parameter
        parameter_name_list.push_back(parameter_name);
    }
    std::vector<std::string> function_content;
    std::string last_line_of_function;
    std::vector<std::string> last_line_of_function_list;
    for(int i =0; i <codefrompy.size();i++){

        if(codefrompy[i].find(function_name)!=-1){
            std::string parameter_name_with_def = codefrompy[i].substr(codefrompy[i].find("(")+1,codefrompy[i].find(")")-codefrompy[i].find("(")-1);
            std::vector<std::string> parameter_name_list_with_def;
            if(parameter_name.find(",")!=-1){//more than 1 parameter
                parameter_name_list_with_def=split(parameter_name_with_def,",");
            }
            else{// only one parameter
                parameter_name_list_with_def.push_back(parameter_name_with_def);
            }
            for(int q=0;q<parameter_name_list_with_def.size();q++){
                //if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    if(findVector(parameter_name_list_with_def[q],1)!=" "){
                        
                        if(findVector(parameter_name_list[q],0)=="variables")//parameter is a variable
                        {
                            variables_d[findTarget(parameter_name_list_with_def[q],1)].second=variables[findTarget(parameter_name_list[q],0)].second;
                        }
                        else if(is_number(parameter_name_list[q])==1){//parameter is a number
                            variables_d[findTarget(parameter_name_list_with_def[q],1)].second=stoi(parameter_name_list[q]);    
                        }
                        else{//parameter is a list
                            l_variables_d[findTarget(parameter_name_list_with_def[q],1)].second = l_variables[findTarget(parameter_name_list[q],0)].second;
                        }
                    }
                    else{
                        if(findVector(parameter_name_list[q],0)=="variables"){//parameter is a variable
                            variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q],0)].second));
                        }
                        else if(is_number(parameter_name_list[q])==1){//parameter is a number
                            variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                        }
                        else{//parameter is a list
                            l_variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q],0)].second));
                        }
                    }
                    
                //}
            }
            for(i=i+1;i <codefrompy.size();i++){
                if(!codefrompy[i].starts_with(" ")||all_of(codefrompy[i].begin(),codefrompy[i].end(),isspace))
                    break;
                function_content.push_back(codefrompy[i]);
            }
            break;
        }
    }
    main_function(function_content);
    last_line_of_function=function_content[function_content.size()-1];
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    last_line_of_function_list = split(last_line_of_function," ");
    if(last_line_of_function.find("+")!=-1){
        int idkwmid=0;
        last_line_of_function=last_line_of_function.substr(last_line_of_function.find(" ")+1);
        last_line_of_function_list=split(last_line_of_function,"+");
        for(int z=0;z<last_line_of_function_list.size();z++){
            last_line_of_function_list[z]=kick_out_useless_space(last_line_of_function_list[z]);
        }
        for(int x=0;x<last_line_of_function_list.size();x++){
            idkwmid+=variables_d[findTarget(last_line_of_function_list[x],1)].second;
        }
        def=false;
        //inmain=true;
        return idkwmid;
    }
    else{
        for(int i =0;i<variables_d.size();i++){
            //std::cout<<"checkpoint 664 "<<variables_d[i].first<<": "<<variables_d[i].second<<" n\n"; 
        }
        def=false;
        //inmain=true;
        return variables_d[findTarget(last_line_of_function_list[1],1)].second;
    }
}

std::string to_check_the_return_type(std::string MSG){
    def= true;
    //inmain=false;
    std::string function_name = MSG.substr(0,MSG.find("("));
    std::string parameter_name = MSG.substr(MSG.find("(")+1,MSG.find(")")-MSG.find("(")-1);
    std::vector<std::string> parameter_name_list;
    std::vector<std::string> function_content;
    std::string last_line_of_function;
    std::vector<std::string> last_line_of_function_list;
    if(parameter_name.find(",")!=-1){//more than 1 parameter
        parameter_name_list=split(parameter_name,",");
    }
    else{// only one parameter
        parameter_name_list.push_back(parameter_name);
    }
    for(int i =0; i <codefrompy.size();i++){
        if(codefrompy[i].find(function_name)!=-1){
            std::string parameter_name_with_def = codefrompy[i].substr(codefrompy[i].find("(")+1,codefrompy[i].find(")")-codefrompy[i].find("(")-1);
            std::vector<std::string> parameter_name_list_with_def;
            if(parameter_name.find(",")!=-1){//more than 1 parameter
                parameter_name_list_with_def=split(parameter_name_with_def,",");
            }
            else{// only one parameter
                parameter_name_list_with_def.push_back(parameter_name_with_def);
            }
            for(int q=0;q<parameter_name_list_with_def.size();q++){
                //if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    if(findVector(parameter_name_list[q],0)=="variables"){//parameter is a variable
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q],0)].second));
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));  
                    }
                    else{//parameter is a list
                        l_variables_d.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q],0)].second));
                    }
                //}
            }
            for(i=i+1;i <codefrompy.size();i++){
                if(!codefrompy[i].starts_with(" ")||all_of(codefrompy[i].begin(),codefrompy[i].end(),isspace))
                    break;
                function_content.push_back(codefrompy[i]);
            }
            break;
        }
    }
    last_line_of_function=function_content[function_content.size()-1];
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    last_line_of_function_list = split(last_line_of_function," ");
    
    if(findVector(last_line_of_function_list[1],1)==" "){
        main_function(function_content);
    }
    std::string returnvalue=findVector(last_line_of_function_list[1],1);
    def=false;
    //inmain = true;
    return returnvalue;
}

void main_function(std::vector<std::string> codefrompy)
{
    for (int lineNo = 1; lineNo <= codefrompy.size(); lineNo++)
    {
        //std::cout << lineNo << " n\n";        /////////////////////////////////////////////////////////////////////////////////////////////////////////// lineno
        int tmp = 0;
        BUFFERlist = {};
        MSG_in_line = codefrompy[lineNo - 1];
        handleError(MSG_in_line, lineNo);
        if (MSG_in_line.starts_with("#"))
        {
            continue;
        }
        if(!MSG_in_line.starts_with(" ")&&!MSG_in_line.starts_with("def")&&MSG_in_line!=""){
            inmain = true;
        }
            
        if(def && MSG_in_line.starts_with(" ")&&!inmain){
            continue;
        }
        else if(def && !MSG_in_line.starts_with(" ")&&inmain){
            def = false;
        }
        if(is_if && !MSG_in_line.starts_with(" ") && MSG_in_line.find("else") == -1){
            is_if = false;
        }
        MSG_in_line = kick_out_useless_space(MSG_in_line);
        // var = {A:1, B:2} cout << var[1].first = B
        if(MSG_in_line.starts_with("def")){
            def=true;
            continue;
        }
        if(MSG_in_line.starts_with("return")){
            return ;
        }    
        if (MSG_in_line.find("else") != -1)
        {
            runelse = true;
            continue;
        }
        if (is_if)
        {
            if (!TorF && !runelse)
            {
                continue;
            }
            else if (TorF && runelse)
            {
                if (!codefrompy[lineNo].starts_with(" "))
                {
                    is_if = false;
                    TorF = false;
                    runelse = false;
                }
                continue;
            }
            else if (!codefrompy[lineNo].starts_with(" ") && !codefrompy[lineNo].starts_with("else"))
            {
                is_if = false;
                TorF = false;
                runelse = false;
            }   
            else if(codefrompy[lineNo-1].find("if")!=-1){
                runelse = false;
            }
        }
        if (MSG_in_line.starts_with("print"))
        { // printtttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt
            if (MSG_in_line.find(",") != -1) // print more than one parameters
            {
                MSG_in_line = MSG_in_line.substr(MSG_in_line.find("(") + 1, MSG_in_line.find(")") - MSG_in_line.find("(") - 1);
                BUFFERPRINT = split(MSG_in_line, ",");
                TEM = BUFFERPRINT[0];
                TEM = TEM.substr(TEM.find_first_of('"') + 1, TEM.find_last_of('"') - 1);
                std::cout << TEM<<" ";
                T = BUFFERPRINT[1];
                T=kick_out_useless_space(T);

                if (findVector(T,0) == "l_variables"&&!def){
                    std::cout << "[";
                    for (int i = 0; i < l_variables[findTarget(T,def)].second.size(); i++)
                    {
                        std::cout << l_variables[findTarget(T,def)].second[i];
                        if (i != l_variables[findTarget(T,def)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
                else if(findVector(T,1) == "l_variables_d"&&def){
                    std::cout << "[";
                    for (int i = 0; i < l_variables_d[findTarget(T,def)].second.size(); i++)
                    {
                        std::cout << l_variables_d[findTarget(T,def)].second[i];
                        if (i != l_variables_d[findTarget(T,def)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
                
                else{
                    if (findVector(T,0) == "variables"&&!def)
                    {
                        std::cout << variables[findTarget(T,def)].second << "\n";
                    }
                    else if(findVector(T,1) == "variables_d"&&def){
                        std::cout << variables_d[findTarget(T,def)].second << "\n";
                    }
                }
            }
            else // only one parameter in print()
            {
                T = MSG_in_line.substr(MSG_in_line.find("(") + 1, MSG_in_line.find(")") - MSG_in_line.find("(") - 1);
                if (findVector(T,0) == "variables"&&!def)
                {
                    std::cout << variables[findTarget(T,def)].second << "\n";
                }
                else if(findVector(T,1) == "variables_d"&&def){
                    std::cout << variables_d[findTarget(T,def)].second << "\n";
                }
                else if (findVector(T,0) == "l_variables"&&!def)
                {
                    std::cout << "[";
                    for (int i = 0; i < l_variables[findTarget(T,def)].second.size(); i++)
                    {
                        std::cout << l_variables[findTarget(T,def)].second[i];
                        if (i != l_variables[findTarget(T,def)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
                else{
                    std::cout << "[";
                    for (int i = 0; i < l_variables_d[findTarget(T,def)].second.size(); i++)
                    {
                        std::cout << l_variables_d[findTarget(T,def)].second[i];
                        if (i != l_variables_d[findTarget(T,def)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
            }
        }
        else if (count(MSG_in_line.begin(), MSG_in_line.end(), '=') == 1 && !MSG_in_line.starts_with("if"))
        { // assign values (only 1 "=")
            var = split(MSG_in_line, "=");
            if (MSG_in_line.find('+') == -1)
            { // no +-*/ (加+减-乘*除/)
                for (int j = 0; j < var.size(); j++)
                {
                    var[j] = kick_out_useless_space(var[j]);
                }
                if (var[0].find('[') == -1)
                { // no [] on left side
                    if (var[1].find('[') != -1)
                    { // at least one [] on the right side
                        if (var[1].starts_with("["))
                        {
                            if(var[1].find(",")==-1){ // only one element inside []
                                BUFFERlist=getList(var[1]);
                            }    
                            else{
                                var[1]=var[1].substr(1,var[1].find("]")-1);
                                std::vector<std::string> content=split(var[1],",");
                                for(int q=0;q<content.size();q++){
                                    if(is_number(content[q])==1){
                                        BUFFERlist.push_back(stoi(content[q]));
                                    }
                                    else{
                                        if(def){
                                            BUFFERlist.push_back(variables_d[findTarget(content[q],def)].second);
                                        }
                                        else{
                                            BUFFERlist.push_back(variables[findTarget(content[q],def)].second);
                                        }    
                                    }
                                }
                            }
                            if(!def){
                                if(findVector(var[0],0)=="l_variables"){
                                    l_variables[findTarget(var[0],def)].second=BUFFERlist;
                                }
                                else{
                                    l_variables.push_back(std::make_pair(var[0],BUFFERlist));
                                }
                            }
                            else{
                                if(findVector(var[0],1)=="l_variables_d"){
                                    l_variables_d[findTarget(var[0],def)].second=BUFFERlist;
                                }
                                else{
                                    l_variables_d.push_back(std::make_pair(var[0],BUFFERlist));
                                }
                            }
                        }
                        else if (var[1].find("[:]")!=-1 ){
                            var[1]=var[1].substr(0,(var[1].find("[")));
                            if(!def){
                                l_variables.push_back(std::make_pair(var[0], l_variables[findTarget(var[1],def)].second));
                            }
                            else{
                                l_variables_d.push_back(std::make_pair(var[0], l_variables_d[findTarget(var[1],def)].second));
                            }
                            
                        }
                        else
                        { // element = list[i]
                            int index = getIndex(var[1]);

                            std::string listName = var[1].substr(0, (var[1].find("[")));
                            int pos = findTarget(listName,def);
                            int value;
                            if(!def){
                                value = l_variables[pos].second[index];
                                if(findVector(var[0],0)==" "){
                                    variables.push_back(std::make_pair(var[0], value));
                                }
                                else{
                                    variables[findTarget(var[0],def)].second=value;
                                }
                            }
                            else{
                                value = l_variables_d[pos].second[index];
                                if(findVector(var[0],1)==" "){
                                    variables_d.push_back(std::make_pair(var[0], value));
                                }
                                else{
                                    variables_d[findTarget(var[0],def)].second=value;
                                }
                            }
                            
                        }
                    }
                    else // no +-*/, no [] on right
                    {
                        if(var[1].find("len(")!=-1){
                            std::string listName= var[1].substr(var[1].find("(")+1,var[1].find(")")-var[1].find("(")-1);
                            if(!def){
                                if(findVector(var[0],0)=="variables"){
                                    variables[findTarget(var[0],def)].second=l_variables[findTarget(listName,def)].second.size();
                                }
                                else{
                                    variables.push_back(std::make_pair(var[0],l_variables[findTarget(listName,def)].second.size()));
                                }
                            }
                            else{
                                if(findVector(var[0],1)=="variables_d"){
                                    variables_d[findTarget(var[0],def)].second=l_variables_d[findTarget(listName,def)].second.size();
                                }
                                else{
                                    variables_d.push_back(std::make_pair(var[0],l_variables_d[findTarget(listName,def)].second.size()));
                                }
                            }
                        }
                        else if(var[1].find("(")!=-1){//yes def on the right              
                            std::string functionname = var[1];
                            std::string variable_on_the_L = var[0];
                            std::string qwer=to_check_the_return_type(var[1]);
                            
                            if(qwer=="variables_d"){
                                qwer="variables";
                            }
                            else if(qwer=="l_variables_d"){
                                qwer="l_variables";
                            }
                            if(!def){
                                if(qwer=="variables"){//function return variables
                                    if(findVector(variable_on_the_L,0)=="variables"){
                                        variables[findTarget(variable_on_the_L,def)].second=run_function(functionname);
                                    }
                                    else{
                                        variables.push_back(std::make_pair(variable_on_the_L,run_function(functionname)));
                                    }
                                    def = false;
                                }
                                else{//function return list
                                
                                    if(findVector(variable_on_the_L,0)=="l_variables")
                                        l_variables[findTarget(variable_on_the_L,def)].second=run_function_L(functionname);
                                    else
                                        l_variables.push_back(std::make_pair(variable_on_the_L,run_function_L(functionname)));
                                    def = false;
                                }
                            }
                            else{
                                if(qwer=="variables"){//function return variables
                                    if(findVector(variable_on_the_L,1)=="variables_d"){
                                        variables_d[findTarget(variable_on_the_L,def)].second=run_function(functionname);
                                    }
                                    else{
                                        variables_d.push_back(std::make_pair(variable_on_the_L,run_function(functionname)));
                                    }
                                    def = false;
                                }
                                else{//function return list
                                    if(findVector(variable_on_the_L,1)=="l_variables_d")
                                        l_variables_d[findTarget(variable_on_the_L,def)].second=run_function_L(functionname);
                                    else
                                        l_variables_d.push_back(std::make_pair(variable_on_the_L,run_function_L(functionname)));
                                    def = false;
                                }
                            }
                            
                        }
                        else{//↓
                            if(!def){
                                if (variables.size() >= 1)
                                {
                                    if (findTarget(var[0],def) == -1)
                                    {
                                        variables.push_back(std::make_pair(var[0], stoi(var[1])));
                                    }
                                    else
                                    {
                                        variables[findTarget(var[0],def)].second = stoi(var[1]);
                                    }
                                }
                                else if (variables.size() == 0)
                                {
                                    variables.push_back(std::make_pair(var[0], stoi(var[1])));
                                }
                            }
                            else{
                                if (variables_d.size() >= 1)
                                {
                                    if (findTarget(var[0],def) == -1)
                                    {
                                        variables_d.push_back(std::make_pair(var[0], stoi(var[1])));
                                    }
                                    else
                                    {
                                        variables_d[findTarget(var[0],def)].second = stoi(var[1]);
                                    }
                                }
                                else if (variables_d.size() == 0)
                                {
                                    variables_d.push_back(std::make_pair(var[0], stoi(var[1])));
                                }
                            }
                            
                        }    
                    }
                }
                else
                { // [] on left and no +-*/ on right
                    int listIndex = getIndex(var[0]);
                    std::string listName = var[0].substr(0, (var[0].find("[")));// listname on the left of =

                    int value;

                    if(var[0].find(":")!=-1){// there is ":" inside the [] on the left of hte =
                        int RlistIndex = getIndex(var[1]);
                        std::string RlistName = var[1].substr(0, (var[1].find("[")));
                        if(!def){
                            for(listIndex;listIndex<l_variables[findTarget(RlistName,def)].second.size();listIndex++){
                                l_variables[findTarget(listName,def)].second[listIndex]=l_variables[findTarget(RlistName,def)].second[RlistIndex];
                                RlistIndex++;
                            }
                        }
                        else{
                            for(listIndex;listIndex<l_variables_d[findTarget(RlistName,def)].second.size();listIndex++){
                                l_variables_d[findTarget(listName,def)].second[listIndex]=l_variables_d[findTarget(RlistName,def)].second[RlistIndex];
                                RlistIndex++;
                            }
                        }
                        
                    }
                    if (is_number(var[1])==1)
                    { // contains [] on left, non-integer on right
                        value = stoi(var[1]);
                    }
                    else
                    {
                        if(!def){
                            if(var[1].find("[")==-1){//no [] on the right
                                value = variables[findTarget(var[1],def)].second;
                            }
                            else{// yes [] on the right
                                
                                int listIndex = getIndex(var[1]);
                                std::string listName = var[1].substr(0, (var[1].find("[")));

                                value = l_variables[findTarget(listName,def)].second[listIndex];
                            }
                        }
                        else{
                            if(var[1].find("[")==-1){//no [] on the right
                                value = variables_d[findTarget(var[1],def)].second;
                            }
                            else{// yes [] on the right
                                
                                int listIndex = getIndex(var[1]);
                                std::string listName = var[1].substr(0, (var[1].find("[")));

                                value = l_variables_d[findTarget(listName,def)].second[listIndex];
                            }
                        }
                        
                    }
                    if(!def){
                        l_variables[findTarget(listName,def)].second[listIndex]=value;
                    }
                    else{
                        l_variables_d[findTarget(listName,def)].second[listIndex]=value;
                    }
                    
                }
            }
            else if (MSG_in_line.find('+') != -1 && var[1].find('[') != -1)
            { // a = temp + y + l3[0] //yes+-*/ && yes[] on the right
                varR = split(var[1], "+");
                var[0] = kick_out_useless_space(var[0]);
                if (var[0].find('[') != -1){ //listZ[0] = listZ[0] + x
                    for (int m = 0; m < varR.size(); m++)
                    {
                        varR[m] = kick_out_useless_space(varR[m]);
                        if(!def){
                            if (is_number(varR[m]) == 1)
                            {
                                tmp = tmp + stoi(varR[m]);
                            }
                            else if(varR[m].find('[')==-1){
                                tmp = tmp + variables[findTarget(varR[m],def)].second;
                            }
                            else{
                                int listIndex = getIndex(varR[m]);
                                std::string listName = varR[m].substr(0, (varR[m].find("[")));
                                tmp=tmp + l_variables[findTarget(listName,def)].second[listIndex];
                            }
                        }
                        else{
                            if (is_number(varR[m]) == 1)
                            {
                                tmp = tmp + stoi(varR[m]);
                            }
                            else if(varR[m].find('[')==-1){
                                tmp = tmp + variables_d[findTarget(varR[m],def)].second;
                            }
                            else{
                                int listIndex = getIndex(varR[m]);
                                std::string listName = varR[m].substr(0, (varR[m].find("[")));
                                tmp=tmp + l_variables_d[findTarget(listName,def)].second[listIndex];
                            }
                        }
                    }
                    int LlistIndex = getIndex(var[0]);
                    std::string LlistName = var[0].substr(0, (var[0].find("[")));
                    if(!def){
                        l_variables[findTarget(LlistName,def)].second[LlistIndex]= tmp; 
                    }
                    else{
                        l_variables_d[findTarget(LlistName,def)].second[LlistIndex]= tmp; 
                    }
                }
                else{
                    for (int m = 0; m < varR.size(); m++)
                    {
                        varR[m] = kick_out_useless_space(varR[m]);
                        if (is_number(varR[m]) == 1)
                        {
                            tmp += stoi(varR[m]);
                        }
                        else
                        {
                            if (varR[m].find("[") == -1)
                            {
                                if(!def){
                                    if(findVector(varR[m],0)=="l_variables"){//whole list
                                        for (int j = 0; j < l_variables[findTarget(varR[m],def)].second.size(); j++)
                                        {
                                            BUFFERlist.push_back(l_variables[findTarget(varR[m],def)].second[j]);
                                        }
                                    }
                                    else
                                        tmp = tmp + variables[findTarget(varR[m],def)].second;
                                }
                                else{
                                    if(findVector(varR[m],1)=="l_variables_d"){//whole list
                                        for (int j = 0; j < l_variables_d[findTarget(varR[m],def)].second.size(); j++)
                                        {
                                            BUFFERlist.push_back(l_variables_d[findTarget(varR[m],def)].second[j]);
                                        }
                                    }
                                    else
                                        tmp = tmp + variables_d[findTarget(varR[m],def)].second;
                                }
        
                            }
                            else if(varR[m].starts_with("[")){
                                BUFFERlist.push_back(getIndex(varR[m]));
                            }
                            else
                            {
                                int listIndex = getIndex(varR[m]);
                                std::string listName = varR[m].substr(0, (varR[m].find("[")));
                                if(!def){
                                    tmp = tmp + l_variables[findTarget(listName,def)].second[listIndex];
                                }
                                else{
                                    tmp = tmp + l_variables_d[findTarget(listName,def)].second[listIndex];
                                }
                            }
                        }
                    }
                    if(!def){
                        if(findVector(var[0],0)=="variables"){
                            variables[findTarget(var[0],def)].second=tmp;
                        }
                        else if(findVector(var[0],0)=="l_variables"){
                            l_variables[findTarget(var[0],def)].second = BUFFERlist;
                        }
                        else{
                            variables.push_back(std::make_pair(var[0], tmp));
                        }
                    }
                    else{
                        if(findVector(var[0],1)=="variables_d"){
                            variables_d[findTarget(var[0],def)].second=tmp;
                        }
                        else if(findVector(var[0],1)=="l_variables_d"){
                            l_variables_d[findTarget(var[0],def)].second = BUFFERlist;
                        }
                        else{
                            variables_d.push_back(std::make_pair(var[0], tmp));
                        }
                    }
                }    
            }
            else if (MSG_in_line.find('+') != -1 && var[1].find('(') != -1){//yes+-*/ and function calling on right
                varR = split(var[1], "+");
                var[0] = kick_out_useless_space(var[0]);
                std::string value_L = var[0];
                std::string return_type ="";
                int buffvalue=0;
                return_type=findVector(varR[0],0);
                if(!def){
                    if(return_type=="variables"){
                        for(int q=0; q<varR.size();q++){
                            if(findVector(varR[q],0)==" "){
                                buffvalue+=run_function(varR[q]);
                            }
                            else{
                                buffvalue+=variables[findTarget(varR[q],def)].second;
                            }
                        }
                        if(findVector(value_L,0)==" "){
                            variables.push_back(std::make_pair(value_L,buffvalue));
                        }
                        else{
                            variables[findTarget(value_L,def)].second=buffvalue;
                        }
                    }
                }
                else{
                    if(return_type=="variables_d"){
                        for(int q=0; q<varR.size();q++){
                            if(findVector(varR[q],1)==" "){
                                buffvalue+=run_function(varR[q]);
                            }
                            else{
                                buffvalue+=variables_d[findTarget(varR[q],def)].second;
                            }
                        }
                        if(findVector(var[0],1)==" "){
                            variables_d.push_back(std::make_pair(value_L,buffvalue));
                        }
                        else{
                            variables_d[findTarget(value_L,def)].second=buffvalue;
                        }
                    }
                }
            }
            else
            { // yes 加减乘除+-*/ in MSG
                for (int j = 0; j < var.size(); j++)
                {
                    var[j] = kick_out_useless_space(var[j]);
                }
                BUFFER = var[1];
                varR = split(BUFFER, "+");
                for (int i = 0; i < varR.size(); i++)
                {
                    varR[i] = kick_out_useless_space(varR[i]);
                    if(!def){
                        if (findVector(varR[i],0) == "l_variables")
                        {
                            if (findVector(var[0],0) == " ")
                            { // variable not in any vectors
                                for (int j = 0; j < l_variables[findTarget(varR[i],def)].second.size(); j++)
                                {
                                    BUFFERlist.push_back(l_variables[findTarget(varR[i],def)].second[j]);
                                }
                                if (i == varR.size() - 1)
                                {
                                    l_variables.push_back(std::make_pair(var[0], BUFFERlist));
                                }
                            }
                            else if(findVector(var[0],0) == "l_variables"){
                                if(i!=varR.size()-1){
                                    for (int j = 0; j < l_variables[findTarget(varR[i],def)].second.size(); j++)
                                    {
                                        BUFFERlist.push_back(l_variables[findTarget(varR[i],def)].second[j]);
                                    }
                                    continue;
                                }
                                else{
                                    for (int j = 0; j < l_variables[findTarget(varR[i],def)].second.size(); j++)
                                    {
                                        BUFFERlist.push_back(l_variables[findTarget(varR[i],def)].second[j]);
                                    }
                                    l_variables[findTarget(var[0],def)].second=BUFFERlist;
                                }
                            }
                            else{  //
                                for (int j = 0; j < l_variables[findTarget(varR[i],def)].second.size(); j++)
                                {
                                    l_variables[findTarget(var[0],def)].second.push_back(l_variables[findTarget(varR[i],def)].second[j]);
                                }
                            }
                        }
                    }
                    else{
                        if (findVector(varR[i],1) == "l_variables_d")
                        {
                            if (findVector(var[0],1) == " ")
                            { // variable not in any vectors
                                for (int j = 0; j < l_variables_d[findTarget(varR[i],def)].second.size(); j++)
                                {
                                    BUFFERlist.push_back(l_variables_d[findTarget(varR[i],def)].second[j]);
                                }
                                if (i == varR.size() - 1)
                                {
                                    l_variables_d.push_back(std::make_pair(var[0], BUFFERlist));
                                }
                            }
                            else if(findVector(var[0],1) == "l_variables_d"){
                                if(i!=varR.size()-1){
                                    for (int j = 0; j < l_variables_d[findTarget(varR[i],def)].second.size(); j++)
                                    {
                                        BUFFERlist.push_back(l_variables_d[findTarget(varR[i],def)].second[j]);
                                    }
                                    continue;
                                }
                                else{
                                    for (int j = 0; j < l_variables_d[findTarget(varR[i],def)].second.size(); j++)
                                    {
                                        BUFFERlist.push_back(l_variables_d[findTarget(varR[i],def)].second[j]);
                                    }
                                    l_variables_d[findTarget(var[0],def)].second=BUFFERlist;
                                }
                            }
                            else{  //
                                for (int j = 0; j < l_variables_d[findTarget(varR[i],def)].second.size(); j++)
                                {
                                    l_variables_d[findTarget(var[0],def)].second.push_back(l_variables_d[findTarget(varR[i],def)].second[j]);
                                }
                            }
                        }
                    }
                    
                }
                if(!def){
                    if(findVector(var[0],0)!="l_variables"){
                        for (int j = 0; j < varR.size(); j++)
                        {
                            varR[j] = kick_out_useless_space(varR[j]);
                        }
                        for (int j = 0; j < varR.size(); j++)
                        {
                            if (is_number(varR[j]) == 1)
                            {
                                tmp = tmp + stoi(varR[j]);
                            }
                            else
                            {
                                tmp=tmp+variables[findTarget(varR[j],def)].second;
                            }
                        }
                        if(findVector(var[0],0)==" "){
                            variables.push_back(std::make_pair(var[0], tmp));
                        }
                        else{
                            variables[findTarget(var[0],def)].second=tmp;
                        }        

                    }
                }
                else{
                    if(findVector(var[0],1)!="l_variables_d"){
                        for (int j = 0; j < varR.size(); j++)
                        {
                            varR[j] = kick_out_useless_space(varR[j]);
                        }
                        for (int j = 0; j < varR.size(); j++)
                        {
                            if (is_number(varR[j]) == 1)
                            {
                                tmp = tmp + stoi(varR[j]);
                            }
                            else
                            {
                                tmp=tmp+variables_d[findTarget(varR[j],def)].second;
                            }
                        }
                        if(findVector(var[0],1)==" "){
                            variables_d.push_back(std::make_pair(var[0], tmp));
                        }
                        else{
                            variables_d[findTarget(var[0],def)].second=tmp;
                        }
                    }
                }
            }
        }
        else if (MSG_in_line.starts_with("if"))
        { // for if statement
            is_if = true;
            int index;
            std::string BUFFER;
            std::vector<std::string> temp;
            if (MSG_in_line.find('(') != -1) // contains () for if statement
            {
                BUFFER = MSG_in_line.substr(MSG_in_line.find('(') + 1, MSG_in_line.find(')') - MSG_in_line.find('(') - 1);
                for (int i = 0; i < COMPARER.size(); i++){
                    if(BUFFER.find(COMPARER[i])!=-1){
                        index = i;
                        break;
                    }
                }
                temp = split(BUFFER, COMPARER[index]);
                
            }
            else
            { // no () for if statement
                BUFFER = MSG_in_line.substr(MSG_in_line.find(" ") + 1);
                BUFFER = BUFFER.erase(BUFFER.find(":"));
                for (int i = 0; i < COMPARER.size(); i++){
                    if(BUFFER.find(COMPARER[i])!=-1){
                        index = i;
                        break;
                    }
                }
                temp = split(BUFFER, COMPARER[index]);
            }

            for (int i = 0; i < temp.size(); i++)
            {
                temp[i]=kick_out_useless_space(temp[i]);
            }

            if (is_number(temp[0]) != 1)//not a NO. on the left of condition
            {
                if(!def){
                    if(temp[0].find("[")!=-1){// yes [] on the left of condition
                        std::string listName = temp[0].substr(0, (temp[0].find("[")));
                        int listIndex = getIndex(temp[0]);
                        TorF= ifstatement(l_variables[findTarget(listName,def)].second[listIndex],index,stoi(temp[1]));
                    }
                    else{
                        TorF = ifstatement(variables[findTarget(temp[0],def)].second, index, stoi(temp[1]));
                    }
                }
                else{
                    if(temp[0].find("[")!=-1){// yes [] on the left of condition
                        std::string listName = temp[0].substr(0, (temp[0].find("[")));
                        int listIndex = getIndex(temp[0]);
                        TorF= ifstatement(l_variables_d[findTarget(listName,def)].second[listIndex],index,stoi(temp[1]));
                    }
                    else{
                        TorF = ifstatement(variables_d[findTarget(temp[0],def)].second, index, stoi(temp[1]));
                    }
                }
                
            }
            else
            {
                TorF = ifstatement(stoi(temp[0]), index, stoi(temp[1]));
            }
            continue;
        }
        else if(MSG_in_line.find("=")==-1 && !MSG_in_line.starts_with("print") && MSG_in_line.find("(") != -1 && MSG_in_line.find(")") != -1){
            run_function_N(MSG_in_line);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string filename;

    if (argc == 2)
    {
        filename = argv[1];
    }
    else
    {
        std::cout << "ERROR! could not open the file" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream inputfile(filename);
    if (!inputfile.is_open())
    {
        std::cerr << "ERROR! could not open the file ' " << filename << "'" << std::endl;
        return EXIT_FAILURE;
    }

    while (getline(inputfile, TEM))
        codefrompy.push_back(TEM);
        
    inputfile.close();
    main_function(codefrompy);

    return 0;
}