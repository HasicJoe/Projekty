/**
 *  @file client.cpp
 *  @brief ISA 3 - client
 *  @author Samuel Valaštín, <xvalas10@stud.fit.vutbr.cz>
*/

#include "client.hpp"

/**
 * @brief Prints help message.
*/
void print_help(){
    std::cout << "usage: client [ <option>... ] <command> [<args>] ...\n\n" <<
	"<option> is one of:\n\n" <<
    "\t-a <addr> / --address <addr> - Server hostname or address to connect to\n" <<
    "\t-p <port> / --port <port> - Server port to connect to\n" <<
    "\t-h / --help - Show this help\n\n" <<
    "Supported commands:\n" <<
    "\tregister <username> <password>\n" <<
    "\tlogin <username> <password>\n" <<
    "\tlist\n" <<
    "\tsend <recipient> <subject> <body>\n" <<
    "\tfetch <id>\n" <<
    "\tlogout\n";
}

/**
 *  @brief Prints correct command with its arguments.
 *  @param command one of following - [register | login | fetch | send]
 *  @param args string with valid command arguments
 */
void command_err(std::string command, std::string args){
    std::cout << command << " " << args << std::endl;
}

/**
 *  @brief Processes register/login command and their arguments.
 *  @param cLA structure to store command line arguments
 *  @param index index of [register | login] in vector of arguments
 *  @param arguments vectof of arguments
 *  @param arguments vectof of unescaped arguments
*/
int parse_register_or_login_cmd(commandLineArgs* cLA, int index, std::vector<std::string> arguments, std::vector<std::string> raw_args){
    if((index+3) != arguments.size()){
        command_err(arguments[index], "<username> <password>");
        return 1;
    }
    else{
        cLA->command = arguments[index];
        if(arguments[index] == "register"){
            cLA->register_flag = true;
        }
        else if(arguments[index] == "login"){
            cLA->login_flag = true;
        }
        cLA->username = arguments[index+1];
        cLA->password = raw_args[index+2];
        return 0;
    }
}

/**
 *  @brief Processes fetch command and its arguments.
 *  @param cLA structure to store command line arguments
 *  @param index index of fetch in vector of arguments
 *  @param arguments vectof of arguments
*/
int parse_fetch_cmd(commandLineArgs* cLA, int index, std::vector<std::string> arguments){
    if((index+2) != arguments.size()){
        command_err(arguments[index], "<id>");
        return 1;
    }
    else{
        cLA->command = arguments[index];
        cLA->fetch_flag = true;
        std::string str_id = arguments[index+1];
        for(int i = 0; i < str_id.length(); i++){
            if(( ((int)str_id[i]) < 48) || ( ((int)str_id[i]) > 57) ){
                command_err(arguments[index], "<id>");
                return 1;
            }
        }
        cLA->fetch_id = std::stoi(arguments[index+1]);
        return 0;
    }
}

/**
 *  @brief Processes send command and its arguments.
 *  @param cLA structure to store command line arguments
 *  @param index index of send in vector of arguments
 *  @param arguments vectof of arguments
*/
int parse_send_cmd(commandLineArgs* cLA, int index, std::vector<std::string> arguments){
    if((index+4) != arguments.size()){
        command_err(arguments[index], "<recipient> <subject> <body>");
        return 1;
    }
    else{
        cLA->command = arguments[index];
        cLA->send_flag = true;
        cLA->recipient = arguments[index+1];
        cLA->subject = arguments[index+2];
        cLA->body = arguments[index+3];
        return 0;
    }
}

/**
 *  @brief Prints error string.
 *  @param str input string
*/
void err_msg(std::string str){
    std::cout << str << std::endl;
}

