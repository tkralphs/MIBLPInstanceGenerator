# An MIBLP Instance Generator 

Run   : `./milpGen m n u s e d t`

Output: `milp_m_n_u_sedt.mps` <br>
        `milp_m_n_u_sedt_sol.txt` (known feas sol)


| argument | meaning | values | comment |
| -------- | ------- | ------ | ------- |
| m | number of rows(m) | > 0 | |
| n | number of cols(n) | > 0 | |
| u |upbound of coeffs(u) | > 0 | |
| s | sign of coeffs | * 0 all negative <br> * 1 all positive <br> * 2 mixed | will effect int vars coeffs and <br> cont vars coeffs to an extent,<br> considering cont type of rows |
| e | sense of rhs | * 0 all 'E' <br> * 1 all 'G' <br> * 2 mixed< br> * 3 all 'L') |
| d | dominancy strength of variables| 1 or 2 | | 
| t | type of problem | * 0 pure <br> * 1 mixed | |

