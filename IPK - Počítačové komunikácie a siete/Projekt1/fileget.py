""" 
IPK 2020/2021 
Projekt 1 - Klient pre súborový systém 
Samuel Valastin <xvalas10@stud.fit.vutbr.cz> 
"""
import sys
import re
import socket
import os

class ERR_STATES:
    invalid_command_line_number = 1
    invalid_parameter = 2
    invalid_nameserver = 3
    invalid_FSP = 4
    invalid_respond = 5
    internal_err = 6


def parse_arguments(pattern):
    """ Function processes the command line arguments by pattern """
    if len(sys.argv) != 5:
        invalid_states(ERR_STATES.invalid_command_line_number)
    if sys.argv[1] == pattern:
        return sys.argv[2]
    elif sys.argv[3] == pattern:
        return sys.argv[4]


def invalid_states(state):
    """ Function processes error messages by param values to stderr and ends program """
    if state == ERR_STATES.invalid_command_line_number:
        sys.stderr.write("ERROR: Invalid number of input parameters! \r\n")
    elif state == ERR_STATES.invalid_parameter:
        sys.stderr.write("ERROR: Invalid input parameters! \r\n")
    elif state == ERR_STATES.invalid_nameserver:
        sys.stderr.write("ERROR: Invalid nameserver parameter! \r\n")
    elif state == ERR_STATES.invalid_FSP:
        sys.stderr.write("ERROR: Invalid FSP parameter! \r\n")
    elif state == ERR_STATES.invalid_respond:
        sys.stderr.write("ERROR: Invalid respond")
    sys.exit(1)


def valid_args(server, service):
    """ Function checks whether the input arguments are valid """
    if server is None or service is None:
        invalid_states(ERR_STATES.invalid_parameter)


def invalid_answer(server_respond):
    """ Function prints an error response"""
    print("ERROR: " + server_respond)
    


def check_split(split, err_value):
    """ Function checks the validity of the splitting according to regular expressions """
    if len(split) != 2:
        invalid_states(err_value)


def udp_request(ip, port, domain_name):
    """ Function performs a connection based on the UDP protocol """
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        quest = "WHEREIS " + domain_name
        server_address = (ip, port)
        sock.sendto(quest.encode("utf-8"), server_address)
        data, address = sock.recvfrom(1024)
        decode_data = data.decode("utf-8")
        respond = re.split("[\s,]+", decode_data)
        result = check_udp_respond(respond)
        if decode_data is None:
            invalid_answer("Server didnt respond")
            invalid_states(ERR_STATES.internal_err)       
    except Exception:
        print(Exception)
        invalid_states(ERR_STATES.internal_err)

    sock.close()
    return result


def check_udp_respond(udp_resp):
    """ Function checks the respond from server """
    if len(udp_resp) < 2:
        invalid_answer("Invalid REQUEST")
    if udp_resp[0] == "OK":
        return udp_resp[1]
    elif udp_resp[0] == "ERR":              # error handling
        if udp_resp[1] == "Syntax":
            invalid_answer("Syntax")
        elif udp_resp[1] == "Not":
            invalid_answer("Not Found")
    else:
        invalid_answer("Other unspecified problem")


def download_file(s,message,file_name):
    """ Function download data via tcp socket """
    s.send(message.encode("utf-8"))
    tcp_out = bytes()
    while True:
        data = s.recv(1024)
        tcp_out += data
        if not data:
            break
    header = tcp_out.split(b'\r\n\r\n',maxsplit=1)    # split by 2 empty lines
    exp_len = header[0].split(b":")
    exp_len = int(exp_len[1].decode("utf-8"))
    recv_data = header[1]                   # data after empty lines
    server_answer = header[0].split(b'\r\n')  
    if re.match("^FSP/1.0 Success$", server_answer[0].decode("utf-8")):
        if len(recv_data) == exp_len:  
            save_data(recv_data, file_name)
        else:
            invalid_answer("Couldnt get he whole length of the: " + file_name)
            save_data(recv_data,file_name)
    else:
        invalid_answer(server_answer[0].decode("utf-8"))
        invalid_states(ERR_STATES.internal_err)


def save_data(data, name):
    """ Function looking if the file is in different subfolder """
    file_split = name.rsplit("/", 1)
    if len(file_split) > 1:                             # looking for subdirs
        act_directory = os.path.abspath(os.getcwd())
        file_path = os.path.join(act_directory, name)
        if not os.path.exists(file_split[0]):
            os.makedirs(file_split[0])
        write_data(data, file_path)
    else:
        write_data(data, name)


def write_data(f_data, f_name):
    """ Function write data to specified file """
    try:
        file = open(f_name, "wb")
        file.write(f_data)
    except Exception:
        print(Exception)
    file.close()


def main():
    # parse command line arguments
    name_server = parse_arguments("-n")
    file_service = parse_arguments("-f")
    valid_args(name_server, file_service)

    # splitting name server to port and ip
    name_split = re.split(":", name_server)
    check_split(name_split, ERR_STATES.invalid_nameserver)
    server_ip = name_split[0]
    server_port = int(name_split[1])

    # splitting url
    file_service_split = re.split("://", file_service)
    check_split(file_service_split, ERR_STATES.invalid_FSP)

    # setup fsp protocol
    file_service_command = "GET "
    file_service_version = file_service_split[0].upper() + "/1.0"
    file_service_url = file_service_split[1]

    # splitting url to domain and file
    file_service_url_split = re.split("/", file_service_url, maxsplit=1)
    check_split(file_service_url_split, ERR_STATES.invalid_FSP)

    if len(file_service_url_split[1]) == 0:
        invalid_states(ERR_STATES.invalid_FSP)          # no file only domain

    file_service_domain = file_service_url_split[0]
    file_service_file = file_service_url_split[1]

    if re.search("\*",file_service_file) is not None:
        subdir_get_all = re.split("\*",file_service_file) [0].replace("/*", "")     # look for possible get all files
        get_all_files = True
        file_service_file = "index"
    else:
        get_all_files = False
    udp_address = udp_request(server_ip, server_port, file_service_domain)

    # split adress to port and ip
    udp_split = re.split(":", udp_address)
    check_split(udp_split, ERR_STATES.invalid_respond)
    tcp_ip = udp_split[0]
    tcp_port = int(udp_split[1])

    # trying to setup TCP CONNECT
    start_message = file_service_command + file_service_file + " " + file_service_version + "\r\n"
    hostname = "Hostname: " + file_service_domain + "\r\n"
    agent = "Agent: xvalas10\r\n\r\n"
    tcp_message = start_message + hostname + agent

    # setup TCP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_addr = (tcp_ip, tcp_port)
    sock.connect(sock_addr)
    download_file(sock, tcp_message, file_service_file)
    sock.close()

    if get_all_files:
        try:
            index_file = open("index", "r")
            file_list = re.split("\n", index_file.read())
        finally:
            index_file.close()

        file_list = list(filter(None, file_list))
        file_list = [file for file in file_list if re.match(subdir_get_all,file)]
        if "fileget.py" in file_list:
            file_list.remove("fileget.py")

        for this_file in file_list:
            try:
                # set up socket and message and download all files from list
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock_addr = (tcp_ip, tcp_port)
                sock.connect(sock_addr)
            except Exception:
                print(Exception)
                invalid_states(ERR_STATES.internal_err)
                
            start_message = file_service_command + this_file + " " + file_service_version + "\r\n"
            msg = start_message + hostname + agent
            download_file(sock, msg, this_file)
            sock.close()


if __name__ == "__main__":
    main()
