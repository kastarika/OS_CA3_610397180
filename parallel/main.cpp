#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

const int THREAD_COUNT = 4;
const int MAXN = 1e4 + 10;
const int ATTR_COUNT = 20;
const int WEIGHT_CLASS_COUNT = 4;

double train_data[THREAD_COUNT][MAXN][ATTR_COUNT + 5];
double weight_vector[WEIGHT_CLASS_COUNT + 5][ATTR_COUNT + 5];
double min_attr[ATTR_COUNT + 5], max_attr[ATTR_COUNT + 5];
double min_batch_attr[THREAD_COUNT + 2][ATTR_COUNT + 5],
       max_batch_attr[THREAD_COUNT + 2][ATTR_COUNT + 5];
//int estimated_class[THREAD_COUNT][MAXN];
int batch_size[THREAD_COUNT];
int correct_count[THREAD_COUNT];
string file_folder = "";

mutex mu;

void *read_data(void *arg){
    int thread_id = (long) arg;

    string file_name = file_folder + "train_" + to_string(thread_id) + ".csv";

    //open the train data file
    ifstream train_data_file;
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
            train_data[thread_id][line_num][i] = stod(buf);
            if(i == ATTR_COUNT){
                continue;
            }
            //update min and max attribute
            min_batch_attr[thread_id][i] = (line_num == 0) ? train_data[thread_id][line_num][i] :
                                            min(min_batch_attr[thread_id][i], train_data[thread_id][line_num][i]);
            max_batch_attr[thread_id][i] = (line_num == 0) ? train_data[thread_id][line_num][i] :
                                            max(max_batch_attr[thread_id][i], train_data[thread_id][line_num][i]);
            //increase word number counter
            i++;
        }
        //increase line number counter
        line_num++;
    }
    //save train data size
    batch_size[thread_id] = line_num - 1;

    //close train data file
    train_data_file.close();

    pthread_exit(NULL);

}


void *solve(void *arg){
    int thread_id = (long) arg;

    //change the data based on calculated max and min of attributes
    for(int i = 0; i < batch_size[thread_id]; i++){
        for(int j = 0; j < ATTR_COUNT; j++){
            train_data[thread_id][i][j] = (train_data[thread_id][i][j] - min_attr[j]) / (max_attr[j] - min_attr[j]);
        }
    }

    //calculate the estimated value for each phone
    for(int i = 0; i < batch_size[thread_id]; i++){
        double max_value = 0;
        int best_class = 0;
        for(int j = 0; j < WEIGHT_CLASS_COUNT; j++){
            double cur_value = 0;
            for(int k = 0; k < ATTR_COUNT; k++){
                cur_value += train_data[thread_id][i][k] * weight_vector[j][k];
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
        //check if the estimation was correct
        if(best_class == train_data[thread_id][i][ATTR_COUNT]){
            correct_count[thread_id]++;
        }
    }
    pthread_exit(NULL);
}



int main(int argc, char *argv[]){
/*
    //time calculator
    auto start = chrono::high_resolution_clock::now();
*/
    file_folder = argv[1];


    //open the weight vectors file
    ifstream weight_file;
    
    string weight_file_name = file_folder;
    weight_file_name += "weights.csv";

    weight_file.open(weight_file_name);

    //get rid of the first line
    string _;
    getline(weight_file, _);

    //extract weight vectors
    int line_num = 0;
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





    /////////////////////////////////////////////////////////////////////////////////////////////////////////



    pthread_t read_threads[THREAD_COUNT];

    for(int i = 0; i < THREAD_COUNT; i++){
        //cerr << 3 << "   " << i << endl;
        pthread_create(&read_threads[i], NULL, read_data, (void*) i);
    }


    void* status;

    for(int i = 0 ; i < THREAD_COUNT; i++){
        pthread_join(read_threads[i], &status);
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////


    //find the min and max for each attribute
    for(int j = 0; j < ATTR_COUNT; j++){
        double cur_min_attr = min_batch_attr[0][j], cur_max_attr = max_batch_attr[0][j];
        for(int k = 0; k < THREAD_COUNT; k++){
            cur_min_attr = min(cur_min_attr, min_batch_attr[k][j]);
            cur_max_attr = max(cur_max_attr, max_batch_attr[k][j]);
        }
        min_attr[j] = cur_min_attr;
        max_attr[j] = cur_max_attr;
    }
    

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    pthread_t solve_threads[THREAD_COUNT];
    
    for(int i = 0; i < THREAD_COUNT; i++){
        pthread_create(&solve_threads[i], NULL, solve, (void*) i);
    }
    
    for(int i = 0; i < THREAD_COUNT; i++){
        pthread_join(solve_threads[i], &status);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    double ans = 0;
    double data_size = 0;
    for(int i = 0; i < THREAD_COUNT; i++){
        ans += correct_count[i];
        data_size += batch_size[i];
    }
    ans = ans / data_size;
    cout << fixed << setprecision(2) << ans * 100 << endl;

/*
    //time calculator
    auto stop = chrono::high_resolution_clock::now(); 

    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start); 

    cout << duration.count() << endl; 
*/
    return 0;
}