const int outputPin1 = 6; // red led
const int outputPin2 = 7; //green Led
const int motorPin = 5;
const int knockSensor = A0;
const int switchSensor = A4; 
const int knockThreashold = 20;
const int matchThreashold = 150;
const int switchThreashold = 100;
const int maxKnockInterval = 3;
const int timeLimitToReset = 10000;
unsigned long firstKnockTime = 0, secondKnockTime = 0,fKTime = 0, sKTime = 0;
int count =0,countTakingKnocks =0 ;
unsigned long knockTimeStored[] = {0,0,0};
unsigned long currentKnockTime[] = {0,0,0};
unsigned long lastInputTime = 0;
unsigned long currentTime = 0;
int resetPrintCount = 0;
unsigned long storeMultipleKnock[3][4] = {{0,0,0,1},{0,0,0,2},{0,0,0,3}};


void setup() {
  //Set the output pin as an OUTPUT
  pinMode(outputPin1, OUTPUT);
  pinMode(outputPin2, OUTPUT);
  pinMode(motorPin, OUTPUT);
  //Begin Serial Communication.
  Serial.begin(9600);
 
}


bool noInputForLong()
{
  currentTime = millis();
  if( (currentTime -lastInputTime)>timeLimitToReset)
  {
    return true;
  }
   
  else 
  {
    resetPrintCount =0;
    return false; 
  } 
}

 
void loop() {
  
    int switchReading = analogRead(switchSensor);
    
    if(switchReading==0){//if switch is pressed new knock's time interval will be stored in the knockTimeStored array
        lastInputTime = millis();
        storingKnockInTemporaryStorage();
    }
    else if(switchReading>switchThreashold) {
      digitalWrite(outputPin2, LOW);
      digitalWrite(outputPin1, LOW); 
      count =0;
      firstKnockTime =0;
      secondKnockTime=0;
      takingKnocks(); 
        
    }

    bool sleepCheck = noInputForLong();
    if(sleepCheck==true){
        //Serial.print("Current time = ");
        //Serial.println(currentTime);
        //Serial.print("Last input time = ");
        //Serial.println(lastInputTime);
        if(resetPrintCount==0)
        {
          Serial.println("Knock is reset please try again...");
          resetPrintCount++;
        }
        fKTime = 0;
        sKTime = 0;
        countTakingKnocks =0;
        digitalWrite(outputPin1, LOW);
        digitalWrite(outputPin2, LOW);
    }

}



void storing(int row)
{
    int i;
    for(i=0;i<maxKnockInterval;i++)
    {
        storeMultipleKnock[row][i] = knockTimeStored[i];
    }

    for(i=0;i<maxKnockInterval;i++)
    {
        if(i!=row)
            --storeMultipleKnock[i][3];
    }
    storeMultipleKnock[row][3] =3;

}


void storingKnockInTemporaryStorage()
{
    if(count>=maxKnockInterval){   //after taking 4 knocks, storing time interval becomes terminated untill switch is pressed further
              digitalWrite(outputPin2, HIGH);
              Serial.println("Knock is successfully stored!");
              //after storing in temporary storage now it will be stored in storeMultipleKnock array;
              int i;
              for(i=0;i<maxKnockInterval;i++){
                  if(storeMultipleKnock[i][3]==1)  // knock will be stored in that row where the priority is high in storeMultipleKnock array;
                  {
                    Serial.print("stored in : ");
                    Serial.println(i);
                      storing(i);
                      break;
                  }
              }
              delay(2000);
    }
    else if(count<maxKnockInterval){
        digitalWrite(outputPin2, LOW);
        int  sensorReading = analogRead(knockSensor);  
        if(sensorReading>=knockThreashold){
            //Serial.println(sensorReading);
            
            if(firstKnockTime==0){
                firstKnockTime = millis();
                Serial.println(firstKnockTime);
                
            }
            else if(secondKnockTime==0){
                          secondKnockTime = millis();
                          Serial.println(secondKnockTime);
                         // digitalWrite(outputPin1, HIGH);
                         // delay(200);
                        //  digitalWrite(outputPin1, LOW); 
                          unsigned long temp = secondKnockTime - firstKnockTime;
                          if(temp>0){
                              knockTimeStored[count]= temp;
                              Serial.print("Knock intervale : ");
                              Serial.print(count);
                              Serial.print("  =  ");
                              Serial.println(temp);
                              firstKnockTime = secondKnockTime;
                              secondKnockTime = 0;
                              count++; 
                          }
                          //if any interval gives negative time, whole knock storing process will be started from the begining
                          else {
                            Serial.println("Invalid knock");
                              firstKnockTime = 0;
                              secondKnockTime = 0;
                              count =0;
                          }
                          
            }
          delay(250);
        } 
    }
}



