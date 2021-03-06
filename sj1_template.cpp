/*  Short job 1
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>


void testuj(float *mX,float *mX2,int n,int m)
{
  // test spravnosti reseni
  // test for correctness
  int i,j,k,pk,err;
  float pom,k1;
  err=0;
  for(i=0;i<(n*m);i++)
  {
    if (fabs(mX[i]-mX2[i])>0.1)
    {
      err++;
      printf("%i = %g,%g\n",i,mX[i],mX2[i]);
    }
  }
  if (err!=0) printf("total ERR=%i/%i \n",err,(n*m));
}


int vyprazdni(int *temp,int k)
{
  // vyprazdni cache
  // flush cache
  int i,s=0;
  for(i=0;i<k;i++) s+=temp[i];
  return s;
}


//!! zacatek casti k modifikaci
//!! beginning of part for modification

void Gauss_BS(const float *__restrict inA, const float *__restrict inB, float *__restrict outX, const int n, const int m) {

    float *__restrict outX2 = (float*)malloc(m*n*sizeof(float));

#pragma omp parallel for
    for(int k=0;k<m;k++)
    {
        for(int i=n-1;i>=0;i--)
        {
            float s=inB[k+i*m];
            const int in = i*n;
            const int kn = k*n;
            const int im = i*m;
            for(int j=i+1;j<n;j++)
            {
                s-=inA[in+j]*outX2[kn+j];
            }
            outX[k+im]=s/inA[in+i];
            outX2[kn+i] = s/inA[in+i];
        }
    }
free(outX2);
}
//!! end of part for modification
//!! konec casti k modifikaci



int main( void ) {

 double start_time,end_time,timea[10];

 int soucet=0,N,i,j,k,n,m,*pomo,v;
 int ri,rj,rk;
 double delta,s_delta=0.0;
 float *mA, *mB,*mX,*mX2,s;

  //int tn[4]={1000,1500,2000,2500};
  int tn[4]={1*1024,2*1024,12*1024,16*1024};
  int tm[4]={1024,256,32,12};
  srand (time(NULL));
  pomo=(int *)malloc(32*1024*1024);
  v=0;

  for(N=0;N<10;N++) timea[N]=0.0;

  for(N=0;N<4;N++)
  {
  n=tn[N];
  m=tm[N];

  mA=(float *)malloc(n * n * sizeof(float));
  mB=(float *)malloc(n * m * sizeof(float));
  mX=(float *)malloc(n * m *sizeof(float));
  mX2=(float *)malloc(n * m * sizeof(float));
  if ((mA==NULL)||(mB==NULL)||(mX==NULL)||(mX2==NULL))
  {
    printf("Insufficient memory!\n");
    return -1;
  }

  for (i=0; i<n; i++) {
  for (j=0; j<n; j++) {
    if (j>=i)
      mA[i*n+j] = (float)(2*(rand()%59)-59);
    else
      mA[i*n+j] = 0.0;
  }}
  for (k=0; k<m; k++) {
  for (j=0; j<n; j++) {
  mX2[j*m+k] = (float)((rand()%29)-14);
  }}

  for (k=0; k<m; k++) {
  for (i=0; i<n; i++) {
  s=0.0;
  for (j=0; j<n; j++) {
    s += mA[i*n+j]*(mX2[j*m+k]);
  }
  mB[i*m+k]=s;
  }}
  soucet+=vyprazdni(pomo,v);
  start_time=omp_get_wtime();
  // improve performance of this call
  // vylepsit vykonnost tohoto volani
  Gauss_BS( mA, mB, mX, n,m);

  end_time=omp_get_wtime();
  delta=end_time-start_time;
  timea[0]+=delta;
  testuj(mX,mX2,n,m);
  printf("n0=%i m0=%i time=%g \n",n,m,delta);
  fflush(stdout);

  free(mX2);
  free(mX);
  free(mB);
  free(mA);
  }
  printf("%i\n",soucet);
  printf("Total time=%g\n",timea[0]);
  //for(N=0;N<10;N++)  printf("%i=%g\n",N,timea[N]);

  return 0;
}