/**
 *  @brief Command line arguments parser.
 *  @param cLA structure to store command line arguments
 *  @param arguments vector of arguments
 *  @param raw_args vector of unescaped arguments
 *  @returns 0 when arguments are valid, 1 otherwise
*/
int parse_cLA(commandLineArgs* cLA, std::vector<std::string> arguments, std::vector<std::string> raw_args){
    if(arguments.size() == 0 || (arguments.size() == 1 && (arguments[0] == "-h" || arguments[0] == "--help"))){
        print_help();
        return 1;
    }
    int error = 0;
    for(int i = 0; i < arguments.size(); i++){
        if(arguments[i] == "-a" || arguments[i] == "--address"){
            cLA->hostname = arguments[++i];
        }
        else if(arguments[i] == "-p" || arguments[i] == "--port"){
            cLA->port = arguments[++i];
            if(cLA->port.length() == 0){
                print_help();
                return 1;
            }
            for(int port_i = 0; port_i < cLA->port.length(); port_i++){
                if((((int)cLA->port[port_i]) < 48) || (((int)cLA->port[port_i]) > 57)){
                    print_help();
                    return 1;
                }
            }
            int port = std::stoi(cLA->port);
            if((port < 0) || (port > 65535)){
                err_msg("Invalid port, allowed range <0, 65535>");
                return 1;
            }
        }
        else if(arguments[i] == "register" || arguments[i] == "login"){
            error = parse_register_or_login_cmd(cLA,i,arguments,raw_args);
            break;
        }
        else if(arguments[i] == "send"){
            error = parse_send_cmd(cLA,i,arguments);
            break;
        }
        else if(arguments[i] == "fetch"){
            error = parse_fetch_cmd(cLA,i,arguments);
            break;
        }
        else if(arguments[i] == "list" || arguments[i] == "logout"){
            if( (i+1) == arguments.size()){
                cLA->command = arguments[i];
                if(arguments[i] == "list"){
                    cLA->list_flag = true;
                }
                else{
                    cLA->logout_flag = true;
                }
                break;
            }
            else{
                print_help();
                return 1;
            }
        }
        else {
            print_help();
            return 1;
        }   
    }
    return error;
}

/**
 *  @brief Calculates non-zero values for base64 encoding.
 *  @param char_vector vectof of chars
 *  @returns number of non-zero values in vector of chars
*/
int get_nonzero_values(std::vector<unsigned char> char_vector){
    int count = 0;
    for(int i = 0; i < char_vector.size(); i++){
        if(char_vector[i] > 0){
            count ++;
        }
    }
    return count;
}

/**
 *  @brief Checks if output character is in the table limits for base64 encoding.
 *  @param base_64_table string with base table
 *  @param character one char of 6_b vector
 *  @returns true if character is in range <0, table len - 1>, otherwise false
*/
bool check_limits(const std::string base_64_table, unsigned char character){
    if((character >= 0) && (character < base_64_table.length())){
        return true;
    }
    return false;
}

/**
 *  @brief Encoding input string to base64 format.
 *  @param input_string  password in plain text
 *  @returns password in base_64 encode
*/
std::string encoded_string(std::string input_string){
    const std::string base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out_string;
    std::vector<unsigned char> char_8b;
    char_8b.resize(3);
    std::vector<unsigned char> char_6b;
    char_6b.resize(4);
    while(!input_string.empty()){
        for(int i = 0; i < char_8b.size(); i++) {
            if(! input_string.empty()){
                char_8b[i] = input_string.front();
                input_string.erase(input_string.begin());
            }
        }
        char_6b[0] = char_8b[0] >> 2u;
        char_6b[1] = (((char_8b[0] << 4u) | (char_8b[1] >> 4u)) & 0b00111111);
        char_6b[2] = (((char_8b[1] << 2u) | (char_8b[2] >> 6u)) & 0b00111111);
        char_6b[3] = char_8b[2] & 0b00111111; 
        int non_zero_chars = get_nonzero_values(char_6b);
        if((non_zero_chars <= 2) && check_limits(base64_table,char_6b[0]) && check_limits(base64_table,char_6b[1])){
            out_string = out_string + base64_table[char_6b[0]] + base64_table[char_6b[1]] + "==";
        }
        else if(non_zero_chars == 3 && check_limits(base64_table,char_6b[0]) 
            && check_limits(base64_table,char_6b[1]) && check_limits(base64_table,char_6b[2])
        ){
            out_string = out_string + base64_table[char_6b[0]] + base64_table[char_6b[1]] + base64_table[char_6b[2]] + "=";
        }
        else if(non_zero_chars == 4 && check_limits(base64_table,char_6b[0]) 
            && check_limits(base64_table,char_6b[1]) && check_limits(base64_table,char_6b[2]) 
            && check_limits(base64_table,char_6b[3])
        ){
            out_string = out_string + base64_table[char_6b[0]] + base64_table[char_6b[1]]
            + base64_table[char_6b[2]] + base64_table[char_6b[3]];
        }
        char_8b.clear();
        char_8b.resize(3);
        char_6b.clear();
        char_6b.resize(4);
    }
    return out_string;
}

