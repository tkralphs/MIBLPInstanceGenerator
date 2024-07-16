# An MIBLP Instance Generator 

## Building on Linux

 1. Install [CoinUtils](https://github.com/coin-or/CoinUtils).
 2. Make sure you have `pkg-config`
 3. Edit the `Makefile` so that `COINROOT` points to the install location of CoinUtils.
 4. Execute `make`

## Usage
```
./milpGen m n u s e d t l j
```
| argument | meaning | values | comment |
| -------- | ------- | ------ | ------- |
| m | number of rows(m) | > 0 | |
| n | number of cols(n) | > 0 | |
| u |upbound of coeffs(u) | > 0 | |
| s | sign of coeffs | 0 all negative <br> 1 all positive <br> 2 mixed | will effect int vars coeffs and <br> cont vars coeffs to an extent,<br> considering cont type of rows |
| e | sense of rhs | 0 all 'E' <br> 1 all 'G' <br> 2 mixed <br> 3 all 'L') |
| d | dominancy strength of variables| 1 or 2 | | 
| t | type of problem | 0 pure <br> 1 mixed | |
| l | number of lower-level variables | 0 < l < m |
| j | dummy to distinguish instances | > 0 | 

Output 
```
milp_m_n_u_sedt_j.mps
milp_m_n_u_sedt_j.sol 
milp_m_n_u_sedt_j.aux 
```
