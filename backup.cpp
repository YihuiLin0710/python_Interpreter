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
std::vector<std::pair<std::string, std::vector<int>>> l_variables;
std::vector<std::string> COMPARER = {">=", "<=", ">", "<", "!=", "=="};

bool is_if = false;
bool TorF = false;
bool runelse = false;
bool def = false;

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



std::string kick_out_useless_space(std::string target)
{
    target.erase(target.find_last_not_of(' ') + 1);
    target.erase(0, target.find_first_not_of(' '));
    return target;
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

int findTarget(std::string target)
{
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
        //std::cout <<"checkpoint149 n "<<input.substr(start_pos + 1, (end_pos - start_pos) - 1)<<" n\n";
        index = variables[findTarget(input.substr(start_pos + 1, (end_pos - start_pos) - 1))].second;
        //std::cout <<"checkpoint151"<<" n\n";
    }
    else
    {
        index = stoi(input.substr(start_pos + 1, (end_pos - start_pos) - 1));
    }
    
    // std::cout<< "list name: " <<listName <<"\n";
    // std::cout<< "index: " << index << "\n";

    return index;
}

std::string findVector(std::string target)
{
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
    /*
    1. (missing parenthesis
    2. extra statements (e.g. recursivefunctions)
    3. unhandled data types (e.g. an object)
    */
    if (1==1)
        return 0;
    if (MSG == "")
        return 0;
    if (MSG.starts_with("#") || MSG.starts_with("print")||MSG.starts_with("return"))
        return 0;

    std::vector<std::string> varR;
    /*
     if ((MSG.find('(') != -1&&MSG.find(')')==-1)||(MSG.find('(')==-1 && MSG.find(')')!=-1)
     if(count(MSG.begin(),MSG.end(),'(') !=count(MSG.begin(),MSG.end(),')'))
    */
    // missing ()
    // if ( (MSG.find('(') != -1 && MSG.find(')') == -1) || (MSG.find('(') == -1 && MSG.find(')') != -1) )
    if (count(MSG.begin(), MSG.end(), '(') != count(MSG.begin(), MSG.end(), ')'))
    {
        error_log << SYNTAX_ERROR << lineno << "\n";
        std::cout << SYNTAX_ERROR << lineno << "\n";
        exit(0);
    }
    // if(count(MSG.begin(),MSG.end(),'"')%2 != 0 || count(MSG.begin(),MSG.end(),"\'")%2 != 0){
    if (count(MSG.begin(), MSG.end(), '"') % 2 != 0)
    {
        error_log << SYNTAX_ERROR << lineno << "\n";
        std::cout << SYNTAX_ERROR << lineno << "\n";
        exit(0);
    }
    if (count(MSG.begin(), MSG.end(), '[') != count(MSG.begin(), MSG.end(), ']'))
    {
        error_log << SYNTAX_ERROR << lineno << "\n";
        std::cout << SYNTAX_ERROR << lineno << "\n";
        exit(0);
    }
    std::string for_if_only;
    for_if_only=kick_out_useless_space(MSG);
    if (for_if_only.starts_with("if"))
    {
        if (MSG.find(":") == -1)
        { //||count(MSG.begin(),MSG.end(),'=')==1
            error_log << SYNTAX_ERROR << lineno << "\n";
            std::cout << SYNTAX_ERROR << lineno << "\n";
            exit(0);
        }
        return 0;
    }

    if (MSG.find('=') != -1)
    {
        varR = split(MSG, "=");
        varR[1] = kick_out_useless_space(varR[1]);
        if (varR[1].find("+") == -1)
        { // no +
            if (varR[1].starts_with("["))
            {
                return 0;
            }

            if (varR[1].find('[') != -1)
            { // variable with []
            
                if (varR[1].find("[:]")==-1) {
                    int index = getIndex(varR[1]);
                    varR[1] = varR[1].substr(0, varR[1].find("["));
                    if (findVector(varR[1]) == " ")
                    { // no this list exist
                        error_log << VAR_UNDEFINED << lineno << "\n";
                        std::cout << VAR_UNDEFINED << lineno << "\n";
                        //std::cout <<"checkpoint282";
                        exit(0);
                    }
                    else
                    { // this list is exist
                        
                        if (index > l_variables[findTarget(varR[1])].second.size() - 1)
                        { // index out of range
                            error_log << OUTRANGE << lineno << "\n";
                            std::cout << OUTRANGE << lineno << "\n";
                            //std::cout<<index<<"  checkpoint304 \n";
                            exit(0);
                        }
                    }
                }
                //std::cout << "check 294" << "\n";
            }
            if (is_number(varR[1]) != 1)
            { // not a digit
                if(varR[1].find("[")==-1){//no []on the right
                    if (findVector(varR[1]) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << lineno << "\n";
                        std::cout << VAR_UNDEFINED << lineno << "\n";
                        //std::cout <<"checkpoint301";
                        exit(0);
                    }
                }
                else{// yes [] on the right
                    std::string listName = varR[1].substr(0, (varR[1].find("[")));
                    if (findVector(listName) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << lineno << "\n";
                        std::cout << VAR_UNDEFINED << lineno << "\n";
                        //std::cout <<"checkpoint328" << "\n";
                        exit(0);
                    }
                }    
            }
            //std::cout << "check 305" << "\n";
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
                    if (findVector(varR[i]) == " ")
                    { // no this list exist
                        error_log << VAR_UNDEFINED << lineno << " \n";
                        std::cout << VAR_UNDEFINED << lineno << " \n";
                        //std::cout <<"checkpoint324";
                        exit(0);
                    }
                    else
                    { // this list is exist
                        if (index > l_variables[findTarget(varR[i])].second.size() - 1)
                        { // index out of range
                            error_log << OUTRANGE << lineno << "\n";
                            std::cout << OUTRANGE << lineno << "\n";
                            //std::cout<<"checkpoint362 \n";
                            exit(0);
                        }
                    }
                }
                if (is_number(varR[i]) != 1)
                { // not a digit
                    if (findVector(varR[i]) == " ")
                    { // no this variable exist
                        error_log << VAR_UNDEFINED << lineno << "\n";
                        std::cout << VAR_UNDEFINED << lineno << "\n";
                        //std::cout <<i<<" checkpoint343 \n";
                        exit(0);
                    }
                }
            }
        }
    }
    return 0;
}

