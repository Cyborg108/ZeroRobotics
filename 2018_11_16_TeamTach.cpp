	//Begin page debris
 
// I am ignoring codesize for now, because we have plenty of it
// you can tinker around with the parameters 
 
void calcBestPath(float MOTOR_PRECISION_ERROR_TOLERANCE = 0.014f, float DISTANCE_MULTIPLIER = 50.0f, float INCREMENT = 0.06f) //runs ONCE to calculate optimal path
{
     
    // finds path that minimizes f(path) = predicted_damage + distance_to_target_sphere*DISTANCE_MULTIPLIER
    // predicted damage in range [0,100]
     
     
    // point to vector distance formula used: http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    float vecOriginal[3];   // start to target point
    float vecPointToFin[3]; // center of debris to target point
    float vecPointToStart[3]; // center of debris to start point
    float crossProduct[3]; // cross product: pointToFin x pointToStart
     
    // iterate through all target points
    for (float x = -0.64f; x < 0.641f; x += INCREMENT) {
        for (float z = -0.64f; z < 0.641f; z += INCREMENT) {
            // blue initial position: 0, 0.75, 0
            // target position: x, 0, z
             
            vecOriginal[0] = x;
            vecOriginal[1] = 0-0.75f;
            vecOriginal[2] = z;
             
            float damage = 0;
            for (int i = 0; i < 41; i++) {
                vecPointToFin[0] = debris[i][0]-x;
                vecPointToFin[1] = debris[i][1];
                vecPointToFin[2] = debris[i][2]-z;
                 
                vecPointToStart[0] = debris[i][0];
                vecPointToStart[1] = debris[i][1]-0.75f;
                vecPointToStart[2] = debris[i][2];
                 
                mathVecCross(crossProduct, vecPointToFin, vecPointToStart);
                float distPointVec = mathVecMagnitude(crossProduct,3)/mathVecMagnitude(vecOriginal,3);
                 
                // if the distance between center of debris and line is too small
                if (distPointVec <= (debris[i][3]+0.11)+MOTOR_PRECISION_ERROR_TOLERANCE) {
                    if (debris[i][3] > 0.08) damage += 50;
                    else if (debris[i][3] > 0.04) damage += 25;
                    else damage += 10;
                }
            }
             
            // deprecated: assuming target sphere position (0,-0.5,0)
            // assuming target sphere position (0.12, -0.5, 0)
            float distToTargetSphereSq = (x-0.12)*(x-0.12) + z*z + 0.25;
             
            if (damage + distToTargetSphereSq*DISTANCE_MULTIPLIER < dist) {
                dist = damage + distToTargetSphereSq*DISTANCE_MULTIPLIER;
                best[0] = x; best[1] = 0; best[2] = z;
                 
                DEBUG(("Going to point (%f,0,%f) predicted damage: %f with extra sq-distance of %f", x, z, damage, distToTargetSphereSq));
            }
        }
    }
}
//End page debris
//Begin page hookem
void hook() 
{
    hookct+=1;
    if(hookct<=8)
    {
        float pi = 3.1415926535;
        float attTarget[3];
        float otherAng = acos(otherSphState[9]);
        for(int i=0; i<3; i++)
        {
            attTarget[i] = otherState[i+6]*(float)-1;
        }
        attTarget[2] +=0.2;
        DEBUG(("1 %f", (otherAng/pi)*180.0f));
        otherAng*=2.0f;
        if(otherState[2]<0.0f)
        {
            otherAng*=-1.0f;
            otherAng+=pi;
        }
        DEBUG(("2 %f", (otherAng/pi)*180.0f));
        rTo(attTarget, otherAng);
        float target[3];
        for(int i=0; i<3; i++)
        {
            target[i] = otherState[i]+ 0.32f*otherState[i+6];
        }
        moveFaster(target, 0.8f);
    }
    else
    {
        float pi = 3.1415926535;
        float attTarget[3];
        float otherAng = acos(otherSphState[9]);
        for(int i=0; i<3; i++)
        {
            attTarget[i] = otherState[i+6]*(float)-1;
        }
        DEBUG(("1 %f", (otherAng/pi)*180.0f));
        otherAng*=2.0f;
        if(otherState[2]<0.0f)
        {
            otherAng*=-1.0f;
            otherAng+=pi;
        }
        DEBUG(("2 %f", (otherAng/pi)*180.0f));
        rTo(attTarget, otherAng);
        float target[3];
        for(int i=0; i<3; i++)
        {
            target[i] = otherState[i]+ 0.34f*otherState[i+6];
        }
        moveFaster(target, 0.8f);
    }
}
void tug() 
{
    float pi = 3.1415926535;
    float attTarget[3];
    float otherAng = acos(otherSphState[9]);
    for(int i=0; i<3; i++)
    {
        attTarget[i] = otherState[i+6]*(float)-1;
    }
    otherAng*=2.0f;
    if(otherState[2]<0.0f)
    {
        otherAng*=-1.0f;
        otherAng+=pi;
    }
    rTo(attTarget, otherAng);
    float hold[3] = {0, 0.01, -0.005};
    api.setVelocityTarget(hold);
}
 
