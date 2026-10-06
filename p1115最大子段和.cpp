
/*#include<bits/stdc++.h>
using namespace std;
int res=0;
int n;
int sum[200005];
int a[200005];
int main(){
	int n;
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
	}
	for(int i=1;i<=n;i++){
		sum[i]=sum[i-1]+a[i];
	}
	for(int i=1;i<=n;i++){
		for(int j=i+1;j<=n;j++){
			
			
			res=max(res,sum[j]-sum[i-1]);
		
		}
	}
	printf("%d",res);
	
	
	return 0;
}
*/
/*#include<bits/stdc++.h>
using namespace std;
int n;
int a[200010];
int p[200010];
int dfs(int k){
	if(p[k]){
		return p[k];
	}
	p[k]=a[k];
	for(int i=k;i<=n;i++){
		if(i<=n&&a[i]>=a[k]){
			p[k]=max(p[k],p[i+1]+a[k]);
		}
	}
	return p[k];
}
int main(){
	scanf("%d",&n);
	for(int i=1;i<=n;i++){
		scanf("%d",&a[i]);
	}
	int ans=0;
	for(int j=1;j<=n;j++){
		ans=max(ans,dfs(j));

	}
	printf("%d\n",ans+1);
	return 0;
}*/
#include<bits/stdc++.h>
using namespace std;
int main()
{
    using namespace std;
    int a,b=0;
    int n,sum=-100001;
    cin>>n;
    for(int i=0;i<n;i++)
    {
        cin>>a; 
        if(b>0) {
			b+=a;
		}
        else {
			b=a;
		}
        if(b>sum){
			sum=b;
		}
	}
    cout<<sum;
    return 0;
}
