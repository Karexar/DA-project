import os

def main():
    
    print("Total list of processes:")
    total_procs = [int(x) for x in input().split()]
    #print("Enter the list of correct processes: ")
    #corrects = [int(x) for x in input().split()]
    #print(corrects)
    
    # Set dependencies
    affectations = [None] * 5
    affectations[0] = [4, 5]
    affectations[1] = [1]
    affectations[2] = [1, 2]
    affectations[3] = []
    affectations[4] = [3, 4]
    
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
        current_proc_ID = current_proc[0][0]
        current_proc_affectations = current_proc[0][1]
        
        # Iterate over all the messages of the current process
        for mi in range(1, len(current_proc)):
            
            # Get the next message which is a 'deliver'
            if current_proc[mi][0] == 'd':
                
                # Store the sender's ID and msg's ID
                senderID = current_proc[mi][1]
                msgID = current_proc[mi][2]
                
                if senderID in current_proc_affectations:
                
                    # Iterate over all processes to find the sender one
                    for j in range(nb_proc):
                        
                        # Store the other process infos
                        other_proc = logs[j]
                        other_process_ID = other_proc[0][0]
                        other_process_affectations = other_proc[0][1]
                        
                        if other_process_ID == senderID:
                        
                            # Iterate over all 'broadcast' msgs in the other
                            # process to find the corresponding msg
                            for mj in range(1, len(logs[j])):
                                other_msg = other_proc[mj]
                                
                                # Find the corresponding broadcast msg
                                if len(other_msg) != 0 \
                                and other_msg[0] == 'b' \
                                and int(other_msg[1]) == int(msgID):
                                    
                                    deliverIDs.clear()
                                    
                                    # Iterate over all previous deliver messages
                                    for mk in range(1, mj):
                                        
                                        msg_to_check = other_proc[mk]
                                        
                                        # If the sender is in the list of affectations
                                        if msg_to_check[0] == 'd' \
                                        and int(msg_to_check[1]) in other_process_affectations:
                                            deliverIDs.append([int(msg_to_check[1]), int(msg_to_check[2])])
                                    
                                    
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
                         
                
                if len(deliverIDs) != 0:
                    print("Error of VC: Current process " + current_proc_ID
                          + " did not deliver all messages delivered by Process "
                          + other_process_ID)
                    noError = False
                                    
    if noError:
        print("VC specification is correct !")     
    
    
if __name__ == '__main__':
    main()