% Usage: SetupCbpmfileioAPI

%   Set up paths
    onLinux = ~isempty(getenv('ACC_RELEASE_DIR'));
    if (~onLinux)
        stars = '***********************************************************************************************';
        msg = sprintf('\n%s\n%s\n%s\n',stars,'Must run on linux for cbpmfileio API',stars);
        error(msg)
    end
    
    addpath([getenv('ACC_RELEASE_DIR') '/cbpmfio/matlab']); load_CBPM_input(); 
    
    str = getenv('HOSTNAME');
    f = strfind(str,'cesr');
    
    if (isempty(f)) || (f == -1)
        error('I suggest you use a computer on the CESR cluster. It will be faster')
    end