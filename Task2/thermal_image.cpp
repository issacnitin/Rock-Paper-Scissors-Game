#include <bits/stdc++.h>

using namespace std;



/*
Returns maximum length by which rotor can be divided.
*/
int getUnit(vector<vector<int> > a, int quadrant, int rotate) {
    int i = a.size()/2-1;
    int j = a[0].size()/2;
    int count = 0;

    //Based on quadrant and rotation direction, we calculate the number of 1s in one row, or in one column, whichever is relevant
    if(quadrant == 1 && rotate == 2) {
        while( j < a[0].size() && a[i][j] == 1) {
            count++;
            j++;
        }
        return count;
    } else if(quadrant == 2 && rotate == 1) {
        i = a.size()/2-1;
        j = a[0].size()/2 - 1;
        while( i >= 0 && a[i][j] == 1) {
            count++;
            i--;
        }
        return count;
    }

    while( i >= 0 && a[i][j] == 1) {
        count++;
        i--;
    }
    if(count == 0) {
        i = a.size()/2-1;
        j = a[0].size()/2 - 1;
        while( j >= 0 && a[i][j] == 1) {
            count++;
            j--;
        }
    }
    return count;
}

/*
Returns the symmetrically mirrored version of arr
Mirrors the position of rotor in first quadrant to 3rd and second quadrant to 4th
*/
vector<vector<int> > mirrored(vector<vector<int> > arr) {

    int n = arr.size();
    int m = arr[0].size();
    vector<vector<int> > b(arr);

    vector<int> t;
    for(int i = 0; i < n/2; i++) {
        t = b[n/2-1-i];
        b[n/2-i-1] = b[n/2 + i];
        b[n/2 + i] = t;
    }
    int temp = 0;

    //Mirroring for symmetrically opposite quadrant
    for(int i = 0; i < m/2 - 1; i++) {
        for(int j = 0; j < n; j++) {
            temp = b[j][m/2-i-1];
            b[j][m/2-i-1] = b[j][m/2+i];
            b[j][m/2+i] = temp;
        }
    }
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            arr[i][j] += b[i][j];
        }
    }
    return arr;
}

/*
Returns the next position of rotor, drawn in the array. 1 represent rotor is present in that coordinate, 0 represent rotor not present in that coordinate
*/
vector<vector<int> > generateNextPattern(vector<vector<int> > a) {
    vector<vector<int> > b(a.size(), vector<int>(a[0].size()));

    //Here we generate the next position of rotor
    int quadrant = 0;
    int rotate = 0;

    //From the current rotor position, we identify the quadrant and rotation direction
    if(a[a.size()/2 - 1][a[0].size()/2] == 1) {
        quadrant = 1;
        if(a[a.size()/2 - 2][a[0].size()/2] == 1)
            rotate = 1;
        else
            rotate = 2;
    } else {
        quadrant = 2;
        if(a[a.size()/2 - 1][a[0].size()/2 - 2] == 1)
            rotate = 2;
        else
            rotate = 1;
    }
    int push = 0;
    //Unit is the number of 1s in sequence (1,1,1,1)=>4 or      (1,1,0,0)
    //                                                          (0,0,1,1)=>2 along horizontal or vertical positions of rotor blade as represented in array(4 in first case, 2 in second case, 1 in 3rd case, 2 in 4th case again)
    int unit = getUnit(a, quadrant, rotate);
    int newUnit;

    //Based on quadrant of rotor heads and rotation direction, we calculate the next position of one rotor head(out of two)
    if(quadrant == 1 && rotate == 1) {
        /*
        Rotor in first quadrant
        Rotation direction as spreading along X axis
        */
        cout<<"Called"<<endl;
        for(int i = 0; i < b.size(); i++) {
            for(int j = 0; j < b[0].size(); j++)
                cout<<b[i][j]<<" ";
            cout<<endl;
        }
        newUnit = unit/2;
        for(int i = a.size()/2 - 1; i >= 0; i-= newUnit) {
            for(int j = 0; j < newUnit; j++) {
                //X remains constant, Y changing
                b[i-j][a[0].size()/2 + push] = 1;
            }
            push++;
        }
    } else if(quadrant == 1 && rotate == 2) {
        /*
        First quadrant, rotation direction as compressing along Y axis
        */
        newUnit = 2*unit==a.size()?unit:2*unit;
        while(push*newUnit < (a[0].size()-2)/2) {
            for(int i = a[0].size()/2 + push*newUnit; i < a[0].size()/2 + (push+1)*newUnit; i++) {
                b[a.size()/2-push-1][i] = 1;
            }
            push++;
        }
    } else if(quadrant == 2 && rotate == 1) {

        /*
        Second quadrant, rotation direction as spreading along Y axis
        */
        newUnit = 2*unit==a.size()?unit:2*unit;
        for(int i = a.size()/2 - 1; i >= 0; i-= newUnit) {
            for(int j = 0; j < newUnit; j++) {
                b[i-j][a[0].size()/2 - push-1] = 1;
            }
            push++;
        }
    } else if(quadrant == 2 && rotate == 2) {
        /*
        Second quadrant, rotation direction as compressing along X axis
        */
        newUnit = unit/2;
        while(push*newUnit < (a[0].size()-2)/2) {
            for(int i = a[0].size()/2 - push*newUnit - 1; i > a[0].size()/2 - (push+1)*newUnit-1; i--) {
                b[a.size()/2-push-1][i] = 1;
            }
            push++;
        }
    }

    //We finally mirror it for the symmetrically opposite rotor head
    return mirrored(b);
}