void main_function(std::vector<std::string> codefrompy);
std::vector<int> run_function_L(std::string MSG){
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
                if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    if(findVector(parameter_name_list_with_def[q])!=" "){
                        variables[findTarget(parameter_name_list_with_def[q])].second=variables[findTarget(parameter_name_list[q])].second;
                        continue;
                    }
                    else if(findVector(parameter_name_list[q])=="variables"){//parameter is a variable
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q])].second));
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                    }
                    else{//parameter is a list
                        l_variables.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q])].second));
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
    last_line_of_function=function_content[function_content.size()-1];
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    last_line_of_function_list = split(last_line_of_function," ");

    return l_variables[findTarget(last_line_of_function_list[1])].second;
}

int run_function(std::string MSG){
    std::string function_name = MSG.substr(0,MSG.find("("));
    std::string parameter_name = MSG.substr(MSG.find("(")+1,MSG.find(")")-MSG.find("(")-1);
    std::vector<std::string> parameter_name_list;
    //std::cout<<parameter_name<<" n\n";
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
                //std::cout<<"checkpoint 425 n\n";
                if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    //std::cout<<"checkpoint 427 n\n";
                    //std::cout<<parameter_name_list[q]<<" n\n";
                    if(findVector(parameter_name_list_with_def[q])!=" "){
                        variables[findTarget(parameter_name_list_with_def[q])].second=variables[findTarget(parameter_name_list[q])].second;
                        continue;
                    }
                    else if(findVector(parameter_name_list[q])=="variables"){//parameter is a variable
                        //std::cout<<"checkpoint 429 n\n";
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q])].second));
                        //std::cout<<"checkpoint 431 n\n";
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                    }
                    else{//parameter is a list
                        l_variables.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q])].second));
                    }
                    //std::cout<<"checkpoint 432 n\n";
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
    //std::cout << "checkpoint443" << "\n";
    last_line_of_function=function_content[function_content.size()-1];
    //std::cout << "checkpoint445" << "\n";
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    //std::cout << "checkpoint447" << "\n";
    last_line_of_function_list = split(last_line_of_function," ");
    //std::cout << "checkpoint449" << "\n";
    //std::cout <<last_line_of_function_list[1]<< " 450\n";
    //std::cout <<variables[findTarget(last_line_of_function_list[1])].second<<" 451n\n";
    for(int c=0;c<variables.size();c++){
        //std::cout<<" showroom : "<<variables[c].first<<" : "<<variables[c].second<<" \n";
    }
    if(last_line_of_function.find("+")!=-1){
        int idkwmid=0;
        last_line_of_function=last_line_of_function.substr(last_line_of_function.find(" ")+1);
        last_line_of_function_list=split(last_line_of_function,"+");
        for(int z=0;z<last_line_of_function_list.size();z++){
            last_line_of_function_list[z]=kick_out_useless_space(last_line_of_function_list[z]);
            //std::cout <<last_line_of_function_list[z]<<" "<<variables[findTarget(last_line_of_function_list[z])].second<<" dddddn\n";
        }
        for(int x=0;x<last_line_of_function_list.size();x++){
            idkwmid+=variables[findTarget(last_line_of_function_list[x])].second;
        }
        //std::cout << "checkpoint517" << "\n";
        return idkwmid;
    }
    else{
        //std::cout<<variables[findTarget(last_line_of_function_list[1])].first<<" to check what t h u r :"<<variables[findTarget(last_line_of_function_list[1])].second<<" \n";
        return variables[findTarget(last_line_of_function_list[1])].second;
    }
}

