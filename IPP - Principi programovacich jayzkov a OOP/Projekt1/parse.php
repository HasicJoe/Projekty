<?php

/**
 * @file parse.php
 * @author Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
 */

ini_set('display_errors', 'stderr');

$xml = new XMLWriter();
$parsed_files = array();
$unique_labels = array();
$stats_on = 0;
$bad_jumps = 0;
$order_args = array();
$jumpers = array();

# associate arrays
$return_values = array(
    "RETURN_OK" => 0,
    "RETURN_MISSING_ARG" => 10,
    "RETURN_IFILE_ERR" => 11,
    "RETURN_OFILE_ERR" =>12,
    "RETURN_INTERN_ERR" =>99,
    "RETURN_INVALID_HEADER" =>21,
    "RETURN_INVALID_OP_CODE" => 22,
    "RETURN_ERR_SOURCE_CODE" => 23);

$inst_regex = array(
    "VAR_SYMB" =>'/(^MOVE$|^INT2CHAR$|^STRLEN$|^TYPE$)/',
    "EMPTY" =>'/(^CREATEFRAME|^PUSHFRAME|^POPFRAME|^RETURN|^BREAK)(\s{0,}#{0,}.{0,})$/',
    "LABEL_SYMB_SYMB" => '/(^JUMPIFEQ$|^JUMPIFNEQ$)/',
    "LABEL" =>'/(^CALL$|^JUMP$|^LABEL$)/',
    "VAR_SYMB_SYMB" => '/(^ADD$|^SUB$|^MUL$|^IDIV$|^LT$|^GT$|^EQ$|^AND$|^OR$|^STRI2INT$|^CONCAT$|^GETCHAR$|^SETCHAR$)/',
    "SYMB"=>'/(^PUSHS$|^WRITE$|^EXIT$|^DPRINT$)/',
    "VAR" => '/(^DEFVAR$|^POPS$)/');

$stats_counter = array(
    "comments" =>0,
    "instructions" =>0,
    "jumps" => 0,
    "fwjumps" => 0,
    "backjumps" =>0 ); #labels stored in $unique_labels


/**
 * @brief Function writes the values of monitored parameters to the files specified by the --stats parameter 
 */
function write_stats(){
    global $stats_counter,$return_values,$parsed_files,$order_args,$unique_labels;
    check_files();
    if(count($parsed_files) != count($order_args)){
        get_return_value($return_values["RETURN_MISSING_ARG"]);
    }
    else {
        for($i = 0 ; $i < count($parsed_files) ; $i = $i + 1){
            $file = fopen($parsed_files[$i],"w");
            if(!$file){
                get_return_value($return_values["RETURN_OFILE_ERR"]);
            }
            $arg_order = preg_split("/,/", $order_args[$i]);
            $data =0;
            for($j = 0 ; $j < count($arg_order) ; $j = $j + 1){
                if($arg_order[$j] == "com" ){
                    $data = $stats_counter["comments"];
                } else if ($arg_order[$j] == "lab" ){
                    $data = count($unique_labels);
                }
                else if ($arg_order[$j] == "jmp"){
                    $data = $stats_counter["jumps"];
                }
                else if ($arg_order[$j] == "fwjmp" ){
                    $data = $stats_counter["fwjumps"];
                }
                else if ($arg_order[$j] == "bcjmp" ){
                    $data = $stats_counter["backjumps"];
                }
                else if ($arg_order[$j] == "bdjmp"){
                    $data = search_badjumps();
                }
                $no_error = fwrite($file,$data."\n");
                if(!$no_error){
                    get_return_value($return_values["RETURN_OFILE_ERR"]);
                }
            }
        }
    }
}


/**
 * @brief Function checks if the file path does not contain subdirectories and if yes -> makedirs
 */
function check_files(){
    global $parsed_files,$return_values;
    if(count(array_unique($parsed_files)) < count($parsed_files)) {
        get_return_value($return_values["RETURN_OFILE_ERR"]);
    }
    $backslash_pattern = "/\\\/";
    $filepath = "";
    for($i = 0 ; $i < count($parsed_files) ; $i = $i + 1){
        if(preg_match($backslash_pattern,$parsed_files[$i])){
            $split_file = preg_split($backslash_pattern,$parsed_files[$i]);
            for($k = 0 ; $k < count($split_file) - 1; $k = $k + 1){
                $filepath .= $split_file[$k];
                $filepath = $filepath . "\\";
            }
            if(!is_dir($filepath)) {
                $my_makedir = mkdir($filepath,0777,true);
                $filepath = "";
                if(!$my_makedir){
                    get_return_value($return_values["RETURN_OFILE_ERR"]);
                }
            }
        }
    }
}


