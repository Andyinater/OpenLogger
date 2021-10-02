# roll: atan((2*([rw]*[rx]+[ry]*[rz]))/(1-2*([rx]*[rx]+[ry]*[ry])))
# pitch: asin(2*([rw]*[ry]-[rz]*[rx]))
# yaw: atan((2*([rw]*[rz]+[rx]*[ry]))/(1-2*([ry]*[ry]+[rz]*[rz])))
# ABOVE IS WRONG, pitch was roll, but yaw was yaw


# roll: asin(2*([rw]*[ry]-[rz]*[rx]))
# pitch: atan((2*([rw]*[rx]+[ry]*[rz]))/(1-2*([rx]*[rx]+[ry]*[ry])))
# yaw: atan((2*([rw]*[rz]+[rx]*[ry]))/(1-2*([ry]*[ry]+[rz]*[rz])))

# x/y/z === rl/fb/ud
# r/p

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import animation
import os
import glob

def rotate(angle):
    ax.view_init(azim=angle)
    
def displayActionMenu():
    print(
'''
Prepared log detected. 
p2:    Plot 2D
p3:    Plot 3D
p3c:   Plot 3D + colour
nc:    Create new column 
r:     Rename column
x:     Delete column
t:     Trim log
s:     Save changes to log 
sa:    Save as new log ''')
    
    
while(True):
# Main logic loop

    # List all *.txt files in the current directory
    print("\nAvailable *.txt log files:")
    
    cwd = os.getcwd()
    fileList = glob.glob(cwd + '/*.txt')
    count = 0
    for file in fileList:
        count += 1
        print(str(count)+ ". " + file[file.rindex("\\") + 1:file.rindex(".txt")])
    
    
    # get log selection from user
    logNum = input("Log to analyze:")
    
    # determine action from selection
    if logNum == "exit":
        # exit the analysis loop
        break
    
    elif 0 < int(logNum) <= count:
        # a valid selection
        logDf_raw = pd.read_csv(fileList[int(logNum) - 1], delimiter = ",")
        file = fileList[int(logNum) - 1]
        fileName = file[file.rindex("\\") + 1:]
        print("\n" + file[file.rindex("\\") + 1:file.rindex(".txt")] + " loaded. Preview below:")
        print(logDf_raw.head(5))
        
    else:
        # invalid selection
        print("\n" + str(logNum) + " is out of range. Must select a log between 1 and " + str(count) + "\n")
        continue
    
    if logDf_raw.iloc[-1][10] != "PreparedByOpenViewer":
        # if log does not contain prepared tag
        prepLog = input("\nRaw log detected. Press enter to prepare the log:")
        if prepLog == "exit":
            break
        
        else:
            # prepare log for analysis. Combine GPS and Accel data lines into unified datapoints
            # only keep device Time(TODO), accel data, gpsTime, lat, latdir, long, longdir, gpsSpeed, gpsCount
            columnNames = ['time','rw','rx','ry','rz','ax','ay','az','GPSTime','lat','latDir','long','longDir','gpsSpeed','gpsCount']
            cleanData = []
            lastGPS = ""
            curGPS = ""
            nextGPS = ""
            curAccel = ""
            curIndex = 0
            lastGPSIndex = None
            firstTime = None
            
            for i in range(len(logDf_raw)):
                newGPS = False
                newAccel = False
                
                # get the new data from the next line
                if logDf_raw.iloc[i][0] == "$PUBX":
                    # if a GPS line
                    newGPS = True
                    lastGPS = curGPS
                    curGPS = logDf_raw.iloc[i]
                    
                else:
                    # if not GPS line, accel line
                    newAccel = True
                    curAccel = logDf_raw.iloc[i]
                    
                
                # move new data to cleanData accordingly
                if newGPS:
                    # if new data was GPS data
                    
                    
                    if lastGPSIndex != None:
                        # if there is a previous GPS datapoint in cleanData, start a new cleanData point
                        cleanData.append([curGPS[21] - firstTime, curAccel[1], curAccel[2], curAccel[3], curAccel[4], curAccel[5], curAccel[6], curAccel[7], float(curGPS[2]), float(curGPS[3]), curGPS[4], float(curGPS[5]), curGPS[6], float(curGPS[11]), int(curGPS[18])])
                        lastGPSIndex = curIndex
                        curIndex += 1
                        
                    else:
                        # if this is the first GPS value encountered
                        
                        if firstTime == None:
                            # if there is no first time taken to zero out GPS clock
                            firstTime = float(curGPS[21])
                        
                        # only keep device Time(TODO), accel data, gpsTime, lat, latdir, long, longdir, gpsSpeed, gpsCount
                        cleanData.append([curGPS[21] - firstTime, None, None, None, None, None, None, None, float(curGPS[2]), float(curGPS[3]), curGPS[4], float(curGPS[5]), curGPS[6], float(curGPS[11]), int(curGPS[18])])
                        
                        lastGPSIndex = curIndex
                        curIndex += 1
                        
                else:
                    # if new data was not GPS data it is accel data
                    if cleanData[lastGPSIndex][1] == None:
                        # if the last GPS data line in cleanData has blank accelerometer data
                        # replace blank data with accel data - ASSUMPTION: accel data was retrieved closer to GPS data time than current time
                        cleanData[lastGPSIndex][1:8] = [float(x) for x in curAccel[1:8]]
                        
                    else:
                        # if last gps data line already has accelerometer data, start a new cleanData point, holding GPS data constant over the period
                        newData = cleanData[-1].copy()
                        newData[:8] = [float(x) for x in curAccel[:8]]
                        newData[0] -= firstTime
                        cleanData.append(newData)
                        curIndex += 1
                        
    
        df = pd.DataFrame(cleanData, columns = columnNames)
        # df['time'] = df['time'].interpolate(method = 'linear', axis = 0)
        # df['time'] = df['time'].round(decimals = 2)
        
        if df.iloc[-1].isnull().any():
            df.drop(df.tail(1).index, axis=0, inplace=True)
            
        # usea unique identifier for processed logs    
        newData = [float("NaN")]*len(columnNames)
        newData[10] = "PreparedByOpenViewer"
        df.loc[len(df)] = newData
        exportName = input("\nRaw log has been processed. Name to save processed log under: ")
        exportName = exportName + ".txt"
        df.to_csv(exportName, index=False)
    
    else:
        # if log contains prepared tag, it is ready for processing
        df = logDf_raw
        while(True):
            # main analysis loop
            
            displayActionMenu()
            selection = input("Select action:")
            if selection == "exit":
                break
            elif selection == "reset":
                continue
            
            elif selection == "p2":
                print("\nFrom available columns: ")
                count = 1
                for c in df.columns:
                    print(str(count) + ". " + c)
                    count += 1
                    
                xColumn = input("\nX-axis: ")
                if xColumn == "exit":
                    break
                elif xColumn == "reset":
                    continue
                
                yColumn = input("\nY-axis: ")
                if yColumn == "exit":
                    break
                elif yColumn == "reset":
                    continue        
                
                plotType = input("\nStyle of plot (default is line): ")
                
                if plotType == "":
                    plotType = "line"
                    df.plot(x = df.columns[int(xColumn)-1], y = df.columns[int(yColumn)-1], kind=plotType);
                elif plotType == "scatter":
                    df.plot(x = df.columns[int(xColumn)-1], y = df.columns[int(yColumn)-1], kind=plotType,s=5);
                plt.show()
                
            elif selection == "p3" or selection == "p3c":
                count = 1
                for c in df.columns:
                    print(str(count) + ". " + c)
                    count += 1
                    
                xColumn = input("\nX-axis: ")
                if xColumn == "exit":
                    break
                elif xColumn == "reset":
                    continue
                
                yColumn = input("\nY-axis: ")
                if yColumn == "exit":
                    break
                elif yColumn == "reset":
                    continue
                
                zColumn = input("\nZ-axis: ")
                if zColumn == "exit":
                    break
                elif zColumn == "reset":
                    continue 
                
                if selection == "p3c":
                    cColumn = input("\nColour Data: ")
                    if zColumn == "exit":
                        break
                    elif zColumn == "reset":
                        continue 
                    else:
                        fig = plt.figure(figsize=(12, 9),dpi=150)
                        ax = Axes3D(fig)
                        ax.scatter(df[df.columns[int(xColumn)-1]], df[df.columns[int(yColumn)-1]], df[df.columns[int(zColumn)-1]], s = 5, c = df[df.columns[int(cColumn)-1]], cmap='coolwarm')
                        ax.set_xlabel(df.columns[int(xColumn)-1])
                        ax.set_ylabel(df.columns[int(yColumn)-1])
                        ax.set_zlabel(df.columns[int(zColumn)-1])
                        plt.show()
                else:
                    fig = plt.figure(figsize=(12, 9),dpi=150)
                    ax = Axes3D(fig)
                    ax.scatter(df[df.columns[int(xColumn)-1]], df[df.columns[int(yColumn)-1]], df[df.columns[int(zColumn)-1]], s = 5)
                    ax.set_xlabel(df.columns[int(xColumn)-1])
                    ax.set_ylabel(df.columns[int(yColumn)-1])
                    ax.set_zlabel(df.columns[int(zColumn)-1])
                    plt.show()
                
                animationSelection = input("\nCreate Animation (y/n) [RESOURCE INTENSIVE]: ")
                if animationSelection == "exit":
                    break
                elif animationSelection == "reset":
                    continue  
                
                elif animationSelection == "y":
                    animationName = input("\nName to save animation under: ")
                    if animationName == "exit":
                        break
                    elif animationName == "reset":
                        continue  
                    
                    else:
                        rot_animation = animation.FuncAnimation(fig, rotate, frames=np.arange(0,364,4),interval=100)
                        rot_animation.save('animations/' + animationName + '.gif', dpi=80, writer='imagemagick')
                        
            elif selection == "nc":
                newColumnName = input("\nNew column name: ")
                if newColumnName == "exit":
                    break
                elif newColumnName == "reset":
                    continue
                
                for c in df.columns:
                    print(c)
                
                newColumnExpression = input("\nNew column expression: ")
                if newColumnExpression == "exit":
                    break
                
                elif newColumnExpression == "reset":
                    continue
                
                for cName in df.columns:
                    newColumnExpression = newColumnExpression.replace("["+cName+"]","df['"+cName+"']")
                
                newColumnExpression = newColumnExpression.replace("sqrt","np.sqrt")
                newColumnExpression = newColumnExpression.replace("atan","np.arctan")
                newColumnExpression = newColumnExpression.replace("asin","np.arcsin")
                
                try:
                    print()
                    print(newColumnExpression)
                    print(eval(newColumnExpression))
                    df[newColumnName] = eval(newColumnExpression)
                    print("\nNew column '" + newColumnName + "' created from expression: " + newColumnExpression)
                except:
                    print()
                    print(newColumnExpression)
                    print("\nGiven expression has an error and cannot be evaluated. Try again")
                    continue
            
            elif selection == "r":
                for c in df.columns:
                    print(c)
                renamePair = input("\nRename column by inputting oldName,newName.\noldName,newName = ")
                if renamePair == "exit":
                    break
                elif renamePair == "reset":
                    continue 
                
                else:
                    oldName = renamePair[:renamePair.index(",")]
                    newName = renamePair[renamePair.index(",")+1:]
                    df = df.rename(columns={oldName:newName})
                    
                    print("\n" + oldName + " renamed to " + newName)
                
            elif selection == "x":
                count = 1
                for c in df.columns:
                    print(str(count) + ". " + c)
                    count += 1
                    
                delColumn = input("\nColumn to remove: ")
                if delColumn == "exit":
                    break
                elif delColumn == "reset":
                    continue
                else:
                    delColumn = int(delColumn)
                    rusure = input("\nWARNING:This action is permanent. Delete '" + df.columns[delColumn-1] + "' column? (y/n): ")
                    if rusure == "y":
                        df.drop(df.columns[delColumn-1], inplace=True, axis=1)
            
            elif selection == "s":
                if df.iloc[-1][10] != "PreparedByOpenViewer":
                    newData = [float("NaN")]*len(df.iloc[-1])
                    newData[9] = "PreparedByOpenViewer"
                    df.loc[len(df)] = newData
                    
                df.to_csv(fileName, index = False)
                print("\n" + fileName + " updated!")
                
            elif selection == "sa":
                exportName = input("\nName to save new log under: ")
                if exportName == "exit":
                    break
                elif exportName == "reset":
                    continue
                
                if df.iloc[-1][10] != "PreparedByOpenViewer":
                    newData = [float("NaN")]*len(df.iloc[-1])
                    newData[10] = "PreparedByOpenViewer"
                    df.loc[len(df)] = newData
                
                exportName = exportName + ".txt"
                fileName = exportName
                df.to_csv(exportName, index=False)
                
            elif selection == "t":
                keepOrRemove = input("\nkeep ('k') or remove ('r'):") 
                if keepOrRemove == "exit":
                    break
                elif keepOrRemove == "reset":
                    continue
                
                count = 1
                for c in df.columns:
                    print(str(count) + ". " + c)
                    count += 1
                
                columnSelection = input("\nBasis column:")
                if columnSelection == "exit":
                    break
                elif columnSelection == "reset":
                    continue
                
                basisColumn = df[df.columns[int(columnSelection)-1]]
                print("\nBasis column has bounds (" + str(min(basisColumn)) + "," + str(max(basisColumn)) + ")")
                
                if keepOrRemove == "k":
                    uInterval = input("\nKeep datapoints having basis values in interval (x,y)\n(x,y): ")
                    if uInterval == "exit":
                        break
                    elif uInterval == "reset":
                        continue
                    
                    lower = float(uInterval[:uInterval.index(",")])
                    upper = float(uInterval[uInterval.index(",")+1:])
                
                    df = df[df[df.columns[int(columnSelection)-1]].between(lower,upper)]
                    
                if keepOrRemove == "r":
                    uInterval = input("\nRemove datapoints having basis values in interval (x,y)\n(x,y): ")
                    if uInterval == "exit":
                        break
                    elif uInterval == "reset":
                        continue
                    
                    lower = float(uInterval[:uInterval.index(",")])
                    upper = float(uInterval[uInterval.index(",")+1:])
                
                    df = df[~df[df.columns[int(columnSelection)-1]].between(lower,upper)]
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        