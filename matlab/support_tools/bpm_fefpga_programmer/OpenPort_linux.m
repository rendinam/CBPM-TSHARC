%AFE4 open a CBI net port and return the socket
function Socket = OpenPort_linux(IPAddress)
    Socket = matlab_cbi_net_fdopen(IPAddress);
    return;
end