--[[    Author : Samuel Valaštín, <xvalas10@stud.fit.vutbr.cz>
        Login : xvalas10
        Date: 2021/10
        ISA 3 - dissector                                       
--]]

isa3_protocol = Proto("ISA3P",  "ISA3 Protocol")
-- protocol fields
data_len = ProtoField.int32("isa3p.data_len", "Data length", base.DEC)
sender = ProtoField.string("isa3p.sender", "Sender", base.ASCII)
reciever = ProtoField.string("isa3p.reciever", "Reciever", base.ASCII)
packet_type = ProtoField.string("isa3p.packet_type", "Type", base.ASCII)
status = ProtoField.string("isa3p.status", "Transfer status", base.ASCII)
data = ProtoField.string("isa3p.data", "Data", base.ASCII)
command = ProtoField.string("isa3p.command", "Command", base.ASCII)
username = ProtoField.string("isa3p.username", "Username", base.ASCII)
login_token = ProtoField.string("isa3p.login_token", "Login token", base.ASCII)
fetch_id = ProtoField.string("isa3p.fetch_id", "Fetch id", base.ASCII)
recipient = ProtoField.string("isa3p.recipient", "Recipient", base.ASCII)
message_subject = ProtoField.string("isa3p.message_subject", "Message subject", base.ASCII)
body = ProtoField.string("isa3p.body", "Body", base.ASCII)
isa3_protocol.fields = {data_len, sender, reciever, status, packet_type, data, command, username, login_token, fetch_id, recipient, message_subject, body}

-- Matchs first whitespace and returns first word of the packet (data prefix)
function get_prefix(buffer)
    for data in buffer:gmatch("%w+") do
        return data
    end
    return nill
end

-- Returns true if mark is odd, otherwise (even) returns false
function odd_mark(buffer)
    count = 0
    for i = 1, buffer:len() do
        if string.sub(buffer, i, i) == '"' then
            count = count + 1
        end
    end
    if count % 2 == 1 then
        return true
    end
    return false
end

-- Returns packet transfer data
function decide_transfer(data_prefix)
    transfer_table = {}
    if data_prefix == "ok" or data_prefix == "err" then
        transfer_table["packet_type"] = "Server respond"
        transfer_table["sender"] = "server"
        transfer_table["reciever"] = "client"
    else
        transfer_table["packet_type"] = "Client request"
        transfer_table["sender"] = "client"
        transfer_table["reciever"] = "server"
    end
    return transfer_table
end

-- Writes transfer data to Transfer subtree.
function write_transfer_data(transferSubtree, subject, proto_prefix, buffer)
    -- if sender is server then status options are - [ok, err] , for client side only ok
    if subject["sender"] == "server" then
        transferSubtree:add_le(status, buffer(1,proto_prefix:len()))
    else
        transferSubtree:add_le(status, "ok")
    end
    -- write other transfer data
    transferSubtree:add_le(packet_type, subject["packet_type"])
    transferSubtree:add_le(sender, subject["sender"])
    transferSubtree:add_le(reciever, subject["reciever"])
    transferSubtree:add_le(data_len, proto_len)
end

-- Returns nth " in buffer
function find_nth_quote_mark(buffer, n)
    for i = 1, buffer:len() do
        if string.sub(buffer, i, i) == '"' and (i-1) >= 1 and string.sub(buffer,i-1,i-1) ~= '\\' then
            n = n - 1
        end
        if n == 0 then
            return i-1
        end
    end
    return 0
end

-- Counts and returns number of occurences of char in buffer
function calculate_occurences_of_char(buffer, char)
    count = 0
    for i=1, buffer:len() do
        if string.sub(buffer,i,i) == char then
            if char == '"' and (i-1) > 0 and  string.sub(buffer,i-1,i-1) ~= '\\' then
                count = count + 1
            elseif char  ~= '"' then
                count = count + 1
            end
        end
    end
    return count
end

