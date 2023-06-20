#!/bin/bash

set -xa

# Check for CMCE if 00 or 12

if [ $cyc -eq 00 ] || [ $cyc -eq 12 ]; then
    let attempts=1
    while [ $attempts -le 5 ]; do
	if [ -d $CMCE_MODEL_DIR ]; then
	    break
	    echo "$CMCE_MODEL_DIR is in!" 
	else
	    sleep 60
	    attempts=$((attempts+1))
	fi
    done
fi

if [[ $attempts -gt 5 ]] && [ ! -d $CMCE_MODEL_DIR ]; then
    msg="WARNING: $CMCE_MODEL_DIR still not available after waiting five minutes... proceeding without..."
    echo "$msg" | mail.py -s "EPOCH - WARNING: CMCE data not available" -c $MAILTO
fi

let attempts=1
while [ $attempts -le 50 ]; do
    count=`ls -A $GEFSA_MODEL_DIR | wc -l` # check if directory is present and not empty
    if [[ -d $GEFSA_MODEL_DIR && $count -ne 0 ]]; then
    	$HOMEepoch/ush/epoch.py -t $PDY$cyc -p $HOMEepoch/parm/EpochNoaa.parms
    	#$USHepoch/epoch.py -t $PDY$cyc -p $PARMepoch/EpochNoaa.parms
	export err=$?; err_chk
	break
    else
	sleep 60
    	attempts=$((attempts+1))
    fi

done
if [[ $attempts -gt 50 ]] && [ ! -d $GEFSA_MODEL_DIR ]; then
    msg="$GEFSA_MODEL_DIR still not available after waiting fifty minutes... exiting"
    err_exit
fi


