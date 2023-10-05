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
 */
bool satisfiesConstraints(long int state) {
    int a =  state      & 7;
    int b = (state>> 3) & 7;
    int c = (state>> 6) & 7;
    int d = (state>> 9) & 7;
    int e = (state>>12) & 7;
    int f = (state>>15) & 7;
    int g = (state>>18) & 7;
    int h = (state>>21) & 7;
    bool result = true;
    // Only checks constraints pertaining to variables that have been assigned values.
    switch (indexOfZero(state)) {
        case -1:
            result = result && a<=h && g<h && abs(h-c)%2==0 && h!=d && e!=(h-2) && h!=f;
        case 7:
            result = result && a>g && abs(g-c)==1 && d>=g && g!=f;
        case 6:
            result = result && abs(f-b)==1 && c!=f && d!=(f-1) && abs(e-f)%2==1;
        case 5:
            result = result && e!=c && e<(d-1);
        case 4:
            result = result && d!=c;
            break;
        default:
            return true;
    }
    return result;
}

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
                cout << char(65+i/3) << "=" << ((state>>i) & 7) << " ";
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
    int failures = 0;
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
        failures  += int(!satisfiesConstraints(currState) && !(currIndex < 7));
        prunes    += int(!satisfiesConstraints(currState) &&   currIndex < 7);

        *(queue+currIndex) = successorFunction(currState, currIndex);
        // Prevents variables from exceeding 4 (domain {1, 2, 3, 4}).
        while (indexOfFive(*(queue+currIndex))!=-1) {
            currIndex--;
            currState = *(queue+currIndex);
            *(queue+currIndex) = successorFunction(currState, currIndex);
        }
    }
    cout << failures << " failing worlds found, " << prunes << " branches pruned (" << failures+prunes << " failures)"
         << "\n\tof " << pow(4,8) << " possible states." << endl;

    delete[] queue;
}