float mystate[12], otherState[12], pf[4][3], distance[3], last_vel[3], p1[3], m, d, p,q,y[3];                 //migliorare lo spostamento del punto
int stato, just_it;                                                                                   //                                                                              //                                                                                //codesizeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
bool attiva,cond, cond2;

void init(){
  
  api.getMyZRState(mystate);
  
  CanYouShowMeDaWaeeee();
  
  stato = just_it = 0;
  
  attiva=cond = cond2 = true;
  
  p=0.6;
  
  d=3.8;
  q=1;
}

void loop(){
  
  game.getOtherEulerState(otherState);
  
  float zred[3], yred[3];
  
  zred[0] = cos(otherState[6]) * sin(otherState[7]) * cos(otherState[8]) + sin(otherState[6]) * sin(otherState[8]);
  zred[1] = cos(otherState[6]) * sin(otherState[7]) * sin(otherState[8]) - sin(otherState[6]) * cos(otherState[8]);
  zred[2] = cos(otherState[6]) * cos(otherState[7]);
  
  yred[0] = cos(otherState[8]) * sin(otherState[7]) * sin(otherState[6]) - sin(otherState[8]) * cos(otherState[6]);
  yred[1] = sin(otherState[8]) * sin(otherState[7]) * sin(otherState[6]) + cos(otherState[8]) * cos(otherState[6]);
  yred[2] = cos(otherState[7]) * sin(otherState[6]);
  
  api.getMyZRState(mystate);
  api.getOtherZRState(otherState);
  
  mathVecSubtract(distance,otherState,mystate,3);
  float teta;
  teta = mathVecInner(&mystate[6],&otherState[6],3);
  DEBUG(("dist : %f", (mathVecMagnitude(distance,3) / (sqrt(2 - 2 * teta)) )));
  
  api.setPosGains(p,0.f,d);

    if(stato == 0) {                                    //DEBRIS

        pf[3][0] = pf[3][1] = pf[3][2] = 0;
        
        mathVecSubtract(p1,pf[3],mystate,3);
        mathVecSubtract(distance,pf[0],mystate,3);
        if(mathVecMagnitude(distance,3) < 0.1) {
            
            stato++;
                
            last_vel[0] = 0;                                                                                    //traslazione
            last_vel[1] = -1;                                                                                    
            last_vel[2] = 0;
            otherState[1] = 0;
            mathVecCross(pf[1],last_vel,otherState);   
            mathVecScalar(pf[1],((game.getThrusterHealth() < 50 ? 0.75 : 0.64) * mathVecMagnitude(p1,3)));   //0.2 * tan(angoglo di traslazione)
            mathVecAdd(pf[2],otherState,pf[1],3);
            mathVecScalar(pf[2],0.2);
            pf[2][1] = -0.5;                          //punto traslato nella circonferenza del rosso
            
            pf[2][0] = 0.2;
            pf[2][2] = 0.0;
            
            p1[0] = 0;
            p1[1] = -6.17;                             //punta del cono
            p1[2] = 0;
            
            mathVecSubtract(pf[0],pf[2],p1,3);          //puntamento rosso traslato
            mathVecScalar(pf[0],0.341);       //0.3
            mathVecAdd(p1,pf[2],pf[0],3);
            //p1[0] =  0.233;     //0.285
            //p1[1] = -0.163;    //-0.175
            //p1[2] = 0;

            p+=(((100-game.getThrusterHealth())*p)/100);
            d+=(((100-game.getThrusterHealth())*d)/100);
        }
        
        if(mathVecNormalize(distance,3) > 0.3) {   //0.15
            
            mathVecScalar(distance,0.2);
            api.setVelocityTarget(distance);
        }
        else api.setPositionTarget(pf[0]);
    }

    if(stato > 0 && game.getGamePhase() < 4) {                      //RENDEZVOUS & HOOKING

        
        if(stato > 1) {              //HOOKING
        
            if(!game.getHookCollision() && cond2) p1[0] -= 0.000275;
            else cond2 = false;
            
            if(cond) {
                
                /*api.getOtherZRState(otherState);
                mathVecScalar(&otherState[6],0.3387);   //0.318
                mathVecAdd(p1,otherState,&otherState[6],3);*/
                /*mathVecScalar(yred,-0.04);
                mathVecAdd(p1,pf[0],yred,3);*/
                /*p1[0] = otherState[0];
                p1[2] = otherState[2];*/
            }
            /*mathVecSubtract(distance,p1,mystate,3);
            if(mathVecMagnitude(distance,3) > 0.02 && cond) {  
                
                distance[0] = -0.256721;
                distance[1] = +0.206963;
                distance[2] = -1.621166 ;   
            
                game.setEulerTarget(distance);           
            }
            else {*/
                
                if(cond) {    
                    
                    p1[0] = otherState[0] + 0.02;
                    p1[2] = otherState[2];
                    
                    float Quat[4], myQuat[4];
                    
                    memcpy(last_vel,&mystate[6],3 * sizeof(float));
                    game.getMyEulerState(mystate);
                    mathVecScalar(&otherState[6],0.173);
                    mathVecAdd(pf[3],otherState,&otherState[6],3);
                    //pf[3][0] += 0.01;
                    mathVecSubtract(distance,pf[3],p1,3);
                    game.eulerToQuaternion(&mystate[6],myQuat);
                    api.attVec2Quat(last_vel,distance,myQuat,Quat);
                    game.quaternionToEuler(Quat,pf[0]);
                }
                cond = false;
                
                game.setEulerTarget(pf[0]); 
                
                /*game.getOtherEulerState(otherState);
        
                distance[0] =  otherState[6];
                distance[1] =  otherState[7];
                distance[2] =  otherState[8] + 3.14;
                
                game.setEulerTarget(distance);*/
            //}
        }
        else {         //RENDEZVOUS
        
            /*mathVecSubtract(distance,pf[2],p1,3);
            mathVecNormalize(distance,3);
            api.setAttitudeTarget(distance);*/
            
            distance[0] =  1.909890;
            distance[1] =  0.059614;
            distance[2] =  4.538269;
            /*
            game.getOtherEulerState(otherState);
        
                distance[0] = -otherState[6];
                distance[1] = -otherState[7];
                distance[2] =  otherState[8] + 3.14;*/
            
            game.setEulerTarget(distance);
        }
        
        game.getOtherEulerState(otherState);
            
        mathVecSubtract(distance,p1,mystate,3);
        m = mathVecNormalize(distance,3);
        
        mathVecScalar(distance,((0.05*(1-exp(-m/0.11)))));   //05
        
        if(m< 0.05) api.setPositionTarget(p1);
        else        api.setVelocityTarget(distance);
        
        if(stato == 1) {if(game.checkRendezvous()) {
            
            
            game.completeRendezvous(); 
            stato++;
        }}
    }
    
    else if(game.getGamePhase() == 4) {                       //towning
        y[0]=0;
        y[1]=0.1;
        y[2]=0;
        api.setAttRateTarget(y);
        p1[1]-=0.01;
            api.setPositionTarget(p1);
    }
}

