#include <iostream>
#include <vector>
#include <pthread.h>
using namespace std;

vector<vector<pair<int, float>>> numbers; vector<vector<pair<int, float>>> source; vector<float> multVector;
float finalResult[100];
float finalRes[100][100];
int maxRow;
int maxColumn;
int numThread;

void * matrixVectorMult(void * args){

    int tId = *((int*) args);
    //printf("Thread : %d -- Size : %ld\n", tId, numbers.size());
    int partialRes=0;
    for (int i = tId; i < numbers.size(); i+=numThread) {
        //printf("TID : %d - IDX : %d\n", tId, i);
        for (int j = 0; j < numbers[i].size(); j++){
            finalResult[i] += numbers[i][j].second*multVector[numbers[i][j].first]; 
        }
    }
    pthread_exit(NULL);
    
}

void * matrixMatrixMulti(void * args){
    for(int i=0; i<maxRow;i++){
        for(int j=0; j < maxColumn; j++){
            finalRes[i][j] = 0;
        }
    }
    
    for(int i=0; i < numbers.size(); i++){
        for(int k=0; k < numbers[i].size(); k++){
            for(int j=0; j < source[numbers[i][k].first].size(); j++){ // erro em source[k]
                //if(j >= source[numbers[i][k].first].size()){break;}
                finalRes[i][source[numbers[i][k].first][j].first] += numbers[i][k].second*source[numbers[i][k].first][j].second;
            }
        }
    }

    pthread_exit(NULL);

}

int main(){

    system("clear");
    printf("Hello mate:)\n\n");
    printf("Type the number of threads : \nNº : ");
    scanf("%d", &numThread);
    system("clear");
    
    int linha, col, linMt2, colMt2, k=0, z=0, opcao;
    system("clear");

    while(1){
        printf("Choose an operation : \n\n -1- (Sparse Matrix - Vector Multiplication) \n -2- (Sparse - Sparse Matrix Multiplication) \n");
        printf(" -3- (Sparse - Dense Matrix Multiplication)\n -4- (Change Nº of threads)\n -0- (Exit)\n\nOption : ");
        scanf("%d", &opcao);
        system("clear");

        switch(opcao){

            case 1:

            {
            int op1, n, userInput;
            
            cout << "Type the size of the vector : ";
            cin >> n;
            cout << "Type the vector :" << endl;

            for(int i=0; i < n; i++){
                cin >> userInput;
                multVector.push_back(userInput);
            }
            system("clear");

            cout << "Nº of Rows : ";
            cin >> linha;
            cout << "Nº of Colums : ";
            cin >> col;
            system("clear");
            cout << "Type the (" << linha << " x " << col << ") matrix :" << endl; 
            float matrizEsp[linha][col];

            pair<int, float> testing;

            for(int i=0; i<linha;i++){
                vector<pair<int, float>> testVector;
                for(int j=0; j<col; j++){
                    scanf("%f", &matrizEsp[i][j]);
                    if(matrizEsp[i][j] != 0){
                        testing.first = j;
                        testing.second = matrizEsp[i][j];
                        testVector.push_back(testing);
                    }
                    //printf("%d - %f\n", testing.first, testing.second);
                }
                numbers.push_back(testVector);
                }

                pthread_t threads[numThread];
                int taskids[numThread];

                for(int i=0; i < numThread; i++){
                    taskids[i] = i;
                    pthread_create(&threads[i], NULL, matrixVectorMult, &taskids[i]);
                    //printf("Thread : %d\n", i);
                    }
                    
                    for(int i=0; i < numThread; i++){
                        pthread_join(threads[i], NULL);
                    }
                    cout << "----" << endl;
                    for(int i=0;i<numbers.size(); i++){
                        printf("%.2f ", finalResult[i]);
                    }
                    cout << endl;
                    numbers.clear(); multVector.clear();
                    printf("\n-Any number- : Menu\n");
                    if(scanf("%d", &op1)){
                        system("clear");
                        break;
                    }

            }

            case 2: case 3:

            {
                int op2;
                cout << "Nº of Rows : ";
                cin >> linha;
                cout << "Nº of Colums : ";
                cin >> col;
                system("clear");
                cout << "Type the 1º (" << linha << " x " << col << ") matrix :" << endl; 
                float matrizEsp[linha][col];

                pair<int, float> testing;

                for(int i=0; i<linha;i++){
                    vector<pair<int, float>> testVector;
                    for(int j=0; j<col; j++){
                        scanf("%f", &matrizEsp[i][j]);
                        if(matrizEsp[i][j] != 0){
                            testing.first = j;
                            testing.second = matrizEsp[i][j];
                            testVector.push_back(testing);
                        }
                        //printf("%d - %f\n", testing.first, testing.second);
                    }
                    numbers.push_back(testVector);
                }

                    cout << "Nº of Rows : ";
                    cin >> linMt2;
                    cout << "Nº of Colums : ";
                    cin >> colMt2;
                    system("clear");
                    cout << "Type the 2º (" << linha << " x " << col << ") matrix :" << endl; 

                    for(int i=0; i<linMt2;i++){
                        vector<pair<int, float>> testVector;
                        for(int j=0; j<colMt2; j++){
                            scanf("%f", &matrizEsp[i][j]);
                            if(matrizEsp[i][j] != 0){
                                testing.first = j;
                                testing.second = matrizEsp[i][j];
                                testVector.push_back(testing);
                            }
                            //printf("%d - %f\n", testing.first, testing.second);
                        }
                        source.push_back(testVector);
                    }
                    maxRow = linha;
                    maxColumn = colMt2;
                    pthread_t threads[numThread];
                    int taskids[numThread];

                    for(int i=0; i < numThread; i++){
                    taskids[i] = i;
                    pthread_create(&threads[i], NULL, matrixMatrixMulti, &taskids[i]);
                    //printf("Thread : %d\n", i);
                    }
                    
                    for(int i=0; i < numThread; i++){
                        pthread_join(threads[i], NULL);
                    }

                    for(int i=0;i<maxRow;i++){
                        for(int j=0; j<maxColumn;j++){
                                printf("%.2f ", finalRes[i][j]);
                        }
                        printf("\n");
                    }
                    numbers.clear(); source.clear();

                    printf("\n(Any number) - Menu\n");
                    if(scanf("%d", &op2)){
                        system("clear");
                        break;
                    }
            }

            case 4:
            {
                system("clear");
                cout << "Type the new Nº of threads : " << endl;
                scanf("%d", &numThread);
            }

            case 0:
            {
                return 0;
            }

            default:
            printf("Choose an right operation :(\n");
        }

    };
    /*for (int i = 0; i < numbers.size(); i++) {
        for (int j = 0; j < numbers[i].size(); j++)
             printf("%d,%.1f", numbers[i][j].first,numbers[i][j].second);
        cout << endl;
    }
    */

    return 0;
}

