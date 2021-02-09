#!/usr/bin/python

import os, sys, string

if len(sys.argv) < 3:
    print ""
    print "****************************************************************"
    print ""
    print "		File: concat_proc_files.py "
    print ""
    print "****************************************************************"
    print ""
    print " Description: Concatenate multiple CBPM process data files "
    print " "
    print " Parameter(s): "
    print "     first_index - index of first process data file  "
    print "     last_index - index of last process data file  "
    print "     Optional parameters: "
    print "         first BPM .. last BPM = list of BPMs to extract from process data files, example:"
    print "            BPM_12W BPM_12W2 BPM_12W3"
    print " "
    print "     ConcatOutputFile.txt = name of file to output concatenated data to "
    print " "
    print " Usage: "
    print " "
    print " ./concat_proc_files.py <first_index> <last_index> [<first_BPM> ... <last_BPM>] > ./ConcatOutputFile.txt"
    print " "
    print " Ex:"
    print " ./concat_proc_files.py 95273 95998 BPM_12W BPM_12W2 BPM_12W3 > ./95273-95998-triplet.txt"
    print " ./concat_proc_files.py 95273 95998 > ./95273-95998-triplet.txt"
    print " "
    print " "
    sys.exit()

# DaysInMonth - calculate number of days from that month to beginning of year (ignores leap year)
def DaysInMonth(month):
	if month == "Jan":
	  return 0;
	elif month == "Feb":
	  return 31;
	elif month == "Mar":
	  return 31+28;
	elif month == "Apr":
	  return 31+28+31;
	elif month == "May":
	  return 31+28+31+31;
	elif month == "Jun":
	  return 31+28+31+31+31;
	elif month == "Jul":
	  return 31+28+31+31+31+30;
	elif month == "Aug":
	  return 31+28+31+31+31+30+31;
	elif month == "Sep":
	  return 31+28+31+31+31+30+31+31;
	elif month == "Oct":
	  return 31+28+31+31+31+30+31+31+30;
	elif month == "Nov":
	  return 31+28+31+31+31+30+31+31+30+31;
	elif month == "Dec":
	  return 31+28+31+31+31+30+31+31+30+31+30;
	else:
	  return 0

# Read in Input parameters
first_index = int(sys.argv[1])  #5788
last_index  = int(sys.argv[2])  #5968

# Calculate number of BPMs to extract data for
numBPMs = len(sys.argv)-3

# If no BPMs specified, assume there are three, and give them the names BPM1, BPM2, and BPM3
if numBPMs == 0:
	instruments = ["BPM1", "BPM2", "BPM3"]
else:

# Read in BPM names
  instruments = []
  for ind in range(1,numBPMs+1):
	instruments.append(sys.argv[ind+2])

#X and Y position coefficients
y_coeff = 0.028800
x_coeff = 0.022300

#Initialize variables
datapath    = "/nfs/cesr/online/instr/data/cbpmII/proc/"
detector    = 0
detcount    = 0

# Print version
print "Version-8"
print "Time(min)           X(m)                 Y(m)              TI RMS(cu)  BI RMS(cu)  BO RMS(cu)  TO RMS(cu)  FileID  CERN(mA) Channel Delays  Button Values                                         Temperatures  Block Delays ChannelB Delays"
num_files = []