void CanYouShowMeDaWaeeee() {
    
    float debris[41][4], distance_debris[3], v[3], p[3], best_distance, origine[3];
    int damage, best_damage;
    
    best_damage   = 300;
    best_distance = 300;
    
    for(int z = 0; z < 100; z++) {
        
        for(int x = 0; x < 100; x++) {
            
            game.getDebris(debris);
            damage = 0;
            
            p[0] = -0.5 + (((float)x) / 100);
            p[1] =  origine[0] = origine[1] = origine[2] = 0;
            p[2] = -0.5 + (((float)z) / 100);
            
            mathVecSubtract(v,p,mystate,3);
            
            for(int j = 0; j < 100; j++) {
                
                float pn[3];
                
                for(int s = 0; s < 3; s++) pn[s] = mystate[s] + (v[s] * (((float)j) / 100));
                
                for(int o = 0; o < 41; o++) {
                    
                    mathVecSubtract(distance_debris,debris[o],pn,3);
                    if(mathVecMagnitude(distance_debris,3) < (debris[o][3] + 0.12f) && debris[o][0] < 30) {
                        
                        debris[o][0] = 33;
                        if     (debris[o][3] < 0.04) damage += 10;
                        else if(debris[o][3] < 0.06) damage += 25;
                        else                         damage += 50;
                    }
                }
            }
            
            if(p[0] > -0.3 && p[0] < 0.3 && p[2] > -0.3 && p[2] < 0.3) damage-= 51;
            
            mathVecSubtract(distance_debris,p,mystate,3);
            
            //DEBUG(("%d/%d) %d - %f", x + 1, z + 1, damage, mathVecMagnitude(distance_debris,3)));
            
            if(damage < best_damage || (damage == best_damage && mathVecMagnitude(distance_debris,3) < best_distance)) {
                    
                memcpy(pf[0],p,3 * sizeof(float)); 
                best_damage = damage; best_distance = mathVecMagnitude(distance_debris,3);
                mathVecSubtract(distance_debris,pf[0],origine,3);
                mathVecScalar(distance_debris,0.02);
                for(int i=0;i<3;i++)    pf[0][i]+=distance_debris[i];
            }
        }
    }
}

void mathVecScalar(float vect[], float n) {
    
    mathVecNormalize(vect,3);
    for(int i = 0; i < 3; i++)
        vect[i] *= n;
}
