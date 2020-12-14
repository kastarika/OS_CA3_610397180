#include <bits/stdc++.h>
#include <chrono>

using namespace std;

const int MAXN = 1e4;
const int ATTR_COUNT = 20;
const int WEIGHT_CLASS_COUNT = 4;


double train_data[MAXN][ATTR_COUNT + 5];
double weight_vector[WEIGHT_CLASS_COUNT + 5][ATTR_COUNT + 5];
double min_attr[ATTR_COUNT + 5], max_attr[ATTR_COUNT + 5];
int estimated_class[MAXN];

int main(int argc, char *argv[]){
    //time calculator
   /*
    auto start = chrono::high_resolution_clock::now();
*/


    //open the train data file
    ifstream train_data_file;
    string file_name = argv[1];
    file_name += "train.csv";
    train_data_file.open(file_name);

    //get rid of the first line
    string _;
    getline(train_data_file, _);

    //extract the train data
    int line_num = 0;
    while(train_data_file){
        //get a line
        string attr_line;
        getline(train_data_file, attr_line);
        //split by comma
        stringstream buf_line(attr_line);
        string buf;
        int i = 0;
        while(getline(buf_line, buf, ',')){
            train_data[line_num][i] = stod(buf);
            //increase word number counter
            i++;
        }
        //increase line number counter
        line_num++;
    }
    //save train data size
    int train_data_size = line_num - 1;
    
    //close train data file
    train_data_file.close();







/*
    //time calculator
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 
*/
    ////////////////////////////////////////////////////////////////////////////////////////////////////

   /* 
    //time calculator
    start = chrono::high_resolution_clock::now();
*/





    //normalize data


    //find the min and max for each attribute
    for(int j = 0; j < ATTR_COUNT; j++){
        double cur_min_attr = train_data[0][j], cur_max_attr = train_data[0][j];
        for(int i = 0; i < train_data_size; i++){
            cur_min_attr = min(cur_min_attr, train_data[i][j]);
            cur_max_attr = max(cur_max_attr, train_data[i][j]);
        }
        min_attr[j] = cur_min_attr;
        max_attr[j] = cur_max_attr;
        //cerr << j << "   " << min_attr[j] << "   " << max_attr[j] << endl;
    }

    //change the data based on calculated max and min of attributes
    for(int i = 0; i < train_data_size; i++){
        for(int j = 0; j < ATTR_COUNT; j++){
            //cerr << train_data[i][j] << "    ";
            train_data[i][j] = (train_data[i][j] - min_attr[j]) / (max_attr[j] - min_attr[j]);
            //cerr << train_data[i][j] << endl;
        }
    }




/*
    //time calculator
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 

*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

    //time calculator
    start = chrono::high_resolution_clock::now();
*/






    //open the weight vectors file
    ifstream weight_file;

    file_name = argv[1];
    file_name += "weights.csv";

    weight_file.open(file_name);

    //get rid of the first line
    getline(weight_file, _);

    //extract weight vectors
    line_num = 0;
    while(weight_file){
        //get a line
        string weight_vector_line;
        getline(weight_file, weight_vector_line);
        //split by comma
        stringstream buf_line(weight_vector_line);
        string buf;
        int i = 0;
        while(getline(buf_line, buf, ',')){
            weight_vector[line_num][i] = stod(buf);
            i++;
        }
        line_num++;
    }
    //close the file
    weight_file.close();





/*
    //time calculator
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 
*/


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    //time calculator
    start = chrono::high_resolution_clock::now();
*/






    //calculate the estimated value for each phone
    for(int i = 0; i < train_data_size; i++){
        double max_value = 0;
        int best_class = 0;
        for(int j = 0; j < WEIGHT_CLASS_COUNT; j++){
            double cur_value = 0;
            for(int k = 0; k < ATTR_COUNT; k++){
                cur_value += train_data[i][k] * weight_vector[j][k];
            }
            //add the bias
            cur_value += weight_vector[j][ATTR_COUNT];

            //update the best value
            if(j == 0){
                max_value = cur_value;
            }
            else if(cur_value > max_value){
                max_value = cur_value;
                best_class = j;
            }
        }
        //save the estimated value class
        estimated_class[i] = best_class;
    }






/*
    //time calculator
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 
*/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    //time calculator
    start = chrono::high_resolution_clock::now();
*/






    //calculate accuracy
    int correct_estimates = 0;
    for(int i = 0; i < train_data_size; i++){
        if(train_data[i][ATTR_COUNT] == estimated_class[i]){
            correct_estimates++;
        }
    }

    //print calculated accuracy
    double answer = 1.0 * correct_estimates / train_data_size;
    cout << "Accuracy: ";
    cout << fixed << setprecision(2) << answer * 100 << '%' << endl;
    //cout << correct_estimates << "     " << train_data_size << endl;



/*
    //time calculator
    stop = chrono::high_resolution_clock::now(); 

    duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 
*/
    return 0;
}