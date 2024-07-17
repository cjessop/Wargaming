import numpy as np
import pandas as pd
import pygame
from scipy import interpolate                    # used to interpolate position, speed etc.
from bisect import bisect_left, bisect_right     #used to find closest stage number to time value
from geopy.distance import geodesic              #used to calculate distance between two lat/lon   depends on geographiclib
import geopy.distance
import random

import sys
import time
import math
import os



# Initialize Pygame
pygame.init()

# Set up display
MAP_WIDTH, MAP_HEIGHT = 8192, 4096
#WIDTH, HEIGHT = 819.2, 409.6
WIDTH, HEIGHT = 1920, 960     #2000,1200

#folder to read csv from
#data_folder = 'Scenarios\sry_Africa\\'             
#data_folder = 'Scenarios\Equirect\\'
data_folder = 'Scenarios\Arb_Stage_test\\'

raid_size = 0          # number of ICBMs to launch, initialised as zero  

#the code below uses a console based method to select scenario
"""
with os.scandir("Scenarios\\") as Dir:
    for entry in Dir:
        if entry.is_dir():
            breakFor = False
            
            while True:
                response=input(f"Run Scenario {entry.name}? \n")            #ask user
                
                if response.lower() in ["y" ,"yes"]:                #set data folder and break for loop
                    data_folder = entry.path +"\\"
                    breakFor=True
                    break
                elif response.lower() in ["n", "no"]:               # move onto next entry
                    break
                else:
                     print("Invalid response please enter yes/no")  # report invalid input before asking again
                     
            if breakFor == True:     # stop going through scenarios
                break
 """               
#%% Scenario Menu
Scenario_list = []
with os.scandir("Scenarios\\") as Dir:
    for entry in Dir:
        if entry.is_dir():
            Scenario_list.append(entry)
            
menuSize=tuple((560,560))            
menu= pygame.display.set_mode(menuSize,pygame.RESIZABLE)
selected = 0
onMenu = True
frameCount = 0
blink = True
Clock=pygame.time.Clock()

# %%% Menu Loop
while onMenu:
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:   
            if event.mod & pygame.KMOD_SHIFT:
                # User can exit by pressing Shift + escape
                onMenu=False
                print("Cancelling Game")
                pygame.quit()
                sys.exit()
                        
        if event.type == pygame.QUIT: # exit using window close button
            onMenu = False
            pygame.quit()
            sys.exit()
            
        if event.type == pygame.KEYDOWN and event.key == pygame.K_UP:  #scroll up list
            if selected == 0:
                selected = len(Scenario_list)-1    #loop back to final scenario
            else:
                selected -= 1    #decrement by one
            
        if event.type == pygame.KEYDOWN and event.key == pygame.K_DOWN:  #scroll down list
            if selected == len(Scenario_list)-1:
                selected =  0    #loop back to first scenario
            else:
                selected += 1    #increment by one
         
        if event.type == pygame.KEYDOWN and event.key == pygame.K_RETURN:  # make selection of scenario
            onMenu = False
            
    menu.fill((0,0,0))   #clear the window
    num = 0
    
    #Title Menu
    menu_str = "SCENARIOS"
    menu_font = pygame.font.SysFont('ocraextended',54, bold=True)
    menu_text = menu_font.render(menu_str, True, (0,255,0))
    menu_rect = menu_text.get_rect(midleft=(80,28))
    menu.blit(menu_text,menu_rect)
    pygame.draw.line(menu, (0,255,0), (0,60), (menuSize[1],60), width = 2)
    
    for i in Scenario_list:
        #populate list of scenarios to screen
        
        with open(i.path + "\\NAME.txt",'r') as file:
            ScName = file.read().rstrip()     #read name from file removing any trailing whitespace
        
        
        menu_str = f"{ScName}"
        menu_font = pygame.font.SysFont('ocraextended',44)
        menu_text = menu_font.render(menu_str, True, (0,255,0))
        menu_rect = menu_text.get_rect(midleft=(80,88+num*50))
        menu.blit(menu_text,menu_rect)
        num +=1
    
    if frameCount % 40 == 0:         #toggle boolean value blink every 40 frames
        blink = not blink 
    if blink:
        #draw circle indicator for current selection if blink is true
        pygame.draw.circle(menu,(0,255,0),center=(50,88+selected*50),radius=15)

    #update screen
    pygame.display.flip()
    frameCount += 1
    # Cap frame rate
    Clock.tick(60)
    
    
    