/**
 *  @brief Read login-token char by char.
 *  @returns string with "login-token" data
*/
std::string read_token(){
    FILE *f = fopen("login-token", "r");
    if(f == NULL){
        return "";
    }
    std::string data = "\"";
    char c;
    while((c = fgetc(f)) != EOF){
        data += c;
    }
    data += "\"";
    fclose(f);
    return data;
}

/**
 *  @brief Markify string.
 *  @param input_str string which will be marked
 *  @returns Marked string.
*/
std::string markify(std::string input_str){
    return "\"" + input_str + "\"";
}

/**
 *  @brief Split one list record to vector of record items.
 *  @param str one record from list response
 *  @returns vector of items
*/
std::vector<std::string> split_list_message(std::string str){
    std::vector<std::string> message_data;
    std::string msg_id;
    std::string tmp;
    const char mark = '"';
    int mark_counter = 0;
    int item_start_index = 0;
    for(int i = 0; i < str.length(); i++){
        if(str[i] == mark){
            if(message_data.size() == 0){
                for(int j = 0; j < (i-1); j++){
                    msg_id += str[j];
                }
                message_data.push_back(msg_id);
                mark_counter++;
                item_start_index = i+1;
            }
            else {
                if(((i-1)>=0) && (str[i-1] != '\\')){
                    mark_counter++;
                    if(mark_counter % 2){
                        item_start_index = i+1;
                    }
                    else{
                        for(int j = item_start_index; j < i; j++){
                            tmp += str[j];
                        }
                        message_data.push_back(tmp);
                        tmp.clear();
                    }
                }
            }
        }
    }
    return message_data;
}

/**
 *  @brief Calculates number of consecutive backslashes.
 *  @param str source string from which calculates consecutive backslashes
 *  @param end starting position for iteration
 *  @returns true if number of consecutive backslashes is odd, false otherwise
*/
bool odd_backslash(std::string str, int end){
    int backslash_count = 0; 
    const char mark = '\\';
    for(int i = end; i >= 0; i--){
        if(str[i] == mark){
            backslash_count++;
        } else{
            break;
        }
    }
    if(backslash_count % 2){
        return true;
    }
    return false;
}

/**
 *  @brief Correct server response to include valid special chars.
 *  @param str source string from server response
 *  @returns string with unescaped special characters
*/
std::string unescape_special_chars(std::string str){
    std::string tmp = str;
    std::vector<char> special_chars = {'\n', '\t', '"'};
    std::vector<char> chars = {'n', 't', '"'};
    int fix_count = 0;
    for(int i = 0; i < special_chars.size(); i++){
        std::string substr(1, special_chars[i]);
        for(int j = 0; j < str.length(); j++){
            if(str[j] == '\\' && ((j+1)< str.length()) && (str[j+1] == chars[i])){
                if(odd_backslash(tmp, j-fix_count)){
                    tmp.replace(j-fix_count, 2, substr);
                    fix_count++;
                }
            }
        }
        str = tmp;
        fix_count = 0;
    }
    return tmp;
}

/**
 *  @brief Generate half of backslashes from the native sequence of backslashes.
 *  @param halfcount count of backslashes in sequence
 *  @returns string with the generated number of backslashes.
*/
std::string half_of_backslash(int halfcount){
    std::string str;
    for(int i = 0; i < halfcount; i++){
        str += '\\';
    }
    return str;
}