/**
 * @brief Function checks if the label is already defined
 * @param label label from array $jumpers
 * @return +1  if the label is not in the unique label array
 */

function is_badjump($label) {
    global $unique_labels;
    foreach($unique_labels as $this_label){
        if($this_label == $label){
            return 0;
        }
    }
    return 1;
}


/**
 * @brief Function count badjumps from the array of jumpers
 * @return bad_jumps number of badjumps
 */
function search_badjumps(){
    global $jumpers,$bad_jumps;

    for($i = 0; $i < count($jumpers) ; $i = $i+1) {
        $bad_jumps += is_badjump($jumpers[$i]);
    }
    return $bad_jumps;
}


/**
 * @brief Function adds new unique label to array of unique labels 
 * @param label name of label which has to be added to array of unique labels
 */
function add_label($label){
    global $unique_labels;
    foreach($unique_labels as $this_label) {
        if($this_label == $label) {
            return;
        }
    }
    array_push($unique_labels,$label);
}

/**
 * @brief Function checks if the label is already defined
 * @param $label label from array $jumpers
 * @return return +1 when the label is already defined
 */
function check_label($label){
    global $unique_labels,$jumpers;
    foreach($unique_labels as $this_label) {
        if($this_label == $label) {
            return 1;
        }
    }
    array_push($jumpers,$label);
    return 0;
}


/**
 * @brief Function parse jumps and also decides if the jumps is backjump or forward jump 
 * @param instruction represents the name of the instruction from the instruction set
 * @param label represents the name of the label
 */
function parse_jumps($instruction,$label){
    global $stats_counter;
    $jump_instructions = array("CALL","JUMP","JUMPIFEQ","JUMPIFNEQ");
    foreach($jump_instructions as $this_instruction){
        if($instruction == $this_instruction){
            $stats_counter["jumps"] = $stats_counter["jumps"] + 1; # add jumps :)
            if(check_label($label)) {
                $stats_counter["backjumps"] = $stats_counter["backjumps"] + 1;
            }
            else {
                $stats_counter["fwjumps"] = $stats_counter["fwjumps"] + 1;
            }
        }
    }
}


/**
 * @brief Function open stdin to read content
 */
function open_stdin(){
    global $return_values;
    $file = fopen('php://stdin','r');
    if(isset($file)) {
        generate_header($file);
    }
    else {
        get_return_value($return_values["RETURN_IFILE_ERR"]);
    }
}


/**
 * @brief Function terminates program run with exit code 
 * @param return_value represents the return value with which the program has to stop execution
 */
function get_return_value($return_value){
    exit($return_value);
}


/**
 * @brief Function generate program header .IPPcode21
 * @param file opened stream of standard input
 */
function generate_header($file){
    global $return_values,$xml;
    # setup xmlwriter to parse to xml
    $xml->openMemory();
    $xml->setIndent(true); # add \n after xml inst
    $xml->startDocument('1.0', 'UTF-8');
    # generate program header
    $lines_to_reach_header = 0;
   
    while(!feof($file)){
        $program_header = trim(fgets($file));
        $program_header = strtoupper($program_header);
        if($lines_to_reach_header == 0 and feof($file) and empty($program_header)){ # for empty file
            get_return_value($return_values["RETURN_INVALID_HEADER"]);
        }
        if(preg_match('/^\.IPPCODE21((\s{0,}#{1,}.{0,})|(\s{0,}))$/', $program_header)){
            break;
        }
        else if(!preg_match('/^#/', $program_header) and !preg_match('/^(^\s{0,})$/', $program_header)){
            get_return_value($return_values["RETURN_INVALID_HEADER"]);
        }
        $lines_to_reach_header = $lines_to_reach_header + 1;
    }    
    $xml->startElement("program");
    $xml->writeAttribute("language", "IPPcode21");
    generate_body($file);
}


/**
 * @brief Function prints help message to stdout
 */
