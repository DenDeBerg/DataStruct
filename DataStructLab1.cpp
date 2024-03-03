#include <iostream>

using namespace std;

int KoltsoVichetov(int a, int n) {
for(int b = 1; b < n; b++) {
if ((a * b) % n == 1) {
return b;
}
}
return 0;
}
int main() {
int a = 0, n = 0;
cin >> a >> n;

int result = KoltsoVichetov(a,n);

cout << result << endl;
return 0;
} 