-- Writes payload data to Payload subtree.
function write_payload_data(dataSubtree, subject, proto_prefix, buffer)
    payload_start = find_nth_quote_mark(buffer:raw(0, -1), 1)
    payload_end = find_nth_quote_mark(buffer:raw(0, -1), calculate_occurences_of_char(buffer:raw(0, -1), '"'))
    if subject["sender"] == "server" then
        if calculate_occurences_of_char(buffer:raw(0,-1), "(") >= 2 then
            -- Server respond to list command
            dataSubtree:add_le(data, buffer(proto_prefix:len()+2, buffer:len()-(proto_prefix:len()+3)))
            return "server: " .. buffer:raw(proto_prefix:len()+2, buffer:len()-(proto_prefix:len()+3))
        else
            if payload_start > 0 and payload_end > 0 then
                token_start = find_nth_quote_mark(buffer:raw(0,-1), 3)
                token_end = find_nth_quote_mark(buffer:raw(0,-1), 4)
                if token_start > 0 and token_end > 0 then
                    -- Server respond to login command
                    data_end = find_nth_quote_mark(buffer:raw(0,-1), 2)
                    dataSubtree:add_le(data, buffer(payload_start+1, data_end-payload_start-1))
                    dataSubtree:add_le(login_token, buffer(token_start, payload_end-token_start-1))
                    return "server: " .. buffer:raw(payload_start+1, data_end-payload_start-1)
                else
                    -- Server respond to other commands
                    dataSubtree:add_le(data, buffer(payload_start+1, payload_end-payload_start-1))
                    return "server: " .. buffer:raw(payload_start+1, payload_end-payload_start-1)
                end
            end
        end
    elseif subject["sender"] == "client" then
        dataSubtree:add_le(command, buffer(1, payload_start-2))
        client_command = buffer:raw(1, payload_start-2)
        -- Handling client commands -> list and logout covered (without arguments)
        if client_command == "register" or client_command == "login" then
            username_end = find_nth_quote_mark(buffer:raw(0, -1), 2)
            if username_end > 0 then
                dataSubtree:add_le(username, buffer(payload_start+1, username_end-payload_start-1))
                token_start = find_nth_quote_mark(buffer:raw(0, -1), 3)
                if token_start > 0 then
                    dataSubtree:add_le(login_token, buffer(token_start+1, payload_end-token_start-1))
                end
            end
        elseif client_command == "fetch" then
            dataSubtree:add_le(login_token, buffer(payload_start+1, payload_end-payload_start-1))
            dataSubtree:add_le(fetch_id, buffer(payload_end+2, buffer:len()-payload_end-3))
        elseif client_command == "send" then
            token_end = find_nth_quote_mark(buffer:raw(0, -1), 2)
            dataSubtree:add_le(login_token, buffer(payload_start+1, token_end-payload_start-1))
            recipient_start = find_nth_quote_mark(buffer:raw(0, -1), 3)
            recipient_end = find_nth_quote_mark(buffer:raw(0, -1), 4)
            dataSubtree:add_le(recipient, buffer(recipient_start+1, recipient_end-recipient_start-1))
            subject_start = find_nth_quote_mark(buffer:raw(0, -1), 5)
            subject_end = find_nth_quote_mark(buffer:raw(0, -1), 6)
            dataSubtree:add_le(message_subject, buffer(subject_start+1, subject_end-subject_start-1))
            body_start = find_nth_quote_mark(buffer:raw(0, -1), 7)
            dataSubtree:add_le(body, buffer(body_start+1, payload_end-body_start-1))
        end
    end
    return "client: " .. client_command
end

function isa3_protocol.dissector(buffer, pinfo, tree)
    -- check if we recieved all data in this packet
    msg_end = buffer:raw(0,-1):sub(-1)
    if msg_end ~= ')' and odd_mark(buffer:raw(0, -1)) then
        pinfo.desegment_len = DESEGMENT_ONE_MORE_SEGMENT
        return
    end
    -- check validity of protocol from prefix
    proto_prefix = get_prefix(buffer:raw(0,-1))
    prefix_valid = false
    valid_prefixes = {"ok", "err", "login", "register", "list", "send", "fetch", "logout"}
    for _,value in pairs(valid_prefixes) do
        if value == proto_prefix then
            prefix_valid = true
        end
    end
    if prefix_valid == false then
        return
    end
    proto_len = buffer:len()
    pinfo.cols.protocol = isa3_protocol.name
    -- Creating protocol tree ISA3 Protocol Data - [Transfer, Payload]
    local subtree = tree:add(isa3_protocol, buffer(), "ISA3 Protocol Data")
    local transferSubtree = subtree:add(isa3_protocol, buffer(), "Transfer")
    local dataSubtree = subtree:add(isa3_protocol, buffer(), "Payload")
    -- get transfer data
    subject = decide_transfer(proto_prefix)
    -- write transfer data
    write_transfer_data(transferSubtree, subject, proto_prefix, buffer)
    -- write payload data
    info_data = write_payload_data(dataSubtree, subject, proto_prefix, buffer)
    if info_data ~= nill then
        pinfo.cols.info = info_data
    end
end

-- register to tcp port 32323
local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(32323, isa3_protocol)