print(f"Scenario folder: {Scenario_list[selected].name}\n\n")
data_folder= Scenario_list[selected].path + '\\'  # use the user selected scenario

#%% Game Set-up
#look for number of rocket csv files in directory
with os.scandir(data_folder) as Dir:
    for entry in Dir:
        if entry.name.endswith('.csv') and entry.is_file():   #if the directory entry is a file ending in .csv, increment raid_size
            raid_size += 1

print('Number of rockets = ' + str(raid_size))
             
PRE_LAUNCH_DELAY = 15  # 15 seconds before launch

font = pygame.font.Font(None, 36)

##screen = pygame.display.set_mode((WIDTH, HEIGHT))
screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
pygame.display.set_caption("Rocket Trajectory")

# Scale Factor for csv coordinates    Earth circumerence (equator) is 40,075 km
#ScaleFactorX = WIDTH/40075
#ScaleFactorY = HEIGHT/39941
#ScaleFactorX = WIDTH/40030
#ScaleFactorY = HEIGHT/40030
ScaleFactorX = WIDTH/360                # scaled based on lat/lon due to equirectangular map projection
#ScaleFactorY = HEIGHT/360

time_scale = 1       # time scale for running outside of realtime

dash_display = 1     # places tracker text over the unoccupied part of the screen

global WARN_start 
WARN_start = time.time()

# Load map image
map_image = pygame.image.load("8k_earth_daymap.jpg").convert()  # Replace with your image path
map_image = pygame.transform.scale(map_image, (WIDTH, int(WIDTH / MAP_WIDTH * MAP_HEIGHT)))

Trajectories=list()
print('trajectory list made!')

for i in range(1, raid_size + 1):     # loop through for rocket/traj from 1 to the size of the raid
    
    print('trajectory file #'+str(i))
    
    #import trajectory data into a dataframe
    trajData=pd.read_csv(data_folder + 'ICBM_traj_'+str(i)+'.csv',names=['stage','time','x','y','z(alt)','V_air','V_x','V_y','V_z','mach'])   # z(alt) in meters


    lat = trajData['y'].values
    lon = trajData['x'].values      
    
    latlon_dict = {'Lat' : lat, 'Lon' : lon}
    trajData= trajData.assign(**latlon_dict)
        
    # lambda function to rescale coordinates
    ScalerX= lambda x: x*ScaleFactorX + 0.5*WIDTH
    ScalerY= lambda y: 0.5*HEIGHT - y*ScaleFactorX
    
    '''
    # adjust coordinates for when latitude goes above 90 degrees (over northern edge)
    trajData['y'] = trajData['y'].apply(lambda x: 90 - (x-90) if x > 90  else x) 
    #adjust coordinates for when latitude goes below -90 degrees (beneath southern edge)
    trajData['y'] = trajData['y'].apply(lambda x: -90 - (x+90) if x < -90  else x)
    '''
    '''      
    #Unnecessary for longitude as tabsys handles it
    trajData['x'] = trajData['x'].apply(lambda x: x-360 if x > 180  else x)
    trajData['x'] = trajData['x'].apply(lambda x: x+360 if x < -180  else x) 
    '''    

            
    
    #apply Scaling function to data frame
    trajData['x'] = trajData['x'].apply(ScalerX)
    trajData['y'] = trajData['y'].apply(ScalerY)
    
    Trajectories.append(trajData)
    print('trajectory added')


# Define launch and target positions (x, y)
#launch_position = [(400, 250), (450, 250)]
#target_position = (1100, 500)
#target_position = (1750,400)
#new target possition based on input data
#target_position = (trajData.values[trajData.index[-1],2],trajData.values[trajData.index[-1],3])


print('Trajectory List compiled!')

scenario_start_time = time.time()
#Globe Time
globeTime = scenario_start_time
Clock.tick(60)

#%%Function Definitions

# Draw grid lines and labels
def draw_grid(screen, width, height, spacing):
    for x in range(0, width, spacing):
        pygame.draw.line(screen, (100, 100, 100), (x, 0), (x, height))
        font = pygame.font.Font(None, 24)
        text = font.render(str(x), True, (255, 255, 255))
        screen.blit(text, (x + 5, 5))
    for y in range(0, height, spacing):
        pygame.draw.line(screen, (100, 100, 100), (0, y), (width, y))
        font = pygame.font.Font(None, 24)
        text = font.render(str(y), True, (255, 255, 255))
        screen.blit(text, (5, y + 5))