/**
 *  @brief Detect last two " and returns content between marks.
 *  @param str string with server response
 *  @returns content between marks (login-token from server response to login request).
*/
std::string find_login_token(std::string str){
    int last_mark = str.find_last_of("\"");
    if(last_mark == std::string::npos){
        return "";
    }
    int pre_last_mark = str.substr(0, last_mark).find_last_of("\"");
    if(pre_last_mark == std::string::npos){
        return "";
    }
    return str.substr(pre_last_mark+1, last_mark - pre_last_mark - 1);
}

/**
 *  @brief Finds and returns data between "".
 *  @param str string from server response
 *  @returns data which are located between "" ("data" -> data)
*/
std::string find_data_between_marks(std::string str){
    int start_index = 0;
    int end_index = 0;
    for(int i = 0; i < str.length(); i++){
        if((str[i] == '"') && ((i-1) >= 0) && (str[i-1] != '\\') && (start_index == 0) && ((i+1) < str.length())){
            start_index = i+1;   
        }
        else if((str[i] == '"') && ((i-1) >= 0) && (str[i-1] != '\\') && (start_index != 0)){
            return str.substr(start_index, i - start_index);
        }
    }
    return "";
}

/**
 *  @brief Unescape data from server response.
 *  @param str string from server response
 *  @returns unescaped data string
*/
std::string unescape_data(std::string str){
    str = unescape_special_chars(str);
    const char mark = '\\';
    std::string out;
    int backslash_count = 0;
    bool backslash_sequence = false;

    for(int i = 0; i < str.length(); i++){
        if(backslash_sequence){
            if(str[i] == mark){
                backslash_count ++;
            } else {
                out += half_of_backslash(backslash_count/2) + str[i];
                backslash_count = 0;
                backslash_sequence = false;
            }
        }else{
            if(str[i] == mark){
                if(((i+1)< str.length()) && (str[i+1] = mark)){
                    backslash_count++;
                    backslash_sequence = true;
                }
                else{
                    out += str[i];
                }
            }
            else{
                out += str[i];
            }
        }
    }
    return out;
}

/**
 *  @brief Add whitespace at the end of the input string.
 *  @param str input string
 *  @returns string following by whitespace
*/
/* Add whitespace at the end of the string. */
std::string add_whitespace(std::string str){
    return str + " ";
}

/**
 *  @brief Add entry bracket to the start string and close bracket to exit of string.
 *  @param str input string
 *  @returns (input string)
*/
std::string bracketify(std::string str){
    return "(" + str + ")";
}

/**
 *  @brief Create message which will then be sent to the server.
 *  @param cLA structure to store command line arguments
 *  @returns message which will be sent to the server as a request
*/
std::string create_client_message(commandLineArgs* cLA){
    std::string message;
    if(cLA->register_flag || cLA->login_flag){
        message = bracketify(add_whitespace(cLA->command) + add_whitespace(markify(cLA->username)) + markify(encoded_string(cLA->password)));
    }
    else if(cLA->list_flag || cLA->logout_flag){
        std::string token = read_token();
        if(token.empty()){
            err_msg("Not logged in");
            return "";
        }
        message = bracketify(add_whitespace(cLA->command) + token);
    }
    else if(cLA->send_flag){
        std::string token = read_token();
        if(token.empty()){
            err_msg("Not logged in");
            return "";
        }
        message = bracketify(add_whitespace(cLA->command) + add_whitespace(token) + add_whitespace(markify(cLA->recipient))
        +add_whitespace(markify(cLA->subject)) + markify(cLA->body)); 
    }
    else if(cLA->fetch_flag){
        std::string token = read_token();
        if(token.empty()){
            err_msg("Not logged in");
            return "";
        }
        message = bracketify(add_whitespace(cLA->command) + add_whitespace(token) + std::to_string(cLA->fetch_id));
    }
    else{
        print_help();
        return "";
    }
    return message;
}

/**
 *  @brief Writes token data into login-token file.
 *  @param token_data string with login token data
 *  @returns true if data was successfully written, otherwise returns false
*/
bool write_token(std::string token_data){
    FILE* f = fopen("login-token", "w+");
    if(f == NULL){
        return false;
    }
    fprintf(f, token_data.c_str());
    fclose(f);
    return true;
}

