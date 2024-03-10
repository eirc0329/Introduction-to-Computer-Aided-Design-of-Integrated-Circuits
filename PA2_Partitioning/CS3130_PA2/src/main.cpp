#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<climits>
#include<algorithm>
#include <cmath>

using namespace std;


int main(int argc, char* argv[]){
    ifstream in_file;
    in_file.open(argv[1]);
    ofstream out_file;
    out_file.open(argv[2]);

    /*variables*/

    int area_limit;
    string dum_str;
    int num_cell;
    int total_area = 0;
    int num_net;
    int cost ;

    /*end of variable*/

    /*read file*/

    in_file >> area_limit;
    in_file >> dum_str;
    in_file >> num_cell;
    int cell_size[num_cell];
    int file_cell_num;
    int file_cell_size;

    for(int i=0;i<num_cell;i++)
    {
        in_file >> file_cell_num;
        in_file >> file_cell_size;
        cell_size[file_cell_num] = file_cell_size;
        total_area += file_cell_size;
    }
    in_file >> dum_str;
    in_file >> num_net;


    if(area_limit/(float)total_area >= 0.5 && num_cell <100000 )//2-way
    {
        vector<int> cell_array[num_cell];
        vector<int> net_array[num_net];
        int cell_inloop;
        int cell_innum;
        int cell_index;

        for(int i=0 ; i<num_net; i++)
        {
            in_file >> cell_innum;
            for(int j=0 ;j< cell_innum;j++)
            {
                in_file >> cell_index;
                cell_array[cell_index].push_back(i);
                net_array[i].push_back(cell_index);
            }
        }

        /*end of read file*/

        bool cell_group[num_cell]={0};
        int area_lowbound = total_area - area_limit;
        int area_center   = (area_limit + area_lowbound)/2;
        bool flag = 1;
        /*initial partition*/

        int g_size = total_area;
        for(int i =0 ;i <num_cell;i++)
        {
            if(g_size <= area_center)break;
            cell_group[i] = 1;
            g_size -= cell_size[i];
        }

        /*end of initial partition*/


        //iteration here

        while(flag == 1)
        {
            /*calculate  group0_size*/
            int group0_size = 0;
            for(int i = 0 ; i< num_cell ;i++)
            {
                if( cell_group[i] == 0 )
                    group0_size +=  cell_size[i];
            }
            /*end of calculate  group0_size*/

            /*contruct net distributtion*/

            int net_distri[num_net][2] ={{0},{0}};
            for(int i = 0 ;i<num_net ;i++)
            {
                int net_size = net_array[i].size();
                for(int j = 0 ;j< net_size ; j++)
                {
                    if(cell_group[net_array[i][j]] == 0)
                        net_distri[i][0]++;
                    else
                        net_distri[i][1]++;
                }
            }

            /*end of contruct net distributtion*/



            /*calculate cost

            cost = 0;
            for(int i = 0 ;i < num_net ;i++)
            {
                if( net_distri[i][0] > 0 && net_distri[i][1] > 0 )
                    cost++;
            }
            cout<<cost<<" "<<area_limit<<" "<<group0_size<<" "<<area_lowbound<<endl;

            end of calculate cost*/

             /*end of if Gk > 0 then swap and  do it again ,else stop*/


            /*initial cell gain*/
            int cell_gain[num_cell]={0};

            for(int i = 0 ;i < num_cell ;i++)
            {
                bool F = cell_group[i];
                bool T = 1-F;
                int siz = cell_array[i].size();

                for(int j = 0 ;j < siz ; j++)
                {
                    if(net_distri[ cell_array[i][j] ][F] == 1)
                        cell_gain[i]++;
                    if(net_distri[ cell_array[i][j] ][T] == 0)
                        cell_gain[i]--;
                }
            }
            /*end of initial cell gain*/

            /*every step's info in one iteration*/

            int chosen_cell[num_cell + 1];//index means the step i of p37
            int max_gain[num_cell + 1];
            int A_size[num_cell + 1];
            bool locked[num_cell]={0};
            A_size[0] = group0_size;

            /*end of every step's info in one iteration*/

            /*choose one cell and lock ,also update others gain*/
            for(int t = 1 ;t <= num_cell ; t++ )//STEP 1~STEP num_cell
            {
                /*find first non-locked,balanced cell gain*/
                int m_gain ;
                int max_gain_index;
                int center_diff;

                for(int i = 0 ; i< num_cell ;i++ )
                {
                    if(locked[i] == 1)continue;
                    int c_size = cell_size[i];
                    int c_group = cell_group [i];
                    int seta_size = A_size[ t - 1] ;
                    if( c_group == 0) seta_size -= c_size;
                    else              seta_size += c_size;

                    if(seta_size <= area_limit && seta_size >= area_lowbound)
                    {
                        max_gain_index = i;
                        center_diff =abs(seta_size - area_center);
                        m_gain = cell_gain[i];
                        break;
                    }
                }

                /*end of find first non-locked,balanced cell gain*/

                /*find the max_gain & need to be balanced & if tie choose more balanced one */

                for(int i = max_gain_index+1 ; i< num_cell ;i++ )
                {
                        if(locked[i] == 1)continue;
                        if(cell_gain[i] >= m_gain)
                        {
                            int c_size = cell_size[i];
                            int c_group = cell_group [i];
                            int seta_size = A_size[ t - 1] ;
                            if( c_group == 0) seta_size -= c_size;
                            else              seta_size += c_size;

                            if(cell_gain[i] == m_gain)//if tie choose more balanced one
                            {

                                int d = abs(seta_size - area_center);
                                if( d < center_diff )
                                {
                                    max_gain_index = i;
                                    center_diff = d;
                                }

                            }
                            else//need to balanced
                            {
                                if(seta_size <= area_limit && seta_size >= area_lowbound)
                                {
                                    max_gain_index = i;
                                    center_diff =abs(seta_size - area_center);
                                    m_gain = cell_gain[i];
                                }
                            }

                        }
                }


                /*end of find the max_gain & need to be balanced & if tie chooce more balanced one */

                /*locked the cell and store this step's info (chosen_cell , max_gain, A_size,locked)*/

                int cell_name = max_gain_index;
                chosen_cell[ t ] = cell_name;
                max_gain[ t ] = m_gain;
                if (cell_group [ cell_name ] == 0)     A_size[ t ] = A_size[ t-1 ] - cell_size[cell_name];
                else                                   A_size[ t ] = A_size[ t-1 ] + cell_size[cell_name];
                locked[cell_name] = 1;

                /*end of locked the cell and store this step's info (chosen_cell , max_gain, A_size,locked)*/

                /*update cell gains & net_distri*/

                int cell_array_size = cell_array[cell_name].size();
                bool F = cell_group [ cell_name ];
                bool T = 1-F;
                for(int i = 0 ;i < cell_array_size ;i++)
                {
                    int net_name = cell_array[cell_name][i];

                    //before the move
                    if( net_distri[ net_name ][T] == 0 )
                    {
                        int net_size = net_array[net_name].size();
                        for(int j = 0 ; j< net_size ; j++)
                        {
                            if( locked [ net_array[net_name][j] ] == 0 )
                                cell_gain[ net_array[net_name][j] ]++;
                        }
                    }
                    else if( net_distri[ net_name ][T] == 1 )
                    {
                        int net_size = net_array[net_name].size();
                        for(int j = 0 ; j< net_size ; j++)
                        {
                            if( locked [ net_array[net_name][j] ] == 0 && cell_group [ net_array[net_name][j] ] == T)
                            {
                                cell_gain[ net_array[net_name][j] ]--;
                                break;
                            }
                        }
                    }

                    //chang F,T to reflect the move
                    net_distri[ net_name ][F]--;
                    net_distri[ net_name ][T]++;

                    //after the move
                    if( net_distri[ net_name ][F] == 0 )
                    {
                        int net_size = net_array[net_name].size();
                        for(int j = 0 ; j< net_size ; j++)
                        {
                            if( locked [ net_array[net_name][j] ] == 0 )
                                cell_gain[ net_array[net_name][j] ]--;
                        }
                    }
                    else if( net_distri[ net_name ][F] == 1 )
                    {
                        int net_size = net_array[net_name].size();
                        for(int j = 0 ; j< net_size ; j++)
                        {
                            if( locked [ net_array[net_name][j] ] == 0 && cell_group [ net_array[net_name][j] ] == F)
                            {
                                cell_gain[ net_array[net_name][j] ]++;
                                break;
                            }
                        }
                    }

                }
                /*end of update cell gains & net_distri*/

            }

            /*end of choose one cell and lock ,also update others gain*/


            /*find the largest partial sum & need to be balance,if tie chooce the most balanced one*/

            int k = 0;
            int max_partial_sum = 0;
            int part_sum = 0;
            int center_dif = INT_MAX;
            for(int i = 1 ;i<=num_cell ;i++)
            {
                part_sum += max_gain[i];
                //need to be balanced
                if(A_size[i] <= area_limit && A_size[i] >= area_lowbound)
                {
                    if(part_sum > max_partial_sum)
                    {
                        k = i;
                        max_partial_sum = part_sum;
                        center_dif = abs(A_size[i] - area_center);
                    }
                    else if(part_sum == max_partial_sum)
                    {
                        if( abs(A_size[i] - area_center) < center_dif )
                        {
                            k = i;
                            center_dif = abs(A_size[i] - area_center);
                        }
                    }
                }
            }
            /*find the largest partial sum & need to be balance,if tie chooce the most balanced one*/


            /*if Gk > 0 then swap and  do it again ,else stop*/

            if( max_partial_sum > 0 )
            {
                for(int i =1 ;i <= k;i++)
                {
                    bool group = cell_group[ chosen_cell[i]];
                    cell_group[ chosen_cell[i]] = !group;
                }
            }
            else
            {
                flag = 0;
            }
            /*end of if Gk > 0 then swap and  do it again ,else stop*/


        }


        /*final cost*/
        int final_cost = 0;
        for(int i = 0; i < num_net ;i++)
        {
            int size = net_array[i].size();
            bool group = cell_group[ net_array[i][0] ];
            for(int j = 1 ;j < size ;j++)
            {
                if(cell_group[ net_array[i][j] ]!= group )
                {
                    final_cost++;
                    break;
                }
            }
        }


        /*end of final cost*/

        /* 2-way output*/

        out_file<<final_cost<<endl;
        out_file<<2<<endl;
        for(int i =0;i<num_cell;i++)
        {
            out_file<<cell_group[i]<<endl;
        }

        /*end of  2-way output*/
    }
    else //k-way
    {
        /*variable*/

        int cell_group[num_cell];
        vector<int> net_array[num_net];
        int num_group = 1;

        /*end of variable*/

        /*read file*/

        int cell_inloop;
        int cell_innum;
        int cell_index;

        for(int i=0 ; i<num_net; i++)
        {
            in_file >> cell_innum;
            for(int j=0 ;j< cell_innum;j++)
            {
                in_file >> cell_index;
                net_array[i].push_back(cell_index);
            }
        }

        /*end of read file*/

        /*initial partition*/

        int cur_size = 0;
        for(int i = 0;i<num_cell;i++)
        {
            if(  (cur_size + cell_size[i]) > area_limit  )
            {
                cur_size = 0;
                ++num_group;
            }
            cell_group[i] = num_group - 1;
            cur_size += cell_size[i];
        }

        /*end of initial partition*/

        /*calculate final_cost*/

        int final_cost = 0;
        for(int i = 0;i<num_net;i++)
        {
            int span = 0;
            bool group [num_group]={0};
            int size = net_array[i].size();

            for(int j = 0 ;j<size ;j++)
            {
                group [ cell_group[ net_array[i][j]  ] ] =1;
            }

            for(int j =0 ;j<num_group;j++)
            {
                if(group[j]==1)
                    span++;
            }
            final_cost+=pow(span-1,2);
        }


        /*end of calculate final_cost*/

        /* k-way output*/

        out_file<<final_cost<<endl;
        out_file<<num_group<<endl;
        for(int i =0;i<num_cell;i++)
        {
            out_file<<cell_group[i]<<endl;
        }

        /*end of  k-way output*/

    }


    return 0;
}
