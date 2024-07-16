/* Input:   number of rows(m)
            number of cols(n)
            upbound of coeffs(u)
	    sign of coeffs (s: 0=all negative, 1=all positive, 
	                            2=mixed)
				    -  will effect int vars coeffs and 
                                       cont vars coeffs to an extend 
                                       considering cont type of rows-
	    sense of rhs(e: 0=all 'E', 1=all 'G', 2= mixed, 3=all 'L')
            dominancy strength of variables(d: 1 or 2) 
	    type of problem (t: 0=pure, 1=mixed)
	    number of lower cols(l) (l = 0 should yield a regular milp)
	    dummy to distinguish instance number (d) 
-----
not applicable 
            #continuity type of value functions of rows(c: 0=left_cont 1=cont 
	    #                                             2=right cont 
            #                                             3=mixed)
------							  
   Run   : ./milpGen m n u s e d t
   Output: milp_m_n_u_sedt.mps
           milp_m_n_u_sedt_sol.txt  -known feas sol-
            
*/
//#############################################################################

#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <time.h>
//#include <ctime>
#include <iostream>
#include <fstream>
#include "CoinPackedMatrix.hpp"
#include "CoinMpsIO.hpp"
#include "CoinHelperFunctions.hpp"



using namespace std;

int main(int argc, char * argv[])
{
   if (argc != 10) {
      cout << "Usage: excutable m n u s e d t l d\n";
      abort();
   }
   

   int i, j, c, nz, least_c_cnt, least_i_cnt, row_c_cnt, row_i_cnt, int_low;
   char * row_c_vars, * row_i_vars, *sense, *is_int;
   int * r_matbeg, *r_matind;
   double *obj, *rhs, *r_matval, *ub, *lb, *range, *feas, d_tmp, *lobj, *lcol;
   double r_eta, r_zeta, r_eta_c, r_zeta_c;
   int indx, counter, low, high, r_eta_c_ind, r_zeta_c_ind;
   CoinMpsIO mps;

   char pname[100]; 
   char solfname[100];
   char lowfname[100];

   time_t timeval;	/* Current time. */
   unsigned char *ptr;	/* Type punned pointed into timeval. */
   unsigned seed;	/* Generated seed. */
   size_t stmp;

   timeval = time (NULL);
   ptr = (unsigned char *) &timeval;
   
   seed = 0;
   for (stmp = 0; stmp < sizeof timeval; stmp++)
     seed = seed * (UCHAR_MAX + 2u) + ptr[stmp];

   //return seed;

   std::cout << "The seed is " << seed << "." << std::endl;
   std::cout << "time(NULL) is " << time(NULL) << std::endl;
   std::cout << "The other seed is " << random() << "." << std::endl;
   std::cout << "The maximum double is " << DBL_MAX << std::endl;
   // initialize random generator.
   //srand ( time(NULL) ); 
   srandom (seed); 




 
   int m = atoi(argv[1]);
   int n = atoi(argv[2]);
   int U = atoi(argv[3]);
   int s_type = atoi(argv[4]);
   int sense_type = atoi(argv[5]);
   int p_type = atoi(argv[6]);
   //int c_type = atoi(argv[7]);
   int t_type = atoi(argv[7]);
   int l = atoi(argv[8]);
   int dummy = atoi(argv[9]);
   int c_type = -1;

   int lcnt = 0;
   bool used = false;
   double tmp = 0.0;

   /* get prob.name*/
   strcpy(pname, "");
   strcpy(solfname, "");
   strcpy(lowfname, "");
   sprintf(pname, "milp_%i_%i_%i_%i%i%i%i_%i", m, n, U, s_type, sense_type, 
	   p_type, t_type, dummy);
   sprintf(solfname, "%s.sol", pname);
   sprintf(lowfname, "%s.aux", pname);
   sprintf(pname,"%s.mps", pname);

   /* first decide the number of cont variables*/
   /* not less than 1, not more than %10n*/
   

   r_matbeg = new int[m+1];
   r_matind = new int[n*m];
   r_matval = new double[n*m];

   obj = new double[n];
   ub = new double[n];
   lb = new double[n];
   rhs = new double[m];
   is_int = new char[n];
   sense = new char[m];   
   range = new double[m];   
   feas = new double[n];
   lobj = new double[l];   
   lcol = new double[l];   

   double sum = 0;
   /* get upper obj first */
   for(i = 0; i < n; i++){
      obj[i]  =1 + static_cast<int>(double(random()) / RAND_MAX * (U - 1));
      feas[i] = 0.0;
      if((double(random()) / RAND_MAX) < 0.5){
	 feas[i] = 1 + (int)(double(random()) / RAND_MAX * (5));
      }
      //      sum += obj[i];
      //ub[i] = DBL_MAX;
      ub[i] = 10 * U; //arbitrary, just to make sure it's bounded
      lb[i] = 0.0;
   }

   /* now get lower obj */
   for(i = 0; i < l; i++){
     lobj[i] = 1 + static_cast<int>(double(random()) / RAND_MAX * (U - 1));
   }
   
   /* set all values in lcol to -1*/
   CoinFillN(lcol, l, -1.0);

   /* select l lower cols, they must be unique */
   while(lcnt < l){
     /* numbered 1..n-1 */
     tmp = 1 + static_cast<int>(double(random()) / RAND_MAX * (n));

     used = false;
     
     /* naive search */
     for(i = 0; i < l; i++){
       if(lcol[i] == tmp)
	 used = true;
     }

     /* if it doesn't appear, add it */
     if(!used){
       lcol[lcnt] = tmp;
       lcnt++;
     }
   }
   
   /* sort the array */
   sort(lcol, lcol + l);
   for(i = 0; i < l; i++){
     cout << "lcol " << lcol[i] << endl;
   }

   if(sense_type == 0){
     memset(sense, 'E', sizeof(char)*m);
   }else if(sense_type == 1){
     memset(sense, 'G', sizeof(char)*m);
   }else if(sense_type == 2){
     for(i = 0; i < m; i++){
       if((double(random()) / RAND_MAX) < 0.5){
	 sense[i] = 'E';
       }else{
	 sense[i] = 'G';
       }	    
     }
   }//else{
   //memset(sense, 'L', sizeof(char)*m);
   //}


   //   cout<<"sum: "<<sum<<endl; 
   
   /* number of max cont variables */
   if(t_type > 0){
     c = 1 + (int)(double(random()) / RAND_MAX * (0.2*n - 1));
   }else{
     c = 0;
   }

   //sort the obj coeffs if needed*/
   if(p_type > 1){
      sort(obj + c, obj + n);
   }
   /* least number  of int variables on each row */
   int_low = (int)(0.2*(n-c));
   

   if(c_type == 0 || c_type == 2 || c_type == 3){
      least_c_cnt = 1;
   }else if(c_type == -1){
      least_c_cnt = 0;
   }else{
      least_c_cnt = 2;
      if(c < least_c_cnt){
	 c = least_c_cnt;
      }
   }

   memset(is_int, false, sizeof(char)*c);
   memset(is_int + c, true, sizeof(char)*(n-c));
   
   row_c_vars = new char[c];
   row_i_vars = new char[n-c];
   nz = 0;
   r_matbeg[0] = 0;

   for(i = 0; i < m; i++){
      /* for each row, decide which variables will have nonzero coeff, 
	 be cont, be integer...*/      
      rhs[i] = 0;
      range[i] = 0;
      row_c_cnt = 0;
      memset(row_c_vars, false, sizeof(char)*c); 
      counter = 0;
      while(row_c_cnt < least_c_cnt && counter < 10){
	 for(j = 0; j < c; j++){
	    if(!row_c_vars[j]){
	       if(m <= 1 || (double(random()) / RAND_MAX) < 0.5){
		  row_c_vars[j] = true;	       
		  indx = nz + row_c_cnt;
		  r_matind[indx] = j;			 
		  r_matval[indx] = 
		     (1 + (int)(double(random()) / RAND_MAX * 
					    (U - 1)));
		  if(s_type == 2 ||       
		     c_type == 0 || ((c_type == 1 || c_type == 3) && 
				     (double(random()) / RAND_MAX) < 0.5)){
		     r_matval[indx] = - r_matval[indx];
		  }
		  rhs[i] += feas[r_matind[indx]]*r_matval[indx];
		  sum += feas[r_matind[indx]]*obj[r_matind[indx]];
		  row_c_cnt++;		  		  
	       }
	    }	    
	 }
	 counter++;
      }
      nz += row_c_cnt; 

   /* not too dense, not too sparse
      each row will have at least least_i_cnt nonzeros
      set least_i_cnt for different value for each row
   */

      if(m == 1){
	 least_i_cnt = n;
      }else{
	 least_i_cnt = int_low + (int)((n-c-int_low+1)*double(random()) / RAND_MAX);
      }
      row_i_cnt = 0;
      memset(row_i_vars, false, sizeof(char)*(n-c)); 
      counter = 0;
      while(row_i_cnt < least_i_cnt && counter < 10){
	 for(j = 0; j < n - c; j++){
	    if(!row_i_vars[j]){
	       if((double(random()) / RAND_MAX) < 0.5){
		  row_i_vars[j] = true;	       
		  r_matind[nz + row_i_cnt] = c + j;
		  row_i_cnt++;
	       }
	    }	    
	 }
	 counter++;
      }
      
      nz += row_i_cnt;
      r_matbeg[i+1] = nz;
      

      r_eta = r_eta_c = DBL_MAX;
      r_zeta = r_zeta_c = -DBL_MAX;

      /* get r_eta_c, r_zeta_c */ 
      for(j = r_matbeg[i]; j < r_matbeg[i] + row_c_cnt; j++){
	 d_tmp = (double)(obj[r_matind[j]]/r_matval[j]);
	 if(r_matval[j] > 0 && d_tmp < r_eta_c){
	    r_eta_c = d_tmp;
	    r_eta_c_ind = r_matind[j];
	 }
	 if(r_matval[j] < 0 && d_tmp > r_zeta_c){
	    r_zeta_c = d_tmp;
	    r_zeta_c_ind = r_matind[j];
	 }
      }
      
      if(c_type == 1){
	 if(r_eta_c >= DBL_MAX){
	    /* means we dont have a pos value matval, choose the first one 
	     and change it to pos */
	    for(j = r_matbeg[i]; j < r_matbeg[i] + row_c_cnt; j++){
	       if(r_matval[j] < 0 && r_zeta_c_ind != r_matind[j]){
		  rhs[i] -= feas[r_matind[j]]*r_matval[j];
		  r_matval[j] = -r_matval[j];
		  rhs[i] += feas[r_matind[j]]*r_matval[j];
		  r_eta_c = (double)(obj[r_matind[j]]/r_matval[j]);
		  break;
	       }
	    }
	    
	 //    indx = (int)(double(rand())/RAND_MAX *(row_c_cnt - 1));
// 	    indx += r_matbeg[i];
// 	    rhs[i] -= feas[r_matind[indx]]*r_matval[indx];
// 	    sum -= feas[r_matind[indx]]*obj[r_matind[indx]];
// 	    r_matval[indx] = - r_matval[indx];
// 	    rhs[i] += feas[r_matind[indx]]*r_matval[indx];
// 	    sum += feas[r_matind[indx]]*obj[r_matind[indx]];
// 	    r_eta_c = (double)(obj[r_matind[indx]]/r_matval[indx]);
	 }
	 if(r_zeta_c <= -DBL_MAX){
	    /* means we dont have a neg value matval, randomly choose one */
	    for(j = r_matbeg[i]; j < r_matbeg[i] + row_c_cnt; j++){
	       if(r_matval[j] > 0 && r_eta_c_ind != r_matind[j]){
		  rhs[i] -= feas[r_matind[j]]*r_matval[j];
		  r_matval[j] = -r_matval[j];
		  rhs[i] += feas[r_matind[j]]*r_matval[j];
		  r_zeta_c = (double)(obj[r_matind[j]]/r_matval[j]);
		  break;
	       }
	    }
	 }
      }

      if((sense[i] == 'G')){
	 r_zeta_c = 0.0;
      }

      
      /* get estimated r_eta, r_zeta */ 
      if(r_eta_c < DBL_MAX){
	 r_eta = (double(random())/RAND_MAX *(r_eta_c));
      }
      if(r_zeta_c > -DBL_MAX){
	 if(r_zeta_c == 0.0){
	    r_zeta = 0.0;
	 }else{
	    r_zeta = (double(random())/RAND_MAX *(r_zeta_c));
	 }
      }

      /* ok now fill in the integer coeffs */
      
      switch(p_type){
       case 1: /* simplest case, random coeffs */
	  //all will require to be in r_eta_c and r_eta ( if possible) 
	  //and in r_zeta_c and r_zeta ( if possible) 

	  for (j = nz-row_i_cnt; j < nz; j++) {
	     if(s_type == 1 || 
		(s_type == 2 && (double(random()) / RAND_MAX) < 0.5)){
	// 	if(can_relax_coeff){
// 		   	low = 0;
// 			high = U;
// 			if( r_eta_c < DBL_MAX){
// 			   low = (int)(obj[r_matind[j]]/r_eta_c) + 1;
// 			   high = (int)(obj[r_matind[j]]/r_eta) + 1;		  
// 			   if(high > U){
// 			      high = U;
// 			   }
// 			}
// 		}else{
// 		   low = -U; 
// 		   high = 0;
		   
// 		   if(r_zeta_c > -DBL_MAX && r_zeta_c != 0.0){
// 		      low = (int)(obj[r_matind[j]]/r_zeta);
// 		      high = (int)(obj[r_matind[j]]/r_zeta_c);
// 		      if(low < -U){
// 			 low = -U;
// 		      }
// 		   }
// 		}		
		low = 0;
		high = U;
		if( r_eta_c < DBL_MAX && r_eta_c != 0.0){
		   low = (int)(obj[r_matind[j]]/r_eta_c) + 1;
		   if(low < U){
		      high = (int)(obj[r_matind[j]]/r_eta) + 1;		  
		      if(high > U){
			 high = U;
		      }
		   }else{
		      low = 0;
		   }
		}
	     }else{
		low = -U; 
		high = 0;
		if(r_zeta_c > -DBL_MAX && r_zeta_c != 0.0){
		   low = (int)(obj[r_matind[j]]/r_zeta);
		   if( low > -U){
		      high = (int)(obj[r_matind[j]]/r_zeta_c);
		      if(high > 0){
			 high = 0;
		      }
		   }else{
		      low = -U;
		   }
		}
	     }
	     r_matval[j] = low + int((high - low + 1)*
				     (double(random())/RAND_MAX));
	     rhs[i] += feas[r_matind[j]]*r_matval[j];
	     sum += feas[r_matind[j]]*obj[r_matind[j]];
	  }		
	  break;
       case 2: /* work on simple dominancy */
	  
	  for (j = nz-row_i_cnt; j < nz; j++) {
	     if(s_type == 1 || 
		(s_type == 2 && (double(random()) / RAND_MAX) < 0.5)){
		low = 1;
		high = U;
		if( j == row_c_cnt){
		   if( r_eta_c < DBL_MAX && r_eta_c != 0.0){
		      low = (int)(obj[r_matind[j]]/r_eta_c) + 1;
		      if( low > U + 1){
			 obj[r_matind[j]] = 1 + int((r_eta_c * U)*
						    (double(random())/RAND_MAX));
			 low = (int)(obj[r_matind[j]]/r_eta_c) + 1;
		      }
		      high = (int)(obj[r_matind[j]]/r_eta) + 1;		  
		      if(high > U){
			 high = U;
		      }
		   }
		}else{
		   if( r_eta_c < DBL_MAX){
		      low = (int)(obj[r_matind[j-1]]/r_eta_c) + 1;
		      if( low > U + 1){
			 obj[r_matind[j]] = 1 + 
			    int((r_eta_c * 
				 (U - obj[r_matind[j-1]])*
				 (double(random())/RAND_MAX)));
			 low = (int)(obj[r_matind[j]]/r_eta_c) + 1;
		      }
		      high = (int)(obj[r_matind[j]]/r_eta) + 1;		  
		      if(high > U){
			 high = U;
		      }
		   }
		}
	     }else{
		low = -U; 
		high = -1;
		if( j == row_c_cnt){
		   if( r_zeta_c > -DBL_MAX && r_zeta_c != 0.0){
		      low = (int)(obj[r_matind[j]]/r_zeta_c) - 1;
		      if( low < -U - 1){
			 obj[r_matind[j]] = 1 + int((r_zeta_c * U)*
						    (double(random())/RAND_MAX));
			 low = (int)(obj[r_matind[j]]/r_zeta_c) + 1;
		      }
		      high = (int)(obj[r_matind[j]]/r_zeta) - 1;
		      if(high < -U){
			 high = -U;
		      }
		   }
		}else{
		   if( r_zeta_c > -DBL_MAX && r_zeta!= 0.0){
		      low = (int)(obj[r_matind[j-1]]/r_zeta_c) + 1;
		      if( low < -U - 1){
			 obj[r_matind[j]] = 1 + 
			    int((r_zeta_c * 
				 (-U + obj[r_matind[j-1]])*
				 (double(random())/RAND_MAX)));
			 low = (int)(obj[r_matind[j]]/r_zeta_c) + 1;
		      }
		      high = (int)(obj[r_matind[j]]/r_zeta) + 1;
		      if(high < -U){
			 high = -U;
		      }
		   }
		}

	     }
	     r_matval[j] = low + int((high - low + 1)*
				     (double(random())/RAND_MAX));
	     rhs[i] += feas[r_matind[j]]*r_matval[j];
	     sum += feas[r_matind[j]]*obj[r_matind[j]];
	  }		
	  
	  break;
       default:
	  cout << "Wrong problem type..."<<endl;
	  abort();
      }           

   }
   
   /*change the obj sense (didn't want to mess with the other code) */
   //for(i = 0; i < n; i++){
    // obj[i] = - obj[i];
  // }
   //for(i = 0; i < n; i++){
   //obj[i] = - obj[i];
  // }
   for(i = 0; i < n; i++){
     if((double(random()) / RAND_MAX) < 0.5){
       obj[i] = - obj[i];
     }else{
       obj[i] = obj[i];
     }	    
   }


   //negate r_matval, sense, rhs 
   if(sense_type == 3){
     for(i = 0; i < m; i++){
       sense[i] = 'L';
       rhs[i] = - rhs[i];
     }
     for(i = 0; i < (n*m); i++){
       r_matval[i] = -r_matval[i];
     }
   }
   
   /* now write the mps file */

   CoinPackedMatrix mat(false, n, m, nz, r_matval, r_matind, r_matbeg, 0);
   mps.setMpsData(mat, DBL_MAX, lb, ub, obj, is_int, sense, rhs, range, NULL, 
		  NULL);
   mps.writeMps(pname);

   ofstream fout(solfname);
   fout << "sol val: "<< sum << endl;
   for(i = 0; i < n; i++){
      fout << feas[i] << endl;
   }
   fout.close();

   ofstream fout2(lowfname);
   fout2 << "N "<< l << endl; // number of lower cols (l)
   fout2 << "M "<< m << endl; // number of lower cons (m - all of them)
   for(i = 0; i < l; i++){
     fout2 << "LC " << (lcol[i] - 1) << endl;
   }
   for(i = 0; i < m; i++){
     fout2 << "LR " << i << endl;
   }
   for(i = 0; i < l; i++){
     if((double(random()) / RAND_MAX) < 0.5){
       fout2 << "LO " << (lobj[i]) << endl;
     }
     else{
       fout2 << "LO " << (-lobj[i]) << endl;
     }
   }
   fout2 << "OS " << 1 << endl;
   fout2.close();

  
   delete [] r_matbeg;
   delete [] r_matind;
   delete [] r_matval;
   delete [] row_c_vars;
   delete [] row_i_vars;
   delete [] obj;
   delete [] ub;
   delete [] lb;
   delete [] rhs;
   delete [] is_int;
   delete [] sense;
   delete [] feas;

}