/**
 *  @brief Deletes login-toke file, after logout.
 *  @returns true if file was successfully deleted, otherwise returns false
*/
bool delete_token(){
    if(remove("login-token") == 0){
        return true;
    }
    return false;
}

/**
 *  @brief Expands server response to vector with recipient, subject & body.
 *  @param str string with server response
 *  @returns vector with following items [recipient, subject, body ]
*/
std::vector<std::string> expand_response(std::string str){
    std::vector<std::string> data;
    int data_start = 0;
    int data_end = 0;
    for(int i = 0; i < str.length();i++){
        if(str[i] == '"' && data_start != 0){
            if(((i-1) < 0) || str[i-1] == '\\'){
                continue;
            }
            else{
            data_end = i;
            data.push_back(str.substr(data_start + 1, data_end-data_start-1));
            data_start = 0;
            data_end = 0;
            }
        }
        else if(str[i] == '"' && data_start == 0){
            if(((i-1) < 0) || str[i-1] == '\\'){
                continue;
            }
            else{
            data_start = i;
            }
        }
    }
    return data;
}

/**
 *  @brief Processes server response to list request - to vector of substrings.
 *  @param str string with server response
 *  @returns vector of substrings
*/
std::vector<std::string> parse_list(std::string in_data){
    std::vector<std::string> list_data;
    int depth = 0;
    bool enabled = false;
    int data_start = 0; 
    
    for(int i = 0; i < in_data.length(); i++){
        if(in_data[i] == '('){
            depth++;
        }
        else if(in_data[i] == ')'){
            depth--;
            if(enabled){
                enabled = false;
                list_data.push_back(in_data.substr(data_start, i - data_start));
            }
        }
        else if(depth == 3 && !enabled){
            enabled = true;
            data_start = i;
        }
    }
    return list_data;
}

/**
 *  @brief Writes result of the communication between client and server to STDOUT.
 *  @param cLA structure to store command line arguments
 *  @param server_response string with server response
*/
void write_response(commandLineArgs *cLA, std::string server_response){
    std::string valid = "ok";
    std::string invalid = "err";
    std::string out;
    //parsing response status
    if(server_response.substr(1, valid.size()) == valid){
        out += "SUCCESS: ";
    }
    else if(server_response.substr(1, invalid.size()) == invalid){
        out += "ERROR: ";
    }
    if(cLA->register_flag){
       std::string register_data = unescape_data(find_data_between_marks(server_response));
       if(!register_data.empty()){
           out += register_data;
       }
    }
    else if(cLA->login_flag){
        if(server_response.substr(1, valid.size()) == valid){
            std::string login_token = find_login_token(server_response);
            if(write_token(login_token) == false){
                err_msg("ERROR: Cannot successfully parse server response.");
            } else{
            out += "user logged in";
            }
        }
        else {
            out += find_data_between_marks(server_response);
        }
    }
    else if(cLA->logout_flag){
        std::string data = find_data_between_marks(server_response);
        out += data;
        delete_token();
    }
    else if(cLA->send_flag){
        std::string data = find_data_between_marks(server_response);
        out += data;
    }
    else if(cLA->fetch_flag){
        if(server_response.substr(1, valid.size()) == valid){
            std::vector<std::string> data = expand_response(server_response);
            out += "\n\nFrom: " + unescape_data(data[0]) + "\nSubject: " + unescape_data(data[1]) + "\n\n" + unescape_data(data[2]);
            std::cout << out;
            return;
        }
        else{
            out += find_data_between_marks(server_response);
        }
    }
    else if(cLA->list_flag){
        std::vector<std::string> list = parse_list(server_response);
        for(int i = 0; i < list.size(); i++){
            std::vector<std::string> data_items = split_list_message(list[i]);
            for(int k = 1; k < data_items.size(); k++){
                data_items[k] = unescape_data(data_items[k]);
            }
            if(data_items.size() == 3){
                out += "\n" + data_items[0] + ":\n" + "  From: " + data_items[1] + "\n  Subject: " + data_items[2];
            }
        }
        out += "\n";
        std::cout << out;
        return;
    }
    std::cout << out << std::endl;
}

