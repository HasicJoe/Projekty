<?php

/**
 * @file test.php
 * @author Samuel Valaštín <xvalas10@stud.fit.vutbr.cz>
 */

ini_set('display_errors', 'stderr');

# as array to store command line arguments constants
$argument_constants = array(
    "parse_script" => "parse.php",
    "change_parse" => false,
    "interpret_script" => "interpret.py",
    "change_interpret" => false,
    "directory"   => "",
    "recursive" => false,
    "parse_only" => false,
    "int_only" => false,
    "jexamxml" => "/pub/courses/ipp/jexamxml/jexamxml.jar",
    "jexamconfig" => "/pub/courses/ipp/jexamxml/options"
);
# command line arguments
$cmd_options = array(
    "help",
    "directory:",
    "recursive",
    "parse-script:",
    "int-script:",
    "parse-only",
    "int-only",
    "jexamxml:",
    "jexamcfg:",
);
# return values
$return_values = array(
    "RETURN_OK" => 0,
    "RETURN_MISSING_ARG" => 10,
    "RETURN_IFILE_ERR" => 11,
    "RETURN_OFILE_ERR" =>12,
    "RETURN_INTERN_ERR" =>99,
    "RETURN_INVALID_DIRECTORY" =>41,
);
# regex which allows possible args
$check_arg_regex = "/^((--help)|(--directory=(.+))|(--recursive)|(--parse-script=(.+))|(--int-script=(.+))|(--parse-only)|(--int-only)|(--jexamxml=(.+))|(--jexamcfg=(.+))$)/";


/**
 * @brief Function checks if an incorrect argument has been entered
 * @param arguments represents array of command line arguments
 */
function check_arguments($arguments){
    global $check_arg_regex,$return_values;
    foreach($arguments as $this_argument){
        if($this_argument != "test.php"){
            if(!preg_match($check_arg_regex ,$this_argument)){
                get_return_value($return_values["RETURN_MISSING_ARG"]);
            }
        }
    }
}


/**
 * @brief Function parsing the command line options and checks their validity
 * @param options output of function getopt
 * @param argv    command line arguments(using for count args)
 */
