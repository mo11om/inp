#include <cstring>
#include <iostream>
#include <string>
#include <string.h>
#include <vector> 
#include "slover.h"
using namespace std;


int do_things_with(string chr){
    string point=".";
    if(point==chr){
        return 0;
    }
    
    else{
        
        return (stoi(chr));
    }
}
 

void init_vec(vector<vector<int>> vec){
    for (int i = 0; i < 9; i++)
	{
	vector<int> row;
	    vec.push_back(row);
		 
	}
}
void print_vec(vector<vector<int>> vec){
     for (int i = 0; i < 9; i++)
	{
        for (int j = 0; j <vec.at(i).size(); j++)
            cout<<vec[i][j];
        cout<<endl;
	}
}
int slover(string str ,vector<vector<int>>& place_need_to_set, int grid[9][9]){
    cout<< str<<endl;
    
    init_vec(place_need_to_set);
 
    for(int i = 4; i < str.length()-1; ++i) {
        
         
        string t_str="";
        t_str=str[i];
        
         
        int tmp =do_things_with( t_str);
        
        grid[(i-4)/9][(i-4)%9]=tmp;
        if (tmp==0)
        place_need_to_set.at((i-4)/9).push_back((i-4)%9);
         
      
    }
    print(grid);
    print_vec(place_need_to_set);
    if (solveSudoku(grid, 0, 0))
		print(grid);
    return 0;
}
// int main()
// {
//     char rec[100]  ="OK: 1.9245..82.56.19.48.6937.5.32.458...59..26.4746871..2.7.38..6.29.41.237..12.7.48.";
//     string str  =rec;
//     int grid[9][9];
//     vector<vector<int>> place_need_to_set(9);
//     slover (rec,place_need_to_set,grid);
        
        
// }