function print_help(){
    global $return_values;
    $help_msg = "\tIPP 2020/2021 - Analyzátor kódu IPPcode21\n";
    $help_msg .= "\tAutor: Samuel Valaštín\n";
    $help_msg .= "\tPoužitie : php parse.php <stdin \n";
    $help_msg .= "\tPovolené prepínače : --help - výpis nápovedy\n";
    $help_msg .= "\tRozšírené prepínače: --stats=file (zbieranie štatistík)\n";
    $help_msg .= "\t --loc       => výpis počtu riadkov s inštrukciami\n";
    $help_msg .= "\t --comment   => výpis počtu riadkov s komentármi\n";
    $help_msg .= "\t --labels    => výpis počtu definovaných náveští\n";
    $help_msg .= "\t --jumps     => výpis počtu všetkých návratov z volania pre skoky\n";
    $help_msg .= "\t --fwjumps   => výpis počtu skokov dopredu\n";
    $help_msg .= "\t --backjumps => výpis počtu spätných skokov\n";
    $help_msg .= "\t --badjumps  => výpis skokov na neexistujúce náveštie\n";
    echo $help_msg;
    get_return_value($return_values["RETURN_OK"]);
}


/**
 * @brief Function parse command line arguments
 * @param opt_args array of command line arguments
 */
