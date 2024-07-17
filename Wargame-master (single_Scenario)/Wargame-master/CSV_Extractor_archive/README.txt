This folder contains different versions of the MATLAB script used to extract the trejectory data from tabsys.

CSV_Extractor.m is the most recent version of the script that can handle a missile with 4 stages (+ an RV), capable of extracting the data from the 
matlab workspace immediately after running the tabsys model, or from the stored results, DATOUT.mat file.
This version does not support multiple RVs, if a rocket with MIRVs is run it should only extract the first RV trajectory (untested).

all the other versions are older versions that may or may not work for a particular rocket set up.