//End page hookem
//Begin page main
//Declare any variables shared between functions here
float myState[12];
float otherState[12];
float otherSphState[13];
float mySphState[13];
float debris[41][4];
float best[3], dist;
float tugMem[4];
int size;
int hookct;
 
/*
state[0][1][2] = pos X, pos Y, pos Z [m]
state[3][4][5] = vel X, vel Y, vel Z [m/s]
state[6][7][8] = roll, pitch, yaw [rad]
state[9][10][11] = rate X, rate Y, rate Z [rad/s]
*/
 
int mainState; 
 
int prendtime; // predicted rendezvous time
 
void init(){
    hookct=0;
    mainState = 0;
    game.getDebris(debris);
    for (int i = 0; i < 41; i++) {
        DEBUG(("Debris %d: (%f, %f, %f) with radius %f", i, debris[i][0], debris[i][1], debris[i][2], debris[i][3]));
    }
 
    size = 0;
    dist = 1000; // need for calc best path
    calcBestPath();
}
 
void loop(){
    api.getOtherSphState(otherSphState);
    api.getMySphState(mySphState);
    api.getMyZRState(myState);
    api.getOtherZRState(otherState);
     
 
    if (mainState==0) {
         
        DEBUG(("BEST: %f %f %f", best[0], best[1], best[2]));
        moveFaster(best);
         
        if (myState[1] < 0.08) {
            mainState = 1;
             
            float targetPos[3] = {otherState[0], otherState[1], otherState[2]};
            prendtime = predict(otherState)+api.getTime();
        }
    }
    if(mainState==1)
    {
        //prendtime = 65;
        DEBUG(("Time: %d", prendtime));
        rendezvous(prendtime);
         
        if (game.getRendezvous() == true)
            mainState = 2;
    }
    if(mainState==2)
    {
        hook();
        if(game.getGamePhase()==4)
        {
            mainState = 3;
            for(int i=0; i<4; i++)
            {
                tugMem[i] = mySphState[i+6];
            }
        }
    }
    if(mainState ==3)
    {
        tug();
    }
     
}
 
//End page main
//Begin page move
float getMag(float *vel){
    return sqrt(powf(vel[0],2)+powf(vel[1],2)+powf(vel[2],2));
}
void multVec(float *vec, float mult) {
    vec[0] *= mult;
    vec[1] *= mult;
    vec[2] *= mult;
}
 
// Fastest move function to get where you're going fast
void moveFaster(float *whereTo, float mult=1.5f) {
    float v[3]; //float mult;
    mathVecSubtract(v, whereTo, myState, 3.0f);
     
    //mult = (1.0+1.0*(getMag(v)/0.6));
    multVec(v, mult);
    mathVecAdd(v, myState, v, 3);
    api.setPositionTarget(v);
}
 
//quaternion rotation, allows rotation around attitude vector. Treat as a setTargetAttitude w/extra angle parameter
 
//End page move
//Begin page rendevous
//place code for rendevousing here
 
void rendezvous(int ptime) {
 
    float angle = (ptime%162)/162.0f*2.0f*acosf(-1.0);           // predict theta, angle btwn vec (0,0,1)
     
    DEBUG(("rend-- angle %f sin %f cos %f", angle, sinf(angle), cosf(angle)));
    float pos[3] = {0.2647f*sinf(angle),-0.183,-0.2647f*cosf(angle)}; // predict where target will be, fine tuned radius and y
     
    float att[3] = {-otherState[6], -otherState[7], -otherState[8]};
    api.setPositionTarget(pos);
    rTo(att, angle);
     
    game.completeRendezvous();
}
void rTo(float *vec, float ang)
{
    float answer[4];
    float mV[3] = {1, 0, 0};
    ang*=0.5;
    float mB[4] = {sinf(ang), 0, 0, cosf(ang)};
    api.attVec2Quat(mV, vec, mB, answer);
    api.setQuatTarget(answer);
}
 
float predict(float* target) {
    float v[3];
     
    mathVecSubtract(v, target, myState, 3);
    float dist = mathVecMagnitude(v, 3);
    int cur = game.getThrusterHealth();
     
    return (dist/0.037f) + (100.0f/cur)*8.5f;
}
 
/* added into doRendezvous func
void getPosition(float *pos, int ptime) {
    float angle = (ptime%162)/162.0f*2.0f*acosf(-1.0);
    pos[1] = -0.23;
    pos[0] = 0.26f*sinf(angle);
    pos[2] = 0.26f*cosf(angle);
}*/
 
 
//End page rendevous