bool checkforRotor(vector<vector<int> > a, vector<vector<int> > templ) {
    //We check if array representing rotor positions overlap with array representing thermal mapping, if hit is found with 87.5% accuracy we've got the rotor's position
    int n = a.size();
    int m = a[0].size();
    int count = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(a[i][j] == templ[i][j] && a[i][j] == 1) {
                count++;
            }
        }
    }
    if(count < a.size()-1)
        return false;

    return true;
}

/*
Returns array of images with rotor removed
*/
vector<vector<int> > removeRotator(vector<vector<int> > a) {
    //How to Remove the Rotor 101
    //We replicate the positions rotor might be in, and match with the thermal mapping. If a match is found(And only one match will be found), we weed out the rotor too
    vector<vector<int> > temp, temp2;
    int n = a.size();
    int m = a[0].size();

    //We start the rotor position at parallel to X axis
    int arr[n][m] = { 
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
                    {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0},
                    {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0} };
    for(int i = 0; i < n; i++) {
        vector<int> t;
        for(int j = 0; j < m; j++) {
            t.push_back(arr[i][j]);
        }
        temp2.push_back(t);
    }
    //We check if the position of rotor is matched in the thermal mapping
    bool flag = checkforRotor(a, temp2);
    int i =0; 
    do {
        flag = checkforRotor(a, temp2);
        //If not, we generate the next position of rotor
        temp2 = generateNextPattern(temp2);   
        i++;
    } while(i <= 4 && !flag); //4 Positions of Rotor is possible with rotor heads in 2nd and 4th quadrants, since it's a 8x8 thermal mapping

    //We remove the rotor from thermal mapping 
    if(flag) {
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
                if(a[i][j] == temp2[i][j] && a[i][j] == 1)
                    a[i][j] = 0;
            }
        }
    }

    //We start with vertical positions, rotor heads being in 1st and 3rd quadrant
    int arr2[n][m] = { 
                    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
                    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                    {0 ,0, 0, 0, 1, 0, 0, 0, 0, 0},
                    {0 ,0, 0, 0, 1, 0, 0, 0, 0, 0},
                    {0 ,0, 0, 0, 1, 0, 0, 0, 0, 0} }; 
    for(int i = 0; i < n; i++) {
        vector<int> t;
        for(int j = 0; j < m; j++) {
            t.push_back(arr2[i][j]);
        }
        temp.push_back(t);
    }
    i =0; 
    //Repeat for the above 
    bool flag2 = checkforRotor(a, temp);
    
    while(i <= 4 && !flag2) {
        temp = generateNextPattern(temp);
        flag2 = checkforRotor(a, temp);
        i++;
    }
    if(flag2) {
        //cout<<"Found"<<endl;
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < m; j++) {
        //        cout<<temp[i][j]<<" ";
                if(a[i][j] == temp[i][j] && a[i][j] == 1)
                    a[i][j] = 0;
            }
        //    cout<<endl;
        }
    }

    //Return the food-only thermal mapping
    return a;
}

int main() {
    vector<vector<int> > a;
    int n = 8, m = 10, t = 0, avg = 0;
    for(int i = 0; i < n; i++) {
        vector<int> aa;
        for(int j = 0; j < m; j++) {
            cin>>t;
            avg += t;
            aa.push_back(t);
        }
        a.push_back(aa);
    }
    //First, we calculate average temperature of the pan(Average is used for this testcase, without more data it is diffcult to determine what statistical tool to use)
    avg = avg/(m*n);

    //Then we weed out all high temperature zones, and convert array to binary(0 for high, 1 for low, since low is what we need)
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            a[i][j]=a[i][j]<avg?1:0;
        }
    }


    //Now the array a will have all low temperature zones recorded, we figure out which position the rotor is in and remove it
    a = removeRotator(a);

    //Then print the food array
    cout<<"[";
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(a[i][j] == 1) {
                cout<<"("<<i<<","<<j<<"), ";
            }
        }
    }
    cout<<"]"<<endl;

    return 0;
}
