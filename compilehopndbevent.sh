#!/bin/bash 

#author		 :Sri
#date            :2015-03-19
#notes           :This script will be called by deploy.sh 
#==============================================================================

# declare an array of cloud machine
machineUser=$1
ndbdistributionDirectory=$2
if [ -z "${machineUser-set}" ]; then
 echo "Remote machine user is not set or empty..can not proceed"
 exit
else
 echo "Remote machine user has been set to : $machineUser"
fi

if [ -z "${ndbdistributionDirectory-set}" ]; then
 echo "Distribution path can not be empty"
 exit
else
 echo "Distribution directory is set to  : $ndbdistributionDirectory"
fi



LIB_NDB_CLIENT_FILE_PATH=/home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/libndbclient.so.6.0.0
LIB_HOPS_NDB_EVENT_LIBRARY=/home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/libhopsndbevent.so

if [ -d "$ndbdistributionDirectory" ]; 
then
   echo "*************  hopsndbevent folder is found , removing and creating new for cluster deployment **************"
   rm -rf	$ndbdistributionDirectory/hopsndbevent 
   rm -rf 	$ndbdistributionDirectory/hopsndbevent.tar.gz
fi

sleep 2s
  
mkdir $ndbdistributionDirectory/hopsndbevent
cp -rf 		$ndbdistributionDirectory/include 		$ndbdistributionDirectory/hopsndbevent
cp -rf 		$ndbdistributionDirectory/src 			$ndbdistributionDirectory/hopsndbevent
cp 		$ndbdistributionDirectory/EventAPIConfig.ini 	$ndbdistributionDirectory/hopsndbevent
cp -rf 		$ndbdistributionDirectory/ndbheaders 		$ndbdistributionDirectory/hopsndbevent
cp  		$ndbdistributionDirectory/Makefile 		$ndbdistributionDirectory/hopsndbevent
cd $ndbdistributionDirectory; tar -zcvf 	hopsndbevent.tar.gz 	hopsndbevent/ > /dev/null 2>&1

arrayOfMachines=( 2 )

for i in "${arrayOfMachines[@]}"
do
        echo "========== Perform symbolic link checking for Event Streaming shared library on cloud$i.sics.se =========="
        if ssh $machineUser@cloud$i.sics.se stat $LIB_NDB_CLIENT_FILE_PATH \> /dev/null 2\>\&1 
	then
        	echo "===> $LIB_NDB_CLIENT_FILE_PATH is already have the symbolic link"
	else
        	echo "===> $LIB_NDB_CLIENT_FILE_PATH is not created symbolic link.. creating now.."
        	ssh  $machineUser@cloud$i.sics.se               "cd /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native; ln -s libndbclient.so libndbclient.so.6.0.0"
	fi
        echo "========== Cleaning up previous libhopsndbevent distribution =========="


        if ssh $machineUser@cloud$i.sics.se stat $LIB_HOPS_NDB_EVENT_LIBRARY \> /dev/null 2\>\&1
        then
        ssh  $machineUser@cloud$i.sics.se                "rm  /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/libhopsndbevent.so"
        fi

 	ssh  $machineUser@cloud$i.sics.se 		 "rm -rf /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent" 
        ssh  $machineUser@cloud$i.sics.se  		 "rm -rf /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent.tar.gz"
       
        echo "=========  Uploading hopsndbevent distribution on cloud$i.sics.se =========="
        scp  $ndbdistributionDirectory/hopsndbevent.tar.gz $machineUser@cloud$i.sics.se:/home/$machineUser/hop_distro/hadoop-2.4.0/lib/native
        
        ssh  $machineUser@cloud$i.sics.se  		"cd  /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native"
        ssh  $machineUser@cloud$i.sics.se  		"cd /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native; tar -zxvf hopsndbevent.tar.gz  > /dev/null 2>&1" 
              
         
        
        ssh  $machineUser@cloud$i.sics.se  		"mkdir /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent/bin"
        ssh  $machineUser@cloud$i.sics.se  		"mkdir /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent/obj"
        
        echo "========== Remote compilation starts now =========="
        ssh  $machineUser@cloud$i.sics.se  		"cd /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent ; make remove; make"
        echo "========== Compilation is done. copy libhopsndbevent.so to native lib folder =========="
     
        ssh  $machineUser@cloud$i.sics.se  		"cp /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent/bin/libhopsndbevent.so /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native"
        ssh  $machineUser@cloud$i.sics.se  		"cp /home/$machineUser/hop_distro/hadoop-2.4.0/lib/native/hopsndbevent/EventAPIConfig.ini /home/$machineUser"
done