/**
 *  @brief Establish connection with server, sends message and recieve response from server.
 *  @param cLA structure to store command line arguments
 *  @param client_message string with data which will be sent to the server
 *  @returns 0 when the communication was successful, 1 otherwise 
*/
int client_infrastructure(commandLineArgs *cLA, std::string client_message){
    // socket structures and variables
    int fd;
    char recieved_data[4096];
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *result, *address;
    // convert strings to char * and const char *
    const char *hostname = cLA->hostname.c_str();
    const char *port = cLA->port.c_str();
    char *message = &*client_message.begin();
    // setup hints
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;
    // load addr info to result struct
    if(getaddrinfo(hostname, port, &hints, &result) != 0){
        std::cout << "ERROR: Invalid hostname " << cLA->hostname << "\n";
        return 1;
    }
    // try to establish tcp connection
    for(address = result; address != NULL; address = address->ai_next){
        fd = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if(fd == -1){
            continue;
        }
        if(connect(fd, address->ai_addr, address->ai_addrlen) != 0){
            close(fd);
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    // check if connection is active
    if(address == NULL){
        std::cout << "Cannot connect to server." <<  std::endl;
        return 1;
    }
    // send client message to server
    if(send(fd,message, strlen(message),0) <= 0){
        std::cout << "Unable to send request." << std::endl;
        return 1;
    }
    // create variables to store server respond
    std::string server_response;
    ssize_t read_len;
    while((read_len = recv(fd, recieved_data, sizeof(recieved_data),0)) > 0){
        server_response = server_response + recieved_data;
    }
    close(fd);
    if(server_response.empty()){
        return 1;
    } else {
        write_response(cLA, server_response);
        return 0;
    }
}

/**
 *  @brief Replaces character specified by seq.
 *  @param str input string
 *  @param seq one of ['\\','\n', '\t', '\"'] sequence chars
 *  @param repl replacements for seq parameter one of ["\\\\", "\\n", "\\\t", "\\\""]
 *  @returns escaped string
*/
std::string fix_string(std::string str, char seq, std::string repl){
    std::vector<std::string> sections_before_seq;
    std::string rest_of_str;
    std::string fixed_str;
    int n;
    while(true){
        n = str.find(seq);
        if(n != std::string::npos){
            sections_before_seq.push_back(str.substr(0, n));
            str = str.substr(n+1, str.length() - (n+1));
        }
        else{
            rest_of_str = str;
            break;
        }
    }
    for(int i = 0; i < sections_before_seq.size(); i++){
        fixed_str = fixed_str + sections_before_seq[i] + repl;
    }
    fixed_str += rest_of_str;
    return fixed_str;
}

/**
 *  @brief Returns escaped command line arguments.
 *  @param raw_args non-escaped command line arguments
 *  @returns escaped command line arguments
*/
std::vector<std::string> escape_special_chars(std::vector<std::string> raw_args){
    std::vector<std::string> clean_args;
    std::vector<char> sequences = {'\\','\n', '\t', '\"'};
    std::vector<std::string> replacements = {"\\\\", "\\n", "\\\t", "\\\""};
    std::vector<int> indexes;
    int n;
    for(int i = 0; i < raw_args.size(); i++){
        for(int x = 0; x < sequences.size() && x < replacements.size();x++){
            if((n = raw_args[i].find(sequences[x])) != std::string::npos){
                raw_args[i] = fix_string(raw_args[i], sequences[x], replacements[x]);
            }
        }
        clean_args.push_back(raw_args[i]);
    }
    return clean_args;
}

int main(int argc, char *argv[]){
    commandLineArgs* cLA = new commandLineArgs();
    std::vector<std::string> raw_args(argv + 1, argv + argc);
    std::vector<std::string> args = escape_special_chars(raw_args);
    int cLAEnd = parse_cLA(cLA, args, raw_args);
    if(cLAEnd){
        delete cLA;
        return 1;
    } 
    else {
        std::string message = create_client_message(cLA);
        if(message.empty()){
            delete cLA;
            return 1;
        }
        else {
            client_infrastructure(cLA, message);
            delete cLA;
            return 0;
        }
    } 
}