void takingKnocks()
{
   if(countTakingKnocks>=maxKnockInterval){   //after taking 4 knocks, checking time interval whether is it matched with the stored time or not
            //digitalWrite(outputPin1, HIGH);
            Serial.println("Knock is listened! wait to check");
            check();
            lastInputTime = millis();
           // delay(3000);
   }
        
  else{
    digitalWrite(outputPin1, LOW);
      int  sensorReading = analogRead(knockSensor);  
      if(sensorReading>=knockThreashold){
          lastInputTime = millis();
          //Serial.println(sensorReading);
          
          if(fKTime==0){
              fKTime = millis();
              Serial.println(fKTime);
              
          }
          else if(sKTime==0){
                        sKTime = millis();
                        Serial.println(sKTime);
                       // digitalWrite(outputPin1, HIGH);
                       // delay(200);
                      //  digitalWrite(outputPin1, LOW); 
                        unsigned long temp = sKTime - fKTime;
                        if(temp>0){
                            currentKnockTime[countTakingKnocks]= temp;
                            Serial.print("Knock intervale : ");
                            Serial.print(countTakingKnocks);
                            Serial.print("  =  ");
                            Serial.println(temp);
                            fKTime = sKTime;
                            sKTime = 0;
                            countTakingKnocks++; 
                        }
                        //if any interval gives negative time, whole knock storing process will be started from the begining
                        else {
                          Serial.println("Invalid knock");
                            fKTime = 0;
                            sKTime = 0;
                            countTakingKnocks =0;
                        } 
                                              
           }
        delay(250);
      } 
  }
}

void check()
{
    int i,row;
    int temp;
    bool match = false;

    for(row=0;row<3;row++){
        for(i=0;i<maxKnockInterval;i++){
           // printf("currentKnockTime = %d\tstoreMultipleKnock=%d\t",currentKnockTime[i],storeMultipleKnock[row][i]);
            if(currentKnockTime[i]>storeMultipleKnock[row][i])
            {

              temp = (currentKnockTime[i] - storeMultipleKnock[row][i]);
            }
            else
            {
                temp = (storeMultipleKnock[row][i] - currentKnockTime[i]);
            }
            /*
            Serial.print("Difference of ");
            Serial.print(i);
            Serial.print("th interval : ");
            Serial.println(temp);
            */

          //  printf("temp = %d\n",temp);
            if(temp>matchThreashold){
              match = false;
              break;
            }
            else
                match = true;
        }

        if(match)
        {
           // printf("matched for %d",row);
            break;
        }


    }

    if(match){
      digitalWrite(outputPin2, HIGH);
      Serial.print("Knock is matched with ");
      Serial.println(row);
       // delay(2000);
      runMotor();
      offMotor();
    }
   else {
      digitalWrite(outputPin1, HIGH);
      Serial.println("Knock is not matched please try again");
      delay(2000);
   }
  
    fKTime = 0;
    sKTime = 0;
    countTakingKnocks =0;
    digitalWrite(outputPin1, LOW);
    digitalWrite(outputPin2, LOW);
    
}

void runMotor()
{   Serial.println("MOTOR IS ON NOW");
    digitalWrite(motorPin,HIGH);
    delay(400);
}

void offMotor()
{   Serial.println("MOTOR IS OFF NOW");
    digitalWrite(motorPin,LOW);
}