def draw_grid_minor(screen, width, height, major_spacing, minor_spacing):
    for x in range(0, width, major_spacing):
        pygame.draw.line(screen, (100, 100, 100), (x, 0), (x, height))
        font = pygame.font.Font(None, 24)
        text = font.render(str(x), True, (255, 255, 255))
        screen.blit(text, (x + 5, 5))
        for minor_x in range(x + minor_spacing, x + major_spacing, minor_spacing):
            pygame.draw.line(screen, (50, 50, 50, 100), (minor_x, 0), (minor_x, height), 1)
    for y in range(0, height, major_spacing):
        pygame.draw.line(screen, (100, 100, 100), (0, y), (width, y))
        font = pygame.font.Font(None, 24)
        text = font.render(str(y), True, (255, 255, 255))
        screen.blit(text, (5, y + 5))
        for minor_y in range(y + minor_spacing, y + major_spacing, minor_spacing):
            pygame.draw.line(screen, (50, 50, 50, 100), (0, minor_y), (width, minor_y), 1)
  


def velocity_tracker(screen, velocity_on, mach_on, which_rocket, airspeed, dispAlt):  
    
    # display rocket velocity and mach number 
    # velocity_on & mach_on control which trackers to display

    # which rocket is an integer value determining which rocket to track 
    # airspeed is whether to use airspeed or coordinate speed components
    # dispAlt determines whether to display the rockets altitude
    Vfont = pygame.font.Font(None, 36)
    
    if dash_display ==1 :
        track_vert = 980
    else:
        track_vert = HEIGHT - 80
    
    if which_rocket > raid_size or which_rocket <= 0:  # set index of rocket (defaults to first rocket if out of bounds)
        R_index = 0
    R_index = which_rocket - 1 
    
    if velocity_on is True and airspeed is True:
        # Velocity tracker
        
        Rocket_Velocity = rocket_list[R_index].AirV
        Rocket_Velocity_str = f"Rocket {VTracked} Airspeed: {Rocket_Velocity :0.0f} m/s"
        Speed_text = Vfont.render(Rocket_Velocity_str, True, rocket_list[R_index].colour)
        screen.blit(Speed_text,(WIDTH - 750, track_vert))
        
                
    elif velocity_on is True and airspeed is False:
            
        Rocket_Velocity = math.sqrt(rocket_list[R_index].XVel**2 + rocket_list[R_index].YVel**2)/ScaleFactorX #*1000    ##different units between x/y & z coordinates so V doesn't use z'

        Rocket_Velocity_str = f"Rocket {VTracked} Velocity: {Rocket_Velocity :0.4g}"
        Speed_text = Vfont.render(Rocket_Velocity_str, True, rocket_list[R_index].colour)
        screen.blit(Speed_text,(WIDTH - 750,  track_vert))
        
   
    if mach_on is True:
        #Mach tracker
        Mach_str = f"Rocket {VTracked} Mach: {rocket_list[R_index].mach :0.2f}"
        Mach_text = Vfont.render(Mach_str, True, rocket_list[R_index].colour)
        screen.blit(Mach_text, (WIDTH - 1050,  track_vert))
    
    if dispAlt is True:
        #altitude tracker
        Alt_str= f"Rocket {VTracked} Altitude: {rocket_list[R_index].Z_Alt *1e-3 : 0.2f} km"  # format altitude into kilometers
        Alt_text = Vfont.render(Alt_str,True,rocket_list[R_index].colour)
        screen.blit(Alt_text, (WIDTH - 400,  track_vert))
    

# credit for this function to  "https://stackoverflow.com/questions/30112202/how-do-i-find-the-closest-values-in-a-pandas-series-to-an-input-number"
def get_closests(df,col,val):
    # if val is in column then bisect_left returns the index of the value in the list & bisect_right returns the index of the next position            
    # if val is not in the list bisect_left & bisect_right return the same value that can be used to insert val while maintaining the lists order '''
        
    # this function returns the index of the val or the index of the value before
    lower_idx = bisect_left(df[col].values, val)
    higher_idx = bisect_right(df[col].values, val)
        
    if higher_idx == lower_idx:   # val is not in list   #probably true in my use case
        return lower_idx - 1  #, lower_idx    #uncomment lower_idx if you want the value afterwards as well
    else:
        return lower_idx

