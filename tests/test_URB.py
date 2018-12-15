import os
import sys

def main():
    
    nb_proc = int(input("How many processes do you have?"))
    print("Enter the list of correct processes: ")
    corrects = [int(x) for x in input().split()]
    print(corrects)
    
    logs = [None] * nb_proc
    
    for i in range(nb_proc):
        filename = '../da_proc_' + str(i+1) + '.out'
        
        if os.stat(filename).st_size == 0:
            print("File " + str(i+1) + " is empty.")
            logs[i] = 0
        else:
            f = open(filename, 'r')
        
            if f.mode == 'r':
                contents = f.read()
                logs[i] = contents.split("\n")
                
                for m in range(len(logs[i])):
                    logs[i][m] = logs[i][m].split(" ")
                    # print("Content of file " + str(i+1) + ":\n")
                    # print(logs[i][m])
        
    # Uniform Reliable Broadcast Test
    # Check all specification of URB
    checkValidity(nb_proc, logs, corrects)              # URB1
    checkNoDuplication(nb_proc, logs)                   # URB2
    checkNoCreation(nb_proc, logs)                      # URB3
    checkUniformAgreement(nb_proc, logs, corrects)      # URB4
    

    
    
    # 4) Uniform Agreement: For any message m, if a process delivers m,
    # then every correct process delivers m


# 1) Validity: If pi and pj are correct, then every message broadcast by pi
# is eventually delivered by pj
def checkValidity(nb_proc, logs, corrects):
    
    noError = True
    
    # Iterate over all processes
    for i in range(nb_proc):
        
        current_proc = logs[i]
        
        # If the process is correct, check the validity spec for all messages
        if (i+1) in corrects:
            
            # Iterate over all the messages of the current process
            for mi in range(len(current_proc)):
                
                # Get the next message which is a 'broadcast'
                if current_proc[mi][0] == 'b':
                    current_msg = current_proc[mi]
                    
                    # Compare with the other processes
                    for j in range(nb_proc):
                        if j != i and logs[j] != 0:
                            
                            # Reset the boolean check
                            isDelivered = False
                            
                            # Get the next process to be checked
                            other_proc = logs[j]
                            
                            # Iterate over all 'delivered' messages
                            # in the other process
                            for mj in range(len(logs[j])):
                                
                                msg_to_check = other_proc[mj]
                                
                                if len(msg_to_check) != 0 and msg_to_check[0] == 'd':
                                    
                                    # Check if the delivery corresponds to the
                                    # broadcast
                                    if int(msg_to_check[1])==(i+1) and int(msg_to_check[2])==int(current_msg[1]):
                                        isDelivered = True
                                        break
                                    
                            if not isDelivered:
                                print("Error of Validity: Process " + str(j+1)
                                + " has failed to deliver message " + str(mi+1)
                                + " from Process " + str(i+1))
                                noError = False
                                #sys.exit()
    
    if noError:
        print("Validity specification is correct !")
    
    
# 2) No duplication: No message is delivered more than once
# Simply check that all 'deliver' message are unique
def checkNoDuplication(nb_proc, logs):
    
    noError = True
    
    # Iterate over all processes
    for i in range(nb_proc):
        
        current_proc = logs[i]
        
        if current_proc != 0:
            msg_list = [None] * len(current_proc)
        
            # Iterate over all the messages of the current process
            for mi in range(len(current_proc)):
                msg = '-'.join(current_proc[mi])
                
                # Check if the message was duplicated
                if(msg in msg_list):
                    print("Error of duplication: Message " + str(mi+1)
                    + "has been duplicated")
                    break
                    noError = False
                
                msg_list[mi] = msg
    
    if noError:
        print("No Duplication specification is correct !")
        

# 3) No creation: No message is delivered unless it was broadcast
def checkNoCreation(nb_proc, logs):
    
    noError = True
    
    # Iterate over all processes
    for i in range(nb_proc):
        
        current_proc = logs[i]
        
        # If the list of messages is not empty,
        # check the no creation spec for all messages
        if current_proc != 0:
            
            # Iterate over all the messages of the current process
            for mi in range(len(current_proc)):
                
                # Get the next message which is a 'delivery'
                if current_proc[mi][0] == 'd':
                    current_msg = current_proc[mi]
                    
                    # Compare with the other processes
                    for j in range(nb_proc):
                        
                        # Find the process that is supposed to have broadcasted
                        if j != i and logs[j] != 0 and int(current_msg[1]) == (j+1):
                            
                            # Reset the boolean check
                            isBroadcast = False
                            
                            # Get the process to be checked
                            other_proc = logs[j]
                            
                            # Iterate over all 'broadcast' messages
                            # in this process
                            for mj in range(len(logs[j])):
                                
                                msg_to_check = other_proc[mj]
                                
                                if len(msg_to_check) != 0 and msg_to_check[0] == 'b':
                                    
                                    # Check if the delivery corresponds to the
                                    # broadcast
                                    if int(msg_to_check[1])==int(current_msg[2]):
                                        isBroadcast = True
                                        break
                                    
                            if not isBroadcast:
                                print("Error of Creation: Process " + str(j+1)
                                + " has not broadcast message " + str(mi+1)
                                + " to Process " + str(i+1))
                                noError = False
                                #sys.exit()
    
    if noError:
        print("No Creation specification is correct !")
        
# 4) Uniform Agreement: For any message m, if a process delivers m,
# then every correct process delivers m
def checkUniformAgreement(nb_proc, logs, corrects):
    
    noError = True
    
    # Iterate over all processes
    for i in range(nb_proc):
        
        current_proc = logs[i]
        
        if current_proc != 0:
            # Iterate over all the messages of the current process
            for mi in range(len(current_proc)):
                
                # Get the next message which is a 'deliver'
                if current_proc[mi][0] == 'd':
                    current_msg = current_proc[mi]
                    
                    # Compare with the other processes
                    for j in range(nb_proc):
                        
                        if j != i and (j+1) in corrects and int(current_msg[1]) != (j+1):
                            
                            # Reset the boolean check
                            isDelivered = False
                            
                            # Get the next process to be checked
                            other_proc = logs[j]
                            
                            # Iterate over all 'delivered' messages
                            # in the other process
                            for mj in range(len(logs[j])):
                                
                                msg_to_check = other_proc[mj]
                                
                                if msg_to_check[0] == 'd' and len(msg_to_check) != 0:
                                    
                                    # Check if the delivery corresponds to the
                                    # first delivery
                                    if int(current_msg[1])==int(msg_to_check[1]) \
                                    and int(current_msg[2])==int(msg_to_check[2]):
                                        isDelivered = True
                                        break
                                    
                            if not isDelivered:
                                print("Error of Uniform Agreement: Process "
                                      + str(j+1)
                                      + " has failed to deliver message "
                                      + str(current_msg))
                                noError = False
                                #sys.exit()
                            
    if noError:
        print("Uniform Agreement specification is correct !")
    
        
if __name__ == '__main__':
    main()