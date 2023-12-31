import boto3
import json

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('Node_MCU')

def lambda_handler(event, context):
    
    s = ["Low","Low","Low"]
    p = [0,0,0]
    D = ["distance1","distance2","distance3"]
    M = ["maxlength1","maxlength2","maxlength3"]
    
    for i in range(len(D)):
        if (int(event[D[i]])>int(event[M[i]])):
            s[i] = "Box open"
            p[i] = 0
        else:
            per = (int(event[M[i]]) - int(event[D[i]]))*100 // int(event[M[i]])
            p[i] = per 
            if(per<35): s[i] = "Low"
            elif(per<68): s[i] = "Medium"
            else: s[i] = "High"
    
    # per = (int(event[D[2]])//M[i]) *100
    # if(per<35): s[2] = "Low"
    # elif(per<68): s[2] = "Medium"
    # else: s[2] = "Medium"
    # p.append(per)
    
    for i in range(3):
        response = table.put_item(Item={'Container' : i+1, 'Percentage': p[i], 'State':s[i]})
    
    return 0;