std::string to_check_the_return_type(std::string MSG){
    //std::cout <<MSG<< "checkpoint456"<<" n\n";
    std::string function_name = MSG.substr(0,MSG.find("("));
    //std::cout <<function_name<< " checkpoint459"<<" n\n";
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
                //std::cout<<"checkpoint 425 n\n";
                if(parameter_name_list_with_def[q]!=parameter_name_list[q]){
                    //std::cout<<"checkpoint 427 n\n";
                    //std::cout<<parameter_name_list[q]<<" n\n";
                    if(findVector(parameter_name_list[q])=="variables"){//parameter is a variable
                        //std::cout<<"checkpoint 429 n\n";
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], variables[findTarget(parameter_name_list[q])].second));
                        //std::cout<<"checkpoint 431 n\n";
                    }
                    else if(is_number(parameter_name_list[q])==1){//parameter is a number
                        variables.push_back(std::make_pair(parameter_name_list_with_def[q], stoi(parameter_name_list[q])));     
                    }
                    else{//parameter is a list
                        l_variables.push_back(std::make_pair(parameter_name_list_with_def[q], l_variables[findTarget(parameter_name_list[q])].second));
                    }
                    //std::cout<<"checkpoint 432 n\n";
                }
            }
            for(i=i+1;i <codefrompy.size();i++){
                if(!codefrompy[i].starts_with(" ")||all_of(codefrompy[i].begin(),codefrompy[i].end(),isspace))
                    break;
                function_content.push_back(kick_out_useless_space(codefrompy[i]));
            }
            break;
        }
    }
    main_function(function_content);
    //std::cout <<function_content[1]<< " checkpoint472"<<" n\n";
    last_line_of_function=function_content[function_content.size()-1];
    //std::cout <<last_line_of_function<<"checkpoint474"<<" n\n";
    last_line_of_function = kick_out_useless_space(last_line_of_function);
    //std::cout << "checkpoint476"<<" n\n";
    last_line_of_function_list = split(last_line_of_function," ");
    //std::cout <<"checkpoint478"<<" n\n";
    //std::cout <<last_line_of_function_list[1]<<" n\n";
    std::string returnvalue=findVector(last_line_of_function_list[1]);
    //std::cout <<"checkpoint480"<<" n\n";
    //std::cout << returnvalue << "  n\n";
    return returnvalue;
}
void main_function(std::vector<std::string> codefrompy)/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
    for (int lineNo = 1; lineNo <= codefrompy.size(); lineNo++)
    {
        if(lineNo==3){
            //std::cout<<l_variables[findTarget("l1")].second.size()<<" \n";
        }
        //std::cout << lineNo << " n\n";
        //std::cout<< codefrompy[lineNo-1]<< "\n";
         //std::cout<< TorF<<" i\n";
        // std::cout << findVector("listZ") << " n\n";
        int tmp = 0;
        BUFFERlist = {};
        MSG_in_line = codefrompy[lineNo - 1];
        //std::cout << "checkpoint363" << "\n";
        handleError(MSG_in_line, lineNo);
        //std::cout << "checkpoint478" << "\n";
        if(def && MSG_in_line.starts_with(" ")){
            //std::cout << "checkpoint489" << "\n";
            continue;
        }
        else if(def && !MSG_in_line.starts_with(" ")){
            def = false;
        }
        //std::cout << "checkpoint1" << "\n";
        MSG_in_line = kick_out_useless_space(MSG_in_line);
        // var = {A:1, B:2} cout << var[1].first = B
        //std::cout << "checkpoint2" << "\n";
        if(MSG_in_line.starts_with("def")){
            def=true;
            continue;
        }
        //std::cout<< MSG_in_line<< "499\n";
        //std::cout << "checkpoint3" << "\n";
        if(MSG_in_line.starts_with("return")){
            //std::cout << "checkpoint501" << "\n";
            return ;
        }    
        //std::cout << "checkpoint4" << "\n";
        if (MSG_in_line.find("else") != -1)
        {
            runelse = true;
            continue;
        }
        //std::cout <<"checkpoint651"<<" n\n";
        if (is_if)
        {
            //std::cout <<"checkpoint654 "<<codefrompy[lineNo]<<" n\n";
            if (!TorF && !runelse)
            {
                //std::cout <<"checkpoint656"<<" n\n";
                continue;
            }
            else if (TorF && runelse)
            {
                //std::cout <<"checkpoint661"<<" n\n";
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
                //std::cout <<"checkpoint672"<<" n\n";
                is_if = false;
                TorF = false;
                runelse = false;
            }   
            else if(codefrompy[lineNo-1].find("if")!=-1){
                //std::cout<<"checkpoint678 n\n";
                runelse = false;
            }
            //std::cout <<"checkpoint682"<<" n\n";
        }
        //std::cout <<"checkpoint684"<<" n\n";
        if (MSG_in_line.starts_with("#"))
        {
            continue;
        }
        else if (MSG_in_line.starts_with("print"))
        {                                    // printtttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt
            if (MSG_in_line.find(",") != -1) // print more than one parameters
            {
                MSG_in_line = MSG_in_line.substr(MSG_in_line.find("(") + 1, MSG_in_line.find(")") - MSG_in_line.find("(") - 1);
                BUFFERPRINT = split(MSG_in_line, ",");
                TEM = BUFFERPRINT[0];
                TEM = TEM.substr(TEM.find_first_of('"') + 1, TEM.find_last_of('"') - 1);
                std::cout << TEM<<" ";
                T = BUFFERPRINT[1];
                T=kick_out_useless_space(T);
                //std::cout << "Checking parameter on the right side of colon ('listX:', ?): " << T << "\n"; 
                //std::cout << "Checking parameter on the left side of colon (?, listX): " << BUFFERPRINT[0] << "\n"; 
                // 修理: print("listX:", listX)
                //for (int i=0; i<l_variables.size(); i++) {std::cout << "l_variables: " << l_variables[i].first << " " << "\n";}
                if (findVector(T) == "l_variables"){
                    //std::cout << "Check 692" << "\n";
                    std::cout << "[";
                    for (int i = 0; i < l_variables[findTarget(T)].second.size(); i++)
                    {
                        std::cout << l_variables[findTarget(T)].second[i];
                        if (i != l_variables[findTarget(T)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
                else{
                    if (findVector(T) == "variables")
                    {
                        // std::cout << T << "\n";
                        std::cout << variables[findTarget(T)].second << "\n";
                    }
                    else if (findVector(T) == "l_variables")
                    {
                        //std::cout << "INSIDE PRINT l_variables CONDITION" << "\n";
                        std::cout << "[";
                        for (int i = 0; i < l_variables[findTarget(T)].second.size(); i++)
                        {
                            std::cout << l_variables[findTarget(T)].second[i];
                            if (i != l_variables[findTarget(T)].second.size() - 1)
                                std::cout << ", ";
                        }
                        std::cout << "]"
                                << "\n";
                    }
                }
            }
            else // only one parameter in print()
            {
                //std::cout << "Checkpoint: else" << "\n";
                T = MSG_in_line.substr(MSG_in_line.find("(") + 1, MSG_in_line.find(")") - MSG_in_line.find("(") - 1);
                // std::cout <<T<<" n\n";
                // std::cout << findTarget("a") << "\n";
                if (findVector(T) == "variables")
                {
                    // std::cout << T << "\n";
                    std::cout << variables[findTarget(T)].second << "\n";
                }
                else if (findVector(T) == "l_variables")
                {
                    //std::cout << "INSIDE PRINT l_variables CONDITION" << "\n";
                    std::cout << "[";
                    for (int i = 0; i < l_variables[findTarget(T)].second.size(); i++)
                    {
                        std::cout << l_variables[findTarget(T)].second[i];
                        if (i != l_variables[findTarget(T)].second.size() - 1)
                            std::cout << ", ";
                    }
                    std::cout << "]" << "\n";
                }
            }
            // std::cout<<T<<"\n";
            /**
            if (findVector(T) == "variables")
            {
                // std::cout << T << "\n";
                std::cout << variables[findTarget(T)].second << "\n";
            }
            else if (findVector(T) == "l_variables")
            {
                //std::cout << "INSIDE PRINT l_variables CONDITION" << "\n";
                std::cout << "[";
                for (int i = 0; i < l_variables[findTarget(T)].second.size(); i++)
                {
                    std::cout << l_variables[findTarget(T)].second[i];
                    if (i != l_variables[findTarget(T)].second.size() - 1)
                        std::cout << ", ";
                }
                std::cout << "]"
                          << "\n";
            }
*/
            /*for (int i=0; i<variables.size(); i++){
                if (variables[i].first == T){
                    std::cout<<variables[i].second<<"\n";
                }
            }*/
        }
        else if (count(MSG_in_line.begin(), MSG_in_line.end(), '=') == 1 && !MSG_in_line.starts_with("if"))
        { // assign values (only 1 "=")
            //std::cout <<"checkpoint453"<<" n\n";
            var = split(MSG_in_line, "=");
            if (MSG_in_line.find('+') == -1)
            { // no 加+减-乘*除/
                //std::cout <<"checkpoint457"<<" n\n";
                for (int j = 0; j < var.size(); j++)
                {
                    //std::cout << "check kick before" << "\n";
                    var[j] = kick_out_useless_space(var[j]);
                    //std::cout << "checkpoint489" << "\n";
                }
                if (var[0].find('[') == -1)
                { // no [] on left side
                    //std::cout <<"checkpoint464"<<" n\n";
                    //  std::cout << "var[1]: " << var[1] << "\n";
                    if (var[1].find('[') != -1)
                    { // at least one [] on the right side

                        //std::cout << "check statement" << "\n";
                        if (var[1].starts_with("["))
                        {
                            //std::cout << "checkpoint664" << " n\n";
                            if(var[1].find(",")==-1){
                                BUFFERlist=getList(var[1]);
                                //l_variables.push_back(std::make_pair(var[0], getList(var[1])));
                                //std::cout<<l_variables[findTarget(var[0])].second.size()<<" 805 n\n";    
                            }    
                            else{
                                var[1]=var[1].substr(1,var[1].find("]")-1);
                                std::vector<std::string> content=split(var[1],",");
                                for(int q=0;q<content.size();q++){
                                    if(is_number(content[q])==1){
                                        BUFFERlist.push_back(stoi(content[q]));
                                    }
                                    else{
                                        BUFFERlist.push_back(variables[findTarget(content[q])].second);
                                    }
                                }
                            }
                            //std::cout <<"checkpoint819"<<" n\n";
                            if(findVector(var[0])=="l_variables"){
                                l_variables[findTarget(var[0])].second=BUFFERlist;
                            }
                            else{
                                l_variables.push_back(std::make_pair(var[0],BUFFERlist));
                            }
                            //std::cout << "checkpoint826" << " n\n";
                            //std::cout<<l_variables[findTarget(var[0])].second[0]<<" 827\n";
                            //std::cout<<l_variables[findTarget(var[0])].second.size()<<" 828\n";
                        }
                        else if (var[1].find("[:]")!=-1 ){
                            var[1]=var[1].substr(0,(var[1].find("[")));
                            //std::cout << "var[1]: " << var[1] <<" \n";
                            //std::cout << "var[0]: " << var[0] << "\n";
                            l_variables.push_back(std::make_pair(var[0], l_variables[findTarget(var[1])].second));
                        }
                        else
                        { // element = list[i]
                            //std::cout <<"checkpoint636"<<" n\n";
                            int index = getIndex(var[1]);
                            //std::cout <<"checkpoint638"<<" n\n";
                            std::string listName = var[1].substr(0, (var[1].find("[")));
                            //std::cout <<"checkpoint640"<<" n\n";
                            int pos = findTarget(listName);
                            //std::cout <<"checkpoint642"<<" n\n";
                            int value = l_variables[pos].second[index];
                            //std::cout <<"checkpoint644"<<" n\n";
                            variables.push_back(std::make_pair(var[0], value));
                            //std::cout << "checkpoint646"<< " n\n";
                        }
                    }
                    else // no +-*/, no [] on right
                    {
                        if(var[1].find("len(")!=-1){
                            std::string listName= var[1].substr(var[1].find("(")+1,var[1].find(")")-var[1].find("(")-1);
                            if(findVector(var[0])=="variables"){
                                variables[findTarget(var[0])].second=l_variables[findTarget(listName)].second.size();
                            }
                            else{
                                variables.push_back(std::make_pair(var[0],l_variables[findTarget(listName)].second.size()));
                            }
                        }
                        else if(var[1].find("(")!=-1){//yes def on the right                      hereeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
                            std::string functionname = var[1];
                            std::string variable_on_the_L = var[0];
                            //std::cout <<"checkpoint643"<<" n\n";
                            std::string qwer=to_check_the_return_type(var[1]);
                            //std::cout <<qwer<< "checkpoint645"<< " n\n";
                            if(qwer=="variables"){//function return variables
                                //std::cout <<functionname<< " 64444444444444444444n\n";
                                if(findVector(variable_on_the_L)=="variables")
                                    variables[findTarget(variable_on_the_L)].second=run_function(functionname);
                                else
                                    variables.push_back(std::make_pair(variable_on_the_L,run_function(functionname)));
                                def = false;
                                //std::cout << variable_on_the_R<< "   643\n";
                                //std::cout << variables[findTarget(variable_on_the_R)].second<< "   644\n";
                            }
                            else{//function return list
                                if(findVector(variable_on_the_L)=="l_variables")
                                    l_variables[findTarget(variable_on_the_L)].second=run_function_L(functionname);
                                else
                                    l_variables.push_back(std::make_pair(variable_on_the_L,run_function_L(functionname)));
                                def = false;
                            }
                        }
                        else{//↓
                            //std::cout << "Check708"<< "\n";
                            if (variables.size() >= 1)
                            {
                                if (findTarget(var[0]) == -1)
                                {
                                    //std::cout <<var[0]<< "Check713"<< "\n";
                                    //std::cout <<var[1]<< "Check713"<< "\n";
                                    //variables.push_back(std::make_pair(var[0], stoi(var[1])));
                                    variables.push_back(std::make_pair(var[0], stoi(var[1])));
                                    //for (int i=0; i < variables.size(); i++){
                                    //    std::cout << "variables: " << variables[i].first << " " << variables[i].second << "\n";
                                    //}
                                    //std::cout << "Check490"<< "\n";
                                }
                                else
                                {
                                    variables[findTarget(var[0])].second = stoi(var[1]);
                                    //std::cout << "Check495"<< "\n";
                                }
                            }
                            else if (variables.size() == 0)
                            {
                                variables.push_back(std::make_pair(var[0], stoi(var[1])));
                                //std::cout << "Check501"<< "\n";
                            }
                        }    
                    }
                    // std::cout << "var[1]: " << var[1] << "\n";
                    // if (var[1].find('[') != -1) { // contains at least one list on the right side
                    // std::string content = var[1].substr(var[1].find("["), 3);

                    // std::cout << "Content of the list: " << "\n";
                    //}
                }
                else
                { // [] on left and no +-*/ on right
                    //std::cout <<"checkpoint1"<<" n\n";
                    int listIndex = getIndex(var[0]);
                    //std::cout <<"checkpoint2"<<" n\n";
                    //std::cout << "listIndex: " << listIndex << "\n";
                    std::string listName = var[0].substr(0, (var[0].find("[")));// listname on the left of =
                    //std::cout <<"checkpoint3"<<" n\n";
                    int value;
                    //std::cout <<listName<<" this is listname\n";
                    //for(int s=0;s<l_variables[findTarget(listName)].second.size();s++)
                    //    std::cout<<l_variables[findTarget(listName)].second[0]<<" \n";
                    // std::cout << "var[1]: n" << var[1] << "n\n";
                    //std::cout <<"checkpoint"<<" n\n";
                    if(var[0].find(":")!=-1){// there is ":" inside the [] on the left of hte =
                        int RlistIndex = getIndex(var[1]);
                        //std::cout << "RlistIndex: " << RlistIndex << "\n";
                        std::string RlistName = var[1].substr(0, (var[1].find("[")));
                        //std::cout <<"RlistName: " << RlistName << "n\n";
                        //std::cout<<l_variables[findTarget(RlistName)].second.size()<<" <<this is size \n";
                        for(listIndex;listIndex<l_variables[findTarget(RlistName)].second.size();listIndex++){
                            l_variables[findTarget(listName)].second[listIndex]=l_variables[findTarget(RlistName)].second[RlistIndex];
                            RlistIndex++;
                        }
                        //std::cout <<"checkpoint561"<<" n\n";
                    }
                    //std::cout <<"checkpoint"<<" n\n";
                    if (is_number(var[1])==1)
                    { // contains [] on left, non-integer on right
                        value = stoi(var[1]);
                    }
                    else
                    {
                        if(var[1].find("[")==-1){//no [] on the right
                            value = variables[findTarget(var[1])].second;
                        }
                        else{// yes [] on the right
                            
                            int listIndex = getIndex(var[1]);
                            std::string listName = var[1].substr(0, (var[1].find("[")));

                            value = l_variables[findTarget(listName)].second[listIndex];
                        }
                        // std::cout << "index: " << findTarget(var[1]) << "\n";
                        // std::cout<<variables[findTarget(var[1])].first<<" v\n";
                    }
                    // std::cout<<"checkpoint"<<" n\n";
                    l_variables[findTarget(listName)].second[listIndex]=value;
                    /*for (int i = 0; i < l_variables.size(); i++)
                    {
                        if (l_variables[i].first == listName)
                        {
                            std::vector<int> tmpV = l_variables[i].second;
                            tmpV[listIndex] = value;
                            l_variables[i].second = tmpV;
                            // std::cout << l_variables[0].first << " " << l_variables[0].second[1] << "\n";
                        }
                    }
                    */
                }
            }
            else if (MSG_in_line.find('+') != -1 && var[1].find('[') != -1)
            { // a = temp + y + l3[0] //yes+-*/ && yes[] on the right
                varR = split(var[1], "+");
                var[0] = kick_out_useless_space(var[0]);
                //std::cout <<"checkpoint797"<<" n\n";
                if (var[0].find('[') != -1){ //listZ[0] = listZ[0] + x
                    //std::cout << "checkpoint555"<<" n\n";
                    for (int m = 0; m < varR.size(); m++)
                    {
                        varR[m] = kick_out_useless_space(varR[m]);
                        if (is_number(varR[m]) == 1)
                        {
                            tmp = tmp + stoi(varR[m]);
                        }
                        else if(varR[m].find('[')==-1){
                            tmp = tmp + variables[findTarget(varR[m])].second;
                        }
                        else{
                            int listIndex = getIndex(varR[m]);
                            std::string listName = varR[m].substr(0, (varR[m].find("[")));
                            tmp=tmp + l_variables[findTarget(listName)].second[listIndex];
                        }
                    }
                    int LlistIndex = getIndex(var[0]);
                    std::string LlistName = var[0].substr(0, (var[0].find("[")));
                    l_variables[findTarget(LlistName)].second[LlistIndex]= tmp;                    
                }
                else{
                    //std::cout <<"checkpoint821"<<" n\n";
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
                                if(findVector(varR[m])=="l_variables"){//whole list
                                    for (int j = 0; j < l_variables[findTarget(varR[m])].second.size(); j++)
                                    {
                                        BUFFERlist.push_back(l_variables[findTarget(varR[m])].second[j]);
                                    }
                                }
                                else
                                    tmp = tmp + variables[findTarget(varR[m])].second;
        
                            }
                            else if(varR[m].starts_with("[")){
                                BUFFERlist.push_back(getIndex(varR[m]));
                            }
                            else
                            {
                                int listIndex = getIndex(varR[m]);
                                std::string listName = varR[m].substr(0, (varR[m].find("[")));
                                tmp = tmp + l_variables[findTarget(listName)].second[listIndex];
                            }
                        }
                    }
                    // std::cout<<tmp<<" tmp ANS\n";
                    if(findVector(var[0])=="variables"){
                        variables[findTarget(var[0])].second=tmp;
                    }
                    else if(findVector(var[0])=="l_variables"){
                        l_variables[findTarget(var[0])].second = BUFFERlist;
                    }
                    else{
                        variables.push_back(std::make_pair(var[0], tmp));
                    }
                }    
            }
            else if (MSG_in_line.find('+') != -1 && var[1].find('(') != -1){//yes+-*/ and function calling on right
                varR = split(var[1], "+");
                var[0] = kick_out_useless_space(var[0]);
                std::string value_L = var[0];
                std::string return_type ="";
                int buffvalue=0;
                return_type=findVector(varR[0]);
                /*for(int q=0; q<varR.size();q++){
                    if(findVector(varR[q])==" "){
                        std::cout <<"checkpoint1071 "<<varR[q]<<" n\n";
                        std::string return_type = to_check_the_return_type(varR[q]);
                        std::cout <<q<<" checkpoint1073 "<<return_type<<" n\n";
                        break;
                    }
                }*/
                //std::cout <<"checkpoint1075 "<<return_type<<" n\n";
                if(return_type=="variables"){
                    //std::cout <<"checkpoint1077"<<" n\n";
                    for(int q=0; q<varR.size();q++){
                        if(findVector(varR[q])==" "){
                            buffvalue+=run_function(varR[q]);
                            //std::cout <<"checkpoint1079 "<<buffvalue<<" n\n";
                        }
                        else{
                            buffvalue+=variables[findTarget(varR[q])].second;
                        }
                    }
                    if(findVector(var[0])==" "){
                        variables.push_back(std::make_pair(value_L,buffvalue));
                        //std::cout <<"checkpoint1092"<<" n\n";
                    }
                    else{
                        variables[findTarget(value_L)].second=buffvalue;
                        //std::cout <<var[0]<<" checkpoint1096"<<" n\n";
                    }
                }
            }
            else
            { // yes 加减乘除+-*/ in MSG
                 //std::cout <<var[0]<<"\n";
                for (int j = 0; j < var.size(); j++)
                {
                    var[j] = kick_out_useless_space(var[j]);
                }
                //std::cout <<"checkpoint821"<<" n\n";
                BUFFER = var[1];
                varR = split(BUFFER, "+");
                for (int i = 0; i < varR.size(); i++)
                {
                    varR[i] = kick_out_useless_space(varR[i]);
                    //std::cout << varR[i] << "   620\n";
                    if (findVector(varR[i]) == "l_variables")
                    {
                        if (findVector(var[0]) == " ")
                        { // variable not in any vectors
                            //std::cout <<"checkpoint831"<<" n\n";
                            for (int j = 0; j < l_variables[findTarget(varR[i])].second.size(); j++)
                            {
                                //std::cout <<"checkpoint1012"<<" n\n";
                                BUFFERlist.push_back(l_variables[findTarget(varR[i])].second[j]);
                                // sort(BUFFERlist.begin(),BUFFERlist.end());
                            }
                            if (i == varR.size() - 1)
                            {
                                l_variables.push_back(std::make_pair(var[0], BUFFERlist));
                            }
                            // std::cout<<l_variables.size()<<"\n";
                        }
                        else if(findVector(var[0]) == "l_variables"){
                            if(i!=varR.size()-1){
                                for (int j = 0; j < l_variables[findTarget(varR[i])].second.size(); j++)
                                {
                                    BUFFERlist.push_back(l_variables[findTarget(varR[i])].second[j]);
                                }
                                continue;
                            }
                            else{
                                for (int j = 0; j < l_variables[findTarget(varR[i])].second.size(); j++)
                                {
                                    BUFFERlist.push_back(l_variables[findTarget(varR[i])].second[j]);
                                }
                                l_variables[findTarget(var[0])].second=BUFFERlist;
                            }
                        }
                        else{  //
                            //std::cout <<"checkpoint845"<<" n\n";
                            //BUFFERlist += l_variables[findTarget(varR[i])];
                            for (int j = 0; j < l_variables[findTarget(varR[i])].second.size(); j++)
                            {
                                //std::cout<<"l_variables.size()"<<" \n";
                                //var[0].push_back(l_variables[findTarget(varR[i])].second[j]);
                                //std::cout <<"checkpoint851"<<" n\n";
                                l_variables[findTarget(var[0])].second.push_back(l_variables[findTarget(varR[i])].second[j]);
                                //std::cout <<"checkpoint853"<<" n\n";
                                //std::cout<<findVector("listZ")<<"  "<<var[0]<<"  n\n";
                            }
                            //for (int x = 0; )

                        }
                    }
                }
                //std::cout <<l_variables[findTarget("listZ")].second.size()<<" listzsize()\n";
                if(findVector(var[0])!="l_variables"){
                    for (int i = 0; i < variables.size(); i++)
                    {
                        if (variables[i].first != var[0] && i == variables.size() - 1)
                        {
                            variables.push_back(std::make_pair(var[0], 0));
                            //std::cout << "check656" << "\n";
                        }
                    }

                    for (int i = 0; i < symbols.size(); i++)
                    {
                        if (BUFFER.find(symbols[i]) != -1)
                        {
                            // varR=split(BUFFER,symbols[i]);
                            // std::cout <<var[0]<<"hey"<<var[1];
                            for (int j = 0; j < varR.size(); j++)
                            {
                                varR[j] = kick_out_useless_space(varR[j]);
                            }
                            for (int j = 0; j < varR.size(); j++)
                            {
                                if (is_number(varR[j]) == 1)
                                {
                                    tmp = tmp + stoi(varR[j]);
                                    //std::cout << tmp <<" varR[j] "<<varR[j]<<"  665tmpn\n";
                                }
                                else
                                {
                                    // std::cout << variables[0].first << "n\n";
                                    tmp=tmp+variables[findTarget(varR[j])].second;
                                    /*for (int i = 0; i < variables.size(); i++)
                                    {
                                        // std::cout <<"checkpoint6 "<<lineNo<<" \n";
                                        // std::cout <<variables[i].first<< "n  on lineno: "<<lineNo<<"\n";
                                        // std::cout <<var[j]<< "n  on lineno: "<<lineNo<<"\n";
                                        if (variables[i].first == varR[j])
                                        {
                                            tmp = tmp + variables[i].second;
                                            std::cout << tmp <<" varR[j] "<<varR[j]<<"  678tmpn\n";
                                            // std::cout << tmp << "on lineno: "<<lineNo<<"\n";
                                        }
                                    }*/
                                }
                            }
                        }
                    }

                    for (int i = 0; i < variables.size(); i++)
                    {
                        if (variables[i].first == var[0])
                        {
                            variables[i].second = tmp;
                        }
                        /*
                        else if (i == variables.size() - 1)
                        {
                            variables.push_back(std::make_pair(var[0], tmp));
                        }*/
                    }
                    // if(var[1].find())
                    //  yes +-*/ on the right of =
                }
            }
            //std::cout<<l_variables[findTarget("head2")].second.size()<<" 1182\n";
            //std::cout << MSG_in_line << "\n";
        }
        else if (MSG_in_line.starts_with("if"))
        { // for if statement
            is_if = true;
            int index;
            std::string BUFFER;
            std::vector<std::string> temp;
            //std::cout <<"checkpoint927"<<" n\n";
            if (MSG_in_line.find('(') != -1) // contains () for if statement
            {
                BUFFER = MSG_in_line.substr(MSG_in_line.find('(') + 1, MSG_in_line.find(')') - MSG_in_line.find('(') - 1);
                // std::cout<<BUFFER<<"\n";
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
            //std::cout <<"checkpoint1207"<<" n\n";
            for (int i = 0; i < temp.size(); i++)
            {
                temp[i]=kick_out_useless_space(temp[i]);
            }
            //std::cout <<"checkpoint1217"<<" n\n";
            if (is_number(temp[0]) != 1)//not a NO. on the left of condition
            {
                if(temp[0].find("[")!=-1){// yes [] on the left of condition
                    std::string listName = temp[0].substr(0, (temp[0].find("[")));
                    int listIndex = getIndex(temp[0]);
                    TorF= ifstatement(l_variables[findTarget(listName)].second[listIndex],index,stoi(temp[1]));
                }
                else{
                    //std::cout <<"checkpoint1226"<<" n\n";
                    TorF = ifstatement(variables[findTarget(temp[0])].second, index, stoi(temp[1]));
                    //std::cout <<"checkpoint1228"<<" n\n";
                }
            }
            else
            {
                //std::cout << "checkpoint1"<< "n \n";
                TorF = ifstatement(stoi(temp[0]), index, stoi(temp[1]));
            }
            //std::cout <<"checkpoint1234"<<" n\n";
             //std::cout << TorF<<" torf n\n";
            continue;
        }
    }
}

int main(int argc, char *argv[])
{
    std::string filename;
    //std::vector<std::string> codefrompy;
    if (argc == 2)
    {
        filename = argv[1];
        /*if(filename.find(".py")==-1)
            filename = filename + ".py";*/
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
