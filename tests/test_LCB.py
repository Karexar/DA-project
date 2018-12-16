import os

def main():
    
    print("Total list of processes:")
    total_procs = [int(x) for x in input().split()]
    #print("Enter the list of correct processes: ")
    #corrects = [int(x) for x in input().split()]
    #print(corrects)
    
    # Change the dependencies here if you change the processes' IDs
    affectations = [None] * 5
    affectations[0] = [6, 7]
    affectations[1] = [3]
    affectations[2] = [3, 4]
    affectations[3] = []
    affectations[4] = [5, 6]
    
    nb_proc = len(total_procs)
    
    logs = [None] * nb_proc
    
    # Initialize the list
    for i in range(nb_proc):
        logs[i] = [None] * 501
        
    
    for i in range(nb_proc):
        filename = '../da_proc_' + str(total_procs[i]) + '.out'
        
        if os.stat(filename).st_size == 0:
            print("File " + str(total_procs[i]) + " is empty.")
            logs[i] = 0
        else:
            f = open(filename, 'r')
        
            if f.mode == 'r':
                contents = f.read()
                # Set the ID of process and affectations
                logs[i][0] = [total_procs[i], [element for element in affectations[i]]]
                logs[i][1:] = contents.split("\n")
                
                
                for m in range(1, len(logs[i])):
                    logs[i][m] = logs[i][m].split(" ")
                    # print("Content of file " + str(i+1) + ":\n")
                    # print(logs[i][m])
        
    # Localized Causal Broadcast Test
    checkVC(nb_proc, logs)            

    
def checkVC(nb_proc, logs):
    
    noError = True
    deliverIDs = []
    
    # Iterate over all processes
    for i in range(nb_proc):
        
        current_proc = logs[i]
        
        # If the logs are not empty
        if current_proc != 0:
        
            current_proc_ID = current_proc[0][0]
            current_proc_affectations = current_proc[0][1]
            
            # Iterate over all the messages of the current process
            for mi in range(1, len(current_proc)):
                
                # Get the next message which is a 'deliver'
                if current_proc[mi][0] == 'd':
                    
                    # Store the sender's ID and msg's ID
                    senderID = int(current_proc[mi][1])
                    msgID = int(current_proc[mi][2])
                    
                    if int(senderID) in current_proc_affectations:
                    
                        # Iterate over all processes to find the sender one
                        for j in range(nb_proc):
                            
                            # Store the other process infos
                            other_proc = logs[j]
                            
                            if other_proc != 0:
                                other_process_ID = int(other_proc[0][0])
                                other_process_affectations = other_proc[0][1]
                                
                                if other_process_ID == senderID:
                                
                                    # Iterate over all 'broadcast' msgs in the other
                                    # process to find the corresponding msg
                                    for mj in range(1, len(logs[j])):
                                        other_msg = other_proc[mj]
                                        other_msg_ID = int(other_msg[1])
                                        
                                        # Find the corresponding broadcast msg
                                        if len(other_msg) != 0 \
                                        and other_msg[0] == 'b' \
                                        and other_msg_ID == msgID:
                                            
                                            deliverIDs.clear()
                                            
                                            # Iterate over all previous deliver messages
                                            for mk in range(1, mj):
                                                
                                                msg_to_check = other_proc[mk]
                                                
                                                # If the sender is in the list of affectations
                                                if msg_to_check[0] == 'd' \
                                                and int(msg_to_check[1]) in other_process_affectations:
                                                    deliverIDs.append([int(msg_to_check[1]), int(msg_to_check[2])])
                                                    #print("Adding element.")
                                            
                                            
                                            break
                                    
                                    break
                                
                    # Check that the deliver messages contained in the sender
                    # were delivered by the current process
                    for ml in range(1, mi):
                        
                        current_msg = current_proc[ml]
                        
                        if current_msg[0] == 'd':
                            deliverElement = [int(current_msg[1]), int(current_msg[2])]
                            
                            if deliverElement in deliverIDs:
                                deliverIDs.remove(deliverElement)
                                #print("Removing element.")
                             
                    
                    if len(deliverIDs) != 0:
                        print("Error of VC: Current process " + str(current_proc_ID)
                              + " did not deliver all messages delivered by Process "
                              + str(other_process_ID))
                        print(deliverIDs)
                        noError = False
                                    
    if noError:
        print("VC specification is correct !")     
    
    
if __name__ == '__main__':
    main()