def draw_timescale(screen,time_scale):
    #this function will draw an indicator of the timescale of the game i.e. is it running x2 speed, realtime or other.
    #triangle side length = 40
    
    triSide= 40
    triLength=0.5*(math.sqrt(3)*triSide)
    triX=82; triY=880
    
    
    if time_scale == 1:   # real-time, draw 1 triangle
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        
    elif time_scale > 1 and time_scale < 5:  # speed between 1x and 5x, draw two triangles
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        
    elif time_scale >= 5 and time_scale < 10:# speed from 5x to 10x, draw three triangles
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        
    elif time_scale >= 10:                  # speed at or above 10x, draw four triangles
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        triX += 0.6*triLength
        pygame.draw.polygon(screen,(255,0,0), [(triX, triY), (triX, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        
            
    elif time_scale < 1 and time_scale > 0: # speed below 1x draw a line & triangle
        pygame.draw.polygon(screen,(255,0,0), [(triX+6, triY), (triX+6, triY + triSide), (triX + triLength, triY + 0.5*triSide)], width=0)
        pygame.draw.line(screen,(255,0,0), (triX, triY), (triX, triY + triSide), width= 5)
        
    elif time_scale == 0:                   # 0 speed is paused
        pygame.draw.line(screen,(255,0,0), (triX + 3, triY), (triX + 3, triY + triSide), width= 10)
        pygame.draw.line(screen,(255,0,0), (triX + 25, triY), (triX + 25, triY + triSide), width= 10)
    
    return

# %% CLASS Definitions

# Rocket class
class Rocket(pygame.sprite.Sprite):
    def __init__(self, launch_position,flight_time, target_position, InterpX, InterpY, InterpZ, IntAirV, IntVx, IntVy, IntVz, IntMach, number,\
                 lat,lon, InterpLat, InterpLon):
        super().__init__()
        self.image = pygame.Surface((10, 10))
        self.RNumber = number                            #number of rocket
        self.launch_position = tuple(launch_position)    #position of rocket launch
        self.target_position = tuple(target_position)    #position of target
        self.lat = lat; self.lon = lon                   # current lat/lon of the rocket
        # self.image = pygame.Surface((40, 40), pygame.SRCALPHA)  # Transparent surface
        # self.image.fill((0, 0, 0, 0))  # Make the surface transparent
        # pygame.draw.circle(self.image, (255, 0, 0), (20, 20), 20)  # Draw a red filled circle
        self.stage = 1
        self.colour = (255,0,0)    #this is the colour used for the rocket sprite and related tracker text
        self.image.fill(self.colour)
        self.rect = self.image.get_rect()
        self.rect.center = self.launch_position
        self.speed = self.calculate_speed()  # Calculate speed based on travel time
        self.start_time = scenario_start_time + PRE_LAUNCH_DELAY
        #self.travel_time = 35 * 60  # 35 minutes in seconds
        self.travel_time = flight_time   # added input of rocket flight time, to be extracted from .csv file
        self.elapsed_time = 0.           # Elapsed time made into a attribute of the rocket
        #Interpolation functions for X & Y positions
        self.InterpX = InterpX
        self.InterpY = InterpY
        self.InterpZ = InterpZ
        self.PosXY = tuple(launch_position)
        self.Z_Alt = 0
        self.InterpLat = InterpLat
        self.InterpLon = InterpLon
        #Interpolation functions for velocity
        self.IntAirV= IntAirV
        self.IntVx = IntVx
        self.IntVy = IntVy
        self.IntVz = IntVz
        self.AirV = 1
        self.XVel = 0
        self.YVel = 0
        self.ZVel = 0
        self.IntMach = IntMach
        self.mach = 0
        
        
        #Trajectory memory
        self.Traj_Memory= list()
        self.Traj_Memory.append(self.launch_position)
        self.Imp_flag = False
        self.EWRADAR = False
        self.Triggered = False
        
    def __repr__(self):
        
        return f"Rocket {self.RNumber} Sprite"

    def update(self):
        
        #Update time elapsed since launch
        self.elapsed_time = globeTime - self.start_time
        #update progress towards target (based on time) measure
        self.progress = self.elapsed_time / self.travel_time
        
        #update rocket stage number
        RInd = self.RNumber -1
        #base rocket stage on the stage at the last trejectory data point
        prev_timestep = get_closests(Trajectories[RInd], 'time', self.elapsed_time)
        self.stage = Trajectories[RInd].values[Trajectories[RInd].index[prev_timestep], 0]
    
        #update color to reflect whether the reentry vehicle has seperated
        if self.stage <= 4:
            self.colour = (255,0,0)
        else :
            self.colour = (255,130,0)
            
        self.image.fill(self.colour) #fill sprite with the updated colour
        
        
        #if self.progress >= 0.1 and self.EWRADAR == False:  # set flag to trigger event
        #    self.EWRADAR = True
        #    global WARN_start ; WARN_start = time.time()  # record time
        
      
            
        if self.progress >= 1:
            self.rect.center = self.target_position
            self.PosXY = self.target_position
            self.Z_Alt = 0
            self.XVel = 0
            self.YVel = 0
            self.ZVel = 0
            self.mach = 0
            self.AirV = 0
            self.image.set_alpha(0)  # hide rocket blip after impact
            if not self.Imp_flag:
                print('Rocket '+str(self.RNumber)+' impacted')      #print confirmation of impact
                self.Imp_flag = True                                #set impact flag to true to prevent repeat messages
                
        else:
            ## Calculate current position based on linear interpolation
            #current_x =self.launch_position[0] + (target_position[0] -self.launch_position[0]) * progress
            #current_y =self.launch_position[1] + (target_position[1] -self.launch_position[1]) * progress
            
            # Interpolate position from csv data
                        
            current_x = (self.InterpX(self.elapsed_time))
            current_y = (self.InterpY(self.elapsed_time))
            current_z = (self.InterpZ(self.elapsed_time))
            self.PosXY = (current_x, current_y)
            self.Z_Alt = current_z
            
            
            #position as lat/lon
            self.lat = self.InterpLat(self.elapsed_time)
            self.lon = self.InterpLon(self.elapsed_time)
            
            self.rect.center = (current_x, current_y)
            
            
            #interpolate velocity from csv data
            self.XVel = self.IntVx(self.elapsed_time)
            self.YVel = self.IntVy(self.elapsed_time)
            self.ZVel = self.IntVz(self.elapsed_time)
            self.mach = self.IntMach(self.elapsed_time)
            self.AirV = self.IntAirV(self.elapsed_time)
            

    def calculate_speed(self):
        # Calculate speed based on travel time and the distance between launch and target positions
        distance = ((self.target_position[0] -self.launch_position[0]) ** 2 + (self.target_position[1] -self.launch_position[1]) ** 2) ** 0.5
        speed = distance / (35 * 60)
        return speed
    

    def draw_trajectory(self, screen,Rocket_Iter):
        #pygame.draw.line(screen, (255, 255, 255),self.launch_position, self.rect.center, 2)
        #track trajectory and draw 
        
        #if self.progress <= 1:
        self.Traj_Memory.append(self.rect.center)   # add point to trajectory list
        
        for i in range(0,Rocket_Iter+1):
            if i != 0:
                # to prevent a line being drawn across the globe when a rocket crosses the pacific
                traveled = self.Traj_Memory[i-1][0] - self.Traj_Memory[i][0] #(Pos1[x] - Pos2[y]) 
                if traveled < 5 and traveled > -5:    #if rocket moved more than 5 pixels east or west in a single frame don't draw line
                    pygame.draw.line(screen,(255,255,255), self.Traj_Memory[i-1],self.Traj_Memory[i],2)
        

#RADAR Class

class RADAR(pygame.sprite.Sprite):
    def __init__(self, R_num,Type, RadarPosition, Range):
        super().__init__()
        self.image= pygame.Surface((11,11))
        self.image.set_colorkey((0,0,0)) # makes the black background transparent
        pygame.draw.line(self.image,(0,255,255),(0,5),(11,5),width=3) #sprite appearance for radar
        pygame.draw.line(self.image,(0,255,255),(5,0),(5,11),width=3) #light blue cross
        
        self.R_num = R_num
        self.lat= RadarPosition[0]  #radar latitude
        self.lon= RadarPosition[1]  # radar longitude
        self.position= tuple((ScalerX(self.lon),ScalerY(self.lat)))
        self.rect = self.image.get_rect()
        self.rect.center = self.position
        self.range= Range                    # radar maximum range (km)
        self.range_circle()                  # generate radar maximum range points
        self.type = Type
        self.Det_P = 0.005                   # 0.005% detection prob per frame (about 25% detection chance per second)
        
    def __repr__(self):
        
        return f"{self.type} RADAR {self.R_num} Sprite"
    
    def range_circle(self):
        ## generates a sequence of points describing a circle of the radar range
        
        points=[]
        #if very short range skip some bearings
        if self.range < 100:
            bearing_int = 5
        else:
            bearing_int = 1

        
        bearings = range(0,360,bearing_int)
        for bear in bearings:
            point = geopy.distance.distance(kilometers=self.range).destination((self.lat,self.lon), bearing = bear) #gen. point range km from radar
            pointX=ScalerX(point[1])  #convert from lat/lon to pixel scale
            pointY=ScalerY(point[0])
            points.append((pointX,pointY))  # add to list
            
        self.range_end = points  #save list as radar attribute
      
        
    def search(self, rocket):
        
        
        distance = geodesic((self.lat,self.lon),(rocket.lat,rocket.lon)).km    #uses geodesic function from geopy to calculate distance in km
        
        #print(f'RADAR- ({self.lat},{self.lon}) -- Rocket {rocket.RNumber}- ({rocket.lat},{rocket.lon}) -- Distance- {distance}')
        roll=random.random()
        
        if distance <= self.range and roll<=self.Det_P:
            global WARN_start
            
            if rocket.EWRADAR == False and rocket.Triggered == False:
               WARN_start = globeTime  # record time
                
            return WARN_start

        else:
            return False
        
    def draw_range(self,screen):

        for i in range(0,len(self.range_end)):
            if i !=0:
                distance = self.range_end[i-1][0] - self.range_end[i][0]
            else:
                distance = self.range_end[len(self.range_end)-1][0] - self.range_end[i][0]  #draw final section of circle
                
            if distance < 5 and distance > -5:   # if range crosses east/west edge of map don't draw line
                pygame.draw.line(screen,(0,255,255), self.range_end[i-1], self.range_end[i], width=1)
            #pygame.draw.lines(screen,color=(0,255,255), closed=True, points=self.range_end, width=1)



# %% Game set-up
# %%% Rocket Creation
## continue with Game logic
# Create rocket sprite

rocket_list = []
for i in range(0, len(Trajectories)):                # loop through all rockets starting from rocket1
    #rand_num1 = np.random.randint(400, 500)
    #rand_num2 = np.random.randint(200, 250)
    #rocket = Rocket((rand_num1, rand_num2))
    
    # coordinates for launch
    
    launchX = Trajectories[i].values[0,2]   # initial location pixels
    launchY = Trajectories[i].values[0,3]
    launchLat = Trajectories[i].values[0,10] # inital location coordinates
    launchLon = Trajectories[i].values[0,11]
    
    #target position
    target_position = (Trajectories[i].values[Trajectories[i].index[-1],2],Trajectories[i].values[Trajectories[i].index[-1],3])
    
    flight_time =Trajectories[i].values[Trajectories[i].index[-1],1]                 #time from launch to impact

    InterpX = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['x'])         #interpolation function for X position
    InterpY = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['y'])         #interpolation function for Y position
    InterpZ = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['z(alt)'])    #interpolation function for Z position
    
    IntVx = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['V_x'])    #interpolation function for X velocity
    IntVy = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['V_y'])    #interpolation function for Y velocity
    IntVz = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['V_z'])    #interpolation function for Z velocity
    IntAirV = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['V_air'])#interpolation function for airspeed
    
    IntMach = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['mach']) #interpolation function for mach
    
    IntLat = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['Lat'])  #interpolation function for Latitude
    IntLon = interpolate.interp1d( Trajectories[i]['time'], Trajectories[i]['Lon'])  #interpolation function for Longitude
    
    #rocket creation (launch coords)   , flight length,    target   ,  interpolation functions for position and vel & mach   ,         rocket number 
    rocket = Rocket((launchX, launchY), flight_time, target_position, InterpX, InterpY, InterpZ, IntAirV, IntVx, IntVy, IntVz, IntMach,i+1,\
                    launchLat, launchLon, IntLat, IntLon)
        #           launch coordinate,     interpolation functions for  lat/lon position
    rocket_list.append(rocket)
    print(f'Rocket {rocket.RNumber} added to Rocket list')

