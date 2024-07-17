% extract relevent data from the output file/workspace output, make sure the
% data structure is in the workspace or is imported from file
% to work from the workspace dat and dat1 need to be passed if no arguemnts
% are passed it will try to import from file.
% this version should work for rockets without seperate RV such as SCUD-B,
% will however only work as intended with 1 RV
function Extractor = CSV_Extractor(folder, TrajNum, dat, dat1)

% clear;clc;
emptyDat1 = 0;

if ~exist('dat','var') && ~exist('dat1','var') 
%importing data from file, if not passed

     load('DATOUT.mat')
     dat=alldat.dat;             %Mainbody stages 1-4
     if strcmp(alldat.dat{1,1}.SpentStageName, 'RV1')
        RVdat=alldat.dat1{1,1};     %RV1  
     else
         RVdat=cell(0,1);   %if dat1s first entry isnt an rv RVdat will be the same as if it were empty
     end
     clear alldat;
     
end
%Stage number
Stage=dat.StageNum;    

%Simulation time
Time=dat.tout;        


%Position
z = dat.geod_alt;  %altitude (m) is z coord


a=6378.137; b=6356.800;    % a=semi-major axis   b= semi-minor axis
ecc=sqrt(1-((b*b)/(a*a))); %eccentricity

%grn2eqa(lat,dlon, [origin], [ellipsoid avg radius(semi-major axis), ellipsoid eccentricity])
%[x,y] = grn2eqa(dat.geod_lat,dat.geod_lon, [0,0], [a,ecc]);
%[RVx,RVy] = grn2eqa(RVdat.geod_lat,RVdat.geod_lon, [0,0], [a,ecc]);

%   Map projection is equirectangular ->  lat long become straight lines -> coordinates directly

x =     dat.geod_lon; y = dat.geod_lat;


%Mach number
Mach=dat.mach;



if size(dat1)~=size(cell(0,1))
    RVdat=dat1{1,1}; 
    StageRV=RVdat.StageNum;
    TimeRV=RVdat.tout;
    RVz = RVdat.geod_alt;
    RVx = RVdat.geod_lon; RVy = RVdat.geod_lat;
    RVmach=RVdat.mach;
    
    %join RV data and main body
    Stage=cat(1,Stage,StageRV);Time=cat(1,Time,TimeRV);x=cat(1,x,RVx);y=cat(1,y,RVy);z=cat(1,z,RVz);Mach=cat(1,Mach,RVmach);
else
    fprintf('empty dat1\n')
    emptyDat1 = 1;


%Velocities
Vx=zeros(size(x));Vy=zeros(size(y));Vz=zeros(size(z));
for i=1:length(x)
    if i==1
        Vx(i,1)=0;
        Vy(i,1)=0;
        Vz(i,1)=0;
    else
        Vx(i,1)= (x(i)-x(i-1))/Time(i);
        Vy(i,1)= (y(i)-y(i-1))/Time(i);
        Vz(i,1)= (z(i)-z(i-1))/Time(i);
    end
end
%Airspeed/Geodetic Velocity (m/s)

Vair=dat.geod_V;
if emptyDat1 == 0
    
    RVVair=RVdat.geod_V;
    %join
    Vair=cat(1,Vair,RVVair);
    
end

%RV
% VxRV=zeros(size(RVx));VyRV=zeros(size(RVy));VzRV=zeros(size(RVz));
% for i=1:length(RVx)
%     if i==1
%         VxRV(i,1)=0;
%         VyRV(i,1)=0;
%         VzRV(i,1)=0;
%     else
%         VxRV(i,1)= (RVx(i)-RVx(i-1))/TimeRV(i);
%         VyRV(i,1)= (RVy(i)-RVy(i-1))/TimeRV(i);
%         VzRV(i,1)= (RVz(i)-RVz(i-1))/TimeRV(i);
%     end
% end

%extract data for RV (if Present) to append


DataTable=table(Stage,Time,x,y,z,Vair,Vx,Vy,Vz,Mach);          %compile data into a table

%handle input variables

if ~exist('folder','var')
    folder=   '' ;  %if no folder passed then use as default
end
if ~exist('TrajNum','var')
    fileName= 'ICBM_traj_1.csv'; %if no trajectory number passed use as default
else
    fileName= append('ICBM_traj_', num2str(TrajNum),'.csv');
end
if strcmp(folder, '')
    writetable(DataTable,fileName,'WriteVariableNames',0);    %save as .csv file
else
    writetable(DataTable,append(folder,'\',fileName),'WriteVariableNames',0);    %save as .csv file
end

fprintf(append('file ',fileName, ' created\n\n'))




% coordinates need conversion using the DCMs recorded in ,for example, E2V etc.
% the DCM is recorded as a 9 length row vector so needs usage of
% reshape() to reform the matrix for each time step

% %convert position from ECI to Body frame
% ECI=dat.pos_eci;
% B_pos=zeros(size(ECI));
% 
% for i=1:length(ECI(:,1))
%     
%     convB=dat.I2B(i,:);         %extract convertion for timestep
%     DCM=reshape(convB,[3,3]);   %reshape back into 3x3 matrix
%     
%     B_pos(i,:)= ECI(i,:)*DCM;       %apply conversion matrix
% end
% 
% %convert velocity from ECI to Body frame
% ECI=dat.vel_eci;
% B_vel=zeros(size(ECI));
% 
% for i=1:length(ECI(:,1))
%     
%     convB=dat.I2B(i,:);         %extract convertion for timestep
%     DCM=reshape(convB,[3,3]);   %reshape back into 3x3 matrix
%     
%     B_vel(i,:)= ECI(i,:)*DCM;       %apply conversion matrix
% end

end