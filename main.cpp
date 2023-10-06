#include <iostream>
using namespace std;

/* 
 * Returns the "index" (i.e., A=0, B=1, ..., H=7) 
 * of the first variable equal to zero.
 * Returns -1 if all variables have been assigned a value.
 */
int indexOfZero(long int state) {
    for (int i=0; i<24; i+=3) {
        if (((state>>i) & 7) == 0) { return i/3; }
    }
    return -1;
}

/* 
 * Returns the "index" (i.e., A=0, B=1, ..., H=7) 
 * of the first variable equal to five.
 * Returns -1 if no such variable is found.
 */
int indexOfFive(long int state) {
    for (int i=0; i<24; i+=3) {
        if (((state>>i) & 7) == 5) { return i/3; }
    }
    return -1;
}

/*
 * Returns true if all constraints are satisfied.
 * Use alternative definitions for search with and without
 * degree heuristics.
 */
bool satisfiesConstraints(long int state) {
    int f =  state      & 7;
    int h = (state>> 3) & 7;
    int g = (state>> 6) & 7;
    int e = (state>> 9) & 7;
    int d = (state>>12) & 7;
    int c = (state>>15) & 7;
    int b = (state>>18) & 7;
    int a = (state>>21) & 7;
    bool result = true;
    // Only checks constraints pertaining to variables that have been assigned values.
    switch (indexOfZero(state)) {
        case -1:
        case 7:
        case 6:
        case 5:
        case 4:
            result = result && (c!=0?d!=c:true);
        case 3:
            result = result && (c!=0?e!=c:true) && (d!=0?e<(d-1):true);
        case 2:
            result = result && (a!=0?g<a:true) && (c!=0?abs(g-c)==1:true) && (d!=0?g<=d:true) && (f!=0?g!=f:true);
        case 1:
            result = result && (a!=0?h>=a:true) && (c!=0?abs(h-c)%2==0:true) && (d!=0?h!=d:true) && (e!=0?h!=e+2:true) && (g!=0?h>g:true);
        case 0:
            result = result && (b!=0?abs(f-b)==1:true) && (c!=0?f!=c:true) && (d!=0?f!=d+1:true) && (e!=0?abs(f-e)%2==1:true) && (h!=0?f!=h:true);
            break;
        default:
            return true;
    }
    return result;
}

// bool satisfiesConstraints(long int state) {
//     int a =  state      & 7;
//     int b = (state>> 3) & 7;
//     int c = (state>> 6) & 7;
//     int d = (state>> 9) & 7;
//     int e = (state>>12) & 7;
//     int f = (state>>15) & 7;
//     int g = (state>>18) & 7;
//     int h = (state>>21) & 7;
//     bool result = true;
//     // Only checks constraints pertaining to variables that have been assigned values.
//     switch (indexOfZero(state)) {
//         case -1:
//             result = result && ah>=a && abs(h-c)%2==0 && h!=d && h!=e+2 && h>g;
//         case 7:
//             result = result && g<a && abs(g-c)==1 && g<=d && g!=f;
//         case 6:
//             result = result && abs(f-b)==1 && f!=c && f!=d+1 && abs(f-e)%2==1 && f!=h;
//         case 5:
//             result = result && e!=c && e<(d-1);
//         case 4:
//             result = result && d!=c;
//             break;
//         default:
//             return true;
//     }
//     return result;
// }
/*
 * External logic makes things look better.
 */
long int successorFunction(long int state, int index) {
    return state + (1<<(3*index));
}

/* 
 * Prints a state. correct=true if printed results must satisfy constraints,
 * maxLength=true if printed results must be found at max search depth.
 */
void printState(long int state, bool correct, bool maxLength) {
    if ((!correct || satisfiesConstraints(state)) && (!maxLength || indexOfZero(state)==-1)) {
        for (int i=0; i<24; i+=3) {
            if (((state>>i) & 7) != 0) {
                cout << "FHGEDCBA"[i/3] << "=" << ((state>>i) & 7) << " ";
            }
        }
        cout << (satisfiesConstraints(state) ? (indexOfZero(state)==-1 ? "solution" : "") : "failure") << endl;
    }
}

int main(int argc, char const *argv[])
{
    /* 
     * Note: a state is represented as a 32-bit binary integer 
     * starting with 8 0s followed by 8 groups of 3 bits
     * each representing a variable from H to A.
     */
    // Number of failing branches.
    int badWorlds = 0;
    int prunes   = 0;
    // Current search path.
    long int* queue = new long int[8];
    // Initialize search path.
    *(queue) = successorFunction(0, 0);
    // Current working index in the queue.
    int currIndex = 0;
    // If the current index is < 0, the algorithm
    // has backtracked past the root node and is complete.
    while (currIndex >= 0) {
        long int currState = *(queue+currIndex);
        /*
         * Note: change this printState call to
         * (..., 0, 0) for the full search tree
         * (..., 1, 1) for solutions only
         * (..., 0, 1) for worlds only (all variables assigned)
         * (..., 1, 0) for passing states only (do not violate constraints)
         */
        printState(currState, 1, 1);
        // Deepens the search if it is not already at max depth 
        // and the current state does not violate constraints.
        currIndex += int( satisfiesConstraints(currState) &&   currIndex < 7);
        badWorlds += int(!satisfiesConstraints(currState) && !(currIndex < 7));
        prunes    += int(!satisfiesConstraints(currState) &&   currIndex < 7);

        *(queue+currIndex) = successorFunction(currState, currIndex);
        // Prevents variables from exceeding 4 (domain {1, 2, 3, 4}).
        while (indexOfFive(*(queue+currIndex))!=-1) {
            currIndex--;
            currState = *(queue+currIndex);
            *(queue+currIndex) = successorFunction(currState, currIndex);
        }
    }
    cout << badWorlds << " failing worlds found, " << prunes << " branches pruned (" << badWorlds+prunes << " failures)"
         << "\n\tof " << pow(4,8) << " possible states." << endl;

    delete[] queue;
}