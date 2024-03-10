#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <climits>
#define limit 10000
using namespace std;
int var_num;
int min_impli_num = INT_MAX;
int literal_cnt = 0;
int petrick_cnt = 0;

set<string> partial_minimum_cover;

vector<vector<string>> left_prime_impli;

int gray_code(string ,string);//return -1 => false else the position of -

void petrick_method(int pos , int max_index , set<string>cur_set);


vector <string> mintern_gen(string product_term);

int main(int argc, char* argv[]){
    ifstream In_File;
    ofstream Out_File;
    int product_num;
    string product_term;


    /*open files*/
    In_File.open(argv[1],ios::in);
    Out_File.open(argv[2],ios::out);
    /*end of open files*/

    In_File >> var_num;
    In_File >> product_num;

    set<string> impli_table [var_num + 1][ var_num+1 ];//first => column ; second => num of 1

    set<string> unused_impli[var_num + 1];

    map<string, vector<string>> prime_impli_chart;

    set<string> minimum_cover;

    /*generate minterm(impli_table[0] is the minterm)*/
    while(In_File >> product_term)
    {
        vector<string> vec = mintern_gen( product_term);
        int siz = vec.size();
        for(int i = 0 ; i < siz ; i++)
        {
            int count = 0;
            int si = vec[i].size();
            for(int j = 0; j < si ; j++ )
            {
                if(vec[i][j] == '1')count ++;
            }
            impli_table[0][count].insert(vec[i]);
            unused_impli[0].insert(vec[i]);
        }
    }
    /*end of generate minterm*/

    /*generate prime implicant*/
    for(int i = 0 ;i < (var_num + 1); i++)//what about column 5 for var=4 ?ex ----
    {
        if(unused_impli[i].size() == 0 )break;//every implicant cant be bigger
        for(int j = 0 ;j < var_num; j++)
        {
            for (auto &former : impli_table[i][j])
            {
                for (auto &later : impli_table[i][j+1])
                {
                    int index = gray_code(former,later);
                    if(index != -1)//generate next column
                    {
                        string comb = former;
                        comb[index] = '-';
                        impli_table[i+1][j].insert(comb);
                        unused_impli[i+1].insert(comb);
                        unused_impli[i].erase(former);
                        unused_impli[i].erase(later);
                    }
                }
            }
        }

    }
    /*end of generate prime implicant*/


    /*generate  prime implicant chart*/
    for(int j = 0 ;j < (var_num + 1); j++)
    {
        for (auto &prime_impli : unused_impli[j])
        {
            vector<string> minterms = mintern_gen(prime_impli);
            for(auto &min : minterms)
            {
                vector<string> temp = prime_impli_chart[min];
                temp.push_back(prime_impli);
                prime_impli_chart[min] = temp;
            }
        }
    }
    /*end of generate  prime implicant chart*/


    /*generate essential prime implicant*/
    for(auto & minterm : prime_impli_chart)
    {
         if(minterm.second.size() == 1)
            minimum_cover.insert(minterm.second[0]);
    }
    /*end of generate essential prime implicant*/

    /*delete column covered by epi from pi_chart*/

    for(auto & epi : minimum_cover)
    {
        vector<string> minterms = mintern_gen(epi);
        for(auto &min : minterms)
        {
                prime_impli_chart.erase(min);
        }
    }

    /*end of delete column covered by epi from pi_chart*/



    if(var_num > 6 )
    {
        /*pusdo_implicant*/
        set<string> covered_minterm;


        for(auto & minterm : prime_impli_chart)
        {
            auto search = covered_minterm.find(minterm.first);
            if (search != covered_minterm.end())
            {
                continue;
            }
            else
            {
                string psudo_epi = minterm.second.back();
                minimum_cover.insert(psudo_epi);
                vector<string> minterms = mintern_gen(psudo_epi);
                for(auto &min : minterms)
                {
                        covered_minterm.insert(min);
                }
            }
        }
        /*end of pusdo_implicant*/
    }
    else
    {
        /*petrick method*/

        for(auto const& minterm : prime_impli_chart)
        {
            left_prime_impli.push_back(minterm.second);
        }


        int max_index = left_prime_impli.size() - 1;
        set<string> cur_set;
        petrick_method(0 , max_index, cur_set);


        for(auto & imp : partial_minimum_cover)//merge epi and partial minimum
        {
            minimum_cover.insert(imp);
        }

        /*end of petrick method*/
    }



    /*output*/
    for(auto & ans : minimum_cover)
    {
        for(int i = 0 ;i < var_num ; i ++)
        {
            if(ans[i]!='-')
                literal_cnt++;
        }
    }
    Out_File<<literal_cnt<<endl;
    Out_File<<minimum_cover.size()<<endl;
    for(auto & ans : minimum_cover)
    {
        Out_File<<ans<<endl;
    }
    /*end of output*/

    return 0;
}
int gray_code(string a,string b)
{
    int dif_num = 0 , index;
    for(int i = 0 ;i < var_num;i++)
    {
        if(a[i]!=b[i])
        {
            dif_num++;
            index = i;
        }
    }
    if(dif_num == 1)return index;
    else return -1;
}

vector <string> mintern_gen(string product_term)
{
        vector<string> vec[var_num + 1];//i-bit using  vec[i+1] to store
        vec[0].push_back(product_term);
        int index = 0;
        for(int i = 0 ; i < var_num ; i++)
        {
            if(product_term [i] == '-')
            {
                int str_num = vec[index].size();
                for(int j = 0 ;j < str_num ; j++)
                {
                    string str1,str2;
                    str1 = str2 = vec[index][j];
                    str1 [i] = '0';
                    str2 [i] = '1';
                    vec[i+1].push_back(str1);
                    vec[i+1].push_back(str2);
                }
                index = i+1;
            }
        }
        return vec[index];
}

void petrick_method(int pos , int max_index , set<string>cur_set)
{
    petrick_cnt++;
    if(petrick_cnt > limit)
    {
        return;
    }


    if(pos > max_index)
    {
        int set_size = cur_set.size();
        if(set_size < min_impli_num)
        {
            min_impli_num = set_size;
            partial_minimum_cover = cur_set;
        }
        return;
    }

    int siz = left_prime_impli[pos].size();
    for(int i = siz -1 ; i >= 0 ; i --)
    {
        if(cur_set.find( left_prime_impli[pos][i] ) == cur_set.end())
        {
            cur_set.insert(left_prime_impli[pos][i]);
            int now_size = cur_set.size();
            if(now_size >= min_impli_num )//branch and bound
            {
                return;
            }
            petrick_method(pos + 1, max_index , cur_set );
            cur_set.erase(left_prime_impli[pos][i]);
        }
        else
        {
            petrick_method(pos + 1, max_index , cur_set );
        }
    }
}