print(rocket_list)


#rocket1 = Rocket((400, 250), flight_time)
#rocket2 = Rocket((450, 250), flight_time)

#create a sprite group for rockets
all_sprites = pygame.sprite.Group()
for i in range(0, len(rocket_list)):
    all_sprites.add(rocket_list[i])
#all_sprites.add(rocket1, rocket2)


# %%% RADAR Creation
Radar_list = []

            # remember lat/long is in different order to x/y
#TestRadar = RADAR(1, (ScalerX(25.23),ScalerY(28.60)), 200)
#TestRadar = RADAR(99,'EW', (15, 30), 2000)
#Radar_list.append(TestRadar)

#Import RADARs from comma delimited file
Radars=pd.read_csv(data_folder + 'RADAR.rdrs',names=['Number','Type','lat','lon','Range']) 

for index, row in Radars.iterrows():
    radar = RADAR(row['Number'],row['Type'],(row['lat'],row['lon']),row['Range'])
    Radar_list.append(radar)

print(Radar_list)

#Radar Sprite group
all_Radar = pygame.sprite.Group()
#add radar sprites
for i in range(0,len(Radar_list)):
    all_Radar.add(Radar_list[i])



print('Sprite groups created & filled!\nStarting Main Game Loop!')


# %%% Main loop prep
running = True
event_spotted = False
Rocket_Iter=0
VTracked=1  # which rocket to track the velocity of
frameCount=0 # Reset frame counter
draw_range=True
pygame.key.set_repeat(400)  # repeat key on hold for 0.4 sec


