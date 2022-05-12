import numpy as np
import falconn
import time
import random

#Instructions:
#1. pip install falconn
#2. Delete first line of data to remove column names
#3. Change path to path of file


path = "/Users/patrick/Desktop/Patrick/AdvancedAlgorithms/project/NoFirstLine.txt"
caseid,dAge,dAncstry1,dAncstry2,iAvail,iCitizen,iClass,dDepart,iDisabl1,iDisabl2,iEnglish,iFeb55,iFertil,dHispanic,dHour89,dHours,iImmigr,dIncome1,dIncome2,dIncome3,dIncome4,dIncome5,dIncome6,dIncome7,dIncome8,dIndustry,iKorean,iLang1,iLooking,iMarital,iMay75880,iMeans,iMilitary,iMobility,iMobillim,dOccup,iOthrserv,iPerscare,dPOB,dPoverty,dPwgt1,iRagechld,dRearning,iRelat1,iRelat2,iRemplpar,iRiders,iRlabor,iRownchld,dRpincome,iRPOB,iRrelchld,iRspouse,iRvetserv,iSchool,iSept80,iSex,iSubfam1,iSubfam2,iTmpabsnt,dTravtime,iVietnam,dWeek89,iWork89,iWorklwk,iWWII,iYearsch,iYearwrk,dYrsserv = np.loadtxt(path, delimiter =',', usecols =tuple(range(69)), unpack = True)

anc1 = np.array([caseid,dAge,dAncstry1,dAncstry2,iAvail,iCitizen,iClass,dDepart,iDisabl1,iDisabl2],dtype=np.float32)
n_dim = len(anc1)
anc = np.transpose(anc1)

num_trials = 100


lsh = falconn.LSHIndex(falconn.get_default_parameters(len(dAncstry1),n_dim))#num_points: int, dimension: int
lsh._params.storage_hash_table = falconn.StorageHashTable.LinearProbingHashTable
before = time.time()
lsh.setup(anc)
after = time.time()
print("Setup Time: " + str(after-before))

lsh_q = lsh.construct_query_object()
total = 0
for i in range(num_trials):
    q = []
    for c in anc1:
        pos = random.randint(0,len(c)-1)
        q.append(c[pos])
    q = np.array(q,dtype=np.float32)
    before = time.time()
    result = lsh_q.find_nearest_neighbor(q)
    after = time.time()
    total+=after-before

print("Average CrossPolytope Query Time: " + str(total/num_trials))


lsh_q = lsh.construct_query_object()
total = 0
for i in range(num_trials):
    q = []
    for c in anc1:
        pos = random.randint(0,len(c)-1)
        q.append(c[pos])
    q = np.array(q,dtype=np.float32)
    before = time.time()
    result = lsh_q.find_nearest_neighbor(q)
    after = time.time()
    total+=after-before
print("Average Hyperplane Query Time: " + str(total/num_trials))

# for i in result:
#     print(anc[i])
#print("found " + str(anc[result]) + " at row " + str(result))