function parse_arguments($opt_args){
    global $return_values,$parsed_files,$stats_on,$order_args;

    if(count($opt_args) == 1){
        open_stdin();
    }
    if(count($opt_args) == 2) {

        if (preg_match("/^--help$/", $opt_args[1])) {
            print_help();
        }
        else {
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    if(preg_match("/^--stats=*/",$opt_args[1])) {

        if(count($opt_args) == 2){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }

        $split_file = preg_split("/=/", $opt_args[1]);
        $stats_on = 1;

        if(!isset($opt_args[2])){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }

        if(preg_match("/^--stats=*/",$opt_args[2])){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }

        if(!isset($split_file[1])){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }

        if(isset($split_file[1])) {

            $file = $split_file[1];
            array_push($parsed_files,$file);
            $order = "";

            for($i = 2; $i < count($opt_args);$i = $i + 1){

                if(preg_match("/^--loc$/",$opt_args[$i])) {
                    $order = $order. "loc,";
                }
                else if(preg_match("/^--comments$/",$opt_args[$i])) {
                    $order = $order."com,";
                }
                else if(preg_match("/^--labels$/",$opt_args[$i])) {
                    $order = $order."lab,";
                }
                else if(preg_match("/^--jumps$/",$opt_args[$i])) {
                    $order = $order."jmp,";
                }
                else if(preg_match("/^--fwjumps$/",$opt_args[$i])) {
                    $order = $order."fwjmp,";
                }
                else if(preg_match("/^--backjumps$/",$opt_args[$i])) {
                    $order = $order."bcjmp,";
                }
                else if(preg_match("/^--badjumps$/",$opt_args[$i])){
                    $order = $order."bdjmp,";
                }
                else if(preg_match("/^--stats=*/",$opt_args[$i])){

                    if($i + 1 == count($opt_args)){
                        get_return_value($return_values["RETURN_MISSING_ARG"]);
                    }
                    if(empty($order)){
                        get_return_value($return_values["RETURN_MISSING_ARG"]);
                    }
                    $another_file = preg_split("/=/", $opt_args[$i]);

                    if(isset($another_file[1])) {

                        $order = substr($order,0,-1);
                        array_push($order_args,$order);
                        $order = "";
                        array_push($parsed_files, $another_file[1]);
                    }
                    else{
                        get_return_value($return_values["RETURN_MISSING_ARG"]);
                    }
                }
                else{
                    get_return_value($return_values["RETURN_MISSING_ARG"]);
                }
            }
            $order = substr($order,0,-1); # remove last ,
            array_push($order_args,$order);
        }
    }
    else {
        get_return_value($return_values["RETURN_MISSING_ARG"]);
    }
    open_stdin();
}


/**
 * @brief Function write instruction to xml
 * @param instruction represents the name of the instruction
 * @param order represents the order of the instruction in the code
 * @return order_count number of written instructions
 */
/* Function write instruction to xml */
function write_instruction($instruction,$order_count){
    global $xml;
    $xml->startElement("instruction");
    $xml->writeAttribute("order",$order_count++);
    $xml->writeAttribute("opcode",$instruction);
    return $order_count;
}


/**
 * @brief Function checks the validity of variables and also write to xml if variable is legal
 * @param inst_argr represents the variable (for example GF@x)
 * @param arg_pos represents the position of the argument in the line
 */
function validate_var($inst_arg,$arg_pos){
    global $xml,$return_values;
    
    if(preg_match("/(^[T|L|G]{1}[F]{1}[@]{1}[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$)/",$inst_arg)){
        $arg_counter = "arg".$arg_pos;
        $split_arg = preg_split("/@/", $inst_arg);
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","var");
        $xml->text($inst_arg);
        $xml->endElement();
    }
    else {
        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
    }
}


/**
 * @brief Function checks if symbs ale legal -> strings, ints, bools, nils 
 * @param inst_argr represents the symb (for example string@dog)
 * @param arg_pos represents the position of the argument in the line
 */
function validate_symb($inst_arg,$arg_pos){
    global $xml,$return_values;
    $arg_counter = "arg".$arg_pos;
    # for variables
    if(preg_match("/(^[T|L|G]{1}[F]{1}[@]{1}[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$)/",$inst_arg)){
        validate_var($inst_arg,$arg_pos);
        return;
    }   
    # for strings
    if(preg_match("/^((string){1}[@]{1}[^#|^\s]{0,})$/",$inst_arg)){ 
        $split_args = preg_split("/string@/",$inst_arg);  
        if(empty($split_args[1])){
            $xml->startElement($arg_counter);
            $xml->writeAttribute("type","string");
            $xml->endElement();
            return;
        }
        else if(preg_match("/^([^\#|^\s|^\\\]|([\\\][0-1][0-9][0-9]))*$/",$split_args[1])){     # max \126 
            $xml->startElement($arg_counter);
            $xml->writeAttribute("type","string");
            $xml->text($split_args[1]);
            $xml->endElement();
            return;
        }
        else{
            get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
        }        
    }
    # for integers
    else if(preg_match("/^((int){1}[@]{1}[^\s]{1,})$/",$inst_arg)){
        $split_args = preg_split("/int@/",$inst_arg);
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","int");
        $xml->text($split_args[1]);
        $xml->endElement();
        return;   
    }
    # for booleans
    else if(preg_match("/^((bool){1}[@]{1}(true|false){1})$/",$inst_arg)){
        $split_args = preg_split("/bool@/",$inst_arg);
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","bool");
        $xml->text($split_args[1]);
        $xml->endElement();
        return;
    }
    # for nils
    else if(preg_match("/^((nil@nil){1})$/",$inst_arg)){
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","nil");
        $xml->text("nil");
        $xml->endElement();
        return;
    }
    else {
        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
    }
}


/**
 * @breif Function checks if the label is legal
 * @param inst_argr represents the label
 * @param arg_pos represents the position of the argument in the line
 */
function validate_label($inst_arg,$arg_pos){

    global $xml,$return_values;
    $arg_counter = "arg".$arg_pos;
    if(preg_match("/^[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$/", $inst_arg)){     #label cannot be empty otherwise string
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","label");
        $xml->text($inst_arg);
        $xml->endElement();
    }
    else {
        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
    }
}


/**
 * @breif Function checks if the type arguments is legal
 * @param inst_argr represents the label
 * @param arg_pos represents the position of the argument in the line
 */
function validate_type($inst_arg,$arg_pos){

    global $xml,$return_values;
    $arg_counter = "arg".$arg_pos;
    if(preg_match("/^(int|bool|string)$/",$inst_arg)){
        $xml->startElement($arg_counter);
        $xml->writeAttribute("type","type");
        $xml->text($inst_arg);
        $xml->endElement();
        return;
    }
    else {
        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
    }
}


/**
 * @brief Function ends instructions
 */
function end_instr(){
    global $xml;
    $xml->endElement();
}


/**
 * @brief Function remove empty elements from array after splitting
 * @param array array after splitting with possible empty fields
 * @return fixed_arr array without empty fields
 */
function fix_split($array){
    if(!is_array($array)){
        return $array;
    } 
    else {
        for($i = 0 ; $i < count($array);$i = $i + 1){
            if(!empty($array[$i])){
                $fixed_arr[] = $array[$i];
            }
        }
        if(isset($fixed_arr)){
            return $fixed_arr;
        } else {
            return $array;
        }
    }
}


/**
 * @brief Main function which controls whole program
 * @param file opened stream of standard input
 */
function generate_body($file){

    global $return_values,$xml,$stats_counter,$stats_on,$inst_regex;
    $order_count = 1;

    while(!feof($file)) {

        $line = trim(fgets($file));
        # replace more empty spaces
        $line = preg_replace("/\s+/", " ", $line);
        $split_line = preg_split("/(\s)/",$line); #split line to words by blank spaces
        $instruction = strtoupper($split_line[0]);
        
        if(!empty($instruction)) {
            $stats_counter["instructions"] = $stats_counter["instructions"] + 1;
        
            #comments handling
            if(preg_match("/#/",$line)){
                $stats_counter["comments"] = $stats_counter["comments"] + 1;
                $line = preg_split("/#/",$line);
                $split_line = preg_split("/(\s)/",$line[0]); #split line to words by blank spaces
                $split_line = fix_split($split_line);
                $instruction = strtoupper($split_line[0]);
            }
            # max 0 1 2 3
            if (isset($split_line[4])) {
                get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
            }
            if(!empty($instruction)) {
            # FSM START -------------------------------------------------------------
                if(preg_match($inst_regex["VAR_SYMB"], $instruction)) {
                    #<var> <symb>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and isset($split_line[2]) and !isset($split_line[3])) {
                        validate_var($split_line[1], 1);
                        validate_symb($split_line[2], 2);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match($inst_regex["EMPTY"], $instruction)) {
                    #<empty>
                    if(isset($split_line[1])){
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                    $order_count = write_instruction($instruction, $order_count);
                    end_instr();
                    if ($instruction == "RETURN") {
                        $stats_counter["jumps"] = $stats_counter["jumps"] + 1;
                    }
                }
                elseif(preg_match($inst_regex["LABEL_SYMB_SYMB"], $instruction)) {
                    # <label> <symb> <symb>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and isset($split_line[2]) and isset($split_line[3])) {
                        validate_label($split_line[1], 1);
                        validate_symb($split_line[2], 2);
                        validate_symb($split_line[3], 3);
                        end_instr();
                        parse_jumps($instruction, $split_line[1]);
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match($inst_regex["LABEL"], $instruction)) {
                    #<label>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and !isset($split_line[2])) {
                        validate_label($split_line[1], 1);
                        end_instr();
                        if(preg_match('/^LABEL$/', $instruction)) {
                            add_label($split_line[1]);
                        }
                        else {
                            parse_jumps($instruction, $split_line[1]);
                        }
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match($inst_regex["VAR_SYMB_SYMB"], $instruction)) {
                    # <var> <symb> <symb>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and isset($split_line[2]) and isset($split_line[3])) {
                        validate_var($split_line[1], 1);
                        validate_symb($split_line[2], 2);
                        validate_symb($split_line[3], 3);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match('/^NOT$/', $instruction)) {
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and isset($split_line[2]) and !isset($split_line[3])){
                        validate_var($split_line[1], 1);
                        validate_symb($split_line[2], 2);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match($inst_regex["VAR"], $instruction)) {
                    #<var>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and !isset($split_line[2])) {
                        validate_var($split_line[1], 1);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match($inst_regex["SYMB"], $instruction)) {
                    #<symb>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and !isset($split_line[2])) {
                        validate_symb($split_line[1], 1);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                elseif(preg_match('/^READ$/', $instruction)) {
                    # <var> <type>
                    $order_count = write_instruction($instruction, $order_count);
                    if(isset($split_line[1]) and isset($split_line[2]) and !isset($split_line[3])) {
                        validate_var($split_line[1], 1);
                        validate_type($split_line[2], 2);
                        end_instr();
                    }
                    else {
                        get_return_value($return_values["RETURN_ERR_SOURCE_CODE"]);
                    }
                }
                # ERROR OPCODE
                else {
                    get_return_value($return_values["RETURN_INVALID_OP_CODE"]);
                }
            # FSM END -------------------------------------------------------------   
            }
        }
    }
    fclose($file);
    $xml->endElement();
    $xml->endDocument();
    echo $xml->outputMemory();
    if($stats_on){
        write_stats();
    }
    get_return_value($return_values["RETURN_OK"]);
}
parse_arguments($argv);

?>