for instrument in instruments:

    num_files.append(0)
    detector = detector + 1
    # Print out current instrument to extract data for
    print instrument

    # Loops through whole range of process files specified
    for filenum in range(first_index, last_index+1):
        detcount = 0
        detCHcount = 0
        detBlkcount = 0
        detGaincount = 0

	# Initialize Temperature variables, in case missing from process data file
	Timing_Temp_C = 0.0
	Digital_Temp_C = 0.0
	Card_Temp_C1 = 0.0
	Card_Temp_C2 = 0.0
	Card_Temp_C3 = 0.0
	Card_Temp_C4 = 0.0

        # Make sure file comes from the appropriately named subdir
        if filenum < 10000:
             file = "PROC-00" + str(filenum) + ".dat"
             num_subdir = "0" + str(filenum / 1000)
        elif filenum < 100000:
             file = "PROC-0" + str(filenum) + ".dat"
             num_subdir = str(filenum / 1000)
	else:
             file = "PROC-" + str(filenum) + ".dat"
             num_subdir = str(filenum / 1000)

	# Initialize variables before inner loop
        ProcFileVersion = 1
        ProcessThisLine = 0
        ProcessLineForChDelaysCount = 0

        try:
            f = open(datapath + str(num_subdir) + "/" + file).readlines()
        except IOError:
	    # if file doesn't exist, don't complain, just go to next iteration of loop (fail safe)
            continue
        
        for line in f:
            if line.find("File_Version") != -1:
                ProcFileVersion = 0
                Version = line[20:]
	
	    # Read In time stamp - looks like this: 
	    # Timestamp         : Wed Feb  8 15:10:46 2012
            if line.find("Timestamp") != -1:
                i = line.find(":")
                Date = "   " + line[i+1:].strip()
                DateCompact = Date.replace(" ","").replace(":","")
                Year = line[i:].split()[5]
		Month = line[i:].split()[2]
		Days = line[i:].split()[3]
		Time = line[i:].split()[4]
                Hours = Time.split(":")[0]
                Minutes = Time.split(":")[1]
                Seconds = Time.split(":")[2]
		# Convert time stamp into number of minutes timestamp is past January 1st, 0000 A.D. 
		# 	(ignores leap year, could be added, probably unnecessary)
                FloatMinutes = float(Seconds)/60 + float(Minutes) + 60*(float(Hours) + 24*(float(Days) + DaysInMonth(Month) + 365*float(Year)))

	    # Read in CERN current, using different methods, depending on version of Process Data File
            if ProcFileVersion == 1:
                 if ProcessThisLine == 1:
                      CurrentRaw = line.split()[2]

                 ProcessThisLine = 0
                 if line.find("CERN Current Reading") != -1:
                      ProcessThisLine = 1
	    else:
		 if line.find("CERN Current Corr.") != -1:
                      CurrentRaw = line[20:].strip().split()[0]
                 if line.find("CERN Current mA") != -1:
                      CurrentRaw = line[20:].strip().split()[0]

	    # Read in Delay chip values - assumes it is Block A, Positrons - a dangerous assumption, but good enough so far
            if ProcFileVersion == 1:
                if ProcessLineForChDelaysCount == 2:
                     Ch1 = line.split()[5]
                     Ch2 = line.split()[6]
                     Ch3 = line.split()[7]
                     Ch4 = line.split()[8]
                     ProcessLineForChDelaysCount = 0

                if ProcessLineForChDelaysCount == 1:
                         ProcessLineForChDelaysCount = 2

                if line.find("Channel Delays") != -1:
                        detCHcount = detCHcount + 1

                        if detCHcount == detector:
                            ProcessLineForChDelaysCount = 1
            else:
                if line.find("Gain_Settings") != -1:
                    detGaincount = detGaincount + 1
                    if detGaincount == detector:
                        Gain = line.split()[2]

                if line.find("Block_Delays") != -1:
                    detBlkcount = detBlkcount + 1
                    if detBlkcount == detector:
                        BlkA = line.split()[2]
                        BlkB = line.split()[3]

                if line.find("Channel_Delays") != -1:
                    detCHcount = detCHcount + 1
                    if detCHcount == detector:

                        Ch1 = line.split()[2]
                        Ch2 = line.split()[3]
                        Ch3 = line.split()[4]
                        Ch4 = line.split()[5]

                        Ch1B = line.split()[6]
                        Ch2B = line.split()[7]
                        Ch3B = line.split()[8]
                        Ch4B = line.split()[9]

            # Read In temperatures
	    if line.find("Timing_Temp_C") != -1:
		Timing_Temp_C = line.split()[2]

	    if line.find("Digital_Temp_C") != -1:
		Digital_Temp_C = line.split()[2]

	    if line.find("Card_Temps_C") != -1:
		Card_Temp_C1 = line.split()[2]
		Card_Temp_C2 = line.split()[3]
		Card_Temp_C3 = line.split()[4]
		Card_Temp_C4 = line.split()[5]


	    # Read in BPM name
	    if line.find("# BPM_") != -1:
		BPMName = line.split()[1]

	    # Read in Button values
            if line.find("0)") != -1:
                detcount = detcount + 1
                # BPMName matches instrument we're looking for or
		#                              no BPMs specified, and this is the appropriate number instrument in the file
		if (BPMName == instrument) or (numBPMs == 0 and detcount == detector):
		    num_files[detector-1] = num_files[detector-1] + 1
                    sys.stdout.write(str(FloatMinutes) + "   ")
                    try:
                         TI = float( line[4:].split()[0] ) #  0
                    except ValueError:
                        continue
                    BI = float( line[4:].split()[1] ) #  1
                    BO = float( line[4:].split()[2] ) #  2
                    TO = float( line[4:].split()[3] ) #  3

                    sum  = abs(TI) + abs(BI) + abs(BO) + abs(TO)
                    Ynum = TO - BO - BI + TI # TOP - BOT
                    Xnum = TO + BO - BI - TI # OUTER - INNER

		    if sum > 0:
                    	Xpos = x_coeff * (Xnum/sum)
                    	Ypos = y_coeff * (Ynum/sum)
		    else:
			Xpos = 0
			Ypos = 0

                    sys.stdout.write( "  " + str(Xpos) + "   " + str(Ypos) + "   " )
                    sys.stdout.write( line[4:].split()[4] + "   ")
                    sys.stdout.write( line[4:].split()[5] + "   ")
                    sys.stdout.write( line[4:].split()[6] + "   ")
                    sys.stdout.write( line[4:].split()[7])

                    sys.stdout.write( "  " + str(filenum) + "  ")
                    sys.stdout.write( CurrentRaw +" ")
                    sys.stdout.write(Ch1 +" "+ Ch2 +" "+ Ch3 +" "+ Ch4 +" ")
                    sys.stdout.write(str(TI) +" "+ str(BI) +" "+ str(BO) +" "+ str(TO) +" ")
		    sys.stdout.write(str(Digital_Temp_C) +" "+ str(Card_Temp_C1) +" "+ str(Card_Temp_C2) +" "+ str(Card_Temp_C3) +" "+ str(Card_Temp_C4) +" "+ str(Timing_Temp_C) +" ")
                    sys.stdout.write(BlkA +" "+ BlkB +" ")
                    sys.stdout.write(Ch1B +" "+ Ch2B +" "+ Ch3B +" "+ Ch4B +" ")
		    sys.stdout.write(Gain +" ")
                    #sys.stdout.write( "   " + DateCompact + "  ")
                    sys.stdout.write("\n")
                    
# Determine whether found the name number of files for all BPMs specified
detector = 0                    
numFilesIs = num_files[0]
equalNumFilesForAllInstruments = True
for instrument in instruments:
	equalNumFilesForAllInstruments = equalNumFilesForAllInstruments & (numFilesIs == num_files[detector])
	detector = detector + 1

# If number of files not same, dispaly how many files for each BPM
detector = 0 
error_str = " "
if equalNumFilesForAllInstruments == False:
    for instrument in instruments:
	sys.stdout.write(instrument + ": " + str(num_files[detector]) + " ")
	error_str = error_str + instrument + ": " + str(num_files[detector]) + " "
	detector = detector + 1

    sys.stdout.write("\n")
    raise "Error: not all instruments have same number of files;  " + error_str
