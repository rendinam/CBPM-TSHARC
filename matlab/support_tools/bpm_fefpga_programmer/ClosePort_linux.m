%AFE4 open a CBI net port and return the socket
function Socket = ClosePort_linux(Socket)
    Socket = int32(Socket);
    try
        matlab_cbi_net_close_socket(Socket);
    catch ME1
        disp('Attempt to close socket failed!');
        Socket = 0;
        return;
    end
    
    try
        matlab_cbi_net_net_close(Socket);
    catch ME1
        disp('Attempt to close socket failed!');
        Socket = 0;
        return;
    end
    
    Socket = 0;
    return;
end