# %% Main Game Loop
while running:
    for event in pygame.event.get():
        
        # %%% Controls
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:   
            if event.mod & pygame.KMOD_SHIFT:
                # User can exit by pressing Shift + escape
                running=False
                
        
        if event.type == pygame.QUIT:
            running = False
            
        if event.type == pygame.KEYDOWN and event.key == pygame.K_LEFT:
            if VTracked == 1:
                VTracked = raid_size    #loop back to last rocket
            else:
                VTracked -= 1    #decrement by one
            
        if event.type == pygame.KEYDOWN and event.key == pygame.K_RIGHT:
            if VTracked == raid_size:
                VTracked = 1    #loop back to last rocket
            else:
                VTracked += 1    #increment by one
        
        # User can use number keys to adjust timescale
        if event.type == pygame.KEYDOWN and event.key == pygame.K_1:
            time_scale = 1    # 1 key - realtime
        if event.type == pygame.KEYDOWN and event.key == pygame.K_2:
            time_scale = 2    # 2 key - double speed
        if event.type == pygame.KEYDOWN and event.key == pygame.K_3:
            time_scale = 5    # 3 key - quintuple speed
        if event.type == pygame.KEYDOWN and event.key == pygame.K_4:
            time_scale = 10   # 4 key - 10x speed
        if event.type == pygame.KEYDOWN and event.key == pygame.K_0:
            time_scale = 0.5  # 0 key - half speed
        if event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
            time_scale = 0  # spacebar - Pause
            
        #Use Hash key to toggle range circles on and off
        if event.type == pygame.KEYDOWN and event.key == pygame.K_HASH:
            if draw_range == True:
                draw_range = False
            else: 
                draw_range = True
                
        # Update time to landing for the tracked rocket
        rocketInd = VTracked - 1    # adjust to an index

