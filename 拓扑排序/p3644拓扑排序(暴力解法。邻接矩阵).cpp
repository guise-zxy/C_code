#include<iostream>
#include<cstring>
#include<algorithm>
using namespace std;
int n;
int a[110][110],k;
int in[110],res[110];
void topsort(int n){
	for(int i=1;i<=n;i++ ){
	
		for(int j=1;j<=n;j++){
			if(!in[j]){
				res[i]=j;
				in[j]--;
				k=j;
				break;
			}
		}
		
		for(int j=1;j<=n;j++){
			if(a[k][j]>=1)in[a[k][j]]--;
		}
	
}
}

void PP(int n){
	for(int i=1;i<=n;i++){
		if(i==1)printf("%d",res[i]);
		else printf(" %d",res[i]);
	}
}
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		for(int j=1;j<=n;j++){
			scanf("%d",&a[i][j]);
			if(!a[i][j])break;
			else in[a[i][j]]++;
		}
	} 
	
		topsort(n);
	
	
	PP(n);
	
	
	
	return 0;
}