function parse_arguments($options,$argv) {
    global $argument_constants,$return_values;
    
    if(isset($options["help"])){
        if(count($argv) == 2){
            print_help();
        } else {
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    if(isset($options["directory"])){
        check_multiplicity($options["directory"]);
        if(is_dir($options["directory"] )) {
            $argument_constants["directory"] = $options["directory"];
        }
        else{
            get_return_value($return_values["RETURN_INVALID_DIRECTORY"]);
        }
    }
    else{
        $argument_constants["directory"] = getcwd(); // actual dir
    }
    
    if(isset($options["recursive"])){
        $argument_constants["recursive"] = true;
    }
    
    if(isset($options["parse-script"])){
        if(is_file($options["parse-script"])) {
            $argument_constants["parse_script"] = $options["parse-script"];
            $argument_constants["change_parse"] = true;
        }
        else {
            get_return_value($return_values["RETURN_INVALID_DIRECTORY"]);
        }
    }
    if(isset($options["int-script"])){
        check_multiplicity($options["directory"]);
        if(is_file($options["int-script"])) {
            $argument_constants["int_script"] = $options["int-script"];
            $argument_constants["change_interpret"] = true;
        }
        else {
            get_return_value($return_values["RETURN_INVALID_DIRECTORY"]);
        }
    }
    if(isset($options["parse-only"])){
        $argument_constants["parse_only"] = true;
    }
    if(isset($options["int-only"])){
        $argument_constants["int_only"] = true;
    }
    if(isset($options["jexamxml"])){
        check_multiplicity($options["jexamxml"]);
        if(is_file($options["jexamxml"])) {
            $argument_constants["jexamxml"] = $options["jexamxml"];
        }
        else {
            get_return_value($return_values["RETURN_INVALID_DIRECTORY"]);
        }
    }
    
    if(isset($options["jexamcfg"])){
        check_multiplicity($options["jexamcfg"]);
        if(is_file($options["jexamcfg"])) {
            $argument_constants["jexamconfig"] = $options["jexamcfg"];
        }
        else {
            get_return_value($return_values["RETURN_INVALID_DIRECTORY"]);
        }
    }
    look_for_conflicts($argument_constants);
}


/**
 * @brief Function checks if the argument was entered only once
 * @param parameter represents parameter from the options
 */
function check_multiplicity($parameter){
    global $return_values;
    if(is_array($parameter)){
        get_return_value($return_values["RETURN_MISSING_ARG"]);
    }
}


/**
 * @brief Function looks for conflicts in command line args combination
 * @param arguments    command line arguments
 */
function look_for_conflicts($arguments){
    global $return_values;
    if($arguments["parse_only"] == true){
        if($arguments["int_only"] == true or $arguments["change_interpret"]){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    if($arguments["int_only"] == true){
        if($arguments["parse_only"] == true or $arguments["change_parse"]){
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    make_tests();
}


/**
 * @brief Function checks if scripts are valid
 */
function check_scripts() {
    global $argument_constants,$return_values;
    $scr_reg = array(
        "interpret" => "/^((.*)(interpret.py){1})$/",
        "parse" => "/^((.*)(parse.php){1})$/"
    );
    if($argument_constants["parse_only"]){
        if(preg_match($scr_reg["parse"],$argument_constants["parse_script"])){
            return;
        } else{
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    else if($argument_constants["int_only"]){
        if(preg_match($scr_reg["interpret"],$argument_constants["interpret_script"])){
            return;
        } else{
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
    else {
        if(preg_match($scr_reg["interpret"],$argument_constants["interpret_script"]) && preg_match($scr_reg["parse"],$argument_constants["parse_script"])){
            return;
        } else{
            get_return_value($return_values["RETURN_MISSING_ARG"]);
        }
    }
}

/**
 * @brief Function searching .in,.out,.src and .rc files and also manage program flow
 */
# recursive -> https://stackoverflow.com/questions/24783862/list-all-the-files-and-folders-in-a-directory-with-php-recursive-function
function make_tests(){
    global $argument_constants;
    check_scripts();
    $src_files = array();
    $out_files = array();
    $in_files = array();
    $returncode_files = array();
    $files_regex = array(
        "SRC_REGEX" => "/^((.+)(.src){1})$/",
        "OUT_REGEX" => "/^((.+)(.out){1})$/",
        "IN_REGEX" => "/^((.+)(.in){1})$/",
        "RETCODE_REGEX" => "/^((.+)(.rc){1})$/");

    if($argument_constants["recursive"] == true){
        
        $iterator = new RecursiveDirectoryIterator($argument_constants["directory"]);
        
        foreach(new RecursiveIteratorIterator($iterator) as $file){
            if(preg_match($files_regex["SRC_REGEX"],$file)){
                $src_files[] = $file;
            } else if(preg_match($files_regex["OUT_REGEX"],$file)){
                $out_files[] = $file;
            }
            else if(preg_match($files_regex["IN_REGEX"],$file)){
                $in_files[] = $file;
            }
            else if(preg_match($files_regex["RETCODE_REGEX"],$file)){
                $returncode_files[] = $file;
            }
        }
    }
    else{
        $files = scandir($argument_constants["directory"]);
        // remove . and .. from file list
        array_shift($files);
        array_shift($files);

        foreach($files as $file) {

            if(preg_match($files_regex["SRC_REGEX"],$file)){
                $src_files[] = $file;
            } else if(preg_match($files_regex["OUT_REGEX"],$file)){
                $out_files[] = $file;
            }
            else if(preg_match($files_regex["IN_REGEX"],$file)){
                $in_files[] = $file;
            }
            else if(preg_match($files_regex["RETCODE_REGEX"],$file)){
                $returncode_files[] = $file;
            }
        }
    }
    // do parse only tests
    if($argument_constants["parse_only"]){
        parse_test($src_files,$out_files,$in_files,$returncode_files);
    }
    // do interpret only tests
    else if($argument_constants["int_only"]) {
        diff_test($src_files,$out_files,$in_files,$returncode_files);
    }
    // both parse + interpret tests
    else {
        full_test($src_files,$out_files,$in_files,$returncode_files);
    }
    exit(0);
}


/**
 * @brief Function generates a new row of html table if the test passed
 * @param name name of the test
 * @param ret_val return value of test
 * @return html row of html table
 */
function add_passed_row($name,$ret_val){
    $html = "<tr style=\"background-color:green;color:white;\"><td>".$name."</td><td style=\"text-align:center;\">".$ret_val."</td><td style=\"text-align:center;\">PASSED</td></tr>";
    return $html;
}


/**
 * @brief Function generates a new row of html table if the test failed
 * @param name name of the test
 * @param ret_val return value of test
 * @return html row of html table
 */
function add_invalid_row($name,$ret_val){
    $html = "<tr style=\"background-color:black;color:red;\"><td>".$name."</td><td style=\"text-align:center;\">".$ret_val."</td><td style=\"text-align:center;\">FAILED</td></tr>";
    return $html;
}


/**
 * @brief Function generates a new row of html table if the test result was different from expected value
 * @param name name of the test
 * @param ret_val return value of test
 * @param exp expected value of test
 * @return html row of html table
 */
function add_invalid_row_exp($name,$ret_val,$exp){
    $html = "<tr style=\"background-color:black;color:white;\"><td>".$name."</td><td style=\"text-align:center;\">".$ret_val."</td><td style=\"text-align:center;\">FAILED(EXPECT:".$exp.")</td></tr>";
    return $html;
}


/**
 * @brief Function generates end of html doc
 * @return html end of HTML5 doc
 */
function html_end_gen(){
    $html = "</tbody></table></body></html>";
    return $html;
}


/**
 * @brief Function generates head of table with some basic style
 * @return html head of HTML table
 */
function setup_table(){
    $html = "<table style=\"width:90%;margin:auto;\"><thead style=\"background-color:crimson;color:white;\"><tr><th>
    <b>Nazov testu </b></th><th><b> Navratova hodnota </b></th><th><b>Vysledok</b></th></tr></thead><tbody>";
    return $html;

}


/**
 * @brief Function generates header of html document
 * @param tests number of tests
 * @param valid number of valid tests
 * @param invalid number of invalid tests
 * @return html header of html5 doc
 */
function generate_review($tests,$valid,$invalid){
    $html = "<!doctype html><html lang=\"sk\"><head><meta charset=\"UTF-8\"><title>Vysledky testov</title></head><body style=\"text-align:center; background-color:lightyellow;\">";
    $html = $html ."<h1>IPP 2021 - Vysledky testov</h1><p><b> Pocet testov: ".$tests.
    "</b></p><p><b> Pocet uspesnych testov: ".$valid."</b></p><p><b>Pocet neuspesnych testov: " .$invalid."</b></p>";
    return $html;
}


/**
 * @brief Function executes tests with parameter --int-only
 * @param src_files .src files
 * @param out_files .out files
 * @param in_files .in files
 * @param returncode_files .rc files
 */
function diff_test($src_files,$out_files,$in_files,$returncode_files){
    global $argument_constants;
    $zero = 0;
    $delta_count = 0;
    $accepted_tests = 0;
    $invalid_tests = 0;
    $tests_counter = 0;
    $page = setup_table();

    foreach($src_files as $source_file){

        $tests_counter = $tests_counter + 1;
        $source_to_stdin = file_get_contents($source_file);
        $test_name = preg_split("/(\.src$)/",$source_file)[0];
        $find_out_file = $test_name . ".out";
        $find_in_file = $test_name . ".in";
        $rc_file = $test_name . ".rc";
        
        // check if in and out files exist
        $in_exist = in_array($find_in_file,$in_files);
        check_if_exist($in_exist,$find_in_file);
        $out_exist = in_array($find_out_file,$out_files);
        check_if_exist($out_exist,$find_out_file);
    
        // get expected return value
        $rc_exist = in_array($rc_file,$returncode_files);
        $expected_value = 0;
        // if there isnt a file with return code then we generate this file with RC 0sssssss
        if($rc_exist) {
           
            $expected_value = file_get_contents($rc_file);
        } else {
            file_put_contents($rc_file,$zero);
        }
        // use exec no shell_exec
        $command_line_string = "python3.8 " . $argument_constants["interpret_script"] . " --source=" . $source_file . " --input=".$find_in_file;
        $output = "";
        exec($command_line_string,$output,$interpret_ret_val);
    
       
        if($interpret_ret_val == 0){
            $delta_count = $delta_count + 1;
            $output_string =  implode("\n",$output);
            
            $my_name = "xvalas10xml_diffsuite".$delta_count.".out";
            file_put_contents($my_name,$output_string);
            
            $diff_str = "diff ". $find_out_file. " " . $my_name;
            exec($diff_str,$diff_out,$diff_rv);
    
            if($diff_rv == 0){
                $page = $page .add_passed_row($test_name,$interpret_ret_val);
                $accepted_tests = $accepted_tests + 1;
            } else {
                $page = $page .add_invalid_row($test_name,$interpret_ret_val);
                $invalid_tests = $invalid_tests + 1;

            }
            unlink($my_name);
            
        } 
        else {
            if($expected_value == $interpret_ret_val){
                $page = $page .add_passed_row($test_name,$interpret_ret_val);
                $accepted_tests = $accepted_tests + 1;
            } 
            else {
                $page = $page .add_invalid_row_exp($test_name,$interpret_ret_val,$expected_value);
                $invalid_tests = $invalid_tests + 1;
            }
        }
    }
   $page = generate_review($tests_counter,$accepted_tests,$invalid_tests) . $page;
   $page = $page . html_end_gen();
   echo $page;
}

/**
 * @brief Function executes tests parse -> interpret -> diff
 * @param src_files .src files
 * @param out_files .out files
 * @param in_files .in files
 * @param returncode_files .rc files
 */
function full_test($src_files,$out_files,$in_files,$returncode_files){
    global $argument_constants;
    $zero = 0;
    $delta_count = 0;
    $accepted_tests = 0;
    $invalid_tests = 0;
    $tests_counter = 0;
    $page = setup_table();

    foreach($src_files as $source_file){

        $tests_counter = $tests_counter + 1;
        $source_to_stdin = file_get_contents($source_file);
        $test_name = preg_split("/(\.src$)/",$source_file)[0];
        $find_out_file = $test_name . ".out";
        $find_in_file = $test_name . ".in";
        $rc_file = $test_name . ".rc";
        // check if in and out files exist
        $in_exist = in_array($find_in_file,$in_files);
        check_if_exist($in_exist,$find_in_file);
        $out_exist = in_array($find_out_file,$out_files);
        check_if_exist($out_exist,$find_out_file);

        // get expected return value
        $rc_exist = in_array($rc_file,$returncode_files);
        $expected_value = 0;
        // if there isnt a file with return code then we generate this file with RC 0sssssss
        if($rc_exist) {
           
            $expected_value = file_get_contents($rc_file);
        } else {
            file_put_contents($rc_file,$zero);
        }
       
        // use exec no shell_exec
        $command_line_string = "php7.4 " . $argument_constants["parse_script"] . " <" . $source_file;
        $output = "";
        exec($command_line_string,$output,$parse_ret_val);
        
        if($parse_ret_val == 0){
            $delta_count = $delta_count + 1;
            $output_string = "";
            $output_string =  implode("\n",$output);
            // create file to store parse output
            $my_name = "xvalas10xml_testsuite".$delta_count.".out";
            file_put_contents($my_name,$output_string);
            // create executional string
            $interpret_string = "python3.8 ".$argument_constants["interpret_script"]. " --source=" . $my_name . " --input=".$find_in_file;
            exec($interpret_string,$interpret_output,$interpret_retval);

            if($interpret_retval == 0) {
                $interpret_output_string = "";
                $interpret_output_string = implode("\n",$interpret_output);
                $diff_name = "xvalas10_diff_".$delta_count.".out";
                file_put_contents($diff_name,$interpret_output_string);
                $diff_string = "diff ". $find_out_file. " ".$diff_name;
                exec($diff_string,$diff_out,$diff_rv);

                if($diff_rv == 0){
                    $page = $page . add_passed_row($test_name,$diff_rv);
                    $accepted_tests = $accepted_tests + 1;
                } else {
                    $page = $page .add_invalid_row($test_name,"different diff output");
                    $invalid_tests = $invalid_tests + 1;
                }
                #delete tmp files
                unlink($my_name);
                unlink($diff_name);
            } 
            else {
                if($interpret_retval == $expected_value){
                    $page = $page . add_passed_row($test_name,$interpret_retval);
                    $accepted_tests = $accepted_tests + 1;
                } 
                else {
                    $page = $page .add_invalid_row_exp($test_name,$interpret_retval,$expected_value);
                    $invalid_tests = $invalid_tests + 1;
                }
                unlink($my_name);
            }
        }
        else {
            if($parse_ret_val == $expected_value){
                $page = $page . add_passed_row($test_name,$parse_ret_val);
                $accepted_tests = $accepted_tests + 1;
            } else {
                $page = $page .add_invalid_row_exp($test_name,$parse_ret_val,$expected_value);
                $invalid_tests = $invalid_tests + 1;
            }
        }
    }
   $page = generate_review($tests_counter,$accepted_tests,$invalid_tests) . $page;
   $page = $page . html_end_gen();
   echo $page;
}


/**
 * @brief Function executes tests with --parse-only parameter on
 * @param src_files .src files
 * @param out_files .out files
 * @param in_files .in files
 * @param returncode_files .rc files
 */
function parse_test($src_files,$out_files,$in_files,$returncode_files){
    global $argument_constants;
    $zero = 0;
    $delta_count = 0;
    $accepted_tests = 0;
    $invalid_tests = 0;
    $tests_counter = 0;
    $page = setup_table();

    foreach($src_files as $source_file){

        $tests_counter = $tests_counter + 1;
        $source_to_stdin = file_get_contents($source_file);
        $test_name = preg_split("/(\.src$)/",$source_file)[0];
        $find_out_file = $test_name . ".out";
        $find_in_file = $test_name . ".in";
        $rc_file = $test_name . ".rc";
        
        // check if in and out files exist
        $in_exist = in_array($find_in_file,$in_files);
        check_if_exist($in_exist,$find_in_file);
        $out_exist = in_array($find_out_file,$out_files);
        check_if_exist($out_exist,$find_out_file);
    
        // get expected return value
        $rc_exist = in_array($rc_file,$returncode_files);
      
        $expected_value = 0;

        if($rc_exist) {
           
            $expected_value = file_get_contents($rc_file);
        } else {
            file_put_contents($rc_file,$zero);
        }
        // use exec no shell_exec
        $command_line_string = "php7.4 " . $argument_constants["parse_script"] . " <" . $source_file;
        $output = "";
        exec($command_line_string,$output,$parse_ret_val);
       
        if($parse_ret_val == 0){
            $delta_count = $delta_count + 1;
            $output_string =  implode("\n",$output);
            
            $my_name = "xvalas10xml_testsuite".$delta_count.".xml";
            file_put_contents($my_name,$output_string);

            // create tmp file to store delta
            $tmp_name = "temporary_delta_ipptestsuite".$delta_count.".xml";
            file_put_contents($tmp_name,"");
            $xml_str = "java -jar ".$argument_constants["jexamxml"]." ".$find_out_file." ".$my_name." ".$tmp_name." ".$argument_constants["jexamconfig"];
            
            exec($xml_str,$compare,$script_retval);
    
            if($script_retval == 0){        
                $page = $page .add_passed_row($test_name,$parse_ret_val);
                $accepted_tests = $accepted_tests + 1;
            } else {
                $page = $page .add_invalid_row($test_name,$parse_ret_val);
                $invalid_tests = $invalid_tests + 1;
            }
            unlink($my_name);
            unlink($tmp_name);
        } 
        else {
            if($expected_value == $parse_ret_val){
                $page = $page .add_passed_row($test_name,$parse_ret_val);
                $accepted_tests = $accepted_tests + 1;
            } else { 
                $page = $page .add_invalid_row_exp($test_name,$parse_ret_val,$expected_value);
                $invalid_tests = $invalid_tests + 1;
            }
        }
    }
   $page = generate_review($tests_counter,$accepted_tests,$invalid_tests) . $page;
   $page = $page . html_end_gen();
   echo $page;
}


/**
 * @brief Function checks if the file exist and if not then create empty file
 * @param is_in_array boolean value - true/false
 * @param filename name of file
 */
function check_if_exist($is_in_array,$filename){
    if($is_in_array){
        return;
    } else {
        file_put_contents($filename,"");
    }
}


/**
 * @brief Function print help message --help parameter is on
 */
function print_help(){
    global $return_values;
    $help_msg = "\t IPP 2020/2021 - Testovací rámec - test.php\n";
    $help_msg .= "\t Autor: Samuel Valaštín\n";
    $help_msg .= "\t Použitie : php7.4 test.php \n";
    $help_msg .= "\t Povolené prepínače : --help - výpis nápovedy\n";
    $help_msg .= "\t --directory=path     => testy v danom adresari / bez pouzitia script hlada v aktualnom adresari\n";
    $help_msg .= "\t --recursive         => testy vyhladava script rekurzivne vo vsetkych jeho podadresaroch\n";
    $help_msg .= "\t --parse-script=file => specifikacia cesty php scriptu parse.php\n";
    $help_msg .= "\t --int-script=file   => specifikacia cesty python scriptu interpret.py\n";
    $help_msg .= "\t --parse-only        => testovanie vyhradne scriptu parse.php\n";
    $help_msg .= "\t --int-only          => testovanie vyhradne scriptu interpret.py\n";
    $help_msg .= "\t --jexamxml=file     => subor s JAR balickom s nastrojom A7Soft JExamXML\n";
    $help_msg .= "\t --jexamcfg=file     => subor s konfiguraciou nastroja A7Soft JExamXMl\n";
    echo $help_msg;
    get_return_value($return_values["RETURN_OK"]);
}

/**
 * @brief Function terminates program run with exit code
 */
function get_return_value($return_value){
    exit($return_value);
}
check_arguments($argv);
$options = getopt("",$cmd_options);
parse_arguments($options,$argv)

?>