# %%% Radar sweep
    # each radar checks if it can see each rocket
    for i in range(0,len(Radar_list)):
        for j in range(0,len(rocket_list)):
            search_res = Radar_list[i].search(rocket_list[j])   # search for rocket
            if search_res != False:                             # if rocket found
                rocket_list[j].EWRADAR = True                   # set EWRadar to true
                if rocket_list[j].Triggered == False:           # if needed record the time of trigger
                    WARN_start = search_res
                    
            
    
    # %%% Screen Drawing
    if event_spotted:
        # Update rocket
        all_sprites.update()

    # Clear the screen
    screen.fill((0, 0, 0))

    # Draw map
    screen.blit(map_image, (0, 0))
    # Draw grid lines
    draw_grid_minor(screen, WIDTH, HEIGHT, 160, 40)  # Adjust spacing as needed (original was 100,50)

    # Draw launch position
    if not event_spotted:
        #pygame.draw.circle(screen, (255, 0, 0), (400, 250), 5, 2)   ## need to adjust IR EVENT circle to match rough location of launch
        for rocket in rocket_list:
            pygame.draw.circle(screen, (255, 0, 0), rocket.launch_position, 5, 2)
            
    # Draw timescale icon
    draw_timescale(screen, time_scale)

    # Draw sprites
    if event_spotted:
        
        #draw radar locations
        all_Radar.draw(screen)
        #draw range if flag set true
        if draw_range==True:
            for Radar in Radar_list:
                Radar.draw_range(screen)
        
        # Draw rocket trajectory
        for rocket in rocket_list:
            rocket.draw_trajectory(screen, Rocket_Iter)
               
        Rocket_Iter+= 1
            
        #draw Rocket sprites
        all_sprites.draw(screen)
        
        
        #%%%% Text
        velocity_tracker(screen, True, True, VTracked, True, True) #draw rocket velocity tracker  (see function comments for details on arguments)
        
            #rocket stage display
        if dash_display == 1:
            stage_vert = 980
            stage_hori = 520
        else:
            stage_vert = HEIGHT -80
            stage_hori = WIDTH - 1500
            
        R_stage = rocket_list[rocketInd].stage
        if R_stage == 5 :                               # stage five is the reentry vehicle stage
            stage_str = f"Rocket {VTracked} Stage: RV"
        else:
            stage_str = f"Rocket {VTracked} Stage: {R_stage:0.0f}"
        stagefont = pygame.font.Font(None, 36)
        stage_txt = stagefont.render(stage_str, True, rocket_list[rocketInd].colour)
        screen.blit(stage_txt,(stage_hori, stage_vert))

    # Display "IR EVENT SPOTTED" text at the top
    if not event_spotted:
        font = pygame.font.Font(None, 36)
        IRfont = pygame.font.SysFont(None, 36)
        text = font.render("IR EVENT SPOTTED", True, (255, 0, 0))
        text_rect = text.get_rect(center=(WIDTH // 2, 52))
        screen.blit(text, text_rect)
        
        #draw radar locations
        all_Radar.draw(screen)
        #draw range if flag set true
        if draw_range:
            for Radar in Radar_list:
                Radar.draw_range(screen)

    if not event_spotted and globeTime - scenario_start_time  >= PRE_LAUNCH_DELAY:
        event_spotted = True
        #print('Rocket start time before = '+str(rocket))
        print("Rocket(s) spotted!")
        for rocket in rocket_list:
            
                rocket.start_time = globeTime
                

   
    
    for rocket in rocket_list:
        if rocket.EWRADAR == True:
            
            if globeTime <= WARN_start + 5:  # display flashing warning for 5 seconds
                EWRfont = pygame.font.SysFont('ocraextended', 50)
                WARN_text= EWRfont.render("WARNING!", True, rocket.colour)
                text_rect = text.get_rect(center=( 1000, 360))
                screen.blit(WARN_text, text_rect)
                
                rocket.Triggered = True  # update triggered flag
                
                            
    #%%%%% Time to landing
    
    #time_to_landing = max(0, rocket_list[rocketInd].travel_time - (time.time() - rocket_list[rocketInd].start_time))
    # Set up time to landing for timer based on whether it is before launch or not
    if not event_spotted and globeTime - scenario_start_time  <= PRE_LAUNCH_DELAY:
        # uses time-start_time because rocket update is not called until after rocket launch, so elpsed time will be 0
        time_to_landing = rocket_list[rocketInd].travel_time + PRE_LAUNCH_DELAY - (globeTime-scenario_start_time)
    else:
        time_to_landing = max(0, rocket_list[rocketInd].travel_time - rocket_list[rocketInd].elapsed_time)
        
    if dash_display == 1:
        time_vert = 980
        time_hori = 40
    else:
        time_vert = 20
        time_hori = WIDTH - 400
        
    time_to_landing_str = "Rocket {} Time to Impact: {:02}:{:02}".format(str(VTracked), int(time_to_landing // 60), int(time_to_landing % 60))
    timefont = pygame.font.Font(None, 36) #pygame.font.SysFont('ocraextended', 24)
    timer_text = timefont.render(time_to_landing_str, True, rocket_list[rocketInd].colour)
    screen.blit(timer_text, (time_hori, time_vert))  # Adjust the position as needed
    

        
    # Update display
    pygame.display.flip()

    frameCount += 1
    # Cap frame rate
    Clock.tick(60)

    #global time = previous time + timeScale adjusted time between frames (obtained in ms)
    globeTime = globeTime + time_scale*(Clock.get_time()/1000)



#%% Game Exit
print('Exiting Game')
pygame.quit()
sys.exit()
